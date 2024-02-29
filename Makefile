#Makefile 

.DEFAULT_GOAL := COR

CC = gcc
CFLAGS= -Wall -g #Change to submission flags before final submission

COR:
	$(CC) $(CFLAGS) COR.c menu.c join.c leave.c user_input.c user_options.c tcp_client.c tcp_server.c direct_join.c -o COR

clean:
	rm COR

