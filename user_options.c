#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "user_input.h"
#include "user_options.h"

void process_user_arguments(int argc, char *argv[], char **regIP, char **regUDP, t_node_info *my_node){
  
    if(argc<3 || argc > 5){
        printf("Invalid arguments, correct usage is:\n./COR IP TCP regIP regUDP or ./COR IP TCP\n");
        exit(1);
    }

    if (strlen(argv[1]) != 9 && strlen(argv[1]) != 13){
        printf("Invalid IP adress\n");
        exit(1);
    }

    if (atoi(argv[2])>65535 || atoi(argv[2])<49152){
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

    if (argc >= 4){ /*If user defines at least regIP*/
        if(argc == 5 && strlen(argv[3])==15 && strlen(argv[4])==15){ /*If user correctly defines both regIP and regUDP*/
            strcpy(*regIP, argv[3]);
            strcpy(*regUDP, argv[4]);
        }
        else if (strlen(argv[3])==15){ /*If user defines only regIP and correctly*/
           strcpy(*regIP, argv[3]); 
        }
        else{
            printf("Invalid regIP or regUDP values (must be XXX.XXX.XXX.XXX and XXXXX, respectively)\n");
            exit(1);
        }
    }
    else{ /*If user doesn't define neither regIP nor regUDP*/
        strcpy(*regIP, "tejo.tecnico.ulisboa.pt");//192.168.1.1
        strcpy(*regUDP, "59000");
    }
}