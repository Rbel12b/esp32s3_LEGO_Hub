#pragma once
#ifndef _PORTS_H_
#define _PORTS_H_

#include "Lpf2DeviceManager.h"
#include "Devices/esp32s3/device.h"
#include "Lpf2HubEmulation.h"
#include "Lpf2Local/Lpf2PortLocal.h"

extern Esp32s3IO portA_IO;
extern Esp32s3IO portB_IO;
extern Esp32s3IO portC_IO;
extern Esp32s3IO portD_IO;

extern Lpf2PortLocal portA;
extern Lpf2PortLocal portB;
extern Lpf2PortLocal portC;
extern Lpf2PortLocal portD;

// extern Lpf2DeviceManager portA_Manager;
// extern Lpf2DeviceManager portB_Manager;
// extern Lpf2DeviceManager portC_Manager;
// extern Lpf2DeviceManager portD_Manager;


void initPorts();
void updatePorts();

#endif