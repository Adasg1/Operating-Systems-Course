CC = gcc
CFLAGS = -Wall

.PHONY: all clean
all: 
	make zad1
	make sender
	make catcher

zad1: zad1.c
	$(CC) zad1.c $(CFLAGS) -o zad1

sender: sender.c
	$(CC) sender.c $(CFLAGS) -o sender

catcher: catcher.c
	$(CC) catcher.c $(CFLAGS) -o catcher	

clean:
	-rm -f zad1	sender catcher