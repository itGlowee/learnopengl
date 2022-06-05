CC=gcc
INCDIRS=-Iinclude
CFLAGS=-Wall -Wextra -g 
LIBRARIES=-lGL -lglfw3 -lX11 -lm -lfreetype

CFILES=src/main.c include/glad/glad.c
OBJECTS=src/main.o include/glad/glad.o

BINARY=build/main

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $(BINARY) $(OBJECTS) $(LIBRARIES)

%.o:%.c
	$(CC) $(CFLAGS) $(INCDIRS) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJECTS)
