CC = gcc
CFLAGS = -Wall -g -std=c17

all : countdown

countdown : countdown.c
	$(CC) $(CFLAGS) countdown.c -o countdown

clean :
	rm -f countdown *-debug.out

.PHONY: all countdown clean