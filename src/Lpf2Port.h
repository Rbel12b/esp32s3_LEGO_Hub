#pragma once
#ifndef _LPF2_PORT_H_
#define _LPF2_PORT_H_

#include <Arduino.h>
#include <Stream.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>
#include "Lpf2Const.h"

class Lpf2Port
{
public:
    Lpf2Port(uint8_t rxPin, uint8_t txPin, int hwSerialNum) : m_rxPin(rxPin), m_txPin(txPin), m_hwSerialNum(hwSerialNum) {};

    void init();

private:
    static void taskEntryPoint(void* pvParameters);
    void uartTask();

private:
    uint8_t m_rxPin, m_txPin;
    int m_hwSerialNum;
    Stream *m_serial;
    HardwareSerial *m_hwSerial;
    SoftwareSerial *m_swSerial;

    enum class LPF2_STATUS{
        /* Something bad happened. */
        STATUS_ERR,
        /* Waiting for data that looks like LEGO UART protocol. */
        STATUS_SYNCING,
        /* Reading device info before changing baud rate. */
        STATUS_INFO,
        /* ACK received, delay changing baud rate. */
        STATUS_ACK,
        /* Ready to send commands and receive data. */
        STATUS_DATA,
    };

    LPF2_STATUS m_status = LPF2_STATUS::STATUS_INFO;
    DeviceType m_deviceType = DeviceType::UNKNOWNDEVICE;
};

#endif