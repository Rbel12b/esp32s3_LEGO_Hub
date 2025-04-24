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
    // put your main code here, to run repeatedly:
}
