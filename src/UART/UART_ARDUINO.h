#pragma once
#include "UART.h"

#if defined(ARDUINO_ARCH_AVR) | defined(ARDUINO_ARCH_STM32)
#include <Arduino.h>

class ArduinoUart;

using Lpf2UartPort = ArduinoUart;

class ArduinoUart : public UartPort {
public:
    explicit ArduinoUart(HardwareSerial& serial)
        : serial_(serial) {}

    bool begin(uint32_t baudrate, uint32_t config = SERIAL_8N1, int = -1, int = -1) override {
        serial_.begin(baudrate, config);
        return true;
    }

    void end() override {
        serial_.end();
    }

    void setBaudrate(uint32_t baudrate) override {
        serial_.begin(baudrate); // AVR has no dynamic baud change
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
    HardwareSerial& serial_;
};

#endif
