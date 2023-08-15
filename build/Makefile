CC=gcc
WARNINGS=-Wall -Wextra -Wswitch-enum -pedantic
CFLAGS=$(WARNINGS) -std=c11 -ggdb -I../include

all:
	$(CC) $(CFLAGS) -o lasm ../src/compiler.c
	$(CC) $(CFLAGS) -o lexe ../src/executer.c