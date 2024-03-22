#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "user_input.h"
#include "user_options.h"

int countDots(const char *str) {
    int count = 0;
    while (*str != '\0') {
        if (*str == '.') {
            count++;
        }
        else if (*str < '0' || *str > '9'){
            return -1; /*There is a letter in the IP*/
        }
        str++; // Move to the next character
    }
    return count;
}

void process_user_arguments(int argc, char *argv[], char **regIP, char **regUDP, t_node_info *my_node){

    int dotsCount = 0;
  
    if(argc < 3 || argc > 5){
        printf("Invalid arguments, correct usage is:\n./COR IP TCP regIP regUDP or ./COR IP TCP\n");
        exit(1);
    }

    dotsCount = countDots(argv[1]);

    if (dotsCount == -1 || dotsCount != 3){
        printf("Invalid IP adress\n");
        exit(1);
    }

    if (atoi(argv[2]) > 65535 || atoi(argv[2]) < 49152){
        printf("Invalid port number\n");
        exit(1);
    }

    *regIP = malloc(16*sizeof(char));
    *regUDP = malloc(6*sizeof(char));

    strcpy(my_node->own_IP, argv[1]);
    strcpy(my_node->own_port, argv[2]);
    strcpy(my_node->sec_suc_IP, argv[1]);
    strcpy(my_node->sec_suc_port, argv[2]);
    strcpy(my_node->succ_IP, argv[1]);
    strcpy(my_node->succ_port, argv[2]);
    strcpy(my_node->pred_IP, argv[1]);
    strcpy(my_node->pred_port, argv[2]);

    if(argc >= 4){ /*If user defines at least regIP*/
        if(argc == 5 && (countDots(argv[3]) != -1 && countDots(argv[3]) == 3) && (atoi(argv[4]) >= 49152 && atoi(argv[4]) <= 65535)){ /*If user correctly defines both regIP and regUDP*/
            strcpy(*regIP, argv[3]);
            strcpy(*regUDP, argv[4]);
        }
        else if (countDots(argv[3]) != -1 && countDots(argv[3]) == 3){ /*If user defines only regIP and correctly*/
           strcpy(*regIP, argv[3]); 
        }
        else{
            printf("Invalid regIP or regUDP values\n");
            exit(1);
        }
    }
    else{ /*If user doesn't define neither regIP nor regUDP*/
        strcpy(*regIP, "tejo.tecnico.ulisboa.pt");//192.168.1.1
        strcpy(*regUDP, "59000");
    }
}