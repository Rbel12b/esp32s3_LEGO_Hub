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

Lpf2DeviceManager portA(&portA_IO);
Lpf2DeviceManager portB(&portB_IO);
Lpf2DeviceManager portC(&portC_IO);
Lpf2DeviceManager portD(&portD_IO);

#define initIOForPort(_port)                                        \
    port##_port##_IO.init(PORT_##_port##_ID_1, PORT_##_port##_ID_2, \
                          PORT_##_port##_ID_1, PORT_##_port##_ID_2, \
                          PORT_##_port##_PWM_1, PORT_##_port##_PWM_2, PORT_##_port##_PWM_UNIT, PORT_##_port##_PWM_TIMER, 1000);
void initPorts()
{
    initIOForPort(A);
    initIOForPort(B);
    // initIOForPort(C);
    initIOForPort(D);

    portA.init();
    portB.init();
    // portC.init();
    portD.init();
}

void updatePorts()
{
    portA.update();
    portB.update();
    // portC.update();
    portD.update();
}

void writePortCallback(byte port, byte value)
{
    if (port == (byte)ControlPlusHubPort::A && portA.device())
    {
        if (auto device = static_cast<BasicMotorControl *>(
                  portA.device()->getCapability(BasicMotor::CAP)))
        {
            device->setSpeed(value);
        }
    }
}

// Lpf2DeviceType lastType = Lpf2DeviceType::UNKNOWNDEVICE;

// bool useSensorReading = false;
// int sensorReading = 0;
// bool hasSensor = false;

// void handledevice(Lpf2Device *_device)
// {
//     if (auto device = static_cast<TechnicDistanceSensorControl *>(
//             _device->getCapability(TechnicDistanceSensor::CAP)))
//     {
//         float value = device->getDistance();

//         if (value > 25.0f)
//             value = 25.0f;

//         value = 25.0f - value;

//         if (value > 25.0f)
//             value = 0.0f;

//         value = map(value, 0.0f, 25.0f, 0, 100);
//         if (value > 100)
//             value = 100;
//         else if (value < 0)
//             value = 0;

//         device->setLight(value, value, value, value);
//         hasSensor = true;
//         sensorReading = static_cast<int>(value);
//     }
//     else if (auto device = static_cast<BasicMotorControl *>(
//                  _device->getCapability(BasicMotor::CAP)))
//     {
//         if (!useSensorReading)
//             device->setSpeed(-50);
//         else
//             device->setSpeed(sensorReading);
//     }
//     else if (auto device = static_cast<EncoderMotorControl *>(
//                  _device->getCapability(EncoderMotor::CAP)))
//     {
//         static bool started = false;
//         static auto lastDevice = _device;
//         if (lastDevice != _device)
//         {
//             started = false;
//             lastDevice = _device;
//         }

//         uint16_t pan = panValue();

//         if (!started)
//         {
//             device->moveToAbsPos(180, 100); // start PID once
//             started = true;
//             // device->setRelPos(0);
//             // device->moveToRelPos(-720, 100);
//         }
//         else
//         {
//             device->setAbsTarget(pan); // smoothly track
//             if (!device->isMovingToPos())
//             {
//                 device->setSpeed(0);
//             }
//         }
//     }
//     else if (auto device = static_cast<TechnicColorSensorControl *>(
//                  _device->getCapability(TechnicColorSensor::CAP)))
//     {
//         switch (device->getColorIdx())
//         {
//         case Lpf2ColorIDX::BLACK:
//             BuitlInRGB_setColor(0, 0, 0);
//             break;
//         case Lpf2ColorIDX::BLUE:
//             BuitlInRGB_setColor(0, 0, 50);
//             break;
//         case Lpf2ColorIDX::GREEN:
//             BuitlInRGB_setColor(0, 50, 0);
//             break;
//         case Lpf2ColorIDX::RED:
//             BuitlInRGB_setColor(50, 0, 0);
//             break;
//         case Lpf2ColorIDX::WHITE:
//             BuitlInRGB_setColor(50, 50, 50);
//             break;
//         case Lpf2ColorIDX::YELLOW:
//             BuitlInRGB_setColor(50, 50, 0);
//             break;
//         case Lpf2ColorIDX::ORANGE:
//             BuitlInRGB_setColor(50, 20, 0);
//             break;
//         case Lpf2ColorIDX::PURPLE:
//             BuitlInRGB_setColor(30, 0, 30);
//             break;
//         case Lpf2ColorIDX::PINK:
//             BuitlInRGB_setColor(50, 0, 20);
//             break;
//         case Lpf2ColorIDX::LIGHTBLUE:
//             BuitlInRGB_setColor(0, 20, 50);
//             break;
//         case Lpf2ColorIDX::CYAN:
//             BuitlInRGB_setColor(0, 50, 50);
//             break;

//         default:
//             BuitlInRGB_setColor(0, 0, 0);
//             break;
//         }
//     }
// }