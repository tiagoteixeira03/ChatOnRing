#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "user_input.h"
#include "join.h"
#include "leave.h"
#include "handle_messages.h"
#include "routing_layer.h"

#define BUFFERSIZE 128

int isFunction(char *abbreviated_function, char *function, char *buffer){
    char first_word[64], second_word[64], first_nd_second[128]="";

    sscanf(buffer, "%s %s", first_word, second_word);

    strcat(first_nd_second, first_word);
    strcat(first_nd_second," ");
    strcat(first_nd_second, second_word);

    if(strcmp(first_word, abbreviated_function) == 0){
        return 1; /*Input in abrreviated form*/
    }
    else if(strcmp(first_nd_second, function)==0 || strcmp(first_word, function)==0){
        return 2; /*Input in long form*/
    }
    else{
        return 0; /*Input does not match function*/
    }
}

int isJoin(char *buffer, t_node_info *my_node){
    int result;

    result = isFunction("j", "join", buffer); /*0-not join; 1-join abbreviated; 2-join long*/

    if((result == 1 || result == 2)){
        if(sscanf(buffer, "%*s %s %s", my_node->ring_id, my_node->own_id) == 2){
            strcpy(my_node->pred_id, my_node->own_id);
            strcpy(my_node->succ_id, my_node->own_id);
            strcpy(my_node->sec_suc_id, my_node->own_id); 
            if(atoi(my_node->ring_id)>999){
                printf("Ring id cant excede 3 digits\n");
            }
            else if(atoi(my_node->own_id)>99){
                printf("Your own id cant excede 2 digits\n");
            }
            else{
                return 1; /*join in abbreviated or long form is correct*/
            }
        }
    }
    return 0; /*join is not correct*/
}

int isLeave(char *buffer){
    int result;

    result = isFunction("l", "leave", buffer);

    if(result == 1){
        if(buffer[1] != '\0'){
            printf("Wrong formatting of function leave\n");
            return 0;
        }
        return 1; /*leave on short form is correct*/
    }
    else if(result == 2){
        if(buffer[6] != '\0'){
            printf("Wrong formatting of function leave\n");
            return 0;
        }
        return 1; /*leave on long form is correct*/
    }
    return 0; /*leave is not correct*/
}

int isExit(char *buffer){
    int result;

    result = isFunction("x", "exit", buffer);

    if(result == 1){
        if(buffer[1] != '\0'){
            printf("Wrong formatting of function exit\n");
            return 0;
        }
        return 1; /*exit on short form is correct*/
    }
    else if(result == 2){
        if(buffer[5] != '\0'){
            printf("Wrong formatting of function exit\n");
            return 0;
        }
        return 1; /*exit on long form is correct*/
    }
    return 0; /*exit is not correct*/
}

int isDirectjoin(char *buffer, t_node_info *my_node){
        int result;

        result = isFunction("dj", "direct join", buffer);

        if((result == 1 && sscanf(buffer, "%*s %s %s %s %s", my_node->own_id, my_node->succ_id, my_node->succ_IP, my_node->succ_port) == 4)
         || (result == 2 && sscanf(buffer, "%*s %*s %s %s %s %s", my_node->own_id, my_node->succ_id, my_node->succ_IP, my_node->succ_port) == 4)){
            if(strlen(my_node->own_id) != 2){
                printf("%s is not a permitted id\n", my_node->own_id);
                exit(1);
            }
            else if(strlen(my_node->succ_id) != 2){
                printf("%s is not a permitted id\n", my_node->succ_id);
                exit(1);
            }
            else if(strlen(my_node->succ_IP) != 9 && strlen(my_node->succ_IP) != 13){
                printf("%s is not a permitted ip\n", my_node->succ_IP);
                exit(1);
            }
            else if(strlen(my_node->succ_port) != 5){
                printf("%s is not a permitted port\n", my_node->succ_port);
                exit(1);
            }
            return 1; /*Direct join is correct*/
        }
        return 0; /*Direct join is not correct*/
}

char *isShowRouting(char *buffer){
    int result;
    char *dest = (char*)malloc(3*sizeof(char));

    result = isFunction("sr", "show routing", buffer);

    if((result == 1 && sscanf(buffer, "%*s %s", dest)) || (result==2 && scanf(buffer, "%*s %*s %s", dest))){
        return dest;
    }
    else{
        return "error";
    }
}

char *isShowPath(char *buffer){
    int result;
    char *dest = (char*)malloc(3*sizeof(char));

    result = isFunction("sp", "show path", buffer);

    if((result == 1 && sscanf(buffer, "%*s %s", dest)) || (result==2 && scanf(buffer, "%*s %*s %s", dest))){
        return dest;
    }
    else{
        return "error";
    }
}

int isShowForwarding(char *buffer){
    int result;

    result = isFunction("sf", "show forwarding", buffer);

    if(result == 1  || result == 2){
        return 1; /*Show Forwarding Form is correct*/
    }
    else{
        return 0;
    }
}

void function_selector(char *buffer, char *regIP, char *regUDP, t_node_info *my_node){
    static int on_node = 0;

    if (isJoin(buffer, my_node)){
        join(regIP, regUDP, my_node);
        on_node = 1;
    }
    else if(isLeave(buffer)){
        if(on_node == 0){
            printf("You aren't currently connected to a node, use the command join to do so\n");
            printf("Please type out a function with the formatting shown above\n\n");
        }
        else{
            leave(regIP, regUDP, my_node);
            on_node = 0;
            printf("Please type out a function with the formatting shown above\n\n");
        }
    }
    else if(isExit(buffer)){
        printf("Thank you for using this chatting app\n");
        exit(0);
    }
    else if(isDirectjoin(buffer, my_node)){
        //If the node id, IP and port art the same we create a node
        if((atoi(my_node->own_id) == atoi(my_node->succ_id)) && (strcmp(my_node->own_IP, my_node->succ_IP)==0) && strcmp(my_node->own_port, my_node->succ_port)==0){
            printf("Created a ring for this node\n");
            strcpy(my_node->sec_suc_id,my_node->own_id);
            printf("Please type out a function with the formatting shown above\n\n");
            return;
        }
        else{
            join_node(my_node);
            printf("Please type out a function with the formatting shown above\n\n");
        }
    }
    else if(isFunction("st", "show topology", buffer)){
        printf("\n");
        printf("Showing this node`s topology:\n");
        printf("My id: %s, My IP: %s, My port: %s\n", my_node->own_id, my_node->own_IP, my_node->own_port);
        printf("Sucessor id: %s, Sucessor IP: %s, Sucessor port: %s\n", my_node->succ_id, my_node->succ_IP, my_node->succ_port);
        printf("Second Sucessor id: %s, Second Sucessor IP: %s, Second Sucessor port: %s\n", my_node->sec_suc_id, my_node->sec_suc_IP, my_node->sec_suc_port);
        printf("Predecessor id: %s\n\n", my_node->pred_id);
        printf("Please type out a function with the formatting shown above\n\n");
    }
    else if(strcmp(isShowRouting(buffer), "error") != 0){
        if((strlen(isShowRouting(buffer))!=2) == (strcmp(isShowRouting(buffer), "all")!=0)){
            printf("%s is not a valid destination id\n", isShowRouting(buffer));
            printf("Please type out a function with the formatting shown above\n\n");
            return;
        }
        else{
            print_routing_table(my_node, isShowRouting(buffer));
            printf("Please type out a function with the formatting shown above\n\n");
        }
    }
    else if(strcmp(isShowPath(buffer), "error") != 0){
        if((strlen(isShowPath(buffer))!=2) == (strcmp(isShowPath(buffer), "all")!=0)){
            printf("%s is not a valid destination id\n", isShowPath(buffer));
        }
        else{
            print_shortest_path(my_node, isShowPath(buffer));
            printf("Please type out a function with the formatting shown above\n\n");
        }
    }
    else if(isShowForwarding(buffer)){
        //print forwarding table
    }
    else{
        printf("Your input is not an available function\n");
        printf("Please type out a function with the formatting shown above\n\n");
        return;
    }
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