/*
 * Copyright © 2016 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Jan Grulich <jgrulich@redhat.com>
 */

#include "qflatpakfiledialog.h"

#include <QtDBus/QtDBus>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QLoggingCategory>
#include <QMimeType>
#include <QMimeDatabase>

Q_LOGGING_CATEGORY(QFlatpakPlatformFileDialog, "qt.qpa.qflatpakplatform.FileDialog")

// Keep in sync with filechooser from xdg-desktop-portal-kde
Q_DECLARE_METATYPE(QFlatpakFileDialog::Filter);
Q_DECLARE_METATYPE(QFlatpakFileDialog::Filters);
Q_DECLARE_METATYPE(QFlatpakFileDialog::FilterList);
Q_DECLARE_METATYPE(QFlatpakFileDialog::FilterListList);

QDBusArgument &operator << (QDBusArgument &arg, const QFlatpakFileDialog::Filter &filter)
{
    arg.beginStructure();
    arg << filter.type << filter.filterString;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator >> (const QDBusArgument &arg, QFlatpakFileDialog::Filter &filter)
{
    uint type;
    QString filterString;
    arg.beginStructure();
    arg >> type >> filterString;
    filter.type = type;
    filter.filterString = filterString;
    arg.endStructure();

    return arg;
}

QDBusArgument &operator << (QDBusArgument &arg, const QFlatpakFileDialog::FilterList &filterList)
{
    arg.beginStructure();
    arg << filterList.userVisibleName << filterList.filters;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator >> (const QDBusArgument &arg, QFlatpakFileDialog::FilterList &filterList)
{
    QString userVisibleName;
    QFlatpakFileDialog::Filters filters;
    arg.beginStructure();
    arg >> userVisibleName >> filters;
    filterList.userVisibleName = userVisibleName;
    filterList.filters = filters;
    arg.endStructure();

    return arg;
}

QFlatpakFileDialog::QFlatpakFileDialog()
    : QPlatformFileDialogHelper()
    , m_winId(0)
    , m_modal(false)
    , m_multipleFiles(false)
    , m_saveFile(false)
{
}

QFlatpakFileDialog::~QFlatpakFileDialog()
{
}

void QFlatpakFileDialog::initializeDialog()
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: initializeDialog()";

    if (options()->fileMode() == QFileDialogOptions::ExistingFiles) {
        m_multipleFiles = true;
    }

    if (options()->isLabelExplicitlySet(QFileDialogOptions::Accept)) {
        m_acceptLabel = options()->labelText(QFileDialogOptions::Accept);
    }

    if (!options()->windowTitle().isEmpty()) {
        m_title = options()->windowTitle();
    }

    if (options()->acceptMode() == QFileDialogOptions::AcceptSave) {
        m_saveFile = true;
    }

    if (!options()->nameFilters().isEmpty()) {
        m_nameFilters = options()->nameFilters();
    }

    if (!options()->mimeTypeFilters().isEmpty()) {
        m_mimeTypesFilters = options()->mimeTypeFilters();
    }

    setDirectory(options()->initialDirectory());

    qCDebug(QFlatpakPlatformFileDialog) << "Initial values: ";
    qCDebug(QFlatpakPlatformFileDialog) << "       Multiple files: " << m_multipleFiles;
    qCDebug(QFlatpakPlatformFileDialog) << "         Accept label: " << m_acceptLabel;
    qCDebug(QFlatpakPlatformFileDialog) << "          Window title: " << m_title;
    qCDebug(QFlatpakPlatformFileDialog) << "            Save/Open: " << (m_saveFile ? "Save" : "Open");
    qCDebug(QFlatpakPlatformFileDialog) << "         Name filters: " << m_nameFilters;
    qCDebug(QFlatpakPlatformFileDialog) << "    MimeTypes filters: " << m_mimeTypesFilters;
    qCDebug(QFlatpakPlatformFileDialog) << "    Initial directory: " << m_directory;
}

void QFlatpakFileDialog::setDirectory(const QUrl &directory)
{
    m_directory = directory.toDisplayString();
}

QUrl QFlatpakFileDialog::directory() const
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: directory()";

    return m_directory;
}

void QFlatpakFileDialog::selectFile(const QUrl &filename)
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: select(" << filename.toDisplayString() << ")";
    m_selectedFiles << filename.toDisplayString();
}

QList<QUrl> QFlatpakFileDialog::selectedFiles() const
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: selectedFiles()";

    QList<QUrl> files;
    Q_FOREACH(const QString &file, m_selectedFiles) {
        files << QUrl(file);
    }
    return files;
}

void QFlatpakFileDialog::setFilter()
{
    // TODO
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: setFilter()";
}

void QFlatpakFileDialog::selectNameFilter(const QString &filter)
{
    Q_UNUSED(filter);
    // TODO
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: selectNameFilter()";
}

QString QFlatpakFileDialog::selectedNameFilter() const
{
    // TODO
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: selectedNameFilter()";

    return QString();
}

void QFlatpakFileDialog::exec()
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: exec()";

    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.FileChooser"),
                                                          m_saveFile ? QLatin1String("SaveFile") : QLatin1String("OpenFile"));
    QString parentWindowId = QLatin1String("x11:") + QString::number(m_winId);

    QVariantMap options;
    if (!m_acceptLabel.isEmpty()) {
        options.insert(QLatin1String("accept_label"), m_acceptLabel);
    }

    options.insert(QLatin1String("modal"), m_modal);
    options.insert(QLatin1String("multiple"), m_multipleFiles);

    if (m_saveFile) {
        if (!m_directory.isEmpty()) {
            options.insert(QLatin1String("current_folder"), m_directory.toLatin1());
        }

        if (!m_selectedFiles.isEmpty()) {
            options.insert(QLatin1String("current_file"), m_selectedFiles.first().toLatin1());
        }
    }

    // Insert filters
    qDBusRegisterMetaType<Filter>();
    qDBusRegisterMetaType<Filters>();
    qDBusRegisterMetaType<FilterList>();
    qDBusRegisterMetaType<FilterListList>();

    FilterListList filterList;

    if (!m_mimeTypesFilters.isEmpty()) {
        Q_FOREACH (const QString &filter, m_mimeTypesFilters) {
            QMimeDatabase mimeDatabase;
            QMimeType mimeType = mimeDatabase.mimeTypeForName(filter);

            // Creates e.g. (1, "image/png")
            Filter filterStruct;
            filterStruct.type = 1; // MimeType
            filterStruct.filterString = filter;

            // Creates e.g. [((1, "image/png"))]
            Filters filters;
            filters << filterStruct;

            // Creates e.g. [("Images", [((1, "image/png"))])]
            FilterList filterListStruct;
            filterListStruct.userVisibleName = mimeType.comment();
            filterListStruct.filters = filters;

            filterList << filterListStruct;
        }
    } else if (!m_nameFilters.isEmpty()) {
        Q_FOREACH (const QString &filter, m_nameFilters) {
            // Do parsing:
            // Supported format is ("Images (*.png *.jpg)")
            QRegExp regexp(QString::fromLatin1(QPlatformFileDialogHelper::filterRegExp));
            if (regexp.indexIn(filter) >= 0) {
                QString userVisibleName = regexp.cap(1);
                QStringList filterStrings = regexp.cap(2).split(QLatin1String(" "));

                Filters filters;
                Q_FOREACH (const QString &filterString, filterStrings) {
                    Filter filterStruct;
                    filterStruct.type = 0; // Global pattern
                    filterStruct.filterString = filterString;
                    filters << filterStruct;
                }

                FilterList filterListStruct;
                filterListStruct.userVisibleName = userVisibleName;
                filterListStruct.filters = filters;

                filterList << filterListStruct;
            }
        }
    }

    if (!filterList.isEmpty()) {
        options.insert(QLatin1String("filters"), QVariant::fromValue(filterList));
    }

    // TODO choices

    message << parentWindowId << m_title << options;

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this] (QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qCCritical(QFlatpakPlatformFileDialog) << "Couldn't get reply";
            qCCritical(QFlatpakPlatformFileDialog) << "Error: " << reply.error().message();
            Q_EMIT reject();
        } else {
            QDBusConnection::sessionBus().connect(QString(),
                                                  reply.value().path(),
                                                  QLatin1String("org.freedesktop.portal.Request"),
                                                  QLatin1String("Response"),
                                                  this,
                                                  SLOT(gotResponse(uint,QVariantMap)));
        }
    });

    // HACK we have to avoid returning until we emit that the dialog was accepted or rejected
    QEventLoop loop;
    loop.connect(this, SIGNAL(accept()), SLOT(quit()));
    loop.connect(this, SIGNAL(reject()), SLOT(quit()));
    loop.exec();
}

void QFlatpakFileDialog::hide()
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: hide()";
}

bool QFlatpakFileDialog::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    Q_UNUSED(windowFlags);

    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: show()";

    initializeDialog();

    m_modal = windowModality != Qt::NonModal;
    m_winId = parent ? parent->winId() : 0;

    return true;
}

void QFlatpakFileDialog::gotResponse(uint response, const QVariantMap &results)
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: gotResponse()";

    if (!response) {
        if (results.contains(QLatin1String("uris"))) {
            qCDebug(QFlatpakPlatformFileDialog) << results.value(QLatin1String("uris")).toStringList();
            m_selectedFiles = results.value(QLatin1String("uris")).toStringList();
        }

        qCDebug(QFlatpakPlatformFileDialog) << "File dialog: gotResponse() - emit accept()";
        Q_EMIT accept();
    } else {
        qCDebug(QFlatpakPlatformFileDialog) << "File dialog: gotResponse() - emit reject()";
        Q_EMIT reject();
    }
}
