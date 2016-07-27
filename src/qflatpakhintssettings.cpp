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

#include "qflatpakhintssettings.h"

#include <QDialogButtonBox>
#include <QDir>
#include <QString>
#include <QStandardPaths>
#include <QPalette>
#include <QToolBar>
#include <QLoggingCategory>

#include <KColorScheme>

Q_LOGGING_CATEGORY(QFlatpakPlatform, "qt.qpa.qflatpakplatform")

QFlatpakHintsSettings::QFlatpakHintsSettings()
    : QObject(0)
    , m_palette(nullptr)
{
    m_desktopName = qgetenv("XDG_CURRENT_DESKTOP");

    qCDebug(QFlatpakPlatform) << "XDG_CURRENT_DESKTOP: " << m_desktopName;

    // Use default configuration
    if (m_desktopName == QLatin1String("gnome")) {
        m_hints[QPlatformTheme::CursorFlashTime] = 1200;
        m_hints[QPlatformTheme::MouseDoubleClickInterval] = 400;
        m_hints[QPlatformTheme::MousePressAndHoldInterval] = 500;
        m_hints[QPlatformTheme::MouseDoubleClickDistance] = 5;
        m_hints[QPlatformTheme::StartDragDistance] = 8;
        m_hints[QPlatformTheme::PasswordMaskDelay] = 0;
        m_hints[QPlatformTheme::SystemIconThemeName] = "Adwaita";
        m_hints[QPlatformTheme::SystemIconFallbackThemeName] = "Adwaita";
        m_hints[QPlatformTheme::IconThemeSearchPaths] = xdgIconThemePaths();

        QStringList styleNames;
        styleNames << QStringLiteral("adwaita")
                << QStringLiteral("gtk+")
                << QStringLiteral("fusion")
                << QStringLiteral("windows");
        m_hints[QPlatformTheme::StyleNames] = styleNames;

        m_hints[QPlatformTheme::DialogButtonBoxLayout] = QDialogButtonBox::GnomeLayout;
        m_hints[QPlatformTheme::DialogButtonBoxButtonsHaveIcons] = true;
        m_hints[QPlatformTheme::KeyboardScheme] = QPlatformTheme::GnomeKeyboardScheme;
        m_hints[QPlatformTheme::IconPixmapSizes] = QVariant::fromValue(QList<int>() << 512 << 256 << 128 << 64 << 32 << 22 << 16 << 8);
        m_hints[QPlatformTheme::PasswordMaskCharacter] = QVariant(QChar(0x2022));
    } else { // Use KDE default configuration for the rest
        m_hints[QPlatformTheme::CursorFlashTime] = 2000;
        m_hints[QPlatformTheme::MouseDoubleClickInterval] = 400;
        m_hints[QPlatformTheme::StartDragDistance] = 10;
        m_hints[QPlatformTheme::StartDragTime] = 500;
        m_hints[QPlatformTheme::ToolButtonStyle] = Qt::ToolButtonTextBesideIcon;
        m_hints[QPlatformTheme::ToolBarIconSize] = 22;
        m_hints[QPlatformTheme::ItemViewActivateItemOnSingleClick] = true;
        m_hints[QPlatformTheme::SystemIconThemeName] = QStringLiteral("breeze");
        m_hints[QPlatformTheme::SystemIconFallbackThemeName] = QStringLiteral("hicolor");
        m_hints[QPlatformTheme::IconThemeSearchPaths] = xdgIconThemePaths();
        QStringList styleNames{
            QStringLiteral("breeze"),
            QStringLiteral("oxygen"),
            QStringLiteral("fusion"),
            QStringLiteral("windows")
        };
        m_hints[QPlatformTheme::StyleNames] = styleNames;
        m_hints[QPlatformTheme::DialogButtonBoxLayout] = QDialogButtonBox::KdeLayout;
        m_hints[QPlatformTheme::DialogButtonBoxButtonsHaveIcons] = true;
        m_hints[QPlatformTheme::UseFullScreenForPopupMenu] = true;
        m_hints[QPlatformTheme::KeyboardScheme] = QPlatformTheme::KdeKeyboardScheme;
        m_hints[QPlatformTheme::UiEffects] = QPlatformTheme::GeneralUiEffect;
        m_hints[QPlatformTheme::IconPixmapSizes] = QVariant::fromValue(QList<int>() << 512 << 256 << 128 << 64 << 32 << 22 << 16 << 8);
        m_hints[QPlatformTheme::WheelScrollLines] = 3;
    }

    // Load fonts
    loadFonts();

    // Load palette
    loadPalette();
}

QFlatpakHintsSettings::~QFlatpakHintsSettings()
{
    qDeleteAll(m_fonts);
    delete m_palette;
}

void QFlatpakHintsSettings::loadFonts()
{
    if (m_desktopName == QLatin1String("gnome")) {
        m_fonts[QPlatformTheme::SystemFont] = new QFont("Cantarell", 11);
        m_fonts[QPlatformTheme::FixedFont] = new QFont("Monospace", 11);
    } else {
        m_fonts[QPlatformTheme::SystemFont] = new QFont("Noto Sans", 10);
        m_fonts[QPlatformTheme::FixedFont] = new QFont("Oxygen Mono", 9);
    }
}

void QFlatpakHintsSettings::loadPalette()
{
    if (m_palette) {
        delete m_palette;
        m_palette = nullptr;
    }

    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("color-schemes/") + QLatin1String("Breeze") + QStringLiteral(".colors"));
    if (!path.isEmpty()) {
        m_palette = new QPalette(KColorScheme::createApplicationPalette(KSharedConfig::openConfig(path)));
    }
}

QStringList QFlatpakHintsSettings::xdgIconThemePaths() const
{
    QStringList paths;

    const QFileInfo homeIconDir(QDir::homePath() + QStringLiteral("/.icons"));
    if (homeIconDir.isDir()) {
        paths << homeIconDir.absoluteFilePath();
    }

    QString xdgDirString = QFile::decodeName(qgetenv("XDG_DATA_DIRS"));

    if (xdgDirString.isEmpty()) {
        xdgDirString = QStringLiteral("/usr/local/share:/usr/share");
    }

    Q_FOREACH (const QString &xdgDir, xdgDirString.split(QLatin1Char(':'))) {
        const QFileInfo xdgIconsDir(xdgDir + QStringLiteral("/icons"));
        if (xdgIconsDir.isDir()) {
            paths << xdgIconsDir.absoluteFilePath();
        }
    }

    return paths;
}

