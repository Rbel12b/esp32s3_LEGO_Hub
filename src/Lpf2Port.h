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
        union mapping {
            uint8_t val;
            struct {
                uint8_t unused_0 : 1;     // Bit 0
                uint8_t unused_1 : 1;     // Bit 1
                uint8_t dis      : 1;     // Bit 2
                uint8_t rel      : 1;     // Bit 3
                uint8_t abs      : 1;     // Bit 4
                uint8_t unused_5 : 1;     // Bit 5
                uint8_t mapping2 : 1;     // Bit 6
                uint8_t nullSupport : 1;  // Bit 7
            };
        };
        mapping in, out;
        uint8_t data_sets, format, figures, decimals;
        std::vector<uint8_t> rawData;
        union __attribute__((packed)) {
            uint64_t val;
            struct {
                // Byte 0
                uint64_t speed     : 1;
                uint64_t apos      : 1;
                uint64_t pos       : 1;
                uint64_t unused_03 : 1;
                uint64_t power     : 1;
                uint64_t motor     : 1;
                uint64_t pin1      : 1;
                uint64_t pin2      : 1;
        
                // Byte 1
                uint64_t unused_10 : 1;
                uint64_t unused_11 : 1;
                uint64_t unused_12 : 1;
                uint64_t unused_13 : 1;
                uint64_t unused_14 : 1;
                uint64_t unused_15 : 1;
                uint64_t calib     : 1;
                uint64_t unused_17 : 1;
        
                // Byte 2
                uint64_t unused_20 : 8;
        
                // Byte 3
                uint64_t unused_30 : 8;
        
                // Byte 4
                uint64_t uses_power12 : 1;
                uint64_t unused_41     : 1;
                uint64_t unknown_42    : 1;
                uint64_t unused_43     : 1;
                uint64_t unused_44     : 1;
                uint64_t unused_45     : 1;
                uint64_t unused_46     : 1;
                uint64_t unused_47     : 1;
        
                // Byte 5
                uint64_t unknown_50    : 8;
            };
        } flags;
    };

    LPF2_STATUS m_status = LPF2_STATUS::STATUS_ERR;
    DeviceType m_deviceType = DeviceType::UNKNOWNDEVICE;
    uint8_t modes, views;
    std::vector<Mode> modeData;
    uint32_t baud = 2400;
    uint16_t modeCombos[16];
    uint8_t comboNum = 0;
    bool nextModeExt = false;

private:
    static void taskEntryPoint(void* pvParameters);
    
    void uartTask();
    void parseMessage(const Lpf2Message& msg);
    void parseMessageCMD(const Lpf2Message& msg);
    void parseMessageInfo(const Lpf2Message& msg);
    void changeBaud(uint32_t baud);
    void sendACK(bool NACK = false);

    static uint8_t getDataSize(uint8_t format);
    static ModeNum getDefaultMode(DeviceType id);
    static bool deviceIsAbsMotor(DeviceType id);

    void sendMessage(std::vector<uint8_t> msg);
    std::vector<uint8_t> makeMessage(uint8_t header, std::vector<uint8_t> msg);
    void setMode(ModeNum num);

private:
    uint8_t m_rxPin, m_txPin;
    int m_hwSerialNum;
    Stream *m_serial;
    HardwareSerial *m_hwSerial;
    SoftwareSerial *m_swSerial;
};

#endif