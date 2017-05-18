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

#include "qflatpakservices.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QLoggingCategory>
#include <QVariantMap>
#include <QUrl>

Q_LOGGING_CATEGORY(QFlatpakPlatformServices, "qt.qpa.qflatpakplatform.Services")

bool QFlatpakServices::openUrl(const QUrl &url)
{
    qCDebug(QFlatpakPlatformServices) << "Open url: " << url;

    if (url.scheme() == QLatin1String("mailto")) {
        QUrlQuery urlQuery(url);
        QVariantMap options;
        options.insert(QLatin1String("address"), url.path());
        options.insert(QLatin1String("subject"), urlQuery.queryItemValue(QLatin1String("subject")));
        options.insert(QLatin1String("body"), urlQuery.queryItemValue(QLatin1String("body")));
        // TODO attachements

        QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                              QLatin1String("/org/freedesktop/portal/desktop"),
                                                              QLatin1String("org.freedesktop.portal.Email"),
                                                              QLatin1String("ComposeEmail"));

        // TODO get parent window id??
        QString parentWindowId = QLatin1String("x11:")/* + QString::number(parent->winId())*/;

        message << parentWindowId << options;

        QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
        watcher->waitForFinished(); // TODO sorry for a blocking call, but we cannot do it asynchronous here
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qCDebug(QFlatpakPlatformServices) << "Couldn't get reply";
            return false;
        }
    } else {
        QDBusMessage message = QDBusMessage::createMethodCall(QLatin1String("org.freedesktop.portal.Desktop"),
                                                              QLatin1String("/org/freedesktop/portal/desktop"),
                                                              QLatin1String("org.freedesktop.portal.OpenURI"),
                                                              QLatin1String("OpenURI"));

        // TODO get parent window id??
        QString parentWindowId = QLatin1String("x11:")/* + QString::number(parent->winId())*/;
        QVariantMap options; // TODO: handle "writable" option

        message << parentWindowId << url.toDisplayString() << options;

        QDBusPendingCall pendingCall = QDBusConnection::sessionBus().asyncCall(message);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pendingCall);
        watcher->waitForFinished(); // TODO sorry for a blocking call, but we cannot do it asynchronous here
        QDBusPendingReply<QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qCDebug(QFlatpakPlatformServices) << "Couldn't get reply";
            return false;
        }
    }

    return true;
}

bool QFlatpakServices::openDocument(const QUrl &url)
{
    return QPlatformServices::openDocument(url);
}
