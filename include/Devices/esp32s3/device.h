#pragma once
#include "config.h"
#include "UART/UART.h"
#include "IO/IO.h"

#ifdef ESP32

#include <Arduino.h>
#include <HardwareSerial.h>

class Esp32s3Uart;
class Esp32s3PWM;

using Lpf2UartPort = Esp32s3Uart;
using Lpf2PwmPort = Esp32s3PWM;

class Esp32s3Uart : public Lpf2Uart
{
public:
    explicit Esp32s3Uart(int uart_num)
        : serial_(uart_num)
    {}

    bool begin(uint32_t baudrate,
               uint32_t config = SERIAL_8N1,
               int rx_pin = -1,
               int tx_pin = -1,
               int id1_pin = -1,
               int id2_pin = -1) override
    {
        rx_pin_  = rx_pin;
        tx_pin_  = tx_pin;
        id1_pin_ = id1_pin;
        id2_pin_ = id2_pin;
        config_  = config;
        baud_    = baudrate;

        // Configure ADC pins if provided
        if (id1_pin_ >= 0) {
            pinMode(id1_pin_, INPUT);
        }
        if (id2_pin_ >= 0) {
            pinMode(id2_pin_, INPUT);
        }

        uartPinsOff();

        return true;
    }

    void end() override
    {
        serial_.end();
        uartPinsOff();
    }

    void setBaudrate(uint32_t baudrate) override
    {
        baud_ = baudrate;
        serial_.updateBaudRate(baudrate);
    }

    size_t write(const uint8_t *data, size_t length) override
    {
        return serial_.write(data, length);
    }

    int read() override
    {
        return serial_.read();
    }

    size_t available() override
    {
        return serial_.available();
    }

    void flush() override
    {
        serial_.flush();
    }

    void setUartPinsState(bool highZ) override
    {
        if (highZ) {
            // Detach UART pins → high impedance
            serial_.end();
            if (rx_pin_ >= 0) pinMode(rx_pin_, INPUT);
            if (tx_pin_ >= 0) pinMode(tx_pin_, INPUT);
        } else {
            // Reattach UART pins
            if (rx_pin_ >= 0 || tx_pin_ >= 0) {
                serial_.end();
                serial_.begin(baud_, config_, rx_pin_, tx_pin_);
            }
        }
    }

    float readCh(uint8_t ch) override
    {
        int pin = -1;

        if (ch == 0) {
            pin = id1_pin_;
        } else {
            pin = id2_pin_;
        }

        if (pin < 0) {
            return 0.0f;
        }

        // ESP32 ADC: 12-bit by default (0–4095), reference ~3.3V
        constexpr float VREF = 3.3f;
        int raw = analogRead(pin);
        return (raw / 4095.0f) * VREF;
    }

private:
    HardwareSerial serial_;

    int rx_pin_  = -1;
    int tx_pin_  = -1;
    int id1_pin_ = -1;
    int id2_pin_ = -1;

    uint32_t baud_   = 115200;
    uint32_t config_ = SERIAL_8N1;
};

class Esp32s3PWM : public Lpf2PWM
{
public:
    explicit Esp32s3PWM() = default;
    int init(int pin1, int pin2, uint32_t freq = 1000, uint8_t resolution = 8, uint8_t channel1 = 0, uint8_t channel2 = 1)
    {
        pin1_ = pin1;
        pin2_ = pin2;
        freq_ = freq;
        resolution_ = resolution;
        channel1_ = channel1;
        channel2_ = channel2;

        ledcSetup(channel1_, freq_, resolution_);
        ledcAttachPin(pin1_, channel1_);

        ledcSetup(channel2_, freq_, resolution_);
        ledcAttachPin(pin2_, channel2_);

        return 0;
    }

    void out(uint8_t ch1, uint8_t ch2) override
    {
        ledcWrite(channel1_, ch1);
        ledcWrite(channel2_, ch2);
    }

private:
    int pin1_ = -1;
    int pin2_ = -1;
    uint8_t channel1_ = 0;
    uint8_t channel2_ = 1;
    uint32_t freq_ = 1000; // 1 kHz
    uint8_t resolution_ = 8; // 8-bit resolution
};

class Esp32s3IO : public Lpf2IO
{
public:
    Esp32s3IO(int uartNum) : m_uart(uartNum) {}

    Lpf2PWM* getPWM() override
    {
        return &m_pwm;
    }

    Lpf2Uart* getUart() override
    {
        return &m_uart;
    }


    /**
     * @brief Initialize the IO ports.
     * @param rx_pin The RX (receive) pin number. Use -1 for default.
     * @param tx_pin The TX (transmit) pin number. Use -1 for default.
     * @param id1_pin The analog input connected to the TX pin, external 47k pull-up is required. (Can be the same as tx_pin)
     * @param id2_pin The analog input connected to the RX pin, external 47k pull-up is required. (Can be the same as rx_pin)
     * @param pwm_pin1 The first PWM output pin. Use -1 for no PWM.
     * @param pwm_pin2 The second PWM output pin. Use -1 for no PWM.
     * @param freq The PWM frequency in Hz.
     * @param resolution The PWM resolution in bits.
     * @param channel1 The PWM channel for the first pin.
     * @param channel2 The PWM channel for the second pin.
     * @return 0 if initialization was successful, -1 otherwise.
     * @note Must be called before any IO operations. (Before calling init on Lpf2Port or update on Lpf2DeviceManager)
     */
    int init(int rx_pin = -1, int tx_pin = -1, int id1_pin = -1, int id2_pin = -1, int pwm_pin1 = -1, int pwm_pin2 = -1, uint32_t freq = 1000, uint8_t channel1 = 0, uint8_t channel2 = 1)
    {
        if (!m_uart.begin(115200, SERIAL_8N1, rx_pin, tx_pin, id1_pin, id2_pin)) {
            return -1;
        }
        if (m_pwm.init(pwm_pin1, pwm_pin2, freq, 8, channel1, channel2) != 0) {
            return -1;
        }
        m_inited = true;
        return 0;
    }

    bool ready() const override
    {
        return m_inited;
    }
private:
    Esp32s3Uart m_uart;
    Esp32s3PWM m_pwm;
    bool m_inited = false;
};

#endif // ESP32
