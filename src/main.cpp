#include "config.h"
#include <Arduino.h>
#include <cstring>
#include "Lpf2Port.h"

Lpf2Port port0(14, 13, 1);

Lpf2Parser *brick = nullptr;
Lpf2Parser *sensor = nullptr;

void printModes(const Lpf2Port &port);

std::vector<uint8_t> data;

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(921600);
    port0.init();

    data.resize(4);
    // size_t baud = 2400;
    // Serial1.begin(baud, SERIAL_8N1, 14, 13);
    // Serial2.begin(baud, SERIAL_8N1, 12, 11);

    // brick = new Lpf2Parser(&Serial2);
    // sensor = new Lpf2Parser(&Serial1);
}

void loop()
{
    // auto brick_msgs = brick->update();
    // auto sensor_msgs = sensor->update();
    // for (size_t i = 0; i < std::max(brick_msgs.size(), sensor_msgs.size()); i++)
    // {
    //     if (i < brick_msgs.size())
    //     {
    //         Serial.printf("brick:  ");
    //         brick->printMessage(brick_msgs[i]);
    //     }
    //     if (i < sensor_msgs.size())
    //     {
    //         Serial.printf("sensor: ");
    //         sensor->printMessage(sensor_msgs[i]);
    //     }
    // }
    vTaskDelay(1);
    // return;

    if (!port0.deviceConnected())
        return;

    float value = port0.getValue(0, 0);

    if (value > 25.0f)
        value = 25.0f;

    value = 25.0f - value;

    if (value > 25.0f)
        value = 0.0f;

    const int LIGHT_MODE = 5;
    if (LIGHT_MODE >= port0.getModes().size())
        return;

    auto &mode = port0.getModes()[LIGHT_MODE];

    value = map(value, 0.0f, 25.0f, mode.SImin, mode.SImax);

    if (value > mode.SImax)
        value = mode.SImax;
    else if (value < mode.SImin)
        value = mode.SImin;

    for (uint8_t i = 0; i < mode.data_sets; i++)
    {
        data[i] = value;
    }

    port0.writeData(LIGHT_MODE, data);
    
    // printModes(port0);
    // log_i("Distance: %s", port0.convertValue(0).c_str());
}

void printModes(const Lpf2Port &port)
{
    Serial.printf("Device: 0x%02X\n", port.getDeviceType());
    Serial.printf("Speed: %u\n", port.getSpeed());
    Serial.printf("Combos:\n");
    for (int i = 0; i < port.getModeComboCount(); i++)
    {
        Serial.printf("\t0x%04X", port.getModeCombo(i));
    }
    for (int i = 0; i < port.getModes().size(); i++)
    {
        auto &mode = port.getModes()[i];

        Serial.printf("Mode %i:\n", i);
        Serial.printf("\tname: %s\n", mode.name.c_str());
        Serial.printf("\tunit: %s\n", mode.unit.c_str());
        Serial.printf("\tmin: %f\n", mode.min);
        Serial.printf("\tmax: %f\n", mode.max);
        Serial.printf("\tSI min: %f\n", mode.SImin);
        Serial.printf("\tSI max: %f\n", mode.SImax);
        Serial.printf("\tData sets: %i\n", mode.data_sets);
        Serial.printf("\tformat: 0x%02X\n", mode.format);
        Serial.printf("\tFigures: %i\n", mode.figures);
        Serial.printf("\tDecimals: %i\n", mode.decimals);
        Serial.printf("\tnegative percentage: %s\n", mode.negativePCT ? "true" : "false");
        auto in = mode.in;
        Serial.printf("\tin: 0x%02X (null: %i, mapping 2.0: %i, abs: %i, rel: %i, dis: %i)\n",
                      in.val, in.nullSupport(), in.mapping2(), in.abs(), in.rel(), in.dis());
        auto out = mode.out;
        Serial.printf("\tout: 0x%02X (null: %i, mapping 2.0: %i, abs: %i, rel: %i, dis: %i)\n",
                      out.val, out.nullSupport(), out.mapping2(), out.abs(), out.rel(), out.dis());
        auto flags = mode.flags;
        uint64_t val;
        std::memcpy(&val, flags.bytes, 6);
        Serial.printf("\tFlags: 0x%012X (speed: %i, apos: %i, power: %i, motor: %i, pin1: %i, pin2: %i, calib: %i, power12: %i)\n",
                      val, flags.speed(), flags.apos(), flags.power(), flags.motor(), flags.pin1(), flags.pin2(), flags.calib(), flags.power12());
        Serial.print("\tRaw:");
        for (int n = 0; n < mode.rawData.size(); n++)
        {
            Serial.printf(" 0x%02X", mode.rawData[n]);
        }
        Serial.printf("\n");
    }
}