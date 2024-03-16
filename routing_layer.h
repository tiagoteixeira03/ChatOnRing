#ifndef routing_layer_h
#define routing_layer_h

#include "user_input.h"

void print_routing_table(t_node_info *my_node);
void print_shortest_paths_table(t_node_info *my_node);
void reset_tables(t_node_info *my_node);
int routing_to_shortest_paths_table(t_node_info *my_node);
void routing_table_init(t_node_info *my_node);
char* convert_single_digit_numbers(int number);
void send_route_messages(t_node_info *my_node, char buffer[512]);
void write_route_messages(t_node_info *my_node);
void process_route_messages(t_node_info *my_node, char buffer[512]);


#endif