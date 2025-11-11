#include "command_parser.h"
#include <string.h> // For strcmp

#define MAX_COMMAND_BUFFER_SIZE 30

#define STATE_IDLE      0
#define STATE_RECEIVE   1

static uint8_t fsm_state = STATE_IDLE;
static uint8_t command_buffer[MAX_COMMAND_BUFFER_SIZE];
static uint8_t command_index = 0;

static uint8_t command_flag = CMD_NO_CMD;

static void parse_command(void) {
    // Add null terminator to make it a valid string
    command_buffer[command_index] = '\0';

    if (strcmp((char*)command_buffer, "RST") == 0) {
        command_flag = CMD_RST;
    } else if (strcmp((char*)command_buffer, "OK") == 0) {
        command_flag = CMD_OK;
    } else {
        // Invalid command
        command_flag = CMD_NO_CMD;
    }
}

void command_parser_fsm(uint8_t received_char) {
    switch(fsm_state) {
        case STATE_IDLE:
            // Wait for the start character '!'
            if (received_char == '!') {
                fsm_state = STATE_RECEIVE;
                command_index = 0; // Reset buffer index
            }
            break;

        case STATE_RECEIVE:
            // Check for the end character '#'
            if (received_char == '#') {
                fsm_state = STATE_IDLE;
                parse_command(); // Process the buffered command
            }
            // Check for a new start character (resets the command)
            else if (received_char == '!') {
                command_index = 0; // Stay in STATE_RECEIVE but reset
            }
            // Otherwise, add the character to the buffer
            else {
                if (command_index < MAX_COMMAND_BUFFER_SIZE - 1) {
                    command_buffer[command_index++] = received_char;
                } else {
                    // Buffer overflow, reset
                    fsm_state = STATE_IDLE;
                    command_index = 0;
                }
            }
            break;

        default:
            // Should not be here, reset to IDLE
            fsm_state = STATE_IDLE;
            break;
    }
}

uint8_t get_command_flag(void) {
    if (command_flag != CMD_NO_CMD) {
        uint8_t temp_flag = command_flag;
        command_flag = CMD_NO_CMD; // Clear the flag after reading
        return temp_flag;
    }
    return CMD_NO_CMD;
}
