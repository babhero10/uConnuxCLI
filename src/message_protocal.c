// -------- message_protocol.c --------
#include "../include/message_protocol.h" // Adjust path as needed
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h> // For sprintf in error messages, if needed

// --- Internal Helper Functions ---

// Simple checksum: XOR sum of all bytes (header to end of payload)
static uint8_t calculate_checksum(const uint8_t *data, size_t length) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum ^= data[i];
    }
    return checksum;
}

// Serialize int16_t (Little-Endian: Low byte first)
static void serialize_int16_le(uint8_t *buffer, int16_t value) {
    buffer[0] = (uint8_t)(value & 0xFF);
    buffer[1] = (uint8_t)((value >> 8) & 0xFF);
}

// Deserialize int16_t (Little-Endian: Low byte first)
static int16_t deserialize_int16_le(const uint8_t *buffer) {
    return (int16_t)(((uint16_t)buffer[1] << 8) | (uint16_t)buffer[0]);
}

// Serialize Vector3 (Little-Endian)
static void serialize_vector3_le(uint8_t *buffer, const Vector3_t *vec) {
    serialize_int16_le(buffer + 0, vec->x);
    serialize_int16_le(buffer + 2, vec->y);
    serialize_int16_le(buffer + 4, vec->z);
}

// Deserialize Vector3 (Little-Endian)
static void deserialize_vector3_le(const uint8_t *buffer, Vector3_t *vec) {
    vec->x = deserialize_int16_le(buffer + 0);
    vec->y = deserialize_int16_le(buffer + 2);
    vec->z = deserialize_int16_le(buffer + 4);
}


// --- Marshalling Implementations ---

MessageError_t create_string_message(const char *str, uint8_t **out_buffer, size_t *out_length) {
    if (!str || !out_buffer || !out_length) return MSG_ERROR_INVALID_PARAM;

    size_t str_len = strlen(str);
    if (str_len > 255) return MSG_ERROR_INVALID_PARAM; // Payload length fits in uint8_t

    // Size: Header(1) + Command(1) + PayloadLen(1) + Payload(str_len) + Checksum(1)
    size_t total_len = 1 + 1 + 1 + str_len + 1;
    uint8_t *buffer = (uint8_t *)malloc(total_len);
    if (!buffer) return MSG_ERROR_MALLOC;

    buffer[0] = MSG_HEADER;
    buffer[1] = CMD_SET_STRING;
    buffer[2] = (uint8_t)str_len; // Payload length
    memcpy(buffer + 3, str, str_len);

    // Checksum is calculated over Header, Command, PayloadLen, Payload
    buffer[total_len - 1] = calculate_checksum(buffer, total_len - 1);

    *out_buffer = buffer;
    *out_length = total_len;
    return MSG_SUCCESS;
}

MessageError_t create_int8_message(int8_t value, uint8_t **out_buffer, size_t *out_length) {
     if (!out_buffer || !out_length) return MSG_ERROR_INVALID_PARAM;

    // Size: Header(1) + Command(1) + Payload(1) + Checksum(1)
    size_t total_len = 1 + 1 + 1 + 1;
    uint8_t *buffer = (uint8_t *)malloc(total_len);
    if (!buffer) return MSG_ERROR_MALLOC;

    buffer[0] = MSG_HEADER;
    buffer[1] = CMD_SET_INT8;
    buffer[2] = (uint8_t)value; // Payload

    // Checksum is calculated over Header, Command, Payload
    buffer[total_len - 1] = calculate_checksum(buffer, total_len - 1);

    *out_buffer = buffer;
    *out_length = total_len;
    return MSG_SUCCESS;
}

MessageError_t create_twist_message(const TwistData_t *twist_data, uint8_t **out_buffer, size_t *out_length) {
    if (!twist_data || !out_buffer || !out_length) return MSG_ERROR_INVALID_PARAM;

    // Size: Header(1) + Command(1) + Payload(6 + 6) + Checksum(1)
    size_t payload_len = sizeof(Vector3_t) + sizeof(Vector3_t); // 6 + 6 = 12
    size_t total_len = 1 + 1 + payload_len + 1;
    uint8_t *buffer = (uint8_t *)malloc(total_len);
    if (!buffer) return MSG_ERROR_MALLOC;

    buffer[0] = MSG_HEADER;
    buffer[1] = CMD_SET_TWIST;

    // Serialize payload
    serialize_vector3_le(buffer + 2, &twist_data->linear);
    serialize_vector3_le(buffer + 2 + sizeof(Vector3_t), &twist_data->angular);

    // Checksum is calculated over Header, Command, Payload
    buffer[total_len - 1] = calculate_checksum(buffer, total_len - 1);

    *out_buffer = buffer;
    *out_length = total_len;
    return MSG_SUCCESS;
}


// --- Unmarshalling/Parsing Implementation ---

MessageError_t parse_message_from_buffer(const uint8_t *recv_buffer, size_t buffer_len,
                                         ParsedMessage_t *parsed_msg, size_t *bytes_consumed)
{
    if (!recv_buffer || !parsed_msg || !bytes_consumed) return MSG_ERROR_INVALID_PARAM;

    *bytes_consumed = 0; // Default: consumed nothing useful yet

    // 1. Find Header
    const uint8_t *msg_start = NULL;
    for (size_t i = 0; i < buffer_len; ++i) {
        if (recv_buffer[i] == MSG_HEADER) {
            msg_start = recv_buffer + i;
            *bytes_consumed = i + 1; // Consumed bytes up to and including header
            break;
        }
    }

    if (!msg_start) {
        *bytes_consumed = buffer_len; // Consumed all bytes, no header found
        return MSG_ERROR_INCOMPLETE; // Or a different error? Maybe just incomplete.
    }

    size_t remaining_len = buffer_len - (msg_start - recv_buffer);

    // 2. Check minimum length (Header + Command + Checksum)
    if (remaining_len < 3) {
        return MSG_ERROR_INCOMPLETE; // Not enough data for basic structure
    }

    // 3. Get Command ID
    CommandID_t cmd = (CommandID_t)msg_start[1];
    *bytes_consumed = (msg_start - recv_buffer) + 2; // Consumed header + command byte

    // 4. Determine Payload Length based on Command
    size_t expected_payload_len = 0;
    bool variable_len = false;

    switch (cmd) {
        case CMD_SET_INT8:
            expected_payload_len = 1;
            break;
        case CMD_SET_TWIST:
            expected_payload_len = sizeof(TwistData_t); // 12 bytes
            break;
        case CMD_SET_STRING:
            variable_len = true;
            // Need payload length byte first
            if (remaining_len < 4) { // Header + Cmd + LenByte + Checksum
                 return MSG_ERROR_INCOMPLETE;
            }
            expected_payload_len = msg_start[2]; // Read length from packet
            *bytes_consumed = (msg_start - recv_buffer) + 3; // Consumed header + cmd + len byte
            break;
        case CMD_ACK: // No payload commands
        case CMD_NACK:
            expected_payload_len = 0;
            break;
        default:
            // Don't know this command, discard the header and command byte
            // *bytes_consumed is already set to header + command
            return MSG_ERROR_UNKNOWN_CMD;
    }

    // 5. Calculate total expected message length
    // Size = Header(1) + Command(1) + [PayloadLen(1) if var] + Payload + Checksum(1)
    size_t expected_total_len = 1 + 1 + (variable_len ? 1 : 0) + expected_payload_len + 1;

    // 6. Check if we have enough bytes for the complete message
    if (remaining_len < expected_total_len) {
        // Don't update bytes_consumed here, we are still waiting for this message
        *bytes_consumed = (msg_start - recv_buffer); // Reset consumed to just before header
        return MSG_ERROR_INCOMPLETE;
    }

    // We potentially have a full message
    *bytes_consumed = (msg_start - recv_buffer) + expected_total_len; // Consume the whole message

    // 7. Verify Checksum
    const uint8_t *payload_start = msg_start + 2 + (variable_len ? 1 : 0);
    uint8_t received_checksum = msg_start[expected_total_len - 1];
    // Checksum calculated over everything *except* the checksum byte itself
    uint8_t calculated_checksum = calculate_checksum(msg_start, expected_total_len - 1);

    if (received_checksum != calculated_checksum) {
        // Checksum failed, discard this message attempt
        return MSG_ERROR_BAD_CHECKSUM;
    }

    // 8. Checksum OK! Allocate and copy payload.
    parsed_msg->command_id = cmd;
    parsed_msg->payload_length = expected_payload_len;
    if (expected_payload_len > 0) {
        parsed_msg->payload = (uint8_t *)malloc(expected_payload_len);
        if (!parsed_msg->payload) {
            return MSG_ERROR_MALLOC; // Allocation failed
        }
        memcpy(parsed_msg->payload, payload_start, expected_payload_len);
    } else {
        parsed_msg->payload = NULL;
    }

    return MSG_SUCCESS; // Message parsed successfully!
}


void free_parsed_message_payload(ParsedMessage_t *msg) {
    if (msg && msg->payload) {
        free(msg->payload);
        msg->payload = NULL;
        msg->payload_length = 0;
    }
}


const char* get_message_error_string(MessageError_t error_code) {
    switch(error_code) {
        case MSG_SUCCESS:               return "Message Success";
        case MSG_ERROR_BUFFER_SMALL:    return "Message Error: Buffer too small";
        case MSG_ERROR_MALLOC:          return "Message Error: Memory allocation failed";
        case MSG_ERROR_INVALID_PARAM:   return "Message Error: Invalid parameter";
        case MSG_ERROR_SERIALIZE:       return "Message Error: Serialization failed";
        case MSG_ERROR_DESERIALIZE:     return "Message Error: Deserialization failed";
        case MSG_ERROR_BAD_HEADER:      return "Message Error: Invalid header";
        case MSG_ERROR_BAD_CHECKSUM:    return "Message Error: Invalid checksum";
        case MSG_ERROR_UNKNOWN_CMD:     return "Message Error: Unknown command ID";
        case MSG_ERROR_INCOMPLETE:      return "Message Error: Incomplete message data";
        default:                        return "Message Error: Unknown error code";
    }
}
