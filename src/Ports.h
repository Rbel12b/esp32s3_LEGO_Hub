#pragma once
#ifndef _PORTS_H_
#define _PORTS_H_

#include "Lpf2/DeviceManager.hpp"
#include "Devices/esp32s3/device.h"
#include "Lpf2/HubEmulation.hpp"
#include "Lpf2/Local/Port.hpp"

extern Esp32s3IO portA_IO;
extern Esp32s3IO portB_IO;
extern Esp32s3IO portC_IO;
extern Esp32s3IO portD_IO;

extern Lpf2::Local::Port portA;
extern Lpf2::Local::Port portB;
extern Lpf2::Local::Port portC;
extern Lpf2::Local::Port portD;

void initPorts();
void updatePorts();

#endif