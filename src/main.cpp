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
    // Serial.printf("Device: 0x%02X\n", port0.m_deviceType);
    // Serial.printf("Speed: %u\n", port0.baud);
    // Serial.printf("Combos:\n");
    // for (int i = 0; i < port0.comboNum; i++)
    // {
    //     Serial.printf("\t0x%04X", port0.modeCombos[i]);
    // }
    // for (int i = 0; i < port0.modes; i++)
    // {
    //     Serial.printf("Mode %i:\n", i);
    //     Serial.printf("\tname: %s\n", port0.modeData[i].name.c_str());
    //     Serial.printf("\tunit: %s\n", port0.modeData[i].unit.c_str());
    //     Serial.printf("\tmin: %f\n", port0.modeData[i].min);
    //     Serial.printf("\tmax: %f\n", port0.modeData[i].max);
    //     Serial.printf("\tSI min: %f\n", port0.modeData[i].SImin);
    //     Serial.printf("\tSI max: %f\n", port0.modeData[i].SImax);
    //     Serial.printf("\tnegative percentage: %s\n", port0.modeData[i].negativePCT ? "true": "false");
    //     auto in = port0.modeData[i].in;
    //     Serial.printf("\tin: (null: %i, mapping 2.0: %i, abs: %i, rel: %i, dis: %i)\n", in.nullSupport, in.mapping2, in.abs, in.rel, in.dis);
    //     auto out = port0.modeData[i].out;
    //     Serial.printf("\tout: (null: %i, mapping 2.0: %i, abs: %i, rel: %i, dis: %i)\n", out.nullSupport, out.mapping2, out.abs, out.rel, out.dis);
    // }
}
