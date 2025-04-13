CC = gcc
CFLAGS = -Wall

.PHONY: all clean
all: 
	make globals pids

globals: globals.c
	$(CC) globals.c $(CFLAGS) -o globals

pids: pids.c
	$(CC) pids.c $(CFLAGS) -o pids

clean:
	-rm -f globals pids
