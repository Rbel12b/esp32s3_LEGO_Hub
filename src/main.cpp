#include "config.h"
#include <Arduino.h>

#include "Board.h"
#include "BuiltInRGB.h"
#include "Ports.h"
#include "Utils.h"

#include "Lpf2/HubEmulation.hpp"
#include "Lpf2/Hub.hpp"
#include "Lpf2/Virtual/Port.hpp"
#include "Lpf2/Virtual/Device.hpp"
#include "Lpf2/DeviceDescLib.hpp"
#include "Lpf2/Devices/ColorSensor.hpp"

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

Lpf2::HubEmulation vHub("Technic Hub", Lpf2::HubType::CONTROL_PLUS_HUB);
Lpf2::Hub hub;

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(981200);
    lpf2_log_printf("booted.");

    Lpf2::DeviceRegistry::registerDefault();
    Lpf2::DeviceDescRegistry::registerDefault();

    esp_log_set_vprintf(serial_vprintf);

    BuitlInRGB_init();
    BuitlInRGB_setColor(0, 0, 10);

    initPorts();

    util_panStartTime = millis();

#if EMULATE_HUB == 1
    vHub.attachPort((Lpf2PortNum)Lpf2::ControlPlusHubPort::A, &portA);
    vHub.start();
    vHub.setHubBatteryLevel(50);
    vHub.setHubBatteryType(Lpf2::BatteryType::NORMAL);
#else
    hub.init();
#endif
}

auto portALastDeviceType = Lpf2::DeviceType::UNKNOWNDEVICE;
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
    if (portA.getDeviceType() == Lpf2::DeviceType::TECHNIC_LARGE_LINEAR_MOTOR)
    {
        ESP.restart();
    }
#else
    if (!hub.isConnected() && !hub.isConnecting())
    {
        hub.init();
        vTaskDelay(500);
    }

    // connect flow. Search for BLE services and try to connect if the uuid of the hub is found
    if (hub.isConnecting())
    {
        hub.connectHub();
        if (hub.isConnected())
        {
            Serial.println("Connected to HUB");
        }
        else
        {
            Serial.println("Failed to connect to HUB");
        }
    }

    if (hub.isConnected())
    {
        hub.update();
        static bool printedInfos = false;
        if (!printedInfos && hub.infoReady())
        {
            auto s = hub.getAllInfoStr();
            for (size_t i = 0; i < s.size(); i += 128)
            {
                Serial.write(s.data() + i, std::min<size_t>(128, s.size() - i));
                delay(1); // let other tasks breathe
            }
            printedInfos = true;
        }

        auto &r_portA = *hub.getPort(Lpf2::PortNum(Lpf2::ControlPlusHubPort::A));
        if (r_portA.deviceConnected())
        {
            static bool portASetupDone = false;
            if (!portASetupDone)
            {
                r_portA.setMode(0);
                portASetupDone = true;
            }

            if (r_portA.getDeviceType() == Lpf2::DeviceType::TECHNIC_COLOR_SENSOR)
            {
                auto device = new Lpf2::Devices::TechnicColorSensor(r_portA);
    
                switch (device->getColorIdx())
                {
                case Lpf2::ColorIDX::BLACK:
                    BuitlInRGB_setColor(0, 0, 0);
                    break;
                case Lpf2::ColorIDX::BLUE:
                    BuitlInRGB_setColor(0, 0, 50);
                    break;
                case Lpf2::ColorIDX::GREEN:
                    BuitlInRGB_setColor(0, 50, 0);
                    break;
                case Lpf2::ColorIDX::RED:
                    BuitlInRGB_setColor(50, 0, 0);
                    break;
                case Lpf2::ColorIDX::WHITE:
                    BuitlInRGB_setColor(50, 50, 50);
                    break;
                case Lpf2::ColorIDX::YELLOW:
                    BuitlInRGB_setColor(50, 50, 0);
                    break;
                case Lpf2::ColorIDX::ORANGE:
                    BuitlInRGB_setColor(50, 20, 0);
                    break;
                case Lpf2::ColorIDX::PURPLE:
                    BuitlInRGB_setColor(30, 0, 30);
                    break;
                case Lpf2::ColorIDX::PINK:
                    BuitlInRGB_setColor(50, 0, 20);
                    break;
                case Lpf2::ColorIDX::LIGHTBLUE:
                    BuitlInRGB_setColor(0, 20, 50);
                    break;
                case Lpf2::ColorIDX::CYAN:
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