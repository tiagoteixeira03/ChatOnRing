#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#include "user_input.h"
#include "handle_messages.h"
#include "tcp_client.h"

void join_node(t_node_info *my_node){
    char buffer_in[128] = "ENTRY ", buffer_out[128], *function;
    ssize_t n;
    struct addrinfo hints,*res;
    int errcode;

    my_node->succ_fd = create_tcp_client_fd();

    function = (char*)malloc(8*sizeof(char));

    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(my_node->succ_IP, my_node->succ_port, &hints, &res);
    if(errcode!=0)/*error*/exit(1);

    n=connect(my_node->succ_fd,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/exit(1);

    strcat(buffer_in, my_node->own_id);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_IP);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_port);
    strcat(buffer_in, "\n");

    n=write(my_node->succ_fd, buffer_in, sizeof(buffer_in));
    if(n==-1)/*error*/exit(1);
 
    n=read(my_node->succ_fd, buffer_out,128);
    if(n==-1)/*error*/exit(1);

    sscanf(buffer_out, "%s", function);

    if(strcmp(function, "SUCC") == 0){
        if(sscanf(buffer_out, "%*s %s %s %s", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port) != 3){
            printf("Error joing the node\n");
        }
        else{
            printf("You have a new second succesor\n");
        }
    }
    if(strcmp(my_node->sec_suc_id, my_node->own_id)==0){
        strcpy(my_node->pred_id, my_node->succ_id);
        strcpy(my_node->pred_IP, my_node->succ_IP);
        strcpy(my_node->pred_port, my_node->succ_port);
        my_node->pred_fd = my_node->succ_fd;
    }
}

void joining_node(t_node_info *my_node, int newfd, char buffer[128]){
    char buffer_out[128] = "SUCC ";
    ssize_t n;
    int second_node=0;

    if(strcmp(my_node->pred_id, my_node->own_id)==0 && strcmp(my_node->succ_id, my_node->own_id)==0 ){
        my_node->succ_fd = newfd;

        second_node = 1;
    }

    if(sscanf(buffer, "%*s %s %s %s", my_node->pred_id, my_node->pred_IP, my_node->pred_port) != 3){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n");
    }
    else{
        printf("The node %s with ip: %s and port: %s, has joined your ring\n", my_node->pred_id, my_node->pred_IP, my_node->pred_port);
    }  

    if(second_node==1){
        strcpy(my_node->succ_IP, my_node->pred_IP);
        strcpy(my_node->succ_id, my_node->pred_id);
        strcpy(my_node->succ_port, my_node->pred_port);
    }

    strcat(buffer_out, my_node->succ_id);
    strcat(buffer_out, " ");
    strcat(buffer_out, my_node->succ_IP);
    strcat(buffer_out, " ");
    strcat(buffer_out, my_node->succ_port);
    strcat(buffer_out, "\n");

    n=write(newfd, buffer_out, sizeof(buffer_out));
    if(n==-1)/*error*/exit(1);
    
    if(second_node!=1){
        update_sucessor(my_node);
    }
    my_node->pred_fd = newfd;
}

void update_sucessor(t_node_info *my_node){
    char buffer_in[128] = "ENTRY ";
    ssize_t n;

    strcat(buffer_in, my_node->pred_id);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->pred_IP);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->pred_port);
    strcat(buffer_in, "\n");

    n=write(my_node->pred_fd, buffer_in, sizeof(buffer_in));
    if(n==-1)/*error*/exit(1);
}

void receive_from_succ(t_node_info *my_node){
    char buffer[128], *function;
    ssize_t n;

    function = (char*)malloc(6*sizeof(char));

    n=read(my_node->succ_fd, buffer,128);
    if(n==-1)/*error*/exit(1);

    sscanf(buffer, "%s", function); //Get only first word from buffer

    if(strncmp(function, "SUCC", 5)==0){
        new_sec_succ(my_node, buffer);
    }
    else if(strncmp(function, "ENTRY", 5)==0){
        new_succ(my_node, buffer);
    }
}

void new_succ(t_node_info *my_node, char buffer[128]){

    strcpy(my_node->sec_suc_id, my_node->succ_id);
    strcpy(my_node->sec_suc_IP, my_node->succ_IP);
    strcpy(my_node->sec_suc_port, my_node->succ_port);
    if(sscanf(buffer, "%*s %s %s %s", my_node->succ_id, my_node->succ_IP, my_node->succ_port) != 3){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n");
    }
    else{
        warn_pred(my_node);
        warn_sec_succ(my_node);
        printf("The node %s with ip: %s and port: %s, has joined your ring\n", my_node->succ_id, my_node->succ_IP, my_node->succ_port);           
    }  
       
}

void new_sec_succ(t_node_info *my_node, char buffer[128]){
    if(sscanf(buffer, "%*s %s %s %s", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port) != 3){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n");
    }
    else{
        printf("The node %s with ip: %s and port: %s, has joined your ring\n", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port);           
    }  
}

void warn_pred(t_node_info *my_node){
    int errcode;
    ssize_t n;
    struct addrinfo hints,*res;
    char buffer_in[128] = "PRED ";

    my_node->succ_fd = create_tcp_client_fd();

    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(my_node->succ_IP, my_node->succ_port, &hints, &res);
    if(errcode!=0)/*error*/exit(1);

    n=connect(my_node->succ_fd,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/exit(1);

    strcat(buffer_in, my_node->own_id);
    strcat(buffer_in, "\n");

    n=write(my_node->succ_fd, buffer_in, sizeof(buffer_in));
    if(n==-1)/*error*/exit(1);
}

void warn_sec_succ(t_node_info *my_node){
    ssize_t n;
    char buffer_in[128] = "SUCC ";

    strcat(buffer_in, my_node->succ_id);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->succ_IP);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->succ_port);
    strcat(buffer_in, "\n");

    n=write(my_node->pred_fd, buffer_in, sizeof(buffer_in));
    if(n==-1)/*error*/exit(1);
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

    if(strncmp(function, "ENTRY", 6)==0){
        joining_node(my_node, newfd, buffer);
    }
    else if(strncmp(function, "PRED", 5)==0){
        new_pred(my_node, newfd, buffer);
    }

    return;
}

void new_pred(t_node_info *my_node, int newfd, char buffer[128]){
    char *function = NULL, buffer_out[128] = "SUCC ";
    ssize_t n;

    if(sscanf(buffer, "%*s %s", my_node->pred_id) != 1){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n");
    }
    else{
        printf("The node %s has joined your ring\n", my_node->pred_id);
        my_node->pred_fd = newfd;
    }  
}