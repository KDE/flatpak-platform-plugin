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

#ifndef QFLATPAK_FILEDIALOG_H
#define QFLATPAK_FILEDIALOG_H

#include <QtCore/qeventloop.h>
#include <qpa/qplatformdialoghelper.h>
#include <QMetaType>
#include <QWindow>

class QFlatpakFileDialog : public QPlatformFileDialogHelper
{
    Q_OBJECT
public:
    // Keep in sync with filechooser from xdg-desktop-portal-kde
    typedef struct {
        uint type;
        QString filterString;
    } Filter;
    typedef QList<Filter> Filters;

    typedef struct {
        QString userVisibleName;
        Filters filters;
    } FilterList;
    typedef QList<FilterList> FilterListList;

    QFlatpakFileDialog();
    ~QFlatpakFileDialog();

    void initializeDialog();

    bool defaultNameFilterDisables() const Q_DECL_OVERRIDE { return false; }
    QUrl directory() const Q_DECL_OVERRIDE;
    void setDirectory(const QUrl &directory) Q_DECL_OVERRIDE;
    void selectFile(const QUrl &filename) Q_DECL_OVERRIDE;
    QList<QUrl> selectedFiles() const Q_DECL_OVERRIDE;
    // TODO
    void setFilter() Q_DECL_OVERRIDE;
    void selectNameFilter(const QString &filter) Q_DECL_OVERRIDE;
    QString selectedNameFilter() const Q_DECL_OVERRIDE;
    void selectedFilesChanged(QList<QUrl> selection);

    void exec() Q_DECL_OVERRIDE;
    void hide() Q_DECL_OVERRIDE;
    bool show(Qt::WindowFlags windowFlags, Qt::WindowModality windowModality, QWindow *parent) Q_DECL_OVERRIDE;
public Q_SLOTS:
    void gotResponse(uint response, const QVariantMap &results);

private:
    WId m_winId;
    bool m_modal;
    bool m_multipleFiles;
    bool m_saveFile;
    QString m_acceptLabel;
    QString m_directory;
    QString m_filename;
    QString m_title;
    QStringList m_nameFilters;
    QStringList m_mimeTypesFilters;
    QStringList m_selectedFiles;
};

#endif // QFLATPAK_FILEDIALOG_H


