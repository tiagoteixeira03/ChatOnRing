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

#include "routing_layer.h"
#include "user_input.h"

void print_routing_table(t_node_info *my_node){
    for(int i=0; i<5; i++){
        for(int j=0; j<5; j++){
            printf("%s |", my_node->routing_table[i][j]);
        }
        printf("\n");
    }
}

void reset_routing_table(t_node_info *my_node){
    for(int i=0; i<100; i++){
        for(int j=0; j<100; j++){
            strcpy(my_node->routing_table[i][j], "-");
        }
    }
}

void routing_table_init(t_node_info *my_node){
    char buffer_succ[256], buffer_pred[256], buffer_sec_succ[256];

    reset_routing_table(my_node);
    
    strcat(buffer_succ, my_node->own_id);
    strcat(buffer_succ, "-");
    strcat(buffer_succ, my_node->succ_id);

    strcat(buffer_pred, my_node->own_id);
    strcat(buffer_pred, "-");
    strcat(buffer_pred, my_node->pred_id);

    strcat(buffer_sec_succ, my_node->own_id);
    strcat(buffer_sec_succ, "-");
    strcat(buffer_sec_succ, my_node->succ_id);
    strcat(buffer_sec_succ, "-");
    strcat(buffer_sec_succ, my_node->sec_suc_id);

    if(strcmp(my_node->succ_id, my_node->own_id)!=0 && strcmp(my_node->pred_id, my_node->own_id)!=0){ //If I'm not alone in the ring
        strcpy(my_node->routing_table[atoi(my_node->succ_id)][atoi(my_node->succ_id)], buffer_succ);
        strcpy(my_node->routing_table[atoi(my_node->pred_id)][atoi(my_node->pred_id)], buffer_pred);
    }
    if(strcmp(my_node->succ_id, my_node->pred_id)!=0){ //If I'm not alone or with just one more node
        strcpy(my_node->routing_table[atoi(my_node->sec_suc_id)][atoi(my_node->succ_id)], buffer_sec_succ);
        print_routing_table(my_node);
    }
}