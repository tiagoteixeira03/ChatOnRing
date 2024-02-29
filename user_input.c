#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "user_input.h"
#include "join.h"
#include "leave.h"
#include "direct_join.h"

#define BUFFERSIZE 128

void function_selector(char *buffer, char *regIP, char *regUDP, t_node_info *my_node){
    char *function;
    static int on_node = 0;

    function = malloc(3*sizeof(char));

    sscanf(buffer, "%s", function);

    if(strcmp(function, "j") == 0){
        if(sscanf(buffer, "%*s %s %s", my_node->ring_id, my_node->own_id) != 2){
            printf("wrong formatting of function join\n");
        }
        else if(atoi(my_node->ring_id)>999){
            printf("Ring id cant excede 3 digits\n");
        }
        else if(atoi(my_node->own_id)>99){
            printf("Your own id cant excede 2 digits\n");
        }
        else{
            join(regIP, regUDP, my_node);
            on_node = 1;
        }
    }
    else if(strcmp(function, "l") == 0){
        if(buffer[1] != '\0'){
            printf("wrong formatting of fucntion leave\n");
        }
        else if(on_node == 0){
            printf("You aren't currently connected to a node, use the command join to do so\n");
        }
        else{
            leave(regIP, regUDP, my_node);
            on_node = 0;
        }
    }
    else if(strcmp(function, "x") == 0){
        printf("Thank you for using this chatting app :3\n");
        exit(0);
    }
    else if(strcmp(function, "dj") == 0){
        if(sscanf(buffer, "%*s %s %s %s %s", my_node->own_id, my_node->succ_id, my_node->succ_IP, my_node->succ_port) != 4){
            printf("wrong formatting of function direct join\n");
        }
        else if(strlen(my_node->own_id) != 2){
            printf("%s is not a permitted id\n", my_node->own_id);
        }
        else if(strlen(my_node->succ_id) != 2){
            printf("%s is not a permitted id\n", my_node->succ_id);
        }
        else if(strlen(my_node->succ_IP) != 9 && strlen(my_node->succ_IP) != 13){
            printf("%s is not a permitted ip\n", my_node->succ_IP);
        }
        else if(strlen(my_node->succ_port) != 5){
            printf("%s is not a permitted port\n", my_node->succ_port);
        }
        //If the node id, IP and port art the same we create a node
        else if((atoi(my_node->own_id) == atoi(my_node->succ_id)) && (strcmp(my_node->own_IP, my_node->succ_IP)==0) && strcmp(my_node->own_port, my_node->succ_port)==0){
            printf("Created a ring for this node\n");
            strcpy(my_node->sec_suc_id,my_node->own_id);
            return;
        }
        else{
            direct_join(my_node);
        }
        strcpy(my_node->sec_suc_id,my_node->own_id);
    }
    else if(strcmp(function, "st") == 0){
        printf("Showing this node`s topology:\n");
        printf("My id: %s, My IP: %s, My port: %s\n", my_node->own_id, my_node->own_IP, my_node->own_port);
        printf("Sucessor id: %s, Sucessor IP: %s, Sucessor port: %s\n", my_node->succ_id, my_node->succ_IP, my_node->succ_port);
        printf("Second Sucessor id: %s, Second Sucessor IP: %s, Second Sucessor port: %s\n", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port);
        printf("Predecessor id: %s, Predecessor IP: %s, Predecessor port: %s\n", my_node->pred_id, my_node->pred_IP, my_node->pred_port);
    }
    else{
        printf("%s is not an available function\n", function);
        return;
    }

    free(function);
}

char* get_user_input(){
    char *buffer;
    int input_size = 0;

    buffer = malloc(BUFFERSIZE*sizeof(char));
    
    fgets(buffer, BUFFERSIZE, stdin);
    
    input_size = strlen(buffer);
    
    buffer[input_size-1] = '\0';

    return buffer;
}

void process_user_input(char *regIP, char *regUDP, t_node_info *my_node){
    char *buffer;

    buffer = get_user_input();
    function_selector(buffer, regIP, regUDP, my_node);

    free(buffer);

    return;

}