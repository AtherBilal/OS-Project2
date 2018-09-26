CC=gcc
CFLAGS=-Wall -std=gnu99
EXECUTABLES=master child
HELPERFILES= src/validation.c
all: $(EXECUTABLES)

master: src/master.c
	mkdir -p bin/
	$(CC) $(CFLAGS) $< $(HELPERFILES) -o bin/$@
child:src/child.c
	$(CC) $(CFLAGS) $< $(HELPERFILES) -o bin/$@
clean:
	rm -rf bin/
