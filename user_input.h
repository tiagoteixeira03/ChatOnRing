#ifndef user_input_h
#define user_input_h

typedef struct node_info{
    char own_IP[16];
    char own_port[6];
    char own_id[3];
    char succ_IP[16];
    char succ_port[6];
    char succ_id[3];
    char sec_suc_IP[16];
    char sec_suc_port[6];
    char sec_suc_id[3];
    char pred_IP[16];
    char pred_port[6];
    char pred_id[3];
    char ring_id[3];
    int tcp_server_fd;
    int tcp_client_fd;
    int udp_client_fd;
}t_node_info;


void process_user_input(char *regIP, char *regUDP, t_node_info *my_node);
char* get_user_input();
void function_selector(char *buffer, char *regIP, char *regUDP, t_node_info *my_node);
t_node_info* alloc_node_memory();

#endif