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
#include "direct_join.h"

void direct_join(t_node_info *my_node){
    int errcode;
    ssize_t n;
    struct addrinfo hints,*res;
    char buffer_in[128] = "ENTRY ";

    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(my_node->succ_IP, my_node->succ_port, &hints, &res);
    if(errcode!=0)/*error*/exit(1);

    n=connect(my_node->tcp_client_fd,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/exit(1);

    strcat(buffer_in, my_node->own_id);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_IP);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_port);
    strcat(buffer_in, "\n");

    n=write(my_node->tcp_client_fd, buffer_in, sizeof(buffer_in));
    if(n==-1)/*error*/exit(1);

    strcpy(my_node->pred_id, my_node->succ_id);
    strcpy(my_node->pred_IP, my_node->succ_IP);
    strcpy(my_node->pred_port, my_node->succ_port);

    freeaddrinfo(res);
}

void receive_message(t_node_info *my_node){
    char buffer[128], *function = NULL;
    int newfd;
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;

    function = (char*)malloc(8*sizeof(char));

    addrlen=sizeof(addr);
    if((newfd=accept(my_node->tcp_server_fd,(struct sockaddr*)&addr,&addrlen))==-1)/*error*/ exit(1);

    n=read(newfd, buffer,128);
    if(n==-1)/*error*/exit(1);

    sscanf(buffer, "%s", function); //Get only first word from buffer

    if(strcmp(function, "ENTRY") == 0){
        if(sscanf(buffer, "%*s %s %s %s", my_node->pred_id, my_node->pred_IP, my_node->pred_port) != 3){
            printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n");
        }
        else{
            printf("The node %s with ip: %s and port: %s, has joined your ring\n", my_node->pred_id, my_node->pred_IP, my_node->pred_port);
        }
    }
    strcpy(my_node->succ_id, my_node->pred_id);
    strcpy(my_node->succ_IP, my_node->pred_IP);
    strcpy(my_node->succ_port, my_node->pred_port);
}