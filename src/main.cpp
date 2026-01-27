#include "config.h"
#include <Arduino.h>

#include "Board.h"
#include "BuiltInRGB.h"
#include "Ports.h"
#include "Utils.h"

#include "Lpf2HubEmulation.h"

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

Lpf2HubEmulation hub("Technic Hub", HubType::CONTROL_PLUS_HUB);

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(981200);

    esp_log_set_vprintf(serial_vprintf);

    BuitlInRGB_init();
    BuitlInRGB_setColor(0, 0, 10);

    initPorts();

    util_panStartTime = millis();

    hub.start();
    hub.setHubBatteryLevel(50);
    hub.setHubBatteryType(BatteryType::NORMAL);

    hub.setWritePortCallback(writePortCallback);
}

auto portALastDeviceType = Lpf2DeviceType::UNKNOWNDEVICE;
bool isConnected = false;

void loop()
{
    vTaskDelay(1);
    updatePorts();
    if (hub.isConnected != isConnected)
    {
        isConnected = hub.isConnected;
        if (isConnected)
        {
            portALastDeviceType = (Lpf2DeviceType)-1;
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
            hub.detachDevice((byte)ControlPlusHubPort::A);
        }
        else
        {
            hub.attachDevice((byte)ControlPlusHubPort::A, (DeviceType)portA.getDeviceType());
        }
    }
}