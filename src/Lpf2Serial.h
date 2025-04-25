#pragma once
#ifndef _LPF2_SERIAL_H_
#define _LPF2_SERIAL_H_

#include <Arduino.h>
#include <vector>
#include <Stream.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

struct Lpf2Message
{
    uint8_t header;
    uint8_t length;
    std::vector<uint8_t> data;
    uint8_t checksum;
    uint8_t msg;
    uint8_t cmd;
    bool system = false;
};

class Lpf2Parser
{
public:
    Lpf2Parser(Stream *serial) : m_serial(serial), buffer(256) {}

    void begin(int rxPin, int txPin);
    std::vector<Lpf2Message> update();

private:
    void resetChecksum();
    void computeChecksum(uint8_t b);
    uint8_t getChecksum() { return checksum; }
    void printMessage(const Lpf2Message &msg);

private:
    Stream *m_serial;
    std::vector<uint8_t> buffer;
    bool hwSerial;
    uint8_t checksum;
};

#endif