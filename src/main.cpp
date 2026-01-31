#include "config.h"
#include <Arduino.h>

#include "Board.h"
#include "BuiltInRGB.h"
#include "Ports.h"
#include "Utils.h"

#include "Lpf2HubEmulation.h"
#include "Lpf2Hub.h"
#include "Lpf2Virtual/Lpf2PortVirtual.h"
#include "Lpf2Virtual/VirtualDevices/Hud_led.h"

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

Lpf2PortVirtual vPort;
Lpf2VirtualHubLed hubLed;

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

    hub.attachPort((Lpf2PortNum)Lpf2ControlPlusHubPort::LED, &vPort);
    hub.attachPort((Lpf2PortNum)Lpf2ControlPlusHubPort::A, &portA);

    hub.start();
    hub.setHubBatteryLevel(50);
    hub.setHubBatteryType(Lpf2BatteryType::NORMAL);
}

auto portALastDeviceType = Lpf2DeviceType::UNKNOWNDEVICE;
bool isSubscribed = false;

void loop()
{
    vTaskDelay(1);

    updatePorts();
    hub.update();

    if (hub.isSubscribed != isSubscribed)
    {
        isSubscribed = hub.isSubscribed;
        if (isSubscribed)
        {
            vPort.attachDevice(&hubLed);
            BuitlInRGB_setColor(0, 10, 0);
        }
        else
        {
            BuitlInRGB_setColor(10, 0, 0);
        }
    }

    // a way to reset
    if (portA.getDeviceType() == Lpf2DeviceType::TECHNIC_LARGE_LINEAR_MOTOR)
    {
        ESP.restart();
    }
}