#include <Arduino.h>
#include <HardwareSerial.h>
#include "Lpf2Port.h"
#include <cstring>

Lpf2Port port0(14, 13, 1);

Lpf2Parser* parser = nullptr;

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(921600);
    port0.init();
    // Serial2.begin(2400, SERIAL_8N1, 12, 11);
    // parser = new Lpf2Parser(&Serial2); // For debugging output from the Lpf2Port class
}

void loop()
{
    // auto msgs = parser->update();
    // for (const auto& msg : msgs)
    // {
    //     std::lock_guard<std::mutex> lock(serialMutex);
    //     Serial.printf("out: ");
    //     parser->printMessage(msg);
    //     Serial.flush();
    // }
    // return;


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
    //     Serial.printf("\tData sets: %i\n", port0.modeData[i].data_sets);
    //     Serial.printf("\tformat: 0x%02X\n", port0.modeData[i].format);
    //     Serial.printf("\tFigures: %i\n", port0.modeData[i].figures);
    //     Serial.printf("\tDecimals: %i\n", port0.modeData[i].decimals);
    //     Serial.printf("\tnegative percentage: %s\n", port0.modeData[i].negativePCT ? "true": "false");
    //     auto in = port0.modeData[i].in;
    //     Serial.printf("\tin: 0x%02X (null: %i, mapping 2.0: %i, abs: %i, rel: %i, dis: %i)\n",
    //         in.val, in.nullSupport(), in.mapping2(), in.abs(), in.rel(), in.dis());
    //     auto out = port0.modeData[i].out;
    //     Serial.printf("\tout: 0x%02X (null: %i, mapping 2.0: %i, abs: %i, rel: %i, dis: %i)\n",
    //         out.val, out.nullSupport(), out.mapping2(), out.abs(), out.rel(), out.dis());
    //     auto flags = port0.modeData[i].flags;
    //     uint64_t val;
    //     std::memcpy(&val, flags.bytes, 6);
    //     Serial.printf("\tFlags: 0x%012X (speed: %i, apos: %i, power: %i, motor: %i, pin1: %i, pin2: %i, calib: %i, power12: %i)\n",
    //         val, flags.speed(), flags.apos(), flags.power(), flags.motor(), flags.pin1(), flags.pin2(), flags.calib(), flags.power12());
    //     Serial.print("\tRaw:");
    //     for (int n = 0; n < port0.modeData[i].rawData.size(); n++)
    //     {
    //         Serial.printf(" 0x%02X", port0.modeData[i].rawData[n]);
    //     }
    //     Serial.printf("\n");
    // }
        
    log_i("Distance: %s", port0.convertValue(0).c_str());

    vTaskDelay(500 / portTICK_PERIOD_MS);
}
