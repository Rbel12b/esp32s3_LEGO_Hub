#pragma once
#include "UART.h"

#ifdef ESP32
#include <HardwareSerial.h>

class Esp32Uart;

using Lpf2UartPort = Esp32Uart;

class Esp32Uart : public UartPort {
public:
    explicit Esp32Uart(int uart_num)
        : serial_(uart_num) {}

    bool begin(uint32_t baudrate, uint32_t config = SERIAL_8N1, int rx_pin = -1, int tx_pin = -1) override {
        serial_.begin(baudrate, config, rx_pin, tx_pin);
        return true;
    }

    void end() override {
        serial_.end();
    }

    void setBaudrate(uint32_t baudrate) override {
        serial_.updateBaudRate(baudrate);
    }

    size_t write(const uint8_t* data, size_t length) override {
        return serial_.write(data, length);
    }

    int read() override {
        return serial_.read();
    }

    size_t available() override {
        return serial_.available();
    }

    void flush() override {
        serial_.flush();
    }

private:
    HardwareSerial serial_;
};

#endif
