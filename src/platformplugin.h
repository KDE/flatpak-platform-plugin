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
#ifndef QFLATPAK_PLATFORM_PLUGIN_H
#define QFLATPAK_PLATFORM_PLUGIN_H

#include <qpa/qplatformtheme.h>
#include <qpa/qplatformthemeplugin.h>

class QFlatpakPlatformThemePlugin : public QPlatformThemePlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QPA.QPlatformThemeFactoryInterface.5.1" FILE "qflatpakplatform.json")
public:
    QFlatpakPlatformThemePlugin(QObject *parent = 0);

    virtual QPlatformTheme *create(const QString &key, const QStringList &paramList);
};

#endif // QFLATPAK_PLATFORM_PLUGIN_H
