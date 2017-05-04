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

#include "qflatpaksystemtrayicon.h"
#include "qflatpakintegration.h"
#include "qsystemtrayicon_p.h"

#include <QApplication>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QWindow>
#include <QtWidgets/QWidget>

#include <qpa/qplatformnativeinterface.h>
#include <qpa/qplatformsystemtrayicon.h>
#include <private/qguiapplication_p.h>

#include <QtPlatformHeaders/qxcbwindowfunctions.h>
#include <QtPlatformHeaders/qxcbintegrationfunctions.h>

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QRect>
#include <QApplication>
#include <QDBusInterface>

SystemTrayMenu::SystemTrayMenu()
    : QPlatformMenu()
    , m_tag(0)
    , m_menu(new QMenu())
{
    connect(m_menu.data(), &QMenu::aboutToShow, this, &QPlatformMenu::aboutToShow);
    connect(m_menu.data(), &QMenu::aboutToHide, this, &QPlatformMenu::aboutToHide);
}

SystemTrayMenu::~SystemTrayMenu()
{
    if (m_menu) {
        m_menu->deleteLater();
    }
}

QPlatformMenuItem *SystemTrayMenu::createMenuItem() const
{
    return new SystemTrayMenuItem();
}

void SystemTrayMenu::insertMenuItem(QPlatformMenuItem *menuItem, QPlatformMenuItem *before)
{
    if (SystemTrayMenuItem *ours = qobject_cast<SystemTrayMenuItem*>(menuItem)) {
        bool inserted = false;
        if (SystemTrayMenuItem *oursBefore = qobject_cast<SystemTrayMenuItem*>(before)) {
            for (auto it = m_items.begin(); it != m_items.end(); ++it) {
                if (*it == oursBefore) {
                    m_items.insert(it, ours);
                    if (m_menu) {
                        m_menu->insertAction(oursBefore->action(), ours->action());
                    }
                    inserted = true;
                    break;
                }
            }
        }
        if (!inserted) {
            m_items.append(ours);
            if (m_menu) {
                m_menu->addAction(ours->action());
            }
        }
    }
}

QPlatformMenuItem *SystemTrayMenu::menuItemAt(int position) const
{
    if (position < m_items.size()) {
        return m_items.at(position);
    }
    return Q_NULLPTR;
}

QPlatformMenuItem *SystemTrayMenu::menuItemForTag(quintptr tag) const
{
    auto it = std::find_if(m_items.constBegin(), m_items.constEnd(), [tag](SystemTrayMenuItem *item) {
        return item->tag() == tag;
    });
    if (it != m_items.constEnd()) {
        return *it;
    }
    return Q_NULLPTR;
}

void SystemTrayMenu::removeMenuItem(QPlatformMenuItem *menuItem)
{
    if (SystemTrayMenuItem *ours = qobject_cast<SystemTrayMenuItem*>(menuItem)) {
        m_items.removeOne(ours);
        if (ours->action() && m_menu) {
            m_menu->removeAction(ours->action());
        }
    }
}

void SystemTrayMenu::setEnabled(bool enabled)
{
    if (!m_menu) {
        return;
    }
    m_menu->setEnabled(enabled);
}

void SystemTrayMenu::setIcon(const QIcon &icon)
{
    if (!m_menu) {
        return;
    }
    m_menu->setIcon(icon);
}

void SystemTrayMenu::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenu::setText(const QString &text)
{
    if (!m_menu) {
        return;
    }
    m_menu->setTitle(text);
}

void SystemTrayMenu::setVisible(bool visible)
{
    if (!m_menu) {
        return;
    }
    m_menu->setVisible(visible);
}

void SystemTrayMenu::syncMenuItem(QPlatformMenuItem *menuItem)
{
    Q_UNUSED(menuItem)
    // nothing to do
}

void SystemTrayMenu::syncSeparatorsCollapsible(bool enable)
{
    if (!m_menu) {
        return;
    }
    m_menu->setSeparatorsCollapsible(enable);
}

quintptr SystemTrayMenu::tag() const
{
    return m_tag;
}

QMenu *SystemTrayMenu::menu() const
{
    return m_menu.data();
}

SystemTrayMenuItem::SystemTrayMenuItem()
    : QPlatformMenuItem()
    , m_tag(0)
    , m_action(new QAction(this))
{
    connect(m_action, &QAction::triggered, this, &QPlatformMenuItem::activated);
    connect(m_action, &QAction::hovered, this, &QPlatformMenuItem::hovered);
}

SystemTrayMenuItem::~SystemTrayMenuItem()
{
}

void SystemTrayMenuItem::setCheckable(bool checkable)
{
    m_action->setCheckable(checkable);
}

void SystemTrayMenuItem::setChecked(bool isChecked)
{
    m_action->setChecked(isChecked);
}

void SystemTrayMenuItem::setEnabled(bool enabled)
{
    m_action->setEnabled(enabled);
}

void SystemTrayMenuItem::setFont(const QFont &font)
{
    m_action->setFont(font);
}

void SystemTrayMenuItem::setIcon(const QIcon &icon)
{
    m_action->setIcon(icon);
}

void SystemTrayMenuItem::setIsSeparator(bool isSeparator)
{
    m_action->setSeparator(isSeparator);
}

void SystemTrayMenuItem::setMenu(QPlatformMenu *menu)
{
    if (SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu *>(menu)) {
        m_action->setMenu(ourMenu->menu());
    }
}

void SystemTrayMenuItem::setRole(QPlatformMenuItem::MenuRole role)
{
    Q_UNUSED(role)
}

void SystemTrayMenuItem::setShortcut(const QKeySequence &shortcut)
{
    m_action->setShortcut(shortcut);
}

void SystemTrayMenuItem::setTag(quintptr tag)
{
    m_tag = tag;
}

void SystemTrayMenuItem::setText(const QString &text)
{
    m_action->setText(text);
}

void SystemTrayMenuItem::setVisible(bool isVisible)
{
    m_action->setVisible(isVisible);
}

void SystemTrayMenuItem::setIconSize(int size)
{
    Q_UNUSED(size);
}

quintptr SystemTrayMenuItem::tag() const
{
    return m_tag;
}

QAction *SystemTrayMenuItem::action() const
{
    return m_action;
}

static inline unsigned long locateSystemTray()
{
    return (unsigned long)QGuiApplication::platformNativeInterface()->nativeResourceForScreen(QByteArrayLiteral("traywindow"), QGuiApplication::primaryScreen());
}

// System tray widget. Could be replaced by a QWindow with
// a backing store if it did not need tooltip handling.
class QSystemTrayIconSys : public QWidget
{
    Q_OBJECT
public:
    explicit QSystemTrayIconSys(QPlatformSystemTrayIcon *q);

    inline QPlatformSystemTrayIcon *systemTrayIcon() const { return q; }

    inline void updateContextMenu(QMenu *newMenu) {
        menu = newMenu;
        update();
    }

    inline void updateIcon(const QIcon &newIcon) {
        icon = newIcon;
        update();
    }

    QRect globalGeometry() const;

protected:
    virtual void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    virtual void mouseDoubleClickEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;
    virtual void moveEvent(QMoveEvent *) Q_DECL_OVERRIDE;

private slots:
    void systemTrayWindowChanged(QScreen *screen);

private:
    bool addToTray();

    QPlatformSystemTrayIcon *q;
    QIcon icon;
    QMenu *menu = nullptr;
    QPixmap background;
};

QSystemTrayIconSys::QSystemTrayIconSys(QPlatformSystemTrayIcon *qIn)
    : QWidget(0, Qt::Window | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint)
    , q(qIn)
{
    setObjectName(QStringLiteral("QSystemTrayIconSys"));
#if defined(QT_TOOLTIP)
    setToolTip(q->toolTip());
#endif
    setAttribute(Qt::WA_AlwaysShowToolTips, true);
    setAttribute(Qt::WA_QuitOnClose, false);
    const QSize size(22, 22); // Gnome, standard size
    setGeometry(QRect(QPoint(0, 0), size));
    setMinimumSize(size);

    // We need two different behaviors depending on whether the X11 visual for the system tray
    // (a) exists and (b) supports an alpha channel, i.e. is 32 bits.
    // If we have a visual that has an alpha channel, we can paint this widget with a transparent
    // background and it will work.
    // However, if there's no alpha channel visual, in order for transparent tray icons to work,
    // we do not have a transparent background on the widget, but set the BackPixmap property of our
    // window to ParentRelative (so that it inherits the background of its X11 parent window), call
    // xcb_clear_region before painting (so that the inherited background is visible) and then grab
    // the just-drawn background from the X11 server.

    // TODO get real value
    // bool hasAlphaChannel = QXcbIntegrationFunctions::xEmbedSystemTrayVisualHasAlphaChannel();
    bool hasAlphaChannel = true;
    setAttribute(Qt::WA_TranslucentBackground, hasAlphaChannel);
    if (!hasAlphaChannel) {
        createWinId();
        QXcbWindowFunctions::setParentRelativeBackPixmap(windowHandle());

        // XXX: This is actually required, but breaks things ("QWidget::paintEngine: Should no
        // longer be called"). Why is this needed? When the widget is drawn, we use tricks to grab
        // the tray icon's background from the server. If the tray icon isn't visible (because
        // another window is on top of it), the trick fails and instead uses the content of that
        // other window as the background.
        // setAttribute(Qt::WA_PaintOnScreen);
    }

    addToTray();
}

bool QSystemTrayIconSys::addToTray()
{
    if (!locateSystemTray())
        return false;

    createWinId();
    setMouseTracking(true);

    if (!QXcbWindowFunctions::requestSystemTrayWindowDock(windowHandle()))
        return false;

    if (!background.isNull())
        background = QPixmap();
    show();
    return true;
}

void QSystemTrayIconSys::systemTrayWindowChanged(QScreen *)
{
    if (locateSystemTray()) {
        addToTray();
    } else {
        QBalloonTip::hideBalloon();
        hide(); // still no luck
        destroy();
    }
}

QRect QSystemTrayIconSys::globalGeometry() const
{
    return QXcbWindowFunctions::systemTrayWindowGlobalGeometry(windowHandle());
}

void QSystemTrayIconSys::mousePressEvent(QMouseEvent *ev)
{
    QPoint globalPos = ev->globalPos();
    // TODO context menu
#ifndef QT_NO_CONTEXTMENU
    if (ev->button() == Qt::RightButton && menu)
        menu->popup(globalPos);
#else
    Q_UNUSED(globalPos)
#endif // QT_NO_CONTEXTMENU

    if (QBalloonTip::isBalloonVisible()) {
        emit q->messageClicked();
        QBalloonTip::hideBalloon();
    }

    if (ev->button() == Qt::LeftButton)
        emit q->activated(QPlatformSystemTrayIcon::Trigger);
    else if (ev->button() == Qt::RightButton)
        emit q->activated(QPlatformSystemTrayIcon::Context);
    else if (ev->button() == Qt::MidButton)
        emit q->activated(QPlatformSystemTrayIcon::MiddleClick);
}

void QSystemTrayIconSys::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
        emit q->activated(QPlatformSystemTrayIcon::DoubleClick);
}

bool QSystemTrayIconSys::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::ToolTip:
        QApplication::sendEvent(q, e);
        break;
#ifndef QT_NO_WHEELEVENT
    case QEvent::Wheel:
        return QApplication::sendEvent(q, e);
#endif
    default:
        break;
    }
    return QWidget::event(e);
}

void QSystemTrayIconSys::paintEvent(QPaintEvent *)
{
    const QRect rect(QPoint(0, 0), geometry().size());
    QPainter painter(this);

    // If we have Qt::WA_TranslucentBackground set, during widget creation
    // we detected the systray visual supported an alpha channel
    if (testAttribute(Qt::WA_TranslucentBackground)) {
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(rect, Qt::transparent);
    } else {
        // clearRegion() was called on XEMBED_EMBEDDED_NOTIFY, so we hope that got done by now.
        // Grab the tray background pixmap, before rendering the icon for the first time.
        if (background.isNull()) {
            background = QGuiApplication::primaryScreen()->grabWindow(winId(),
                                0, 0, rect.size().width(), rect.size().height());
        }
        // Then paint over the icon area with the background before compositing the icon on top.
        painter.drawPixmap(QPoint(0, 0), background);
    }
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    icon.paint(&painter, rect);
}

void QSystemTrayIconSys::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    if (QBalloonTip::isBalloonVisible())
        QBalloonTip::updateBalloonPosition(globalGeometry().center());
}

void QSystemTrayIconSys::resizeEvent(QResizeEvent *event)
{
    update();
    QWidget::resizeEvent(event);
    if (QBalloonTip::isBalloonVisible())
        QBalloonTip::updateBalloonPosition(globalGeometry().center());
}

QFlatpakSystemTrayIcon::QFlatpakSystemTrayIcon()
    : QPlatformSystemTrayIcon()
{
}

QFlatpakSystemTrayIcon::~QFlatpakSystemTrayIcon()
{
}

void QFlatpakSystemTrayIcon::init()
{
    if (!sys && locateSystemTray()) {
        sys = new QSystemTrayIconSys(this);
    }
}

void QFlatpakSystemTrayIcon::cleanup()
{
    QBalloonTip::hideBalloon();
    sys->hide(); // this should do the trick, but...
    delete sys; // wm may resize system tray only for DestroyEvents
    sys = 0;
}

void QFlatpakSystemTrayIcon::updateIcon(const QIcon &icon)
{
    if (!sys) {
        return;
    }

    sys->updateIcon(icon);
}

void QFlatpakSystemTrayIcon::updateToolTip(const QString &tooltip)
{
    if (!sys) {
        return;
    }
#ifndef QT_NO_TOOLTIP
    sys->setToolTip(tooltip);
#endif
}

void QFlatpakSystemTrayIcon::updateMenu(QPlatformMenu *menu)
{
    if (!sys) {
        return;
    }

    if (SystemTrayMenu *ourMenu = qobject_cast<SystemTrayMenu*>(menu)) {
        sys->updateContextMenu(ourMenu->menu());
    }
}

QRect QFlatpakSystemTrayIcon::geometry() const
{
    if (!sys) {
        return QRect();
    }

    return sys->globalGeometry();
}

void QFlatpakSystemTrayIcon::showMessage(const QString &title, const QString &msg, const QIcon &icon, QPlatformSystemTrayIcon::MessageIcon iconType, int secs)
{
    if (!sys) {
        return;
    }

    QBalloonTip::showBalloon(icon, title, msg, sys->systemTrayIcon(), sys->globalGeometry().center(), secs);
}

bool QFlatpakSystemTrayIcon::isSystemTrayAvailable() const
{
    return locateSystemTray();
}

bool QFlatpakSystemTrayIcon::supportsMessages() const
{
    return true;
}

QPlatformMenu * QFlatpakSystemTrayIcon::createMenu() const
{
    return new SystemTrayMenu();
}

#include "qflatpaksystemtrayicon.moc"
