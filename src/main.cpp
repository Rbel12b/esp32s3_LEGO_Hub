#include "config.h"
#include <Arduino.h>

#include "Board.h"
#include "BuiltInRGB.h"
#include "Ports.h"
#include "Utils.h"
#include "IMU.h"
#include "ExtSerialGPIO.h"

#include "Lpf2/HubEmulation.hpp"
#include "Lpf2/Hub.hpp"
#include "Lpf2/Virtual/Port.hpp"
#include "Lpf2/Virtual/Device.hpp"
#include "Lpf2/DeviceDescLib.hpp"
#include "Lpf2/Devices/ColorSensor.hpp"

Lpf2::HubEmulation vHub("Technic Hub", Lpf2::HubType::CONTROL_PLUS_HUB);

Lpf2::Virtual::Port vLEDPort;
Lpf2::Virtual::GenericDevice vLED(Lpf2::DeviceDescriptors::HUB_LED);

int vLEDWriteCallback(uint8_t mode, const std::vector<uint8_t> &data, void* userData);

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(981200);
    lpf2_log_printf("Booted.");

    Lpf2::DeviceRegistry::registerDefault();
    Lpf2::DeviceDescRegistry::registerDefault();

    BuitlInRGB_init();
    BuitlInRGB_setColor(0, 0, 0);

    gpio_set_pull_mode((gpio_num_t)I2C_SDA, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode((gpio_num_t)I2C_SCL, GPIO_PULLUP_ONLY);
    I2C_HW.begin(I2C_SDA, I2C_SCL, 100000);

    Ports_init();
    IMU_init(I2C_HW);

    vLED.setWriteDataCallback(vLEDWriteCallback);
    vLEDPort.attachDevice(&vLED);

    vHub.attachPort((Lpf2::PortNum)Lpf2::ControlPlusHubPort::A, &portA);
    vHub.attachPort((Lpf2::PortNum)Lpf2::ControlPlusHubPort::B, &portB);
    vHub.attachPort((Lpf2::PortNum)Lpf2::ControlPlusHubPort::C, &portC);
    vHub.attachPort((Lpf2::PortNum)Lpf2::ControlPlusHubPort::D, &portD);
    vHub.attachPort((Lpf2::PortNum)Lpf2::ControlPlusHubPort::LED, &vLEDPort);
    vHub.setUseBuiltInDevices(false); // We'll provide these
    vHub.start();
    vHub.setHubBatteryLevel(50);
    vHub.setHubBatteryType(Lpf2::BatteryType::NORMAL);
}

auto portALastDeviceType = Lpf2::DeviceType::UNKNOWNDEVICE;
size_t lastBlink = 0;
bool lastBlinkstate = false;

void loop()
{
    vTaskDelay(1);
    
    if (Serial.available()) {
        uint8_t c = Serial.read();
        if (c == 0x03) {
            // Ctrl+C received
            ESP.restart();
        }
    }

    Ports_update();
    IMU_update();

    vHub.update();

    if (!vHub.isSubscribed)
    {
        auto now = LPF2_GET_TIME();
        if (now - lastBlink >= 500)
        {
            lastBlink = now;
            lastBlinkstate = !lastBlinkstate;
            if (lastBlinkstate)
            {
                BuitlInRGB_setColor(150, 150, 150);
            }
            else
            {
                BuitlInRGB_setColor(0, 0, 0);
            }
        }
    }
}

int vLEDWriteCallback(uint8_t mode, const std::vector<uint8_t> &data, void* userData)
{
    if (mode == 0 && data.size() >= 1)
    {
        Lpf2::ColorIDX color = (Lpf2::ColorIDX)data[0];
        switch (color)
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
    return 0;
}