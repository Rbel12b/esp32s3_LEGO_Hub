#include "Lpf2Serial.h"
#include "Lpf2SerialDef.h"

void Lpf2Parser::begin(int rxPin, int txPin)
{
    buffer.clear();
    if (hwSerial)
    {
        m_hwSerial = static_cast<HardwareSerial *>(m_serial);
        m_hwSerial->begin(2400, SERIAL_8N1, rxPin, txPin);
    }
    else
    {
        m_swSerial = static_cast<SoftwareSerial *>(m_serial);
        m_swSerial->begin(2400, EspSoftwareSerial::SWSERIAL_8N1, rxPin, txPin);
    }
    deviceInited = false;
}

void Lpf2Parser::update()
{
    while (m_serial->available())
    {
        uint8_t b = m_serial->read();
        buffer.push_back(b);
    }

    while (buffer.size())
    {
        Lpf2Message message;

        resetChecksum();

        uint8_t b = buffer[0];
        computeChecksum(b);

        message.header = b;

        if (b == BYTE_ACK || b == BYTE_NACK || b == BYTE_SYNC)
        {
            message.system = true;
            parseMessage(message);
            buffer.erase(buffer.begin());
            continue;
        }
        else
        {
            message.system = false;
        }

        message.length = GET_MESSAGE_LENGTH(b);

        if (GET_MESSAGE_TYPE(message.header) == MESSAGE_INFO)
        {
            message.length++; // +1 command byte
        }
        if (buffer.size() < message.length + 2)
        {
            break;
        }

        message.data.clear();
        message.data.reserve(message.length);

        for (int i = 0; i < message.length; i++)
        {
            message.data.push_back(buffer[i + 1]);
            computeChecksum(buffer[i + 1]);
        }

        b = buffer[message.length + 1];

        message.checksum = b;

        message.msg = GET_MESSAGE_TYPE(message.header);
        message.cmd = GET_CMD_COMMAND(message.header);

        if (b != getChecksum())
        {
            log_d("Checksum mismatch: 0x%02X != 0x%02X", b, getChecksum());
            Serial.print("    ");
            printMessage(message);
            buffer.erase(buffer.begin());
            continue;
        }

        parseMessage(message);

        buffer.erase(buffer.begin(), buffer.begin() + message.length + 2);
    }
}

void Lpf2Parser::sendACK(bool NACK)
{
    m_serial->write(NACK ? BYTE_NACK : BYTE_ACK);
}

void Lpf2Parser::resetChecksum()
{
    checksum = 0xFF;
}

void Lpf2Parser::computeChecksum(uint8_t b)
{
    checksum ^= b;
}

void Lpf2Parser::parseMessage(const Lpf2Message &msg)
{
    printMessage(msg);
}

void Lpf2Parser::printMessage(const Lpf2Message &msg)
{
    Serial.print("[LPF2] ");
    if (msg.system)
    {
        Serial.print("System Message: ");
        switch (msg.header)
        {
        case BYTE_ACK:
            Serial.println("ACK");
            break;
        case BYTE_NACK:
            Serial.println("NACK");
            break;
        case BYTE_SYNC:
            Serial.println("SYNC");
            break;
        default:
            Serial.print("Unknown (0x");
            Serial.print(msg.header, HEX);
            Serial.println(")");
            break;
        }
        return;
    }

    Serial.print("Header: 0x");
    Serial.print(msg.header, HEX);
    Serial.print(", Length: ");
    Serial.print(msg.length);
    Serial.print(", MsgType: 0x");
    Serial.print(msg.msg, HEX);
    Serial.print(", Cmd: 0x");
    Serial.print(msg.cmd, HEX);
    Serial.print(", Data: ");

    for (uint8_t b : msg.data)
    {
        Serial.print("0x");
        if (b < 0x10)
            Serial.print("0"); // Padding for single hex digit
        Serial.print(b, HEX);
        Serial.print(" ");
    }
    Serial.print(", c: 0x");
    if (msg.checksum < 0x10)
        Serial.print("0");
    Serial.print(msg.checksum, HEX);
    Serial.println();
}
