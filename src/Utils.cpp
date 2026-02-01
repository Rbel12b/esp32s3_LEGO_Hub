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