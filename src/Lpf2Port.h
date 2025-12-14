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

    enum class LPF2_STATUS
    {
        /* Something bad happened. */
        STATUS_ERR,
        /* Waiting for data that looks like LEGO UART protocol. */
        STATUS_SYNCING,
        /* Reading device info before changing baud rate. */
        STATUS_INFO,
        /* Waiting for ACK */
        STATUS_ACK_WAIT,
        /* Waiting for SYNC */
        STATUS_SYNC_WAIT,
        /* Sending ACK, for data phase begin */
        STATUS_ACK_SENDING,
        /* Sending speed change request */
        STATUS_SPEED_CHANGE,
        /* Speed change accepted */
        STATUS_SPEED,
        /* Waiting for first data packet */
        STATUS_DATA_RECEIVED,
        /* First data packet received, sending setup data */
        STATUS_DATA_START,
        /* Normal data receiving state */
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
        struct Mapping
        {
            uint8_t val;

            bool nullSupport() const { return val & (1 << 7); }
            bool mapping2() const { return val & (1 << 6); }
            bool abs() const { return val & (1 << 4); }
            bool rel() const { return val & (1 << 3); }
            bool dis() const { return val & (1 << 2); }
        };
        Mapping in, out;
        uint8_t data_sets = 0, format = 0, figures = 0, decimals = 0;
        std::vector<uint8_t> rawData;
        struct Flags
        {
            uint8_t bytes[6] = {0, 0, 0, 0, 0, 0};

            bool speed() const { return bytes[0] & (1 << 0); }
            bool apos() const { return bytes[0] & (1 << 1); }
            bool pos() const { return bytes[0] & (1 << 2); }
            bool power() const { return bytes[0] & (1 << 4); }
            bool motor() const { return bytes[0] & (1 << 5); }
            bool pin1() const { return bytes[0] & (1 << 6); }
            bool pin2() const { return bytes[0] & (1 << 7); }

            bool calib() const { return bytes[1] & (1 << 6); }

            bool power12() const { return bytes[4] & (1 << 0); }
        };
        Flags flags;
    };

    static float getValue(const Mode &modeData, uint8_t dataSet);
    float getValue(uint8_t modeNum, uint8_t dataSet) const;
    static std::string formatValue(float value, const Mode &modeData);
    static std::string convertValue(Mode modeData);
    std::string convertValue(uint8_t modeNum) const
    {
        if (modeNum >= modeData.size())
        {
            return "<mode not found>";
        }
        return convertValue(modeData[modeNum]);
    }

    int writeData(uint8_t modeNum, std::vector<uint8_t> data);

    DeviceType getDeviceType() const { return m_deviceType; }
    size_t getSpeed() const { return baud; }
    uint8_t getModeCount() const { return modes; }
    uint8_t getViewCount() const { return views; }
    const std::vector<Mode>& getModes() const { return modeData; }
    uint8_t getModeComboCount() const { return comboNum; }

    uint16_t getModeCombo(uint8_t combo) const
    {
        if (combo >= comboNum || combo >= 16)
            return 0;
        return modeCombos[combo];
    }

    bool deviceConnected();

    std::vector<Mode> modeData;

private:
    LPF2_STATUS m_status = LPF2_STATUS::STATUS_ERR;
    LPF2_STATUS m_new_status = LPF2_STATUS::STATUS_ERR;
    DeviceType m_deviceType = DeviceType::UNKNOWNDEVICE;
    uint8_t modes, views;
    uint32_t baud = 2400;
    uint16_t modeCombos[16];
    uint8_t comboNum = 0;
    bool nextModeExt = false;

private:
    static void taskEntryPoint(void *pvParameters);

    void uartTask();
    void parseMessage(const Lpf2Message &msg);
    void parseMessageCMD(const Lpf2Message &msg);
    void parseMessageInfo(const Lpf2Message &msg);
    void changeBaud(uint32_t baud);
    void sendACK(bool NACK = false);

    static uint8_t getDataSize(uint8_t format);
    static ModeNum getDefaultMode(DeviceType id);
    static bool deviceIsAbsMotor(DeviceType id);

    void setMode(uint8_t num);
    void requestSpeedChange(uint32_t speed);

    void resetDevice();

    uint8_t process(unsigned long &start, unsigned long now);

    /// Parse a signed 8-bit integer from raw bytes
    static float parseData8(const uint8_t *ptr);

    /// Parse a signed 16-bit little-endian integer from raw bytes
    static float parseData16(const uint8_t *ptr);

    /// Parse a signed 32-bit little-endian integer from raw bytes
    static float parseData32(const uint8_t *ptr);

    /// Parse a 32-bit IEEE-754 little-endian float
    static float parseDataF(const uint8_t *ptr);

private:
    uint8_t m_rxPin, m_txPin;
    int m_hwSerialNum;
    Stream *m_serial;
    HardwareSerial *m_hwSerial;
    SoftwareSerial *m_swSerial;
    xQueueHandle m_serialMutex;

    /**
     * Time of the last data received (millis since startup).
     */
    uint64_t m_timeStart = 0;
};

#endif