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
#include "routing_layer.h"
#include "message.h"

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

    n=write(my_node->succ_fd, buffer_in, sizeof(buffer_in)); //"ENTRY i i.IP i.TCP"
    if(n==-1)/*error*/exit(1);
 
    n=read(my_node->succ_fd, buffer_out,sizeof(buffer_out)-1);
    if(n==-1)/*error*/exit(1);
    buffer_out[n]='\0';


    sscanf(buffer_out, "%s", function);

    if(strcmp(function, "SUCC") == 0){
        if(sscanf(buffer_out, "%*s %s %s %s", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port) != 3){
            printf("Error joing the node\n\n");
        }
        else{
            printf("You have a new second succesor\n\n");
        }
    }
    if(strcmp(my_node->sec_suc_id, my_node->own_id)==0){ //There's only one more node besides me in this ring
        strcpy(my_node->pred_id, my_node->succ_id);
        strcpy(my_node->pred_IP, my_node->succ_IP);
        strcpy(my_node->pred_port, my_node->succ_port);
        my_node->pred_fd = my_node->succ_fd;
    }
    freeaddrinfo(res);
    free(function);
}

void joining_node(t_node_info *my_node, int newfd, char buffer[128]){
    char buffer_out[128] = "SUCC ";
    ssize_t n;
    int second_node=0;

    if(strcmp(my_node->pred_id, my_node->own_id)==0 && strcmp(my_node->succ_id, my_node->own_id)==0 ){
        my_node->succ_fd = newfd;

        second_node = 1; //I was alone in the ring and a second_node is joining me
    }

    if(sscanf(buffer, "%*s %s %s %s", my_node->pred_id, my_node->pred_IP, my_node->pred_port) != 3){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n\n");
    }
    else{
        printf("The node %s with ip: %s and port: %s, has joined your ring\n\n", my_node->pred_id, my_node->pred_IP, my_node->pred_port);
    }  

    if(second_node==1){ /*If im alone and a second node joins as my predecessor, my successor will also be this new node*/
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
    
    if(second_node!=1){ /*If I wasn't alone before this new node joined*/
        update_sucessor(my_node);
    }
    my_node->pred_fd = newfd;
    
    if(second_node==1){
        routing_table_init(my_node);
    }
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

    n=write(my_node->pred_fd, buffer_in, sizeof(buffer_in)); //"ENTRY i i.IP i.TCP" is sent to my old predecessor before the new node joined
    if(n==-1)/*error*/exit(1);
}

void receive_from_succ(t_node_info *my_node){
    char buffer[512]="", *function=NULL;
    ssize_t n;

    function = (char*)calloc(6,sizeof(char));

    n=read(my_node->succ_fd, buffer, sizeof(buffer) - 1);
    if(n==-1)/*error*/exit(1);

    buffer[n]='\0';

    if(n == 0){
        my_node->node_just_left=1;
        delete_node_from_tables(my_node, my_node->succ_id);
        write_route_messages(my_node, my_node->succ_id);
        close(my_node->succ_fd);
        node_left(my_node);
        return;
    }

    sscanf(buffer, "%s", function); //Get only first word from buffer

    if(strncmp(function, "SUCC", 5)==0){
        new_sec_succ(my_node, buffer); /*If I receive a SUCC from my SUCC means that my sucessor is informing me of my SEC Succ*/
    }
    else if(strncmp(function, "ENTRY", 6)==0){ /*If I receive a ENTRY from my SUCC means that im being informed that my SUCC and SEC SUCC are going to change*/
        new_succ(my_node, buffer);
    }
    else if(strncmp(function, "ROUTE", 6)==0){
        process_route_messages(my_node, buffer);
    }
    else if(strncmp(function, "CHAT", 5)==0){
        receive_chat_instruction(my_node, buffer);
    }
    free(function);
}

void receive_from_pred(t_node_info *my_node){
    char buffer[512]="", *function=NULL;
    ssize_t n;

    function = (char*)calloc(6,sizeof(char));

    n=read(my_node->pred_fd, buffer, sizeof(buffer) - 1);
    if(n==-1)/*error*/exit(1);

    buffer[n] = '\0';

    if(n==0){
        delete_node_from_tables(my_node, my_node->pred_id);
        write_route_messages(my_node, my_node->pred_id);
        my_node->node_just_left=1;
        close(my_node->pred_fd);
        my_node->pred_fd=0;
        return;
    }

    sscanf(buffer, "%s", function); //Get only first word from buffer

    if(strncmp(function, "ROUTE", 6)==0){
        process_route_messages(my_node, buffer);
    }
    else if(strncmp(function, "CHAT", 5)==0){
        receive_chat_instruction(my_node, buffer);
    }
    free(function);
}

void new_succ(t_node_info *my_node, char buffer[128]){

    strcpy(my_node->sec_suc_id, my_node->succ_id);
    strcpy(my_node->sec_suc_IP, my_node->succ_IP);
    strcpy(my_node->sec_suc_port, my_node->succ_port);
    if(sscanf(buffer, "%*s %s %s %s", my_node->succ_id, my_node->succ_IP, my_node->succ_port) != 3){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n\n");
    }
    else{
        warn_pred(my_node);
        warn_sec_succ(my_node);
        printf("New Sucessor %s\n\n", my_node->succ_id);           
    }   
}

void new_sec_succ(t_node_info *my_node, char buffer[128]){
    if(sscanf(buffer, "%*s %s %s %s", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port) != 3){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n\n");
    }
    else{
        printf("New second succesor %s\n\n", my_node->sec_suc_id);           
    }
    if(my_node->node_just_left==1){
        routing_table_init(my_node);
        my_node->node_just_left = 0;
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

    freeaddrinfo(res);
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
    char buffer[128], *function = NULL;
    int newfd;
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;

    function = (char*)malloc(8*sizeof(char));

    addrlen=sizeof(addr);
    if((newfd=accept(my_node->tcp_server_fd,(struct sockaddr*)&addr,&addrlen))==-1)/*error*/ exit(1);

    n=read(newfd, buffer,sizeof(buffer)-1);
    if(n==-1)/*error*/exit(1);
    buffer[n]='\0';

    sscanf(buffer, "%s", function); //Get only first word from buffer

    if(strncmp(function, "ENTRY", 6)==0){
        joining_node(my_node, newfd, buffer);
    }
    else if(strncmp(function, "PRED", 5)==0){
        new_pred(my_node, newfd, buffer);
        routing_table_init(my_node);
    }
    free(function);
    return;
}

void new_pred(t_node_info *my_node, int newfd, char buffer[128]){
    char buffer_out[128] = "SUCC ";

    if(sscanf(buffer, "%*s %s", my_node->pred_id) != 1){
        printf("An atempt at joing your ring chat was made but it failed due to bad formatting\n\n");
    }
    else{
        printf("The node %s is your new pred\n\n", my_node->pred_id);
        my_node->pred_fd = newfd;
    }

    if(my_node->node_just_left == 1){
        ssize_t n;

        strcat(buffer_out, my_node->succ_id);
        strcat(buffer_out, " ");
        strcat(buffer_out, my_node->succ_IP);
        strcat(buffer_out, " ");
        strcat(buffer_out, my_node->succ_port);
        strcat(buffer_out, "\n");

        n=write(my_node->pred_fd, buffer_out, sizeof(buffer_out));
        if(n==-1)/*error*/exit(1);

        routing_table_init(my_node);
        my_node->node_just_left = 0;
    }
}

void node_left(t_node_info *my_node){
    int errcode;
    ssize_t n;
    struct addrinfo hints,*res;
    char buffer_pred[128] = "PRED ", buffer_succ[128] = "SUCC ";

    my_node->succ_fd = create_tcp_client_fd();

    memset(&hints,0,sizeof hints);

    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(my_node->sec_suc_IP, my_node->sec_suc_port, &hints, &res);
    if(errcode!=0)/*error*/exit(1);

    n=connect(my_node->succ_fd,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/exit(1);

    strcpy(my_node->succ_id, my_node->sec_suc_id);
    strcpy(my_node->succ_IP, my_node->sec_suc_IP);
    strcpy(my_node->succ_port, my_node->sec_suc_port);

    strcat(buffer_pred, my_node->own_id);
    strcat(buffer_pred, "\n");

    n=write(my_node->succ_fd, buffer_pred, sizeof(buffer_pred));
    if(n==-1)/*error*/exit(1);

    strcat(buffer_succ, my_node->sec_suc_id);
    strcat(buffer_succ, " ");
    strcat(buffer_succ, my_node->sec_suc_IP);
    strcat(buffer_succ, " ");
    strcat(buffer_succ, my_node->sec_suc_port);
    strcat(buffer_succ, "\n");

    n=write(my_node->pred_fd, buffer_succ, sizeof(buffer_succ));
    if(n==-1)/*error*/exit(1);

    freeaddrinfo(res);
}