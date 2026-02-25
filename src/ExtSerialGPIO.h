#pragma once

#include <Arduino.h>
#include "Lpf2/Local/IO/IO.hpp"
#include "Wire.h"
#include "SC16IS750.h"
#include "Devices/esp32s3/device.h"

class ExtSerialGPIO;

class Lpf2ExtSerial : public Lpf2::Local::Uart
{
public:
    Lpf2ExtSerial(ExtSerialGPIO *parent);

    /**
     * id1 and id2 pins must be adc inputs on the host mcu, with 47K pull-ups
     * @param tx_disable_pin the #OE pin of the TX output buffer, connected to one of the SC16IS750's GPIOs
     */
    void init(int id1_pin, int id2_pin, int tx_disable_pin);

    void end() override;
    void setBaudrate(uint32_t baudrate) override;
    size_t write(const uint8_t *data, size_t length) override;
    int read() override;
    int available() override;
    void flush() override;
    void setUartPinsState(bool highZ) override;

    float readCh(uint8_t ch) override
    {
        int pin = -1;

        if (ch == 0)
        {
            pin = id1_pin_;
        }
        else
        {
            pin = id2_pin_;
        }

        if (pin < 0)
        {
            return 0.0f;
        }

        // ESP32 ADC: 12-bit by default (0–4095), reference ~3.3V
        constexpr float VREF = 3.3f;
        int raw = analogRead(pin);
        return (raw / 4095.0f) * VREF;
    }

private:
    ExtSerialGPIO *m_parent;

    int id1_pin_ = -1;
    int id2_pin_ = -1;

    int txd_pin_ = -1;

    uint32_t baud_ = 115200;
    bool m_uartPinState = true; // true = high impedance, false = active
};

class ExtSerialGPIO
{
private:
    TwoWire &m_WirePort;

public:
    ExtSerialGPIO(TwoWire &WirePort, uint32_t crystal_freq, uint8_t sc16is750_addr = SC16IS750_ADDRESS_00);
    ~ExtSerialGPIO();

    void begin();

    SC16IS750 sc;
    uint32_t crystal_freq;
};