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
    comboNum = 0;
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

    auto start = millis();

    sendACK(true);

    while (1)
    {
        auto messages = parser.update();

        for (const auto &msg : messages)
        {
            parseMessage(msg);
        }

        vTaskDelay(pdMS_TO_TICKS(1));

        auto now = millis();

        if (now - start >= 100 && m_status == LPF2_STATUS::STATUS_DATA)
        {
            start = now;
            sendACK(true);
        }
    }
}

uint8_t Lpf2Port::getDataSize(uint8_t format)
{
    switch (format)
    {
    case DATA8:
        return 1;
    
    case DATA16:
        return 2;

    case DATA32:
        return 4;

    case DATAF:
        return 4;
    default:
        break;
    }
    return 0;
}

bool Lpf2Port::deviceIsAbsMotor(DeviceType id) {
    switch (id) {
        case DeviceType::TECHNIC_LARGE_LINEAR_MOTOR:
        case DeviceType::TECHNIC_XLARGE_LINEAR_MOTOR:
        case DeviceType::TECHNIC_LARGE_ANGULAR_MOTOR:
        case DeviceType::TECHNIC_LARGE_ANGULAR_MOTOR_GREY:
        case DeviceType::TECHNIC_MEDIUM_ANGULAR_MOTOR:
        case DeviceType::TECHNIC_MEDIUM_ANGULAR_MOTOR_GREY:
        case DeviceType::MEDIUM_LINEAR_MOTOR:
        case DeviceType::SIMPLE_MEDIUM_LINEAR_MOTOR:
            return true;
        default:
            return false;
    }
}

void Lpf2Port::sendMessage(std::vector<uint8_t> msg)
{
    m_serial->write(msg.data(), msg.size());
}

std::vector<uint8_t> Lpf2Port::makeMessage(uint8_t header, std::vector<uint8_t> msg)
{
    std::vector<uint8_t> m;
    m.reserve(msg.size() + 2);
    m.push_back(header);
    m.insert(m.end(), msg.begin(), msg.end());
    uint8_t checksum = header ^ 0xFF;
    for (int i = 0; i < msg.size(); i++)
    {
        checksum ^= msg[i];
    }
    m.push_back(checksum);
    return m;
}

void Lpf2Port::setMode(ModeNum num)
{
    sendMessage(makeMessage(MESSAGE_CMD | CMD_SELECT, std::vector<uint8_t>{(uint8_t)num}));
}

ModeNum Lpf2Port::getDefaultMode(DeviceType id) {

    if (deviceIsAbsMotor(id)) {
        return ModeNum::MOTOR__CALIB;
    }

    switch (id) {
        case DeviceType::COLOR_DISTANCE_SENSOR:
        case DeviceType::TECHNIC_COLOR_SENSOR:
            return ModeNum::COLOR_DISTANCE_SENSOR__RGB_I;
        default:
            return ModeNum::_DEFAULT;
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
        break;
    }
    case MESSAGE_SYS:
    {
        switch (msg.header)
        {
        case BYTE_ACK:
            if (m_status == LPF2_STATUS::STATUS_INFO)
            {
                log_d("ACK received.");
                m_status = LPF2_STATUS::STATUS_DATA;
                sendACK();
                log_d("Changing speed to %i baud", baud);
                changeBaud(baud);
                vTaskDelay(1);
                setMode(getDefaultMode(m_deviceType));
            }
            else if (m_status == LPF2_STATUS::STATUS_ERR)
            {
                sendACK(true);
                log_d("ACK received in error state");
            }
            break;
        case BYTE_SYNC:
            break;
        
        default:
            break;
        }
        break;
    }
    case MESSAGE_DATA:
    {
        if (m_status == LPF2_STATUS::STATUS_SYNCING)
        {
            m_status = LPF2_STATUS::STATUS_DATA;
            log_d("Succesfully changed speed.");
        }
        else if (m_status != LPF2_STATUS::STATUS_DATA)
        {
            log_w("Received data message outside data phase.");
            break;
        }
        uint8_t mode = GET_MODE(msg.header);
        if (nextModeExt)
        {
            mode += 8;
            nextModeExt = false;
        }
        uint8_t size = modeData[mode].data_sets * getDataSize(modeData[mode].format);
        if (size != modeData[mode].rawData.size())
        {
            modeData[mode].rawData.resize(size);
        }
        uint8_t readLen = size;
        if (msg.length < size)
        {
            readLen = msg.length;
        }
        for (int i = 0; i < readLen; i++)
        {
            modeData[mode].rawData[i] = msg.data[i];
        }
        break;
    }
    default:
    {
        log_e("Unknown message type: 0x%02X", msg.msg);
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
        m_status = LPF2_STATUS::STATUS_INFO;
        nextModeExt = false;
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
    case CMD_VERSION:
    {
        break;
    }
    case CMD_EXT_MODE:
    {
        nextModeExt = true;
        break;
    }
    default:
    {
        log_w("Unknown command: 0x%02X", msg.cmd);
        break;
    }
    }
}

void Lpf2Port::parseMessageInfo(const Lpf2Message &msg)
{
    switch (msg.data[0] & 0xDF)
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
    case INFO_MODE_COMBOS:
    {
        uint8_t num = (msg.length - 1) / 2;
        if (num > 15)
        {
            break;
        }
        for (int i = 0; i < num; i++)
        {
            std::memcpy(&modeCombos[num], msg.data.data() + 1 + (i * 2), 2);
            if (comboNum == 0 && modeCombos[num] == 0)
            {
                comboNum = i;
            }
        }
    }
    case INFO_FORMAT:
    {
        uint8_t mode = GET_MODE(msg.cmd) + ((msg.data[0] & INFO_MODE_PLUS_8) ? 8 : 0);
        if (mode >= modes || msg.length < 5)
        {
            break;
        }
        modeData[mode].data_sets = msg.data[1];
        modeData[mode].format = msg.data[2];
        modeData[mode].figures = msg.data[3];
        modeData[mode].decimals = msg.data[4];
        break;
    }
    default:
    {
        log_w("Unknown info: 0x%02X", msg.data[0]);
        break;
    }
    }
}

void Lpf2Port::changeBaud(uint32_t baud)
{
    if (m_hwSerialNum >= 0)
    {
        m_hwSerial = static_cast<HardwareSerial *>(m_serial);
        m_hwSerial->updateBaudRate(baud);
    }
    else
    {
        m_swSerial = static_cast<SoftwareSerial *>(m_serial);
        m_swSerial->begin(baud, EspSoftwareSerial::SWSERIAL_8N1, m_rxPin, m_txPin);
    }
}

void Lpf2Port::sendACK(bool NACK)
{
    m_serial->write(NACK ? BYTE_NACK : BYTE_ACK);
}