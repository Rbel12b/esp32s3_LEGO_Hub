#pragma once
#ifndef _LEGO_HUB_UTILS_H_
#define _LEGO_HUB_UTILS_H_

#include "Lpf2Port.h"

void printModes(const Lpf2Port &port);
float map(float x, float in_min, float in_max, float out_min, float out_max);

extern unsigned long util_panStartTime;
int panValue();

#endif // _LEGO_HUB_UTILS_H_