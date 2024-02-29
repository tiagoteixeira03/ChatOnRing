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
#include "direct_join.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

void alloc_memory(t_node_info *my_node){
    my_node->own_IP = (char*)malloc(16*sizeof(char));
    my_node->succ_IP = (char*)malloc(16*sizeof(char));
    my_node->sec_suc_IP = (char*)malloc(16*sizeof(char));
    my_node->pred_IP = (char*)malloc(16*sizeof(char));

    my_node->own_id = (char*)malloc(3*sizeof(char));
    my_node->succ_id = (char*)malloc(3*sizeof(char));
    my_node->sec_suc_id = (char*)malloc(3*sizeof(char));
    my_node->pred_id = (char*)malloc(3*sizeof(char));
    my_node->ring_id = (char*)malloc(3*sizeof(char));

    my_node->own_port = (char*)malloc(6*sizeof(char));
    my_node->succ_port = (char*)malloc(6*sizeof(char));
    my_node->sec_suc_port = (char*)malloc(6*sizeof(char));
    my_node->pred_port = (char*)malloc(6*sizeof(char));

    return;
}

int main(int argc, char* argv[]){
    char *regIP, *regUDP;
    int max_fd=0;
    fd_set current_sockets, ready_sockets;
    t_node_info *my_node;

    my_node = (t_node_info*)malloc(1*sizeof(t_node_info));

    alloc_memory(my_node);

    process_user_arguments(argc, argv, &regIP, &regUDP, my_node);
    write_option_menu();

    my_node->udp_client_fd = create_udp_client_fd();
    my_node->tcp_client_fd = create_tcp_client_fd();
    my_node->tcp_server_fd = create_tcp_server_fd(my_node);

    max_fd = MAX(my_node->udp_client_fd, my_node->tcp_client_fd);
    max_fd = MAX(max_fd, my_node->tcp_server_fd);

    FD_ZERO(&current_sockets);
    FD_SET(0, &current_sockets);
    FD_SET(my_node->udp_client_fd, &current_sockets);
    FD_SET(my_node->tcp_server_fd, &current_sockets);
    FD_SET(my_node->tcp_client_fd, &current_sockets);

    printf("Please type out a function with the formatting shown above\n\n");

    while(1){
        ready_sockets = current_sockets;

        if(select(max_fd+1, &ready_sockets, NULL, NULL, NULL) < 0){
            perror("select error");
            exit(EXIT_FAILURE);
        }
        if(FD_ISSET(0, &ready_sockets)){//input do utilizador no terminal
            process_user_input(regIP, regUDP, my_node);
            printf("Please type out a function with the formatting shown above\n\n");
        }
        if(FD_ISSET(my_node->tcp_server_fd, &ready_sockets)){//mensagem de outro utilizador
            receive_message(my_node);
            printf("Please type out a function with the formatting shown above\n\n");
        }
        FD_ZERO(&current_sockets);
        FD_SET(0, &current_sockets);
        FD_SET(my_node->udp_client_fd, &current_sockets);
        FD_SET(my_node->tcp_server_fd, &current_sockets);
        FD_SET(my_node->tcp_client_fd, &current_sockets);

    }
}