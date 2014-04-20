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

#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include "plugin/deviceclass.h"
#include "plugin/device.h"
#include "plugin/devicedescriptor.h"

#include "types/event.h"
#include "types/action.h"
#include "types/vendor.h"

#include <QObject>
#include <QTimer>

class Device;
class DevicePlugin;
class Radio433;

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    enum HardwareResource {
        HardwareResourceNone = 0x00,
        HardwareResourceRadio433 = 0x01,
        HardwareResourceRadio868 = 0x02,
        HardwareResourceTimer = 0x04
    };
    Q_DECLARE_FLAGS(HardwareResources, HardwareResource)

    enum DeviceError {
        DeviceErrorNoError,
        DeviceErrorDeviceNotFound,
        DeviceErrorDeviceClassNotFound,
        DeviceErrorActionTypeNotFound,
        DeviceErrorMissingParameter,
        DeviceErrorPluginNotFound,
        DeviceErrorSetupFailed,
        DeviceErrorDuplicateUuid,
        DeviceErrorCreationMethodNotSupported,
        DeviceErrorDeviceParameterError,
        DeviceErrorActionParameterError,
        DeviceErrorDeviceDescriptorNotFound
    };

    explicit DeviceManager(QObject *parent = 0);

    QList<DevicePlugin*> plugins() const;
    DevicePlugin* plugin(const PluginId &id) const;
    void setPluginConfig(const PluginId &pluginId, const QVariantMap &pluginConfig);

    QList<Vendor> supportedVendors() const;
    QList<DeviceClass> supportedDevices(const VendorId &vendorId = VendorId()) const;
    DeviceError discoverDevices(const DeviceClassId &deviceClassId, const QVariantMap &params) const;

    QList<Device*> configuredDevices() const;
    DeviceError addConfiguredDevice(const DeviceClassId &deviceClassId, const QList<Param> &params, const DeviceId id = DeviceId::createDeviceId());
    DeviceError addConfiguredDevice(const DeviceClassId &deviceClassId, const DeviceDescriptorId &deviceDescriptorId, const DeviceId &id = DeviceId::createDeviceId());
    DeviceError removeConfiguredDevice(const DeviceId &deviceId);

    Device* findConfiguredDevice(const DeviceId &id) const;
    QList<Device*> findConfiguredDevices(const DeviceClassId &deviceClassId) const;
    DeviceClass findDeviceClass(const DeviceClassId &deviceClassId) const;

signals:
    void loaded();
    void emitEvent(const Event &event);
    void deviceStateChanged(Device *device, const QUuid &stateTypeId, const QVariant &value);
    void devicesDiscovered(const DeviceClassId &deviceClassId, const QList<DeviceDescriptor> &devices);

public slots:
    DeviceError executeAction(const Action &action);

private slots:
    void loadPlugins();
    void loadConfiguredDevices();
    void storeConfiguredDevices();
    void createNewAutoDevices();
    void slotDevicesDiscovered(const DeviceClassId &deviceClassId, const QList<DeviceDescriptor> deviceDescriptors);

    // Only connect this to Devices. It will query the sender()
    void slotDeviceStateValueChanged(const QUuid &stateTypeId, const QVariant &value);

    void radio433SignalReceived(QList<int> rawData);
    void timerEvent();

private:
    DeviceError addConfiguredDeviceInternal(const DeviceClassId &deviceClassId, const QList<Param> &params, const DeviceId id = DeviceId::createDeviceId());
    bool setupDevice(Device *device);

    QHash<VendorId, Vendor> m_supportedVendors;
    QHash<VendorId, QList<DeviceClassId> > m_vendorDeviceMap;
    QHash<DeviceClassId, DeviceClass> m_supportedDevices;
    QList<Device*> m_configuredDevices;
    QHash<DeviceClassId, QHash<DeviceDescriptorId, DeviceDescriptor> > m_discoveredDevices;

    QHash<PluginId, DevicePlugin*> m_devicePlugins;

    // Hardware Resources
    Radio433* m_radio433;
    QTimer m_pluginTimer;
    QList<Device*> m_pluginTimerUsers;

    friend class DevicePlugin;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(DeviceManager::HardwareResources)

#endif // DEVICEMANAGER_H
