#pragma once
#ifndef _LPF2_PORT_H_
#define _LPF2_PORT_H_

#include <Arduino.h>
#include <Stream.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include "Lpf2Const.h"
#include "Lpf2Serial.h"
#include "Lpf2SerialDef.h"

class Lpf2Port
{
public:
    Lpf2Port(uint8_t rxPin, uint8_t txPin, int hwSerialNum) : m_rxPin(rxPin), m_txPin(txPin), m_hwSerialNum(hwSerialNum) {};

    void init();

    enum class LPF2_STATUS{
        /* Something bad happened. */
        STATUS_ERR,
        /* Waiting for data that looks like LEGO UART protocol. */
        STATUS_SYNCING,
        /* Reading device info before changing baud rate. */
        STATUS_INFO,
        /* info received, waiting for ack. */
        STATUS_WAIT_ACK,
        /* ACK received, delay changing baud rate. */
        STATUS_ACK,
        /* Ready to send commands and receive data. */
        STATUS_DATA,
    };

    class Mode
    {
    public:
        std::string name;
        float min = 0.0f, max = 1023.0f;
        float SImin = 0.0f, SImax = 1023.0f;
        bool negativePCT = false;
        std::string unit;
        union mapping
        {
            struct
            {
                uint8_t nullSupport : 1;
                uint8_t  : 1;
                uint8_t mapping2 : 1;
                uint8_t abs : 1;
                uint8_t rel : 1;
                uint8_t dis : 1;
                uint8_t  : 2;
            };
            uint8_t val;
        };
        mapping in, out;
        uint8_t data_sets, format, figures, decimals;
    };

    LPF2_STATUS m_status = LPF2_STATUS::STATUS_INFO;
    DeviceType m_deviceType = DeviceType::UNKNOWNDEVICE;
    uint8_t modes, views;
    std::vector<Mode> modeData;
    unsigned int baud = 2400;
    uint16_t modeCombos[16];
    uint8_t comboNum = 0;

private:
    static void taskEntryPoint(void* pvParameters);
    void uartTask();
    void parseMessage(const Lpf2Message& msg);
    void parseMessageCMD(const Lpf2Message& msg);
    void parseMessageInfo(const Lpf2Message& msg);

private:
    uint8_t m_rxPin, m_txPin;
    int m_hwSerialNum;
    Stream *m_serial;
    HardwareSerial *m_hwSerial;
    SoftwareSerial *m_swSerial;
};

#endif