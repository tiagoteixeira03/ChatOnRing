#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "menu.h"

void write_option_menu(){
    printf("Welcome to this chat room!\n");
    printf("-------------------------------------------------------------\n");
    printf("Here is a list of the several commands that you are able to use:\n");
    printf("- join (j) ring id\n");
    printf("- direct join (dj) id succid succIP succTCP\n");
    printf("- chord (c)\n");
    printf("- remove chord (rc)\n");
    printf("- show topology (st)\n");
    printf("- show routing (sr) dest\n");/* n sei se se escreve assim*/ 
    printf("- show path (sp) dest\n");
    printf("- show forwarding (sf)\n");
    printf("- message (m) dest message\n");
    printf("- leave (l)\n");
    printf("- exit (x)\n");

    return;
}