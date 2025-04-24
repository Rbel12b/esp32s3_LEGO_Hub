#include "Lpf2Port.h"
#include <string>
#include <cstring>

void Lpf2Port::init()
{
    if (m_hwSerialNum >= 0)
    {
        m_hwSerial = new HardwareSerial(m_hwSerialNum);
        m_serial = m_hwSerial;
    }
    else
    {
        m_swSerial = new SoftwareSerial();
        m_serial = m_swSerial;
    }
    std::string taskName = "uartTask";
    taskName += m_rxPin;
    taskName += m_txPin;
    xTaskCreate(
        &Lpf2Port::taskEntryPoint, // Static entry point
        "uartTask",
        4096,
        this, // Pass this pointer
        1,
        nullptr);
    modes = views = 0;
}

void Lpf2Port::taskEntryPoint(void *pvParameters)
{
    Lpf2Port *self = static_cast<Lpf2Port *>(pvParameters);
    self->uartTask(); // Call actual member function
}

void Lpf2Port::uartTask()
{

    if (m_hwSerialNum >= 0)
    {
        m_hwSerial = static_cast<HardwareSerial *>(m_serial);
        m_hwSerial->begin(2400, SERIAL_8N1, m_rxPin, m_txPin);
    }
    else
    {
        m_swSerial = static_cast<SoftwareSerial *>(m_serial);
        m_swSerial->begin(2400, EspSoftwareSerial::SWSERIAL_8N1, m_rxPin, m_txPin);
    }

    Lpf2Parser parser(m_serial);

    parser.begin(m_rxPin, m_txPin);

    vTaskDelay(pdMS_TO_TICKS(100));

    log_i("Initialization done, rx: %i, tx: %i", m_rxPin, m_txPin);

    while (1)
    {
        auto messages = parser.update();

        for (const auto &msg : messages)
        {
            parseMessage(msg);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void Lpf2Port::parseMessage(const Lpf2Message &msg)
{
    switch (msg.msg)
    {
    case MESSAGE_CMD:
    {
        parseMessageCMD(msg);
        break;
    }
    case MESSAGE_INFO:
    {
        parseMessageInfo(msg);
    }
    }
}

void Lpf2Port::parseMessageCMD(const Lpf2Message &msg)
{
    switch (msg.cmd)
    {
    case CMD_TYPE:
    {
        m_deviceType = (DeviceType)msg.data[0];
        break;
    }
    case CMD_MODES:
    {
        if (msg.length == 1)
        {
            modes = views = msg.data[1] + 1;
        }
        else if (msg.length == 2)
        {
            modes = msg.data[0] + 1;
            views = msg.data[1] + 1;
        }
        else if (msg.length == 4)
        {
            modes = msg.data[2] + 1;
            views = msg.data[3] + 1;
        }
        modeData.resize(modes);
        break;
    }
    case CMD_SPEED:
    {
        if (msg.length != 4)
        {
            break;
        }
        baud = msg.data[0] | (msg.data[1] << 8) | (msg.data[2] << 16) | (msg.data[3] << 24);
        break;
    }
    }
}

void Lpf2Port::parseMessageInfo(const Lpf2Message &msg)
{
    switch (msg.data[0] & 0xF)
    {
    case INFO_NAME:
    {
        uint8_t mode = GET_MODE(msg.cmd) + ((msg.data[0] & INFO_MODE_PLUS_8) ? 8 : 0);
        if (mode >= modes)
        {
            break;
        }
        std::string name;
        int i = 0;
        for (i = 1; i < msg.length; i++)
        {
            if (msg.data[i] == '\0')
            {
                break;
            }
            name += msg.data[i];
        }
        modeData[mode].name = name;
        i++;
        if (i < msg.length)
        {
            // TODO: implement flag parsing
        }
        break;
    }
    case INFO_RAW:
    {
        uint8_t mode = GET_MODE(msg.cmd) + ((msg.data[0] & INFO_MODE_PLUS_8) ? 8 : 0);
        if (mode >= modes || msg.length < 9)
        {
            break;
        }
        std::memcpy(&modeData[mode].min, msg.data.data() + 1, 4);
        std::memcpy(&modeData[mode].max, msg.data.data() + 5, 4);
        break;
    }
    case INFO_PCT:
    {
        uint8_t mode = GET_MODE(msg.cmd) + ((msg.data[0] & INFO_MODE_PLUS_8) ? 8 : 0);
        if (mode >= modes || msg.length < 5)
        {
            break;
        }
        float val;
        std::memcpy(&val, msg.data.data() + 1, 4);
        if (val == -100.0f)
        {
            modeData[mode].negativePCT = true;
        }
        break;
    }
    case INFO_SI:
    {
        uint8_t mode = GET_MODE(msg.cmd) + ((msg.data[0] & INFO_MODE_PLUS_8) ? 8 : 0);
        if (mode >= modes || msg.length < 9)
        {
            break;
        }
        std::memcpy(&modeData[mode].SImin, msg.data.data() + 1, 4);
        std::memcpy(&modeData[mode].SImax, msg.data.data() + 5, 4);
        break;
    }
    case INFO_UNITS:
    {
        uint8_t mode = GET_MODE(msg.cmd) + ((msg.data[0] & INFO_MODE_PLUS_8) ? 8 : 0);
        if (mode >= modes)
        {
            break;
        }
        std::string unit;
        int i = 0;
        for (i = 1; i < msg.length; i++)
        {
            if (msg.data[i] == '\0')
            {
                break;
            }
            unit += msg.data[i];
        }
        modeData[mode].unit = unit;
        break;
    }
    case INFO_MAPPING:
    {
        uint8_t mode = GET_MODE(msg.cmd) + ((msg.data[0] & INFO_MODE_PLUS_8) ? 8 : 0);
        if (mode >= modes || msg.length < 3)
        {
            break;
        }
        modeData[mode].in.val = msg.data[1];
        modeData[mode].out.val = msg.data[2];
        break;
    }
    }
}