/*
 * Copyright © 2016-2017 Red Hat, Inc
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

#include "qflatpakintegration.h"
#include "qflatpakplatformtheme.h"
#include "qflatpakservices.h"

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>
#include <qpa/qplatformintegrationfactory_p.h>

QFlatpakIntegration *QFlatpakIntegration::m_instance = Q_NULLPTR;

QFlatpakIntegration::QFlatpakIntegration(const QStringList &parameters, int &argc, char ** argv)
    : m_platformIntegration(Q_NULLPTR)
{
    m_instance = this;

    const QString platformPluginPath = QString::fromLocal8Bit(qgetenv("QT_QPA_PLATFORM_PLUGIN_PATH"));
    // Use user defined platform plugin
    QString platformPlugin = QString::fromLocal8Bit(qgetenv("QT_QPA_FLATPAK_PLATFORM"));
    if (!platformPlugin.isEmpty() && platformPlugin != QLatin1String("flatpak")) {
        m_platformIntegration = QPlatformIntegrationFactory::create(platformPlugin, parameters, argc, argv, platformPluginPath);
    } else {
        // Load xcb platform plugin by default
        m_platformIntegration = QPlatformIntegrationFactory::create(QLatin1String("xcb"), parameters, argc, argv, platformPluginPath);
        // TODO wayland by default when detected
    }
}

QFlatpakIntegration::~QFlatpakIntegration()
{
    m_instance = Q_NULLPTR;
}

QPlatformWindow *QFlatpakIntegration::createPlatformWindow(QWindow *window) const
{
    return m_platformIntegration->createPlatformWindow(window);
}

#ifndef QT_NO_OPENGL
QPlatformOpenGLContext *QFlatpakIntegration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    return m_platformIntegration->createPlatformOpenGLContext(context);
}
#endif

QPlatformBackingStore *QFlatpakIntegration::createPlatformBackingStore(QWindow *window) const
{
    return m_platformIntegration->createPlatformBackingStore(window);
}

QPlatformOffscreenSurface *QFlatpakIntegration::createPlatformOffscreenSurface(QOffscreenSurface *surface) const
{
    return m_platformIntegration->createPlatformOffscreenSurface(surface);
}

bool QFlatpakIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    return m_platformIntegration->hasCapability(cap);
}

QAbstractEventDispatcher *QFlatpakIntegration::createEventDispatcher() const
{
    return m_platformIntegration->createEventDispatcher();
}

void QFlatpakIntegration::initialize()
{
    return m_platformIntegration->initialize();
}

QPlatformFontDatabase *QFlatpakIntegration::fontDatabase() const
{
    return m_platformIntegration->fontDatabase();
}

QPlatformNativeInterface * QFlatpakIntegration::nativeInterface() const
{
    return m_platformIntegration->nativeInterface();
}

#ifndef QT_NO_CLIPBOARD
QPlatformClipboard *QFlatpakIntegration::clipboard() const
{
    return m_platformIntegration->clipboard();
}
#endif

#ifndef QT_NO_DRAGANDDROP
QPlatformDrag *QFlatpakIntegration::drag() const
{
    return m_platformIntegration->drag();
}
#endif

QPlatformInputContext *QFlatpakIntegration::inputContext() const
{
    return m_platformIntegration->inputContext();
}

// #ifndef QT_NO_ACCESSIBILITY
// QPlatformAccessibility *QFlatpakIntegration::accessibility() const
// {
//     return m_platformIntegration->accessibility();
// }
// #endif

QPlatformServices *QFlatpakIntegration::services() const
{
    return new QFlatpakServices;
}

Qt::KeyboardModifiers QFlatpakIntegration::queryKeyboardModifiers() const
{
    return m_platformIntegration->queryKeyboardModifiers();
}

QList<int> QFlatpakIntegration::possibleKeys(const QKeyEvent *e) const
{
    return m_platformIntegration->possibleKeys(e);
}

QStringList QFlatpakIntegration::themeNames() const
{
    return QStringList(QLatin1String("flatpak"));
}

QPlatformTheme *QFlatpakIntegration::createPlatformTheme(const QString &name) const
{
    if (name == QLatin1String("flatpak")) {
        return new QFlatpakPlatformTheme;
    }

    return m_platformIntegration->createPlatformTheme(name);
}

QVariant QFlatpakIntegration::styleHint(QPlatformIntegration::StyleHint hint) const
{
    return m_platformIntegration->styleHint(hint);
}
