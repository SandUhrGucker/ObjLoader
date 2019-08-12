.PHONY: clean all

CC = g++
CFLAGS = -Os -fomit-frame-pointer -Wall
LDFLAGS= -lm -lGL -lSDL2 -lGLU -lglut -ldl
all:
	$(CC) $(CFLAGS) -o main.o -c main.cpp
	$(CC) $(CFLAGS) -o objloader.o -c objloader.cpp
	$(CC) -o objloader main.o objloader.o $(LDFLAGS)

clean:
	@echo "Bereinige Build-Dateien"
	rm -f main.o
	rm -f objloader.o
	rm -f objloader

