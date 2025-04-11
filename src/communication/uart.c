#include "../../include/uart.h" // Adjust path as needed
#include <errno.h> // For errno
#include <fcntl.h> // File control definitions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // For strerror, memset
#include <sys/select.h> // For select-based timeout in receive
#include <termios.h>    // POSIX terminal control definitions
#include <unistd.h>     // UNIX standard function definitions

// --- Internal Structure Definition ---
struct UartConnection_t {
  int fd;
  struct termios old_tio;
};

// --- Helper Function (Baud Rate) ---
static speed_t get_posix_baud(unsigned int baud_rate) {
  switch (baud_rate) {
  case 9600:    return B9600;
  case 19200:   return B19200;
  case 38400:   return B38400;
  case 57600:   return B57600;
  case 115200:  return B115200;
  case 230400:  return B230400;
  case 460800:  return B460800;
  case 921600:  return B921600;
  default:      return B0; // Indicate invalid/unsupported
  }
}

// --- init_uart_connection ---
UartHandle init_uart_connection(const char *device_name, unsigned int baud_rate) {
  if (!device_name) {
    return NULL;
  }

  speed_t posix_baud = get_posix_baud(baud_rate);
  if (posix_baud == B0 && baud_rate != 0) { // Allow B0 only if baud_rate is 0 (maybe for closing?)
    return NULL;
  }

  UartHandle connection = (UartHandle)malloc(sizeof(struct UartConnection_t));
  if (!connection) {
    return NULL;
  }

  memset(connection, 0, sizeof(struct UartConnection_t));
  connection->fd = -1;

  // Open the serial port
  connection->fd = open(device_name, O_RDWR | O_NOCTTY | O_NDELAY);
  if (connection->fd < 0) {
    // Store UART_ERROR_OPEN? (Capture errno?)
    perror("UART Error (open)");
    free(connection);
    return NULL;
  }

  // Set to blocking mode for subsequent operations (we handle timeouts with select)
  int flags = fcntl(connection->fd, F_GETFL, 0);
  if (flags == -1) {
      perror("UART Error (fcntl F_GETFL)");
      close(connection->fd);
      free(connection);
      return NULL;
  }
  if (fcntl(connection->fd, F_SETFL, flags & ~O_NDELAY) == -1) {
      perror("UART Error (fcntl F_SETFL)");
      close(connection->fd);
      free(connection);
      return NULL;
  }


  struct termios tio;
  // Get current settings
  if (tcgetattr(connection->fd, &tio) != 0) {
    perror("UART Error (tcgetattr)");
    close(connection->fd);
    free(connection);
    return NULL;
  }

  // Save old settings
  connection->old_tio = tio;

  // Set baud rate
  cfsetospeed(&tio, posix_baud);
  cfsetispeed(&tio, posix_baud);

  // Set serial port parameters: 8N1, raw mode
  tio.c_cflag |= (CLOCAL | CREAD); // Enable receiver, ignore modem control lines
  tio.c_cflag &= ~CSIZE;  // Mask data size bits
  tio.c_cflag |= CS8;     // Select 8 data bits
  tio.c_cflag &= ~PARENB; // Disable parity
  tio.c_cflag &= ~CSTOPB; // Select 1 stop bit
  tio.c_cflag &= ~CRTSCTS; // Disable hardware flow control

  // Input modes: Raw input
  tio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL |
                   IXON | IXOFF | IXANY);

  // Output modes: Raw output
  tio.c_oflag &= ~OPOST;

  // Local modes: Non-canonical mode
  tio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

  // Control characters (timeouts): Important for raw mode
  // VMIN = 0, VTIME = 0: Non-blocking read. read() returns immediately.
  //                      We rely on select() for blocking/timeouts.
  tio.c_cc[VMIN] = 0;
  tio.c_cc[VTIME] = 0;

  // Apply the settings
  if (tcsetattr(connection->fd, TCSANOW, &tio) != 0) {
    perror("UART Error (tcsetattr)");
    tcsetattr(connection->fd, TCSANOW, &connection->old_tio); // Try to restore
    close(connection->fd);
    free(connection);
    return NULL;
  }

  // Flush terminal input/output buffers
  tcflush(connection->fd, TCIOFLUSH); // Flush both directions

  return connection;
}


// --- uart_send (Robust version) ---
int uart_send(UartHandle handle, const unsigned char *data, size_t length) {
    if (!handle || handle->fd < 0 || !data || length == 0) {
        return UART_ERROR_PARAM;
    }
    size_t total_written = 0;
    ssize_t bytes_written;

    while (total_written < length) {
        bytes_written = write(handle->fd, data + total_written, length - total_written);

        if (bytes_written < 0) {
            if (errno == EINTR) {
                continue; // Interrupted, try again
            } else {
                perror("UART Send Error (write)");
                // Store error? Map errno to UART_ERROR_ code?
                return UART_ERROR_WRITE; // Generic write error
            }
        } else if (bytes_written == 0) {
            // write() returning 0 on a blocking descriptor typically indicates
            // a problem (maybe disconnected?) rather than just needing to retry.
            fprintf(stderr, "UART Send Error: write returned 0 unexpectedly.\n");
            return UART_ERROR_WRITE; // Treat as an error
        } else {
            total_written += (size_t)bytes_written;
        }
    }

    return (int)total_written; // Return total bytes written
}


// --- uart_receive (Robust version) ---
int uart_receive(UartHandle handle, unsigned char *buffer, size_t max_length,
                 int timeout_ms) {
    if (!handle || handle->fd < 0 || !buffer || max_length == 0) {
        return UART_ERROR_PARAM;
    }

    fd_set read_fds;
    struct timeval tv, *tv_ptr;
    int retval;

    // Prepare fd_set for select
    FD_ZERO(&read_fds);
    FD_SET(handle->fd, &read_fds);

    // Prepare timeval for select
    if (timeout_ms < 0) {
        tv_ptr = NULL; // Block indefinitely
    } else {
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        tv_ptr = &tv;
    }

    // Wait for data using select, handle EINTR
    do {
        // select() can modify timeout, so reset it if looping due to EINTR
        if (timeout_ms >= 0) {
            tv.tv_sec = timeout_ms / 1000;
            tv.tv_usec = (timeout_ms % 1000) * 1000;
            tv_ptr = &tv;
        }
        FD_ZERO(&read_fds); // Re-initialize fd_set before each select call
        FD_SET(handle->fd, &read_fds);

        retval = select(handle->fd + 1, &read_fds, NULL, NULL, tv_ptr);
    } while (retval == -1 && errno == EINTR);

    // Check select result
    if (retval == -1) {
        perror("UART Receive Error (select)");
        return UART_ERROR_READ; // Or a specific select error
    } else if (retval == 0) {
        return UART_ERROR_TIMEOUT; // Timeout occurred
    }

    // Data is available (FD_ISSET should be true, but we don't need to check)
    // Perform the read, handle EINTR
    ssize_t bytes_read;
    do {
        bytes_read = read(handle->fd, buffer, max_length);
    } while (bytes_read == -1 && errno == EINTR);

    if (bytes_read < 0) {
        perror("UART Receive Error (read)");
        // Check errno for specific conditions if needed (e.g., EIO)
        return UART_ERROR_READ;
    }

    // read() returns 0 on EOF (e.g., disconnected), >0 bytes read successfully
    return (int)bytes_read;
}


// --- close_uart_connection ---
// (Your implementation is good)
void close_uart_connection(UartHandle handle) {
  if (!handle) {
    return;
  }

  if (handle->fd >= 0) {
    // Restore old terminal settings before closing (best effort)
    tcsetattr(handle->fd, TCSANOW, &handle->old_tio);
    if (close(handle->fd) != 0) {
      // Should this return an error? The handle is being freed anyway.
      perror("Warning: Error closing UART file descriptor");
    }
    handle->fd = -1;
  }

  free(handle);
}

// --- get_uart_error_string ---
// (Your implementation is fine)
const char *get_uart_error_string(int error_code) {
  switch (error_code) {
  case UART_NO_ERROR:       return "No error";
  case UART_ERROR_OPEN:     return "Error opening port";
  case UART_ERROR_CONFIG:   return "Error configuring port";
  case UART_ERROR_READ:     return "Error reading from port";
  case UART_ERROR_WRITE:    return "Error writing to port";
  case UART_ERROR_TIMEOUT:  return "Operation timed out";
  case UART_ERROR_CLOSE:    return "Error closing port"; // Less common
  case UART_ERROR_PARAM:    return "Invalid parameter";
  case UART_ERROR_OTHER:    return "Other/unspecified UART error";
  default:                  return "Unknown error code";
  }
}
