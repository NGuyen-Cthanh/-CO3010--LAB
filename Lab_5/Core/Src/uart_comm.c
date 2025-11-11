
#include "uart_comm.h"
#include "command_parser.h"
#include <stdio.h>
#include <string.h>

#define STATE_IDLE          0
#define STATE_WAIT_FOR_OK   1

static uint8_t  fsm_state = STATE_IDLE;
static uint32_t timer_start = 0;
static uint32_t adc_value = 0;
static char     send_buffer[50]; // Buffer for the string "!ADC=1234#"

// Timeout duration (3 seconds)
#define TIMEOUT_DURATION 3000 // in milliseconds

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;

void uart_communication_fsm(void) {
    // Check for a command from the parser
    uint8_t command = get_command_flag();

    switch (fsm_state) {
        case STATE_IDLE:
            // Waiting for the user to send !RST#
            if (command == CMD_RST) {
                // --- Start ADC Conversion and Read Value ---
                // Note: For simplicity, we use polling.
                HAL_ADC_Start(&hadc1);
                // Poll for conversion with a 10ms timeout
                if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
                    adc_value = HAL_ADC_GetValue(&hadc1);
                }
                HAL_ADC_Stop(&hadc1);

                // --- Format and Send Packet ---
                int len = sprintf(send_buffer, "!ADC=%lu#", adc_value);
                HAL_UART_Transmit(&huart2, (uint8_t*)send_buffer, len, 100);

                // --- Start Timer and Change State ---
                timer_start = HAL_GetTick();
                fsm_state = STATE_WAIT_FOR_OK;
            }
            break;

        case STATE_WAIT_FOR_OK:
                // Waiting for the user to send !OK#
                if (command == CMD_OK) {
                    // Acknowledgment received, go back to idle
                    fsm_state = STATE_IDLE;
                }
                // VVVV CHANGE THIS LINE VVVV
                else if (HAL_GetTick() - timer_start > TIMEOUT_DURATION) {
                // ^^^^ TO "else if" ^^^^

                // Timeout! Resend the *previous* packet
                int len = sprintf(send_buffer, "!ADC=%lu#", adc_value);
                HAL_UART_Transmit(&huart2, (uint8_t*)send_buffer, len, 100);

                // Restart the timer
                timer_start = HAL_GetTick();
                }
                break;

        default:
            fsm_state = STATE_IDLE;
            break;
    }
}
