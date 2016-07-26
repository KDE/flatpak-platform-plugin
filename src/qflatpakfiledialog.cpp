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
#include "qflatpakglobal.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QLoggingCategory>
#include <QWindow>

Q_LOGGING_CATEGORY(QFlatpakPlatformFileDialog, "qt.qpa.qflatpakplatform.FileDialog")

QFlatpakFileDialog::QFlatpakFileDialog()
    : QPlatformFileDialogHelper()
    , m_multipleFiles(false)
    , m_saveFile(false)
{
}

QFlatpakFileDialog::~QFlatpakFileDialog()
{
}

void QFlatpakFileDialog::initializeDialog()
{
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

    setDirectory(options()->initialDirectory());
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
    m_filename = filename.toDisplayString();
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
}

void QFlatpakFileDialog::hide()
{
    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: hide()";
}

bool QFlatpakFileDialog::show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent)
{
    Q_UNUSED(windowFlags);

    qCDebug(QFlatpakPlatformFileDialog) << "File dialog: show()";

    QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                          QLatin1String("/org/freedesktop/portal/desktop"),
                                                          QLatin1String("org.freedesktop.portal.FileChooser"),
                                                          m_saveFile ? QLatin1String("SaveFile") : QLatin1String("OpenFile"));
    QString parentWindowId = QLatin1String("x11:") + QString::number(parent->winId());
    QVariantMap options;
    if (!m_acceptLabel.isEmpty()) {
        options.insert(QLatin1String("accept_label"), m_acceptLabel);
    }
    options.insert(QLatin1String("modal"), windowModality != Qt::NonModal);
    options.insert(QLatin1String("multiple"), m_multipleFiles);
    // TODO filters
    // TODO choices

    message << parentWindowId << m_title << options;

    QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this] (QDBusPendingCallWatcher *watcher) {
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qCDebug(QFlatpakPlatformFileDialog) << "Couldn't get reply";
        } else {
            QDBusConnection::sessionBus().connect(QString(),
                                                  reply.value().path(),
                                                  QLatin1String("org.freedesktop.portal.Request"),
                                                  QLatin1String("Response"),
                                                  this,
                                                  SLOT(gotResponse(uint,QVariantMap)));
        }
    });

    return true;
}

void QFlatpakFileDialog::gotResponse(uint response, const QVariantMap &results)
{
    if (!response) {
        if (results.contains(QLatin1String("uris"))) {
            m_selectedFiles = results.value(QLatin1String("uris")).toStringList();
        }

        Q_EMIT accept();
    }

    Q_EMIT reject();
}
