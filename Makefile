# Makefile for sdlblocks

CC = clang-3.6
CFLAGS = -O3 -IPC -ISource -std=c89
LDFLAGS = -lSDL

SRC = Source/main.c PC/PC_SDL12.c Source/other.c
OBJ = main.o PC_SDL12.o other.o

sdlblocks: $(SRC)
	$(CC) -c $(CFLAGS) $(SRC)
	$(CC) -o tetris.elf $(OBJ) $(LDFLAGS)

clean:
	rm sdlblocks
	rm *.o
