#include "Ports.h"
#include "Board.h"
#include "Lpf2Devices/DistanceSensor.h"
#include "Lpf2Devices/ColorSensor.h"
#include "Lpf2Devices/BasicMotor.h"
#include "Lpf2Devices/EncoderMotor.h"
#include "Utils.h"

Esp32s3IO portA_IO(PORT_A_HWS);
Esp32s3IO portB_IO(PORT_B_HWS);
Esp32s3IO portC_IO(PORT_C_HWS);
Esp32s3IO portD_IO(PORT_D_HWS);

Lpf2PortLocal portA(&portA_IO);
Lpf2PortLocal portB(&portB_IO);
Lpf2PortLocal portC(&portC_IO);
Lpf2PortLocal portD(&portD_IO);

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