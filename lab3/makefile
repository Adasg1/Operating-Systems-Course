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
	./flipper ./art ./flipped_art
	./flipper ./flipped_art ./restored_art # Drugie odwrócenie

flipper : flipper.c
	$(CC) flipper.c $(CFLAGS) -o flipper

.PHONY: clean
clean :
	-rm -r ./flipped_art ./restored_art
	-rm flipper