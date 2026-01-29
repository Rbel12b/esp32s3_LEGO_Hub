#include "config.h"
#include <Arduino.h>

#include "Board.h"
#include "BuiltInRGB.h"
#include "Ports.h"
#include "Utils.h"

#include "Lpf2HubEmulation.h"
#include "Lpf2Hub.h"

extern "C" int serial_vprintf(const char *fmt, va_list args)
{
    BuitlInRGB_setColor(0, 10, 0);
    va_list copy;
    va_copy(copy, args);

    int len = vsnprintf(nullptr, 0, fmt, copy);
    va_end(copy);

    if (len <= 0)
        return 0;

    std::vector<char> buf(len + 1);
    vsnprintf(buf.data(), buf.size(), fmt, args);

    Serial.write((uint8_t *)buf.data(), len);
    return len;
}

Lpf2HubEmulation hub("Technic Hub", Lpf2HubType::CONTROL_PLUS_HUB);
Lpf2Hub lpf2Hub;

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(981200);
    lpf2_log_printf("booted.");

    Lpf2DeviceRegistry::registerDefaultFactories();

    esp_log_set_vprintf(serial_vprintf);

    BuitlInRGB_init();
    BuitlInRGB_setColor(0, 0, 10);

    initPorts();

    util_panStartTime = millis();

    hub.start();
    hub.setHubBatteryLevel(50);
    hub.setHubBatteryType(Lpf2BatteryType::NORMAL);

    hub.setWritePortCallback(writePortCallback);

    // lpf2Hub.init();
}

auto portALastDeviceType = Lpf2DeviceType::UNKNOWNDEVICE;
bool isSubscribed = false;

void loop()
{
    // vTaskDelay(1);

    // if (!lpf2Hub.isConnected() && !lpf2Hub.isConnecting())
    // {
    //     lpf2Hub.init();
    //     vTaskDelay(500);
    // }

    // // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
    // if (lpf2Hub.isConnecting())
    // {
    //     lpf2Hub.connectHub();
    //     if (lpf2Hub.isConnected())
    //     {
    //         Serial.println("Connected to HUB");
    //         Serial.print("Hub address: ");
    //         Serial.println(lpf2Hub.getHubAddress().toString().c_str());
    //         Serial.print("Hub name: ");
    //         Serial.println(lpf2Hub.getHubName().c_str());
    //         lpf2Hub.setLedColor(Lpf2Color::GREEN);
    //     }
    //     else
    //     {
    //         Serial.println("Failed to connect to HUB");
    //     }
    // }

    // if (lpf2Hub.isConnected())
    // {
    //     auto devices = lpf2Hub.getConnectedDevices();
    //     Serial.print("Number of connected devices: ");
    //     Serial.println(devices.size());
    //     for (auto device : devices)
    //     {
    //         Serial.print("Device on port ");
    //         Serial.print(device.PortNumber);
    //         Serial.print(" of type ");
    //         Serial.println(device.DeviceType);
    //     }
    // }
    updatePorts();
    if (hub.isSubscribed != isSubscribed)
    {
        isSubscribed = hub.isSubscribed;
        if (isSubscribed)
        {
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::LED, Lpf2DeviceType::HUB_LED);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::CURRENT, Lpf2DeviceType::CURRENT_SENSOR);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::VOLTAGE, Lpf2DeviceType::VOLTAGE_SENSOR);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::TEMP, Lpf2DeviceType::TECHNIC_MEDIUM_HUB_TEMPERATURE_SENSOR);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::TEMP2, Lpf2DeviceType::TECHNIC_MEDIUM_HUB_TEMPERATURE_SENSOR);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::ACCELEROMETER, Lpf2DeviceType::TECHNIC_MEDIUM_HUB_ACCELEROMETER);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::GYRO, Lpf2DeviceType::TECHNIC_MEDIUM_HUB_GYRO_SENSOR);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::TILT, Lpf2DeviceType::TECHNIC_MEDIUM_HUB_TILT_SENSOR);
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::GESTURE, Lpf2DeviceType::TECHNIC_MEDIUM_HUB_GEST_SENSOR);
            BuitlInRGB_setColor(0, 10, 0);
        }
        else
        {
            BuitlInRGB_setColor(10, 0, 0);
        }
    }
    {

    }
    if (portA.getDeviceType() != portALastDeviceType)
    {
        if (portA.getDeviceType() == Lpf2DeviceType::TECHNIC_LARGE_LINEAR_MOTOR)
        {
            ESP.restart(); // Soft reset, because the board does not have a hard reset button.
        }
        portALastDeviceType = portA.getDeviceType();
        if (portA.getDeviceType() == Lpf2DeviceType::UNKNOWNDEVICE)
        {
            hub.detachDevice((byte)Lpf2ControlPlusHubPort::A);
        }
        else
        {
            hub.attachDevice((byte)Lpf2ControlPlusHubPort::A, portA.getDeviceType());
        }
    }
}