#include "config.h"
#include <Arduino.h>

#include "Board.h"
#include "BuiltInRGB.h"
#include "Ports.h"
#include "Utils.h"

#include "Lpf2HubEmulation.h"
#include "Lpf2Hub.h"
#include "Lpf2Virtual/Lpf2PortVirtual.h"
#include "Lpf2Virtual/Lpf2VirtualDevice.h"
#include "Lpf2DeviceDescLib.h"
#include "Lpf2Devices/ColorSensor.h"

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

Lpf2HubEmulation vHub("Technic Hub", Lpf2HubType::CONTROL_PLUS_HUB);
Lpf2Hub realHub;

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(981200);
    lpf2_log_printf("booted.");

    Lpf2DeviceRegistry::registerDefault();
    Lpf2DeviceDescRegistry::registerDefault();

    esp_log_set_vprintf(serial_vprintf);

    BuitlInRGB_init();
    BuitlInRGB_setColor(0, 0, 10);

    initPorts();

    util_panStartTime = millis();

#if EMULATE_HUB == 1
    vHub.attachPort((Lpf2PortNum)Lpf2ControlPlusHubPort::A, &portA);
    vHub.start();
    vHub.setHubBatteryLevel(50);
    vHub.setHubBatteryType(Lpf2BatteryType::NORMAL);
#else
    realHub.init();
#endif
}

auto portALastDeviceType = Lpf2DeviceType::UNKNOWNDEVICE;
bool isSubscribed = false;

void loop()
{
    vTaskDelay(1);

    updatePorts();
    if (portB.deviceConnected())
    {
        auto s = portB.getInfoStr();
        for (size_t i = 0; i < s.size(); i += 128)
        {
            Serial.write(s.data() + i, std::min<size_t>(128, s.size() - i));
            delay(1); // let other tasks breathe
        }
    }
#if EMULATE_HUB == 1
    vHub.update();

    if (vHub.isSubscribed != isSubscribed)
    {
        isSubscribed = vHub.isSubscribed;
        if (isSubscribed)
        {
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
#else
    if (!realHub.isConnected() && !realHub.isConnecting())
    {
        realHub.init();
        vTaskDelay(500);
    }

    // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
    if (realHub.isConnecting())
    {
        realHub.connectHub();
        if (realHub.isConnected())
        {
            Serial.println("Connected to HUB");
        }
        else
        {
            Serial.println("Failed to connect to HUB");
        }
    }

    if (realHub.isConnected())
    {
        realHub.update();
        static bool printedInfos = false;
        if (!printedInfos && realHub.infoReady())
        {
            auto s = realHub.getAllInfoStr();
            for (size_t i = 0; i < s.size(); i += 128)
            {
                Serial.write(s.data() + i, std::min<size_t>(128, s.size() - i));
                delay(1); // let other tasks breathe
            }
            printedInfos = true;
        }

        auto &r_portA = *realHub.getPort(Lpf2PortNum(Lpf2ControlPlusHubPort::A));
        if (r_portA.deviceConnected())
        {
            static bool portASetupDone = false;
            if (!portASetupDone)
            {
                r_portA.setMode(0);
                portASetupDone = true;
            }

            if (r_portA.getDeviceType() == Lpf2DeviceType::TECHNIC_COLOR_SENSOR)
            {
                auto device = new TechnicColorSensor(r_portA);
    
                switch (device->getColorIdx())
                {
                case Lpf2ColorIDX::BLACK:
                    BuitlInRGB_setColor(0, 0, 0);
                    break;
                case Lpf2ColorIDX::BLUE:
                    BuitlInRGB_setColor(0, 0, 50);
                    break;
                case Lpf2ColorIDX::GREEN:
                    BuitlInRGB_setColor(0, 50, 0);
                    break;
                case Lpf2ColorIDX::RED:
                    BuitlInRGB_setColor(50, 0, 0);
                    break;
                case Lpf2ColorIDX::WHITE:
                    BuitlInRGB_setColor(50, 50, 50);
                    break;
                case Lpf2ColorIDX::YELLOW:
                    BuitlInRGB_setColor(50, 50, 0);
                    break;
                case Lpf2ColorIDX::ORANGE:
                    BuitlInRGB_setColor(50, 20, 0);
                    break;
                case Lpf2ColorIDX::PURPLE:
                    BuitlInRGB_setColor(30, 0, 30);
                    break;
                case Lpf2ColorIDX::PINK:
                    BuitlInRGB_setColor(50, 0, 20);
                    break;
                case Lpf2ColorIDX::LIGHTBLUE:
                    BuitlInRGB_setColor(0, 20, 50);
                    break;
                case Lpf2ColorIDX::CYAN:
                    BuitlInRGB_setColor(0, 50, 50);
                    break;
    
                default:
                    BuitlInRGB_setColor(0, 0, 0);
                    break;
                }
            }
        }
    }
#endif
}