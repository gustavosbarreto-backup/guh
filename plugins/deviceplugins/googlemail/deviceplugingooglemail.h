/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  This file is part of guh.                                              *
 *                                                                         *
 *  Guh is free software: you can redistribute it and/or modify            *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, version 2 of the License.                *
 *                                                                         *
 *  Guh is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with guh. If not, see <http://www.gnu.org/licenses/>.            *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef DEVICEPLUGINGOOGLEMAIL_H
#define DEVICEPLUGINGOOGLEMAIL_H

#include "plugin/deviceplugin.h"
#include "smtpclient.h"

class DevicePluginGoogleMail : public DevicePlugin
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "guru.guh.DevicePlugin" FILE "deviceplugingooglemail.json")
    Q_INTERFACES(DevicePlugin)

public:
    explicit DevicePluginGoogleMail();
    ~DevicePluginGoogleMail();

    QList<Vendor> supportedVendors() const override;
    QList<DeviceClass> supportedDevices() const override;

    bool deviceCreated(Device *device) override;
    DeviceManager::HardwareResources requiredHardware() const override;
    DeviceManager::DeviceError executeAction(Device *device, const Action &action) override;

    QString pluginName() const override;
    PluginId pluginId() const override;

private:
    SmtpClient *m_smtpClient;

private slots:

public slots:


};

#endif // DEVICEPLUGINMAIL_H
