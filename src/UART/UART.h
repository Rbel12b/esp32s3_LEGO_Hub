#pragma once
#include <stddef.h>
#include <stdint.h>

/**
 * @class UartPort
 * @brief Abstract base class for UART port communication.
 *
 * Provides an interface for UART serial communication operations including
 * initialization, data transmission/reception, and configuration management.
 * Derived classes must implement all pure virtual methods.
 */
class UartPort
{
public:
    virtual ~UartPort() = default;

    /**
     * @brief Initialize the UART port with the specified baudrate and configuration.
     * @param baudrate The desired baudrate for communication (e.g., 9600, 115200).
     * @param config The UART configuration flags (data bits, parity, stop bits).
     * @param rx_pin The RX (receive) pin number. Use -1 for default or auto-selection.
     * @param tx_pin The TX (transmit) pin number. Use -1 for default or auto-selection.
     * @return true if initialization was successful, false otherwise.
     * @note Must be called before any other UART operations.
     */
    virtual bool begin(uint32_t baudrate, uint32_t config, int rx_pin = -1, int tx_pin = -1) = 0;

    /**
     * @brief Close and deinitialize the UART port.
     * @note Releases resources and stops UART operation.
     */
    virtual void end() = 0;

    /**
     * @brief Set a new baudrate for the UART port.
     * @param baudrate The new desired baudrate for communication.
     * @note The port must be initialized before calling this method.
     */
    virtual void setBaudrate(uint32_t baudrate) = 0;

    /**
     * @brief Write data to the UART port.
     * @param data Pointer to the byte array to transmit.
     * @param length The number of bytes to write.
     * @return The number of bytes successfully written.
     */
    virtual size_t write(const uint8_t *data, size_t length) = 0;

    /**
     * @brief Read a single byte from the UART port.
     * @return The byte read from the receive buffer, or -1 if no data is available.
     * @note Non-blocking operation.
     */
    virtual int read() = 0;

    /**
     * @brief Get the number of bytes available to read from the UART receive buffer.
     * @return The number of available bytes ready to read.
     */
    virtual size_t available() = 0;

    /**
     * @brief Flush the UART transmit buffer, ensuring all pending data is sent.
     * @note This is a blocking operation that waits until the buffer is empty.
     */
    virtual void flush() = 0;

    /**
     * @brief Write a single byte to the UART port (convenience overload).
     * @param byte The byte to transmit.
     * @return The number of bytes written (1 on success, 0 on failure).
     * @note Calls the multi-byte write() method internally.
     */
    size_t write(uint8_t byte)
    {
        return write(&byte, 1);
    }
};
