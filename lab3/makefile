sources = flipper
CC = gcc
CFLAGS = -Wall

.PHONY: all
all :
	make flipper

.PHONY: run
run :
	make flipper
	./flipper ./art ./flipped_art

#dwukrotne odwracanie
.PHONY: run-double
run-double :
	make flipper
	./flipper ./art ./flipped_art         # Pierwsze odwrócenie
	./flipper ./flipped_art ./art_restored # Drugie odwrócenie

flipper : flipper.c
	$(CC) flipper.c $(CFLAGS) -o flipper

.PHONY: clean
clean :
	-rm $(sources) ./flipped_art/* ./art_restored/*
	-rmdir ./flipped_art ./art_restored