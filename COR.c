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

int main(int argc, char* argv[]){
    char *regIP, *regUDP;
    int max_fd=0;
    fd_set current_sockets, ready_sockets;
    t_node_info* my_node = alloc_node_memory();

    process_user_arguments(argc, argv, &regIP, &regUDP, my_node);
    write_option_menu();

    my_node->udp_client_fd = create_udp_client_fd();
    my_node->tcp_client_fd = create_tcp_client_fd();
    my_node->tcp_server_fd = create_tcp_server_fd(my_node);

    max_fd = MAX(my_node->udp_client_fd, my_node->tcp_client_fd);
    max_fd = MAX(max_fd, my_node->tcp_server_fd);

    printf("Please type out a function with the formatting shown above\n\n");

    while(1){
        FD_ZERO(&ready_sockets); /*Remove all descriptors*/
        FD_SET(0, &ready_sockets); /*add descriptor 0 (stdin)*/
        FD_SET(my_node->udp_client_fd, &ready_sockets); /*add descriptor udp_client_fd*/
        FD_SET(my_node->tcp_server_fd, &ready_sockets); /*add descriptor tcp_server_fd*/
        FD_SET(my_node->tcp_client_fd, &ready_sockets); /*add descriptor tcp_client_fd*/

        if(select(max_fd+1, &ready_sockets, NULL, NULL, NULL) < 0){
            perror("select error");
            exit(EXIT_FAILURE);
        }
        if(FD_ISSET(0, &ready_sockets)){//user input on stdin
            process_user_input(regIP, regUDP, my_node);
            printf("Please type out a function with the formatting shown above\n\n");
        }
        if(FD_ISSET(my_node->tcp_server_fd, &ready_sockets)){//message from another node
            receive_message(my_node);
            printf("Please type out a function with the formatting shown above\n\n");
        }
    }
}