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

#ifndef QFLATPAK_PLATFORM_SERVICES_H
#define QFLATPAK_PLATFORM_SERVICES_H

#include <QUrl>
#include <qpa/qplatformservices.h>

class QFlatpakPlatformServices : public QPlatformServices
{
public:
    bool openUrl(const QUrl &url);
    bool openDocument(const QUrl &url);

};

#endif // QFLATPAK_PLATFORM_SERVICES_H
