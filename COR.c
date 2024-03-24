#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#include "user_input.h"
#include "menu.h"
#include "user_options.h"
#include "join.h"
#include "tcp_client.h"
#include "tcp_server.h"
#include "handle_messages.h"
#include "routing_layer.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

int main(int argc, char* argv[]){
    char *regIP, *regUDP;
    int max_fd=0;
    fd_set ready_sockets;
    t_node_info* my_node = malloc(1*sizeof(t_node_info));

    reset_tables(my_node);

    my_node->succ_fd=0;
    my_node->pred_fd=0;
    my_node->node_just_left=0;

    process_user_arguments(argc, argv, &regIP, &regUDP, my_node);
    write_option_menu();

    my_node->udp_client_fd = create_udp_client_fd();
    my_node->tcp_server_fd = create_tcp_server_fd(my_node);

    while(1){
        max_fd = 0;
        max_fd = MAX(my_node->udp_client_fd, my_node->succ_fd);
        max_fd = MAX(max_fd, my_node->pred_fd);
        max_fd = MAX(max_fd, my_node->tcp_server_fd);
        max_fd = MAX(max_fd, my_node->succ_fd);
        max_fd = MAX(max_fd, my_node->pred_fd);
        FD_ZERO(&ready_sockets); /*Remove all descriptors*/
        FD_SET(0, &ready_sockets); /*add descriptor 0 (stdin)*/
        FD_SET(my_node->udp_client_fd, &ready_sockets); /*add descriptor udp_client_fd*/
        FD_SET(my_node->tcp_server_fd, &ready_sockets); /*add descriptor tcp_server_fd*/
        if(my_node->succ_fd>0){
            FD_SET(my_node->succ_fd, &ready_sockets); /*add descriptor succ_fd*/
        }
        if(my_node->pred_fd>0){
            FD_SET(my_node->pred_fd, &ready_sockets); /*add descriptor pred_fd*/
        }

        if(select(max_fd+1, &ready_sockets, NULL, NULL, NULL) < 0){
            perror("select error");
            exit(EXIT_FAILURE);
        }
        if(FD_ISSET(0, &ready_sockets)){//user input on stdin
            process_user_input(regIP, regUDP, my_node);
        }
        if(FD_ISSET(my_node->tcp_server_fd, &ready_sockets)){//message from a new node
            receive_message(my_node);
            continue;
        }
        if(my_node->succ_fd>0){
            if(FD_ISSET(my_node->succ_fd, &ready_sockets)){//message from the successor
                receive_from_succ(my_node);
                continue;
            }
        }
        if(my_node->pred_fd>0){
            if(FD_ISSET(my_node->pred_fd, &ready_sockets)){//message from the successor(In this case its when it closes)
                receive_from_pred(my_node);
                continue;
            }
        }
    }
}