CC = gcc
CFLAGS = -Wall

all: zad1 zad2 calculate

zad1: zad1.c
	$(CC) zad1.c $(CFLAGS) -o zad1

zad2: zad2.c
	$(CC) zad2.c $(CFLAGS) -o zad2

calculate: calculate.c
	$(CC) calculate.c $(CFLAGS) -o calculate

test: zad1.c
	./zad1 0.000000002 10

clean:
	rm -f zad1 zad2 calculate

.PHONY: all clean