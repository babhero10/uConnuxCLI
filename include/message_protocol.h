// -------- message_protocol.h --------
#ifndef MESSAGE_PROTOCOL_H
#define MESSAGE_PROTOCOL_H

#include <stdint.h> // For uint8_t, int16_t, etc.
#include <stddef.h> // For size_t

// --- Constants ---
#define MSG_HEADER 0xAA // Example header byte

// --- Error Codes for Protocol Layer ---
typedef enum {
    MSG_SUCCESS = 0,
    MSG_ERROR_BUFFER_SMALL = -101,
    MSG_ERROR_MALLOC = -102,
    MSG_ERROR_INVALID_PARAM = -103,
    MSG_ERROR_SERIALIZE = -104,     // Failure during marshalling
    MSG_ERROR_DESERIALIZE = -105,   // Failure during unmarshalling
    MSG_ERROR_BAD_HEADER = -106,
    MSG_ERROR_BAD_CHECKSUM = -107,
    MSG_ERROR_UNKNOWN_CMD = -108,
    MSG_ERROR_INCOMPLETE = -109,    // Not enough data received yet
} MessageError_t;

// --- Command IDs ---
typedef enum {
    CMD_SET_STRING = 0x01,
    CMD_SET_INT8 = 0x02,
    CMD_SET_TWIST = 0x03, // Vector3 linear, Vector3 angular
    // Add more commands as needed
    CMD_ACK = 0xFE,
    CMD_NACK = 0xFF
} CommandID_t;

// --- Common Data Structures ---
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} Vector3_t;

// --- Payload Structures for Specific Commands ---
// CMD_SET_TWIST Payload
typedef struct {
    Vector3_t linear;
    Vector3_t angular;
} TwistData_t;

// --- Generic Parsed Message Structure ---
// Used to return data from the receiver/parser
typedef struct {
    CommandID_t command_id;
    uint8_t *payload;      // Dynamically allocated buffer holding payload data
    size_t payload_length;
} ParsedMessage_t;


// --- Marshalling Functions (Creating packets to send) ---

/**
 * @brief Creates a message packet for a string payload.
 * Dynamically allocates the output buffer. Caller must free *out_buffer.
 * Format: [HEADER][CMD_SET_STRING][Payload Len (1 byte)][String Data...][CHECKSUM]
 */
MessageError_t create_string_message(const char *str,
                                     uint8_t **out_buffer, size_t *out_length);

/**
 * @brief Creates a message packet for an int8 payload.
 * Dynamically allocates the output buffer. Caller must free *out_buffer.
 * Format: [HEADER][CMD_SET_INT8][int8 value][CHECKSUM]
 */
MessageError_t create_int8_message(int8_t value,
                                   uint8_t **out_buffer, size_t *out_length);

/**
 * @brief Creates a message packet for a TwistData (2x Vector3) payload.
 * Dynamically allocates the output buffer. Caller must free *out_buffer.
 * Format: [HEADER][CMD_SET_TWIST][LinearX][LinearY][LinearZ][AngularX][AngularY][AngularZ][CHECKSUM]
 * (Assumes int16 are sent low-byte first, then high-byte - i.e. little-endian)
 */
MessageError_t create_twist_message(const TwistData_t *twist_data,
                                    uint8_t **out_buffer, size_t *out_length);

// --- Unmarshalling/Parsing Functions ---

/**
 * @brief Attempts to find and parse one complete message from the start of a receive buffer.
 *
 * Scans the buffer for the header, reads command, payload (if any), and checksum.
 * Validates the checksum. If a valid message is found, it's parsed into 'parsed_msg'.
 * The payload buffer within 'parsed_msg' is dynamically allocated and must be freed
 * by the caller if the function returns MSG_SUCCESS.
 *
 * @param recv_buffer The buffer containing raw received bytes.
 * @param buffer_len The number of bytes currently in recv_buffer.
 * @param parsed_msg Output structure to hold the parsed message details if successful.
 * @param bytes_consumed Output parameter indicating how many bytes from recv_buffer
 *                       were part of the successfully parsed message (or attempted parse).
 * @return MSG_SUCCESS if a valid message was parsed.
 *         MSG_ERROR_INCOMPLETE if not enough bytes for a full message yet.
 *         MSG_ERROR_BAD_HEADER if the header byte is wrong.
 *         MSG_ERROR_BAD_CHECKSUM if the checksum fails.
 *         MSG_ERROR_UNKNOWN_CMD if the command ID is not recognized.
 *         Other MessageError_t codes on other failures.
 */
MessageError_t parse_message_from_buffer(const uint8_t *recv_buffer, size_t buffer_len,
                                         ParsedMessage_t *parsed_msg, size_t *bytes_consumed);


/**
 * @brief Frees the dynamically allocated payload within a ParsedMessage_t.
 */
void free_parsed_message_payload(ParsedMessage_t *msg);


/**
 * @brief Helper to get a descriptive string for message protocol errors.
 */
const char* get_message_error_string(MessageError_t error_code);


#endif // MESSAGE_PROTOCOL_H
