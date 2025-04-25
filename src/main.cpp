#include <Arduino.h>
#include <HardwareSerial.h>
#include "Lpf2Port.h"

Lpf2Port port0(14, 13, 1);

void setup()
{
    Serial.begin(115200);
    port0.init();
}

void loop()
{
    vTaskDelay(1000);
    log_i("Device: 0x%02X", port0.m_deviceType);
    log_i("Speed: %u", port0.baud);
    for (int i = 0; i < port0.modes; i++)
    {
        log_i("Mode %i: %s", i, port0.modeData[i].name.c_str());
    }
}
