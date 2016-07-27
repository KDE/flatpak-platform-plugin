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

#ifndef QFLATPAK_HINTS_SETTINGS_H
#define QFLATPAK_HINTS_SETTINGS_H

#include <QFont>
#include <QObject>
#include <QVariant>

#include <qpa/qplatformtheme.h>

class QPalette;

class QFlatpakHintsSettings : public QObject
{
    Q_OBJECT
public:
    explicit QFlatpakHintsSettings();
    virtual ~QFlatpakHintsSettings();

    inline QFont * font(QPlatformTheme::Font type) const
    {
        if (m_fonts.contains(type)) {
            return m_fonts[type];
        } else if (m_fonts.contains(QPlatformTheme::SystemFont)) {
            return m_fonts[QPlatformTheme::SystemFont];
        } else {
            // GTK default font
            return new QFont(QLatin1String("Noto Sans"), 10);
        }
    }

    inline QVariant hint(QPlatformTheme::ThemeHint hint) const
    {
        return m_hints[hint];
    }

    inline QPalette *palette() const
    {
        return m_palette;
    }

private Q_SLOTS:
    void loadFonts();
    void loadPalette();

private:
    QStringList xdgIconThemePaths() const;

    QString m_desktopName;
    QPalette *m_palette;
    QHash<QPlatformTheme::Font, QFont*> m_fonts;
    QHash<QPlatformTheme::ThemeHint, QVariant> m_hints;
};

#endif // GNOME_HINTS_SETTINGS_H

