#include "config.h"
#include <Arduino.h>
#include "Lpf2Port.h"

#include "Devices/esp32s3/device.h"

Esp32s3IO io(1);

Lpf2Port port0(&io);

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

    io.getUart()->begin(115200, SERIAL_8N1, 12, 11);
    port0.init();

    data.resize(4);
}

void loop()
{
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
    printf("Device: 0x%02X\n", (unsigned int)port.getDeviceType());
    printf("Speed: %u\n", port.getSpeed());
    printf("Combos:\n");
    for (int i = 0; i < port.getModeComboCount(); i++)
    {
        printf("\t0x%04X", port.getModeCombo(i));
    }
    for (int i = 0; i < port.getModeCount(); i++)
    {
        auto &mode = port.getModes()[i];

        printf("Mode %i:\n", i);
        printf("\tname: %s\n", mode.name.c_str());
        printf("\tunit: %s\n", mode.unit.c_str());
        printf("\tmin: %f\n", (double)mode.min);
        printf("\tmax: %f\n", (double)mode.max);
        printf("\tSI min: %f\n", (double)mode.SImin);
        printf("\tSI max: %f\n", (double)mode.SImax);
        printf("\tData sets: %i\n", mode.data_sets);
        printf("\tformat: 0x%02X\n", mode.format);
        printf("\tFigures: %i\n", mode.figures);
        printf("\tDecimals: %i\n", mode.decimals);
        printf("\tnegative percentage: %s\n", mode.negativePCT ? "true" : "false");
        auto in = mode.in;
        printf("\tin: 0x%02X (null: %i, mapping 2.0: %i, m_abs: %i, m_rel: %i, m_dis: %i)\n",
               in.val, in.nullSupport(), in.mapping2(), in.m_abs(), in.m_rel(), in.m_dis());
        auto out = mode.out;
        printf("\tout: 0x%02X (null: %i, mapping 2.0: %i, m_abs: %i, m_rel: %i, m_dis: %i)\n",
               out.val, out.nullSupport(), out.mapping2(), out.m_abs(), out.m_rel(), out.m_dis());
        auto flags = mode.flags;
        uint64_t val;
        memcpy(&val, flags.bytes, 6);
        printf("\tFlags: 0x%012llX (speed: %i, apos: %i, power: %i, motor: %i, pin1: %i, pin2: %i, calib: %i, power12: %i)\n",
               val, flags.speed(), flags.apos(), flags.power(), flags.motor(), flags.pin1(), flags.pin2(), flags.calib(), flags.power12());
        printf("\tRaw:");
        for (int n = 0; n < 128; n++)
        {
            printf(" 0x%02X", mode.rawData[n]);
        }
        printf("\n");
    }
}