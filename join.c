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
#include "join.h"
#include "handle_messages.h"

#define BUFFERSIZE 128

int create_udp_client_fd(){
    int fd;

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    return fd;
}

void join(char *regIP, char *regUDP, t_node_info *my_node){
    int errcode, used_id[BUFFERSIZE], i=0, is_used=0, biggest_id=0, second_node=0;
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    struct addrinfo hints, *res;
    char buffer_in[BUFFERSIZE] = "REG ", buffer_out[BUFFERSIZE], node_info[10] = "NODES ", nodes_list[BUFFERSIZE], *token, id_check[3], new_id_num[2], nodes_list_buffer[BUFFERSIZE];

    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode=getaddrinfo(regIP,regUDP,&hints,&res); 
    
    if(errcode!=0) /*error*/ exit(1);

    strcat(node_info, my_node->ring_id);
    //strcat(node_info, "\n");

    n=sendto(my_node->udp_client_fd, node_info, strlen(node_info), 0, res->ai_addr, res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(my_node->udp_client_fd, nodes_list, 128, 0, (struct sockaddr*)&addr, &addrlen);
    if(n==-1) /*error*/ exit(1);

    strcpy(nodes_list_buffer, nodes_list);

    token = strtok(nodes_list, "\n");
    while(token != NULL){
        sscanf(token, "%s", id_check);
        if(strcmp(id_check, my_node->own_id)==0){
            is_used = 1;
        }
        used_id[i] = atoi(id_check);
        if(used_id[i] > biggest_id){
            biggest_id = used_id[i];
        }
        i++;
        token = strtok(NULL, "\n");
    }

    char *new_id = (char*)malloc(3*sizeof(char));

    if(is_used == 1){
        sprintf(new_id_num, "%d", biggest_id+1);
        if(strlen(new_id_num)<2){
            strcat(new_id, "0");
            strcat(new_id, new_id_num);
            strcpy(my_node->own_id, new_id);
        }
        else{
            strcpy(new_id, new_id_num);
        }
        printf("The chosen id is already in use, your new id is %s\n", my_node->own_id);
    }

    sscanf(nodes_list_buffer, "%*s %*s %s %s %s", my_node->succ_id, my_node->succ_IP, my_node->succ_port);
    if(i>1){
        if(i==3){
            second_node = 1;
        }
        my_node->no_succ = 1;
        join_node(my_node);
    }

    strcat(buffer_in, my_node->ring_id);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_id);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_IP);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_port);
    strcat(buffer_in, "\n");
    
    n=sendto(my_node->udp_client_fd,buffer_in,strlen(buffer_in),0,res->ai_addr,res->ai_addrlen);
    
    if(n==-1) /*error*/ exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(my_node->udp_client_fd,buffer_out,128,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ exit(1);

    if(strncmp(buffer_out, "OKREG", 5) == 0){
        printf("Node %s has been successfully added to ring %s\n\n", my_node->own_id, my_node->ring_id);
    }
    else{
        printf("There was a problem connecting with the node server\n\n");
    }
}
