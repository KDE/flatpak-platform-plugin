/*
 * Copyright © 2017 Red Hat, Inc
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

#ifndef QFLATPAK_SYSTEMTRAYICON_H
#define QFLATPAK_SYSTEMTRAYICON_H

#include <qpa/qplatformmenu.h>
#include <qpa/qplatformsystemtrayicon.h>

class QSystemTrayIconSys;
class SystemTrayMenuItem;
class QAction;
class QMenu;

class SystemTrayMenu : public QPlatformMenu
{
    Q_OBJECT
public:
    SystemTrayMenu();
    ~SystemTrayMenu() Q_DECL_OVERRIDE;
    void insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before) Q_DECL_OVERRIDE;
    QPlatformMenuItem *menuItemAt(int position) const Q_DECL_OVERRIDE;
    QPlatformMenuItem *menuItemForTag(quintptr tag) const Q_DECL_OVERRIDE;
    void removeMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setVisible(bool visible) Q_DECL_OVERRIDE;
    void syncMenuItem(QPlatformMenuItem *menuItem) Q_DECL_OVERRIDE;
    void syncSeparatorsCollapsible(bool enable) Q_DECL_OVERRIDE;
    quintptr tag() const Q_DECL_OVERRIDE;
    QPlatformMenuItem *createMenuItem() const Q_DECL_OVERRIDE;

    QMenu *menu() const;

private:
    quintptr m_tag;
    QPointer<QMenu> m_menu;
    QList<SystemTrayMenuItem*> m_items;
};

class SystemTrayMenuItem : public QPlatformMenuItem
{
    Q_OBJECT
public:
    SystemTrayMenuItem();
    ~SystemTrayMenuItem() Q_DECL_OVERRIDE;
    void setCheckable(bool checkable) Q_DECL_OVERRIDE;
    void setChecked(bool isChecked) Q_DECL_OVERRIDE;
    void setEnabled(bool enabled) Q_DECL_OVERRIDE;
    void setFont(const QFont &font) Q_DECL_OVERRIDE;
    void setIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void setIsSeparator(bool isSeparator) Q_DECL_OVERRIDE;
    void setMenu(QPlatformMenu *menu) Q_DECL_OVERRIDE;
    void setRole(MenuRole role) Q_DECL_OVERRIDE;
    void setShortcut(const QKeySequence &shortcut) Q_DECL_OVERRIDE;
    void setTag(quintptr tag) Q_DECL_OVERRIDE;
    void setText(const QString &text) Q_DECL_OVERRIDE;
    void setVisible(bool isVisible) Q_DECL_OVERRIDE;
    quintptr tag() const Q_DECL_OVERRIDE;
    void setIconSize(int size) Q_DECL_OVERRIDE;

    QAction *action() const;

private:
    quintptr m_tag;
    QAction *m_action;
};

class QFlatpakSystemTrayIcon : public QPlatformSystemTrayIcon
{
public:
    QFlatpakSystemTrayIcon();
    ~QFlatpakSystemTrayIcon() Q_DECL_OVERRIDE;

    void init() Q_DECL_OVERRIDE;
    void cleanup() Q_DECL_OVERRIDE;
    void updateIcon(const QIcon &icon) Q_DECL_OVERRIDE;
    void updateToolTip(const QString &tooltip) Q_DECL_OVERRIDE;
    void updateMenu(QPlatformMenu *menu) Q_DECL_OVERRIDE;
    QRect geometry() const Q_DECL_OVERRIDE;
    void showMessage(const QString &title, const QString &msg,
                     const QIcon &icon, MessageIcon iconType, int secs) Q_DECL_OVERRIDE;

    bool isSystemTrayAvailable() const Q_DECL_OVERRIDE;
    bool supportsMessages() const Q_DECL_OVERRIDE;
    QPlatformMenu *createMenu() const Q_DECL_OVERRIDE;

private:
    QSystemTrayIconSys *sys = nullptr;
};

#endif
