#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#include "user_input.h"
#include "tcp_server.h"

#define BUFFERSIZE 128

int create_tcp_server_fd(t_node_info *my_node){
    int fd,errcode;
    ssize_t n;
    struct addrinfo hints,*res;

    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(my_node->own_IP, my_node->own_port, &hints, &res);
    if((errcode)!=0)/*error*/exit(1);

    n=bind(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        printf("%s\n", strerror(errno));
        exit(-1);
    }

    listen(fd,50);

    return fd;
}
