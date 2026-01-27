#pragma once
#ifndef _PORTS_H_
#define _PORTS_H_

#include "Lpf2DeviceManager.h"
#include "Devices/esp32s3/device.h"
#include "Lpf2HubEmulation.h"

extern Esp32s3IO portA_IO;
extern Esp32s3IO portB_IO;
extern Esp32s3IO portC_IO;
extern Esp32s3IO portD_IO;

extern Lpf2DeviceManager portA;
extern Lpf2DeviceManager portB;
extern Lpf2DeviceManager portC;
extern Lpf2DeviceManager portD;

void initPorts();
void updatePorts();

void writePortCallback(byte port, byte value);

#endif