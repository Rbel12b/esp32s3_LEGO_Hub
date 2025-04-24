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
    Lpf2Parser(Stream *serial, bool hwSerial) : m_serial(serial), buffer(256), hwSerial(hwSerial) {}

    void begin(int rxPin, int txPin);
    void update();

private:
    void sendACK(bool NACK = false);
    void resetChecksum();
    void computeChecksum(uint8_t b);
    uint8_t getChecksum() { return checksum; }
    void printMessage(const Lpf2Message &msg);

private:
    Stream *m_serial;
    HardwareSerial *m_hwSerial;
    SoftwareSerial *m_swSerial;
    std::vector<uint8_t> buffer;
    bool hwSerial;
    bool deviceInited = false;
    uint8_t checksum;

    void parseMessage(const Lpf2Message &raw);
};

#endif