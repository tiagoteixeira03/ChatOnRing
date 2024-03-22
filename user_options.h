#ifndef user_options_h
#define user_options_h

int countDots(const char *str);
void process_user_arguments(int argc, char *argv[], char **regIP, char **regUDP, t_node_info *my_node);

#endif