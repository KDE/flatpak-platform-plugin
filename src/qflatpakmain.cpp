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

#include <qpa/qplatformintegrationplugin.h>

#include "qflatpakintegration.h"

class QFlatpakIntegrationPlugin : public QPlatformIntegrationPlugin
{
   Q_OBJECT
   Q_PLUGIN_METADATA(IID QPlatformIntegrationFactoryInterface_iid FILE "flatpak.json")
public:
    QPlatformIntegration *create(const QString&, const QStringList&, int &, char **) Q_DECL_OVERRIDE;
};

QPlatformIntegration* QFlatpakIntegrationPlugin::create(const QString& system, const QStringList& parameters, int &argc, char **argv)
{
    if (!system.compare(QLatin1String("flatpak"), Qt::CaseInsensitive))
        return new QFlatpakIntegration(parameters, argc, argv);

    return 0;
}

#include "qflatpakmain.moc"
