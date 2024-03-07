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
    fd_set ready_sockets;
    struct addrinfo hints,*res;
    struct timeval tv;
    char buffer_in[128] = "ENTRY ", buffer_out[128], *function = NULL;

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    function = (char*)malloc(8*sizeof(char));

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

    n=read(my_node->tcp_client_fd, buffer_out,128);
    if(n==-1){
        perror("Erro read dj\n");
        /*error*/exit(1);
    }

    if(strcmp(function, "SUCC") == 0){
        if(sscanf(buffer_out, "%*s %s %s %s", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port) != 3){
            printf("Error joing the node\n");
        }
        else{
            printf("You have a new second succesor\n");
        }
    }

    strcpy(my_node->pred_id, my_node->succ_id);
    strcpy(my_node->pred_IP, my_node->succ_IP);
    strcpy(my_node->pred_port, my_node->succ_port);

    freeaddrinfo(res);
}

void receive_message(t_node_info *my_node){
    char buffer[128], *function = NULL, buffer_out[128] = "SUCC ";
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

    strcat(buffer_out, my_node->succ_id);
    strcat(buffer_out, " ");
    strcat(buffer_out, my_node->succ_IP);
    strcat(buffer_out, " ");
    strcat(buffer_out, my_node->succ_port);
    strcat(buffer_out, "\n");

    n=write(newfd, buffer_out, sizeof(buffer_out));
    if(n==-1){
        perror("Erro direct join\n");
        /*error*/exit(1);
    }
}