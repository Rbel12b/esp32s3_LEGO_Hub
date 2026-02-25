#include "ExtSerialGPIO.h"

ExtSerialGPIO::ExtSerialGPIO(TwoWire &WirePort, uint32_t crystal_freq, uint8_t sc16is750_addr)
    : m_WirePort(WirePort), crystal_freq(crystal_freq),
    sc(m_WirePort, SC16IS750_PROTOCOL_I2C, sc16is750_addr)
{
}

ExtSerialGPIO::~ExtSerialGPIO()
{
}

void ExtSerialGPIO::begin()
{
    LPF2_LOG_D("Initializing ExtSerialGPIO with crystal frequency: %u", crystal_freq);
    sc.begin(crystal_freq, 115200);
    for (uint8_t pin = 0; pin < 8; pin++)
    {
        sc.pinMode(pin, INPUT);
    }
}

Lpf2ExtSerial::Lpf2ExtSerial(ExtSerialGPIO *parent)
    : m_parent(parent)
{
}

void Lpf2ExtSerial::init(int id1_pin, int id2_pin, int tx_disable_pin)
{
    id1_pin_ = id1_pin;
    id2_pin_ = id2_pin;
    txd_pin_ = tx_disable_pin;
    pinMode(id1_pin_, INPUT);
    pinMode(id2_pin_, INPUT);
    uartPinsOff();
}

void Lpf2ExtSerial::end()
{
    uartPinsOff();
    return;
}

void Lpf2ExtSerial::setBaudrate(uint32_t baudrate)
{
    baud_ = baudrate;
    m_parent->sc.begin(m_parent->crystal_freq, baudrate);
    setUartPinsState(m_uartPinState);
    LPF2_LOG_D("Baudrate set to %u", baudrate);
    m_parent->sc.flush();
}

size_t Lpf2ExtSerial::write(const uint8_t *data, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        m_parent->sc.write(data[i]);
    }
    return length;
}

int Lpf2ExtSerial::read()
{
    return m_parent->sc.read();
}

int Lpf2ExtSerial::available()
{
    return m_parent->sc.available();
}

void Lpf2ExtSerial::flush()
{
    m_parent->sc.flush();
}

void Lpf2ExtSerial::setUartPinsState(bool highZ)
{
    m_uartPinState = highZ;
    LPF2_LOG_D("Setting uart pins state: highZ=%s", highZ ? "true" : "false");

    if (highZ)
    {
        m_parent->sc.pinMode(txd_pin_, OUTPUT);
        m_parent->sc.digitalWrite(txd_pin_, LOW);
    }
    else
    {
        m_parent->sc.pinMode(txd_pin_, OUTPUT);
        m_parent->sc.digitalWrite(txd_pin_, HIGH);
    }
}
