#ifndef INC_COMMAND_PARSER_H_
#define INC_COMMAND_PARSER_H_

#include "main.h"

#define CMD_NO_CMD  0
#define CMD_RST     1
#define CMD_OK      2

void command_parser_fsm(uint8_t received_char);

uint8_t get_command_flag(void);

#endif /* INC_COMMAND_PARSER_H_ */
