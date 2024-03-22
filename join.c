#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "user_input.h"
#include "join.h"
#include "handle_messages.h"
#include "routing_layer.h"

#define BUFFERSIZE 128

bool isIdUsed(int id, int used_ids[16], int n) {
    for (int i = 0; i < n; ++i) {
        if (used_ids[i] == id) {
            return true;
        }
    }
    return false;
}

int generateNewId(int* used_ids, int n) {
    int new_id;
    srand(time(NULL)); // Seed for random number generation

    // Generate random ids until finding one not used
    do {
        new_id = rand() % 99 + 1; // Generate random id between 1 and 99
    } while (isIdUsed(new_id, used_ids, n));

    return new_id;
}

int create_udp_client_fd(){
    int fd;

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    return fd;
}

void join(char *regIP, char *regUDP, t_node_info *my_node){
    int errcode, used_ids[100], nodeslist_lines=0;
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    struct addrinfo hints, *res;
    char buffer_in[BUFFERSIZE] = "REG ", buffer_out[BUFFERSIZE], node_info[10] = "NODES ", nodes_list[512]="", *token, id_check[16], nodes_list_buffer[512]="";

    for(int i=0; i<100; i++){
        used_ids[i] = 0;
    }

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
        if(strcmp(id_check, "NODESLIST")==0){
            nodeslist_lines++;
            token = strtok(NULL, "\n");
            continue;
        }
        used_ids[nodeslist_lines]=atoi(id_check);
        nodeslist_lines++;
        token = strtok(NULL, "\n");
    }

    if(isIdUsed(atoi(my_node->own_id), used_ids, nodeslist_lines)){
        printf("The ID %s is already used\n", my_node->own_id);

        int new_id = generateNewId(used_ids, nodeslist_lines);

        strcpy(my_node->own_id, convert_single_digit_numbers(new_id));

        printf("Your new ID is %s\n", my_node->own_id);
    }

    sscanf(nodes_list_buffer, "%*s %*s %s %s %s", my_node->succ_id, my_node->succ_IP, my_node->succ_port);
    if(nodeslist_lines>1){
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
        printf("Please type out a function with the formatting shown above\n\n");
    }
    else{
        printf("There was a problem connecting with the node server\n\n");
        printf("Please type out a function with the formatting shown above\n\n");
    }
    freeaddrinfo(res);
}