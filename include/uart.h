#ifndef UART_H
#define UART_H

#include <stddef.h> // For size_t

// --- Opaque Handle ---
// Hides the platform-specific implementation details (file descriptor or HANDLE)
struct UartConnection_t;
typedef struct UartConnection_t* UartHandle;

// --- Constants ---
#define UART_NO_ERROR       0
#define UART_ERROR_OPEN    -1
#define UART_ERROR_CONFIG  -2
#define UART_ERROR_READ    -3
#define UART_ERROR_WRITE   -4
#define UART_ERROR_TIMEOUT -5
#define UART_ERROR_CLOSE   -6
#define UART_ERROR_PARAM   -7
#define UART_ERROR_OTHER   -99

#define UART_READ_TIMEOUT_MS 100   // Shorter timeout for polling loop
#define MAIN_BUFFER_SIZE 1024      // Max size for accumulating received data

// Simple sleep helper
#define MYSLEEP(ms) usleep(ms * 1000)

// --- Function Prototypes ---

/**
 * @brief Initializes a connection to a serial port (UART).
 *
 * Opens the specified serial port device and configures it with the
 * given baud rate and default settings (8 data bits, No parity, 1 stop bit).
 *
 * @param device_name The platform-specific name of the serial port
 *                    (e.g., "/dev/ttyUSB0", "/dev/cu.usbmodem14101" on Linux/macOS,
 *                     "COM3" or "\\\\.\\COM10" on Windows).
 * @param baud_rate The desired baud rate (e.g., 9600, 115200).
 * @return A handle to the UART connection on success, NULL on failure.
 *         Check return value of get_uart_last_error() on failure.
 */
UartHandle init_uart_connection(const char *device_name, unsigned int baud_rate);

/**
 * @brief Sends data over the UART connection.
 *
 * Attempts to write the specified number of bytes to the serial port.
 * This is typically a blocking call, but behavior might depend on OS settings.
 *
 * @param handle The UART connection handle obtained from init_uart_connection.
 * @param data Pointer to the data buffer to send.
 * @param length The number of bytes to send.
 * @return The number of bytes actually sent on success, or a negative UART_ERROR_* code on failure.
 */
int uart_send(UartHandle handle, const unsigned char *data, size_t length);

/**
 * @brief Receives data from the UART connection.
 *
 * Attempts to read up to 'max_length' bytes from the serial port.
 * This function will wait for up to 'timeout_ms' milliseconds for data to arrive.
 *
 * @param handle The UART connection handle obtained from init_uart_connection.
 * @param buffer Pointer to the buffer where received data will be stored.
 * @param max_length The maximum number of bytes to receive (size of the buffer).
 * @param timeout_ms Timeout in milliseconds to wait for data.
 *                   A value of 0 might attempt a non-blocking read (OS-dependent).
 *                   A negative value might indicate blocking indefinitely (use with caution).
 * @return The number of bytes actually received (can be 0 if timeout occurs before
 *         any data arrives), or a negative UART_ERROR_* code on failure.
 *         Specifically returns UART_ERROR_TIMEOUT if the timeout expires before
 *         receiving any data.
 */
int uart_receive(UartHandle handle, unsigned char *buffer, size_t max_length, int timeout_ms);

/**
 * @brief Closes the UART connection.
 *
 * Releases any resources associated with the serial port connection.
 *
 * @param handle The UART connection handle to close.
 */
void close_uart_connection(UartHandle handle);

/**
 * @brief Gets a string representation of the last error for a specific handle (if available).
 *
 * @param handle The UART handle (can be NULL, but might not yield specific info).
 * @return A descriptive string of the last error encountered. The string
 *         is static or depends on OS error functions (e.g., strerror, FormatMessage).
 *         Returns "No error" if no error is recorded.
 */
const char* get_uart_error_string(int error_code); // Consider adding handle param if storing last error per handle

#endif // UART_H
