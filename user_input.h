#ifndef user_input_h
#define user_input_h

typedef struct node_info{
    char *own_IP;
    char *own_port;
    char *own_id;
    char *succ_IP;
    char *succ_port;
    char *succ_id;
    char *sec_suc_IP;
    char *sec_suc_port;
    char *sec_suc_id;
    char *pred_IP;
    char *pred_port;
    char *pred_id;
    char *ring_id;
    int tcp_server_fd;
    int tcp_client_fd;
    int udp_client_fd;
}t_node_info;

void process_user_input(char *regIP, char *regUDP, t_node_info *my_node);
char* get_user_input();
void function_selector(char *buffer, char *regIP, char *regUDP, t_node_info *my_node);
t_node_info* alloc_node_memory();

#endif