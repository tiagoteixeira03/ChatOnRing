
#ifndef join_h
#define join_h

int create_udp_client_fd();
void join(char *regIP, char *regUDP, t_node_info *my_node);

#endif