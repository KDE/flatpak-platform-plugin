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

#ifndef QFLATPAK_INTEGRATION_H
#define QFLATPAK_INTEGRATION_H

#include <qpa/qplatformintegration.h>

class QFlatpakIntegration : public QPlatformIntegration
{
public:
    QFlatpakIntegration(const QStringList &parameters, int &argc, char **argv);
    ~QFlatpakIntegration();

    QPlatformWindow *createPlatformWindow(QWindow *window) const Q_DECL_OVERRIDE;
#ifndef QT_NO_OPENGL
    QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const Q_DECL_OVERRIDE;
#endif
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const Q_DECL_OVERRIDE;

    QPlatformOffscreenSurface *createPlatformOffscreenSurface(QOffscreenSurface *surface) const Q_DECL_OVERRIDE;

    bool hasCapability(Capability cap) const Q_DECL_OVERRIDE;
    QAbstractEventDispatcher *createEventDispatcher() const Q_DECL_OVERRIDE;
    void initialize() Q_DECL_OVERRIDE;

    QPlatformFontDatabase *fontDatabase() const Q_DECL_OVERRIDE;

    QPlatformNativeInterface *nativeInterface()const Q_DECL_OVERRIDE;

#ifndef QT_NO_CLIPBOARD
    QPlatformClipboard *clipboard() const Q_DECL_OVERRIDE;
#endif
#ifndef QT_NO_DRAGANDDROP
    QPlatformDrag *drag() const Q_DECL_OVERRIDE;
#endif

    QPlatformInputContext *inputContext() const Q_DECL_OVERRIDE;

// #ifndef QT_NO_ACCESSIBILITY
//     QPlatformAccessibility *accessibility() const Q_DECL_OVERRIDE;
// #endif

    QPlatformServices *services() const Q_DECL_OVERRIDE;

    Qt::KeyboardModifiers queryKeyboardModifiers() const Q_DECL_OVERRIDE;
    QList<int> possibleKeys(const QKeyEvent *e) const Q_DECL_OVERRIDE;

    QStringList themeNames() const Q_DECL_OVERRIDE;
    QPlatformTheme *createPlatformTheme(const QString &name) const Q_DECL_OVERRIDE;
    QVariant styleHint(StyleHint hint) const Q_DECL_OVERRIDE;

    static QFlatpakIntegration *instance() { return m_instance; }

    const QPlatformIntegration *loadedPlatformPlugin() { return m_platformIntegration; }
private:
    static QFlatpakIntegration *m_instance;

    QPlatformIntegration *m_platformIntegration;
};

#endif
