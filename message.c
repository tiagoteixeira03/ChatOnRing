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

#include "message.h"
#include "routing_layer.h"

void send_chat_instruction(t_node_info *my_node, char origin[3], char dest[3], char message[128]){
    char chat_instruction[512]="CHAT ";
    ssize_t n;
    int next_node_fd, next_node_id;

    shortest_paths_to_forwarding_table(my_node);

    strcat(chat_instruction, origin);
    strcat(chat_instruction, " ");
    strcat(chat_instruction, dest);
    strcat(chat_instruction, " ");
    strcat(chat_instruction, message);
    strcat(chat_instruction, "\n"); //CHAT origin_id dest_id message\n

    next_node_id = atoi(my_node->forwarding_table[atoi(dest)]);
    
    if(next_node_id == atoi(my_node->succ_id))
        next_node_fd = my_node->succ_fd;
    
    if(next_node_id == atoi(my_node->pred_id))
        next_node_fd = my_node->pred_fd;
    
    n = write(next_node_fd, chat_instruction, strlen(chat_instruction));
    if(n==-1) exit(1);
}

void receive_chat_instruction(t_node_info *my_node, char chat_instruction[512]){
    char origin[3], dest[3], message[128];

    if(sscanf(chat_instruction, "%*s %s %s %s", origin, dest, message) == 3){
        if(strcmp(dest, my_node->own_id) == 0){
            printf("Message from node %s:\n%s\n", origin, message);
        }
        else{
            send_chat_instruction(my_node, origin, dest, message);
        }
    }
    else{
        printf("Received a CHAT instruction that is incorrectly formatted\n");
    }
}