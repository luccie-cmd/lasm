CC=gcc
WARNINGS=-Wall -Wextra -Wswitch-enum -pedantic
CFLAGS=$(WARNINGS) -std=c11 -ggdb -I../include

all:
	$(CC) $(CFLAGS) -o lasm ../src/compiler.c ../src/ast.c ../src/insts.c
	$(CC) $(CFLAGS) -o lexe ../src/executer.c ../src/ast.c ../src/insts.c
# g++ -o help_me ../src/help_me.cpp