#pragma once
#ifndef _LPF2_SERIAL_H_
#define _LPF2_SERIAL_H_

#include <Arduino.h>
#include <vector>
#include <Stream.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

#ifndef LPF2_LOG_LEVEL
#define LPF2_LOG_LEVEL 2
#endif

#if LPF2_LOG_LEVEL > 0
#define LPF2_LOG_E log_e
#define LPF2_DEBUG_EXPR_E(...) \
    do                  \
    {                   \
        __VA_ARGS__;    \
    } while (0)
#else
#define LPF2_LOG_E(...) \
    do                  \
    {                   \
    } while (0)
#define LPF2_DEBUG_EXPR_E(...) \
    do                  \
    {                   \
    } while (0)
#endif
#if LPF2_LOG_LEVEL > 1
#define LPF2_LOG_W log_w
#define LPF2_DEBUG_EXPR_W(...) \
    do                  \
    {                   \
        __VA_ARGS__;    \
    } while (0)
#else
#define LPF2_LOG_W(...) \
    do                  \
    {                   \
    } while (0)
#define LPF2_DEBUG_EXPR_W(...) \
    do                  \
    {                   \
    } while (0)
#endif
#if LPF2_LOG_LEVEL > 2
#define LPF2_LOG_I log_i
#define LPF2_DEBUG_EXPR_I(...) \
    do                  \
    {                   \
        __VA_ARGS__;    \
    } while (0)
#else
#define LPF2_LOG_I(...) \
    do                  \
    {                   \
    } while (0)
#define LPF2_DEBUG_EXPR_I(...) \
    do                  \
    {                   \
    } while (0)
#endif
#if LPF2_LOG_LEVEL > 3
#define LPF2_LOG_D log_d
#define LPF2_DEBUG_EXPR_D(...) \
    do                  \
    {                   \
        __VA_ARGS__;    \
    } while (0)
#else
#define LPF2_LOG_D(...) \
    do                  \
    {                   \
    } while (0)
#define LPF2_DEBUG_EXPR_D(...) \
    do                  \
    {                   \
    } while (0)
#endif
#if LPF2_LOG_LEVEL > 4
#define LPF2_LOG_V log_v
#define LPF2_DEBUG_EXPR_V(...) \
    do                  \
    {                   \
        __VA_ARGS__;    \
    } while (0)
#else
#define LPF2_LOG_V(...) \
    do                  \
    {                   \
    } while (0)
#define LPF2_DEBUG_EXPR_V(...) \
    do                  \
    {                   \
    } while (0)
#endif

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
    
    std::vector<Lpf2Message> update();
    void printMessage(const Lpf2Message &msg);

private:
    void resetChecksum();
    void computeChecksum(uint8_t b);
    uint8_t getChecksum() { return checksum; }

private:
    Stream *m_serial;
    std::vector<uint8_t> buffer;
    bool hwSerial;
    uint8_t checksum;
};

#endif