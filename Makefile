CC=gcc
CFLAGS=-Wall -std=gnu99
EXECUTABLES=master child

all: $(EXECUTABLES)

master: src/master.c
	mkdir -p bin/
	$(CC) $(CFLAGS) $< -o bin/$@
child:src/child.c
	$(CC) $(CFLAGS) $< -o bin/$@
