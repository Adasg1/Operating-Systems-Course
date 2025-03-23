CC = gcc
CFLAGS = -Wall -O0

all: libcollatz.a libcollatz.so client-static client-shared client-dynamic

#kompilacja biblioteki statycznej
libcollatz.a: collatz.c
	$(CC) -c collatz.c
	ar -rcs libcollatz.a collatz.o 
	rm collatz.o

#kompilacja biblioteki współdzielonej
libcollatz.so: collatz.c
	$(CC) -fPIC -c collatz.c
	$(CC) -shared collatz.o -o libcollatz.so
	rm collatz.o

#użycie trzech bibliotek
client-static: client.c libcollatz.a
	$(CC) $(CFLAGS) client.c libcollatz.a -o client-static

client-shared: client.c libcollatz.so
	$(CC) $(CFLAGS) client.c -L. -lcollatz -Wl,-rpath,. -o client-shared

client-dynamic: client.c libcollatz.so
	$(CC) $(CFLAGS) client.c -D DYNAMIC=1 -o client-dynamic 

#usuwanie plików wykonywalnych
clean:
	 rm -f libcollatz.a libcollatz.so client-static client-shared client-dynamic

.PHONY: all clean
