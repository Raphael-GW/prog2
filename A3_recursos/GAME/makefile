CC = gcc
CFLAGS = -Wall -g

PKG_CFLAGS := $(shell pkg-config --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_ttf-5 allegro_image-5)
PKG_LIBS   := $(shell pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_ttf-5 allegro_image-5)

CFLAGS += $(PKG_CFLAGS)
LDFLAGS := $(PKG_LIBS)

.PHONY: all clean valgrind

all: game

game: main.o joker.o joystick.o enemy.o
	$(CC) $(CFLAGS) -o game main.o joker.o joystick.o enemy.o $(LDFLAGS)

main.o: main.c joker.h
	$(CC) $(CFLAGS) -c main.c

joker.o: joker.c joker.h
	$(CC) $(CFLAGS) -c joker.c

enemy.o: enemy.c enemy.h
	$(CC) $(CFLAGS) -c enemy.c

joystick.o: joystick.c joystick.h
	$(CC) $(CFLAGS) -c joystick.c

clean:
	rm -f *.o game

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all -s --track-origins=yes ./game