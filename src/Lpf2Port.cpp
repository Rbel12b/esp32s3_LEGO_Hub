#include "Lpf2Port.h"
#include "Lpf2Serial.h"
#include <string>

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
        this,               // Pass this pointer
        1,
        nullptr
    );
}

void Lpf2Port::taskEntryPoint(void *pvParameters)
{
    Lpf2Port* self = static_cast<Lpf2Port*>(pvParameters);
    self->uartTask(); // Call actual member function
}

void Lpf2Port::uartTask()
{
    Lpf2Parser parser(m_serial, m_hwSerialNum >= 0);

    parser.begin(m_rxPin, m_txPin);

    vTaskDelay(pdMS_TO_TICKS(100));

    log_i("Initialization done, rx: %i, tx: %i", m_rxPin, m_txPin);

    while (1)
    {
        parser.update();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
