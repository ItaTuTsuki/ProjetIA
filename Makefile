CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
EXEC = puissance_n

SRC = src/main.c src/game.c
OBJ = main.o game.o

all:
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC)

clean:
	rm -f *.o $(EXEC)