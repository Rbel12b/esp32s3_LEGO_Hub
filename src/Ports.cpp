#include "Ports.h"
#include "Board.h"
#include "Lpf2/Devices/DistanceSensor.hpp"
#include "Lpf2/Devices/ColorSensor.hpp"
#include "Lpf2/Devices/BasicMotor.hpp"
#include "Lpf2/Devices/EncoderMotor.hpp"
#include "Utils.h"

Esp32s3IO portA_IO(PORT_A_HWS);
Esp32s3IO portB_IO(PORT_B_HWS);
Esp32s3IO portC_IO(PORT_C_HWS);
Esp32s3IO portD_IO(PORT_D_HWS);

Lpf2::Local::Port portA(&portA_IO);
Lpf2::Local::Port portB(&portB_IO);
Lpf2::Local::Port portC(&portC_IO);
Lpf2::Local::Port portD(&portD_IO);

#define initIOForPort(_port)                                        \
    port##_port##_IO.init(PORT_##_port##_ID_1, PORT_##_port##_ID_2, \
                          PORT_##_port##_ID_1, PORT_##_port##_ID_2, \
                          PORT_##_port##_PWM_1, PORT_##_port##_PWM_2, PORT_##_port##_PWM_UNIT, PORT_##_port##_PWM_TIMER, 1000);
void initPorts()
{
    initIOForPort(A);
    initIOForPort(B);
    initIOForPort(C);
    initIOForPort(D);

    portA.init();
    portB.init();
    portC.init();
    portD.init();
}

void updatePorts()
{
    portA.update();
    portB.update();
    portC.update();
    portD.update();
}