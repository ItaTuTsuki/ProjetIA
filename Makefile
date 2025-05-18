CC = gcc
CFLAGS = -Wall -Wextra -Iinclude `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`
EXEC = puissance_n

SRC = src/main.c src/game.c src/graphical_interface.c src/ai.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS) -lm

clean:
	rm -f $(EXEC)
