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

void print_routing_table(t_node_info *my_node, char dest[3]){
    // Print table neighbours
    printf("| %-10s | %-10s | %-10s |\n", " ", my_node->succ_id, my_node->pred_id);
    
    // Print divider
    printf("|------------|------------|------------|\n");
    
    if(strcmp(dest, "all")!=0){ //If user selects a single destination
        // Print table row
        printf("| %-10s | %-10s | %-10s |\n", dest, my_node->routing_table[atoi(dest)][atoi(my_node->succ_id)], my_node->routing_table[atoi(dest)][atoi(my_node->pred_id)]);
    }
    else{
        for(int i=0; i<100; i++){
            if(strcmp(my_node->routing_table[i][atoi(my_node->succ_id)], "-") != 0 || strcmp(my_node->routing_table[atoi(dest)][atoi(my_node->pred_id)], "-") != 0){
                // Print table row
                printf("| %-10d | %-10s | %-10s |\n", i, my_node->routing_table[i][atoi(my_node->succ_id)], my_node->routing_table[i][atoi(my_node->pred_id)]);
            }
        }
    }
}

void print_shortest_path(t_node_info *my_node, char dest[3]){
    printf("| %-10s | %-10s |\n", "Dest", "Path");
    
    // Print divider
    printf("|------------|------------|\n");
    
    if(strcmp(dest, "all")!=0){ //If user selects a single destination        
        // Print table row
        printf("| %-10s | %-10s |\n", dest, my_node->shortest_paths_table[atoi(dest)]);
    }
    else{
        for(int i=0; i<100; i++){
            if(strcmp(my_node->shortest_paths_table[i], "-")!=0){ //If there is a path
                // Print table row
                printf("| %-10d | %-10s |\n", i, my_node->shortest_paths_table[i]);
            } 
        }
    }
}

void reset_tables(t_node_info *my_node){
    for(int i=0; i<100; i++){
        for(int j=0; j<100; j++){
            strcpy(my_node->routing_table[i][j], "-");
        }
        strcpy(my_node->shortest_paths_table[i], "-");
        strcpy(my_node->forwarding_table[i], "-");
    }
}

int routing_to_shortest_paths_table(t_node_info *my_node){
    int modified = 0, succ_id = atoi(my_node->succ_id), pred_id = atoi(my_node->pred_id);

    for(int i=0; i<100; i++){
        if(strcmp(my_node->routing_table[i][succ_id], "-")==0){
            if(strcmp(my_node->routing_table[i][pred_id], "-")==0){
                continue;
            }
            else{
                if(strcmp(my_node->shortest_paths_table[i], my_node->routing_table[i][pred_id])!=0){
                    strcpy(my_node->shortest_paths_table[i], my_node->routing_table[i][pred_id]);
                    modified = 1;
                }
            }
        }
        else if(strcmp(my_node->routing_table[i][pred_id], "-")!=0){
            if(strlen(my_node->routing_table[i][pred_id]) < strlen(my_node->routing_table[i][succ_id])){
                if(strcmp(my_node->shortest_paths_table[i], my_node->routing_table[i][pred_id])!=0){
                    strcpy(my_node->shortest_paths_table[i], my_node->routing_table[i][pred_id]);
                    modified = 1;
                }
            }
            else{
                if(strcmp(my_node->shortest_paths_table[i], my_node->routing_table[i][succ_id])!=0){
                    strcpy(my_node->shortest_paths_table[i], my_node->routing_table[i][succ_id]);
                    modified = 1;
                }
            }
        }
        else{
            if(strcmp(my_node->shortest_paths_table[i], my_node->routing_table[i][succ_id])!=0){
                strcpy(my_node->shortest_paths_table[i], my_node->routing_table[i][succ_id]);
                modified = 1;
            }
        }
    }
    return modified;
}

void routing_table_init(t_node_info *my_node){
    char buffer_succ[256]="", buffer_pred[256]="", buffer_sec_succ[256]="";

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

    if(strcmp(my_node->succ_id, my_node->own_id)!=0 && strcmp(my_node->pred_id, my_node->own_id)!=0){ 
        strcpy(my_node->routing_table[atoi(my_node->succ_id)][atoi(my_node->succ_id)], buffer_succ);
        strcpy(my_node->routing_table[atoi(my_node->pred_id)][atoi(my_node->pred_id)], buffer_pred);
    }
    if(strcmp(my_node->succ_id, my_node->pred_id)!=0){ //If there are at least 3 nodes
        strcpy(my_node->routing_table[atoi(my_node->sec_suc_id)][atoi(my_node->succ_id)], buffer_sec_succ);
    }

    routing_to_shortest_paths_table(my_node); /*Update shortest paths table*/
    write_route_messages(my_node); /*Send ROUTE messages to my neighbours*/
}

char* convert_single_digit_numbers(int number){
    char *converted_number=(char*)malloc(10*sizeof(char)), string_number[3];

    sprintf(string_number, "%d", number);

    if(number<10){
        strcat(converted_number, "0");
        strcat(converted_number, string_number);
    }
    else{
        strcpy(converted_number, string_number);
    }
    return converted_number;
}

void send_route_messages(t_node_info *my_node, char buffer[512]){
    ssize_t n;

    n = write(my_node->succ_fd, buffer, strlen(buffer));
    if(n==-1) exit(1);

    n = write(my_node->pred_fd, buffer, strlen(buffer));
    if(n==-1) exit(1);
}


void write_route_messages(t_node_info *my_node){
    char buffer[512]="";

    for(int i=0; i<100; i++){
        if(strcmp(my_node->shortest_paths_table[i], "-")!=0){
            strcat(buffer, "ROUTE ");
            strcat(buffer, my_node->own_id);
            strcat(buffer, " ");
            strcat(buffer, convert_single_digit_numbers(i));
            strcat(buffer, " ");
            strcat(buffer, my_node->shortest_paths_table[i]);
            strcat(buffer, "\n");
        }
    }
    strcat(buffer, "ROUTE ");
    strcat(buffer, my_node->own_id);
    strcat(buffer, " ");
    strcat(buffer, my_node->own_id);
    strcat(buffer, " ");
    strcat(buffer, my_node->own_id);
    strcat(buffer, "\n");

    send_route_messages(my_node, buffer);
}

void process_route_messages(t_node_info *my_node, char buffer[512]){
    char *token, neighbour_id[3]="", destination_id[3]="", path[48]="", new_path[56]="";
    
    token = strtok(buffer, "\n");
    while(token != NULL){
        sscanf(token, "%*s %s %s %s", neighbour_id, destination_id, path);
        if(strstr(path, my_node->own_id) == NULL){
            sprintf(new_path, "%s-%s", my_node->own_id, path);
            strcpy(my_node->routing_table[atoi(destination_id)][atoi(neighbour_id)], new_path);
        }
        else{
            strcpy(my_node->routing_table[atoi(destination_id)][atoi(neighbour_id)], "-");
        } 
        token = strtok(NULL, "\n");
    }
    if(routing_to_shortest_paths_table(my_node)){ /*If shortest paths table changed*/
        write_route_messages(my_node); //Send my new table to my neighbours
    }
}