CC = gcc
CFLAGS = -Wall

all: client server

client: client.c
	$(CC) client.c $(CFLAGS) -o client

server: server.c
	$(CC) server.c $(CFLAGS) -o server

clean: 
	rm -f client server

.PHONY: all clean