#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#include "tcp_client.h"

#define BUFFERSIZE 128

int create_tcp_client_fd(){
    int fd;

    fd=socket(AF_INET,SOCK_STREAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    return fd;
}
