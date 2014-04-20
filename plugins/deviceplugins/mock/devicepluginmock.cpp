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

#include "devicepluginmock.h"
#include "httpdaemon.h"

#include "plugin/device.h"
#include "devicemanager.h"

#include <QDebug>
#include <QStringList>

VendorId guhVendorId = VendorId("2062d64d-3232-433c-88bc-0d33c0ba2ba6");
DeviceClassId mockDeviceClassId = DeviceClassId("753f0d32-0468-4d08-82ed-1964aab03298");
DeviceClassId mockDeviceAutoClassId = DeviceClassId("ab4257b3-7548-47ee-9bd4-7dc3004fd197");
EventTypeId mockEvent1Id = EventTypeId("45bf3752-0fc6-46b9-89fd-ffd878b5b22b");
EventTypeId mockEvent2Id = EventTypeId("863d5920-b1cf-4eb9-88bd-8f7b8583b1cf");
StateTypeId mockIntStateId = StateTypeId("80baec19-54de-4948-ac46-31eabfaceb83");
StateTypeId mockBoolStateId = StateTypeId("9dd6a97c-dfd1-43dc-acbd-367932742310");
ActionTypeId mockAction1Id = ActionTypeId("dea0f4e1-65e3-4981-8eaa-2701c53a9185");
ActionTypeId mockAction2Id = ActionTypeId("defd3ed6-1a0d-400b-8879-a0202cf39935");

DevicePluginMock::DevicePluginMock()
{
}

QList<Vendor> DevicePluginMock::supportedVendors() const
{
    QList<Vendor> ret;
    Vendor guh(guhVendorId, "guh");
    ret.append(guh);
    return ret;
}

QList<DeviceClass> DevicePluginMock::supportedDevices() const
{
    QList<DeviceClass> ret;

    DeviceClass deviceClassMock(pluginId(), guhVendorId, mockDeviceClassId);
    deviceClassMock.setName("Mock Device");

    QList<ParamType> mockParams;
    ParamType portParam("httpport", QVariant::Int);
    mockParams.append(portParam);

    deviceClassMock.setParams(mockParams);

    QList<StateType> mockStates;

    StateType intState(mockIntStateId);
    intState.setName("Dummy int state");
    intState.setType(QVariant::Int);
    intState.setDefaultValue(10);
    mockStates.append(intState);

    StateType boolState(mockBoolStateId);
    boolState.setName("Dummy bool state");
    boolState.setType(QVariant::Int);
    boolState.setDefaultValue(false);
    mockStates.append(boolState);

    deviceClassMock.setStates(mockStates);

    QList<EventType> mockEvents;
    
    EventType event1(mockEvent1Id);
    event1.setName("Mock Event 1");
    mockEvents.append(event1);

    EventType event2(mockEvent2Id);
    event2.setName("Mock Event 2");
    mockEvents.append(event2);

    deviceClassMock.setEvents(mockEvents);

    QList<ActionType> mockActions;

    ActionType action1(mockAction1Id);
    action1.setName("Mock Action 1");
    mockActions.append(action1);

    ActionType action2(mockAction2Id);
    action2.setName("Mock Action 2");
    mockActions.append(action2);

    deviceClassMock.setActions(mockActions);

    ret.append(deviceClassMock);

    // Auto created mock device
    DeviceClass deviceClassMockAuto(pluginId(), guhVendorId, mockDeviceAutoClassId);
    deviceClassMockAuto.setName("Mock Device (Auto created)");
    deviceClassMockAuto.setCreateMethod(DeviceClass::CreateMethodAuto);

    mockParams.clear();
    deviceClassMockAuto.setParams(mockParams);
    deviceClassMockAuto.setStates(mockStates);
    deviceClassMockAuto.setEvents(mockEvents);
    deviceClassMockAuto.setActions(mockActions);

    ret.append(deviceClassMockAuto);

    return ret;
}

DeviceManager::HardwareResources DevicePluginMock::requiredHardware() const
{
    return DeviceManager::HardwareResourceTimer;
}

QString DevicePluginMock::pluginName() const
{
    return "Mock Devices";
}

PluginId DevicePluginMock::pluginId() const
{
    return PluginId("727a4a9a-c187-446f-aadf-f1b2220607d1");
}

bool DevicePluginMock::deviceCreated(Device *device)
{
    qDebug() << "Mockdevice created returning true" << device->paramValue("httpport").toInt();

    HttpDaemon *daemon = new HttpDaemon(device, this);
    m_daemons.insert(device, daemon);

    if (!daemon->isListening()) {
        qDebug() << "HTTP port opening failed.";
        return false;
    }

    connect(daemon, &HttpDaemon::triggerEvent, this, &DevicePluginMock::triggerEvent);
    connect(daemon, &HttpDaemon::setState, this, &DevicePluginMock::setState);

    return true;
}

void DevicePluginMock::deviceRemoved(Device *device)
{
    m_daemons.take(device)->deleteLater();
}

bool DevicePluginMock::configureAutoDevice(QList<Device *> loadedDevices, Device *device) const
{
    Q_ASSERT(device->deviceClassId() == mockDeviceAutoClassId);

    qDebug() << "checking loadedDevices" << loadedDevices.count();
    // We only want to have one auto mock device. So if there's already anything in loadedDevices, don't crearte a new one.
    if (loadedDevices.count() > 0) {
        return false;
    }

    device->setName("Mock Device (Auto created)");
    QList<Param> params;
    Param param("httpport", 4242);
    params.append(param);
    device->setParams(params);
    return true;
}

DeviceManager::DeviceError DevicePluginMock::executeAction(Device *device, const Action &action)
{
    qDebug() << "Should execute action" << action.actionTypeId();
    m_daemons.value(device)->actionExecuted(action.actionTypeId());
    return DeviceManager::DeviceErrorNoError;
}

void DevicePluginMock::setState(const StateTypeId &stateTypeId, const QVariant &value)
{
    HttpDaemon *daemon = qobject_cast<HttpDaemon*>(sender());
    if (!daemon) {
        return;
    }

    Device *device = m_daemons.key(daemon);
    device->setStateValue(stateTypeId, value);
}

void DevicePluginMock::triggerEvent(const EventTypeId &id)
{
    HttpDaemon *daemon = qobject_cast<HttpDaemon*>(sender());
    if (!daemon) {
        return;
    }

    Device *device = m_daemons.key(daemon);

    Event event(id, device->id());

    qDebug() << "Emitting event " << event.eventTypeId();
    emit emitEvent(event);
}
