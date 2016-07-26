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

#include "qflatpakintegration.h"
#include "qflatpaktheme.h"

QFlatpakIntegration *QFlatpakIntegration::m_instance = Q_NULLPTR;

QFlatpakIntegration::QFlatpakIntegration(const QStringList &parameters, int &argc, char ** argv)
{
    m_instance = this;
}

QFlatpakIntegration::~QFlatpakIntegration()
{
    m_instance = Q_NULLPTR;
}

QPlatformTheme * QFlatpakIntegration::createPlatformTheme(const QString &name) const
{
    Q_UNUSED(name)

    return new QFlatpakTheme;
}

QPlatformWindow * QFlatpakIntegration::createPlatformWindow(QWindow* window) const
{
    return 0;
}

QPlatformBackingStore * QFlatpakIntegration::createPlatformBackingStore(QWindow* window) const
{
    return 0;
}

QAbstractEventDispatcher * QFlatpakIntegration::createEventDispatcher() const
{
    return 0;
}
