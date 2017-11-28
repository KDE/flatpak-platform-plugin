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
#if QT_VERSION < QT_VERSION_CHECK(5,8,0)
#include <QtPlatformSupport/private/qgenericunixthemes_p.h>
#else
#include <QtThemeSupport/private/qgenericunixthemes_p.h>
#endif
#include <QMimeDatabase>
#include <QMimeType>

const char *QFlatpakPlatformTheme::name = "flatpak";

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QPlatformThemeFactoryInterface_iid, QLatin1String("/platformthemes"), Qt::CaseInsensitive))

QFlatpakPlatformTheme::QFlatpakPlatformTheme()
    : m_platformTheme(Q_NULLPTR)
{
    QList<QString> gtkBasedEnvironments = { QLatin1String("GNOME"), QLatin1String("X-CINNAMON"), QLatin1String("UNITY"),
                                            QLatin1String("MATE"), QLatin1String("XFCE"), QLatin1String("LXDE")
                                          };
    const QString platformTheme = QString::fromLocal8Bit(qgetenv("QT_QPA_FLATPAK_PLATFORMTHEME"));

    // Try to load first platform theme specified by user
    if (!platformTheme.isEmpty()) {
        m_platformTheme = qLoadPlugin<QPlatformTheme, QPlatformThemePlugin>(loader(), platformTheme, QStringList{platformTheme});
    }

    // Try to load platform theme based on desktop environment
    if (!m_platformTheme) {
        const QString desktopName = QString::fromLocal8Bit(qgetenv("XDG_CURRENT_DESKTOP"));
        if (gtkBasedEnvironments.contains(desktopName)) {
            m_platformTheme = qLoadPlugin<QPlatformTheme, QPlatformThemePlugin>(loader(), QLatin1String("gnome"), QStringList{"gnome"});

            if (!m_platformTheme) {
                m_platformTheme = qLoadPlugin<QPlatformTheme, QPlatformThemePlugin>(loader(), QLatin1String("gtk3"), QStringList{"gtk3"});
            }

            // Fallback to QGnomeTheme
            if (!m_platformTheme) {
                m_platformTheme = QGenericUnixTheme::createUnixTheme(QLatin1String("gnome"));
            }
        } else if (desktopName == QLatin1String("KDE")) {
            m_platformTheme = qLoadPlugin<QPlatformTheme, QPlatformThemePlugin>(loader(), QLatin1String("kde"), QStringList{"kde"});

            // Fallback to QKdeTheme
            if (!m_platformTheme) {
                m_platformTheme = QGenericUnixTheme::createUnixTheme(QLatin1String("kde"));
            }
        }
    }

    // Fallback to QGenericUnixTheme
    if (!m_platformTheme) {
        m_platformTheme = QGenericUnixTheme::createUnixTheme(QLatin1String("generic"));
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

QPlatformMenuBar * QFlatpakPlatformTheme::createPlatformMenuBar() const
{
    return m_platformTheme->createPlatformMenuBar();
}

#if !defined(QT_NO_DBUS) && !defined(QT_NO_SYSTEMTRAYICON)
QPlatformSystemTrayIcon * QFlatpakPlatformTheme::createPlatformSystemTrayIcon() const
{
    return m_platformTheme->createPlatformSystemTrayIcon();
}
#endif

QList<QKeySequence> QFlatpakPlatformTheme::keyBindings(QKeySequence::StandardKey key) const
{
    return m_platformTheme->keyBindings(key);
}

QString QFlatpakPlatformTheme::standardButtonText(int button) const
{
    return m_platformTheme->standardButtonText(button);
}
