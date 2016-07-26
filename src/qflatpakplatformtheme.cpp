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

#include "qflatpakplatformtheme.h"
#include "qflatpakfiledialog.h"

#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QLoggingCategory>

#include <QtGui/private/qguiapplication_p.h>
#include <qpa/qplatformintegration.h>

const char *QFlatpakPlatformTheme::name = "flatpak";

Q_LOGGING_CATEGORY(QFlatpakPlatform, "qt.qpa.qflatpakplatform")

QFlatpakPlatformTheme::QFlatpakPlatformTheme()
{
    qCDebug(QFlatpakPlatform) << "Initialize QFlatpakPlatform";
}

QFlatpakPlatformTheme::~QFlatpakPlatformTheme()
{
}

bool QFlatpakPlatformTheme::usePlatformNativeDialog(QPlatformTheme::DialogType type) const
{
    qCDebug(QFlatpakPlatform) << "Use native platform dialog: " << type;
    switch (type) {
    case FileDialog:
        return true;
    default:
        return false;
    }
}

QPlatformDialogHelper *QFlatpakPlatformTheme::createPlatformDialogHelper(QPlatformTheme::DialogType type) const
{
    qCDebug(QFlatpakPlatform) << "Create platform dialog helper: " << type;

    switch (type) {
    case FileDialog:
        return new QFlatpakFileDialog;
        break;
    default:
        return 0;
    }
}
