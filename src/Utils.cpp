#include "Utils.h"
#include "Arduino.h"

unsigned long util_panStartTime = 0;

int panValue()
{
    const unsigned long halfPeriod = 3000; // 3 seconds
    const unsigned long fullPeriod = 6000; // 6 seconds

    unsigned long now = millis();
    unsigned long t = (now - util_panStartTime) % fullPeriod;

    if (t < halfPeriod)
    {
        // 0 -> 180
        return map(t, 0l, halfPeriod, 0l, 180l);
    }
    else
    {
        // 180 -> 0
        return map(t - halfPeriod, 0l, halfPeriod, 180l, 0l);
    }
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
    for (int i = 0; i < port.getModes().size(); i++)
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
        for (int n = 0; n < mode.rawData.size(); n++)
        {
            printf(" 0x%02X", mode.rawData[n]);
        }
        printf("\n");
    }
}