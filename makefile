CC = gcc
CFLAGS = -I.
EXEC = main

$(EXEC): main.c
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(EXEC) main.o *~ core
