#ifndef message_h
#define message_h

#include "user_input.h"

void send_chat_instruction(t_node_info *my_node, char origin[3], char dest[3], char message[128]);
void receive_chat_instruction(t_node_info *my_node, char chat_instruction[512]);

#endif