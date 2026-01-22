#include "config.h"
#include <Arduino.h>

#include "Manager/DeviceManager.h"
#include "Lpf2Devices/DistanceSensor.h"
#include "Lpf2Devices/ColorSensor.h"
#include "Lpf2Devices/BasicMotor.h"
#include "Lpf2Devices/EncoderMotor.h"
#include "Devices/esp32s3/device.h"

#include "Board.h"
#include "BuiltInRGB.h"

Esp32s3IO portA_IO(PORT_A_HWS);
Esp32s3IO portB_IO(PORT_B_HWS);

Lpf2DeviceManager portA(&portA_IO);
Lpf2DeviceManager portB(&portB_IO);

Lpf2Parser *brick = nullptr;
Lpf2Parser *sensor = nullptr;

void printModes(const Lpf2Port &port);

std::vector<uint8_t> data;

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned long startTime = 0;

int panValue()
{
    const unsigned long halfPeriod = 3000; // 3 seconds
    const unsigned long fullPeriod = 6000; // 6 seconds

    unsigned long now = millis();
    unsigned long t = (now - startTime) % fullPeriod;

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

void setup()
{
    heap_caps_check_integrity_all(true);
    Serial.begin(981200);
    data.resize(4);

    BuitlInRGB_init();
    BuitlInRGB_setColor(0, 10, 0);

    portA_IO.init(PORT_A_ID_1, PORT_A_ID_2,
        PORT_A_ID_1, PORT_A_ID_2,
        PORT_A_PWM_1, PORT_A_PWM_2, PORT_A_PWM_UNIT, PORT_A_PWM_TIMER, 1000);
    portB_IO.init(PORT_B_ID_1, PORT_B_ID_2,
        PORT_B_ID_1, PORT_B_ID_2,
        PORT_B_PWM_1, PORT_B_PWM_2, PORT_B_PWM_UNIT, PORT_B_PWM_TIMER, 1000);
    portA.init();
    portB.init();

    startTime = millis();
}

Lpf2DeviceType lastType = Lpf2DeviceType::UNKNOWNDEVICE;

bool useSensorReading = false;
int sensorReading = 0;

void loop()
{
    vTaskDelay(1);
    portA.update();
    // portB.update();

    bool hasSensor = false;
        
    if (portA.device())
    {
        if (auto device = static_cast<TechnicDistanceSensorControl*>(
            portA.device()->getCapability(TechnicDistanceSensor::CAP)))
        {
            BuitlInRGB_setColor(0, 10, 50); // set color to indicate that a distance sensor is connected
            float value = device->getDistance();

            if (value > 25.0f)
                value = 25.0f;

            value = 25.0f - value;

            if (value > 25.0f)
                value = 0.0f;

            value = map(value, 0.0f, 25.0f, 0, 100);
            if (value > 100)
                value = 100;
            else if (value < 0)
                value = 0;

            device->setLight(value, value, value, value);
            hasSensor = true;
            sensorReading = static_cast<int>(value);
        }
        else if (auto device = static_cast<BasicMotorControl*>(
            portA.device()->getCapability(BasicMotor::CAP)))
        {
            BuitlInRGB_setColor(50, 10, 0); // set color to indicate that a motor is connected
            if (!useSensorReading)
                device->setSpeed(-50);
            else
                device->setSpeed(sensorReading);
        }
        else if (auto device = static_cast<EncoderMotorControl*>(
            portA.device()->getCapability(EncoderMotor::CAP)))
        {
            BuitlInRGB_setColor(50, 0, 50); // set color to indicate that a smart motor is connected
            static int lastPan = -1;
            if (lastPan != panValue())
            {
                lastPan = panValue();
                device->moveToAbsPos(panValue(), 100);
            }
        }
        else if (auto device = static_cast<TechnicColorSensorControl*>(
            portA.device()->getCapability(TechnicColorSensor::CAP)))
        {
            switch (device->getColorIdx())
            {
            case Lpf2ColorIDX::BLACK:
                BuitlInRGB_setColor(0, 0, 0);
                break;
            case Lpf2ColorIDX::BLUE:
                BuitlInRGB_setColor(0, 0, 50);
                break;
            case Lpf2ColorIDX::GREEN:
                BuitlInRGB_setColor(0, 50, 0);
                break;
            case Lpf2ColorIDX::RED:
                BuitlInRGB_setColor(50, 0, 0);
                break;
            case Lpf2ColorIDX::WHITE:
                BuitlInRGB_setColor(50, 50, 50);
                break;
            case Lpf2ColorIDX::YELLOW:
                BuitlInRGB_setColor(50, 50, 0);
                break;
            case Lpf2ColorIDX::ORANGE:
                BuitlInRGB_setColor(50, 20, 0);
                break;
            case Lpf2ColorIDX::PURPLE:
                BuitlInRGB_setColor(30, 0, 30);
                break;
            case Lpf2ColorIDX::PINK:
                BuitlInRGB_setColor(50, 0, 20);
                break;
            case Lpf2ColorIDX::LIGHTBLUE:
                BuitlInRGB_setColor(0, 20, 50);
                break;
            case Lpf2ColorIDX::CYAN:
                BuitlInRGB_setColor(0, 50, 50);
                break;
            
            default:
                BuitlInRGB_setColor(10, 10, 10);
                break;
            }
        }
        else
        {
            BuitlInRGB_setColor(0, 10, 0);
        }
    }
    else
    {
        // if (portA.getDeviceType() != Lpf2DeviceType::UNKNOWNDEVICE)
        // {
        //     printModes(portA.getPort());
        // }
        BuitlInRGB_setColor(0, 10, 0);
    }

    useSensorReading = hasSensor;
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