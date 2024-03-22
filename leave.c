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
#include "leave.h"

#define BUFFERSIZE 128

void leave(char *regIP, char *regUDP, t_node_info *my_node){
    int errcode;
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    struct addrinfo hints, *res;
    char buffer_in[BUFFERSIZE] = "UNREG ", buffer_out[BUFFERSIZE];

    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode=getaddrinfo(regIP,regUDP,&hints,&res);
    
    if(errcode!=0) /*error*/ exit(1);

    strcat(buffer_in, my_node->ring_id);
    strcat(buffer_in, " ");
    strcat(buffer_in, my_node->own_id);
    strcat(buffer_in, "\n");
    
    n=sendto(my_node->udp_client_fd,buffer_in,12,0,res->ai_addr,res->ai_addrlen);
    
    if(n==-1) /*error*/ exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(my_node->udp_client_fd,buffer_out,128,0,(struct sockaddr*)&addr,&addrlen);
    
    if(n==-1) /*error*/ exit(1);

    if(strncmp(buffer_out, "OKUNREG", 5) == 0){
        printf("Node %s has been successfully removed from ring %s\n\n", my_node->own_id, my_node->ring_id);
    }
    else{
        printf("There was a problem connecting with the node server\n\n");
    }

    if(my_node->pred_fd>0){
        close(my_node->pred_fd);
        my_node->pred_fd=0;
    }
    if(my_node->succ_fd>0){
        close(my_node->succ_fd);
        my_node->succ_fd=0;
    }

    freeaddrinfo(res);

    return;
}