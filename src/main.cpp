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

    util_panStartTime = millis();

    vHub.attachPort((Lpf2::PortNum)Lpf2::ControlPlusHubPort::A, &portA);
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