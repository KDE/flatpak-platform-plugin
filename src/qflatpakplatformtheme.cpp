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

#include <QDebug>
#include <qpa/qplatformtheme.h>
#include <qpa/qplatformintegration.h>
#include <qpa/qplatformthemefactory_p.h>
#include <qpa/qplatformthemeplugin.h>
#include <private/qfactoryloader_p.h>

const char *QFlatpakPlatformTheme::name = "flatpak";

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QPlatformThemeFactoryInterface_iid, QLatin1String("/platformthemes"), Qt::CaseInsensitive))

QFlatpakPlatformTheme::QFlatpakPlatformTheme()
    : m_platformTheme(Q_NULLPTR)
{
    const QString realPlatformTheme = qgetenv("QT_FLATPAK_REAL_PLATFORM_THEME");

    // TODO: not a nice solution, but the only one working
    // TODO: add ifdef for Qt 5.7 as qLoadPlugin1 is deprecated there

    if (!realPlatformTheme.isEmpty()) {
        m_platformTheme = qLoadPlugin1<QPlatformTheme, QPlatformThemePlugin>(loader(), realPlatformTheme, QStringList{realPlatformTheme});
    } else {
        const QString desktopName = qgetenv("XDG_CURRENT_DESKTOP");
        if (desktopName == QLatin1String("gnome")) {
            m_platformTheme = qLoadPlugin1<QPlatformTheme, QPlatformThemePlugin>(loader(), QLatin1String("qgnomeplatform"), QStringList{"qgnomeplatform"});

            // prefer the GTK3 theme implementation with native dialogs etc.
            if (!m_platformTheme) {
                m_platformTheme = qLoadPlugin1<QPlatformTheme, QPlatformThemePlugin>(loader(), QLatin1String("gtk3"), QStringList{"gtk3"});
            }
            // fallback to the generic Gnome theme if loading the GTK3 theme fails
            if (!m_platformTheme) {
                m_platformTheme = qLoadPlugin1<QPlatformTheme, QPlatformThemePlugin>(loader(), QLatin1String("gnome"), QStringList{"gnome"});
            }
        } else if (desktopName == QLatin1String("KDE")) {
            m_platformTheme = qLoadPlugin1<QPlatformTheme, QPlatformThemePlugin>(loader(), QLatin1String("kde"), QStringList{"kde"});
        }
    }

    if (!m_platformTheme) {
        m_platformTheme = new QPlatformTheme;
    }
}

QFlatpakPlatformTheme::~QFlatpakPlatformTheme()
{
}

QVariant QFlatpakPlatformTheme::themeHint(QPlatformTheme::ThemeHint hintType) const
{
    return m_platformTheme->themeHint(hintType);
}

const QPalette *QFlatpakPlatformTheme::palette(Palette type) const
{
    return m_platformTheme->palette(type);
}

const QFont *QFlatpakPlatformTheme::font(Font type) const
{
    return m_platformTheme->font(type);
}

bool QFlatpakPlatformTheme::usePlatformNativeDialog(QPlatformTheme::DialogType type) const
{
    switch (type) {
    case FileDialog:
        return true;
    default:
        return m_platformTheme->usePlatformNativeDialog(type);
    }
}

QPlatformDialogHelper *QFlatpakPlatformTheme::createPlatformDialogHelper(QPlatformTheme::DialogType type) const
{
    switch (type) {
    case FileDialog:
        return new QFlatpakFileDialog;
        break;
    default:
        return m_platformTheme->createPlatformDialogHelper(type);
    }
}

QList<QKeySequence> QFlatpakPlatformTheme::keyBindings(QKeySequence::StandardKey key) const
{
    return m_platformTheme->keyBindings(key);
}

QString QFlatpakPlatformTheme::standardButtonText(int button) const
{
    return m_platformTheme->standardButtonText(button);
}
