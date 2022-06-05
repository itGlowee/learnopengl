CC=gcc
INCDIRS=-Iinclude
CFLAGS=-Wall -Wextra -g 
LIBRARIES=-lGL -lglfw3 -lX11 -lm -lfreetype

CFILES=src/main.c include/glad/glad.c
OBJECTS=src/main.o include/glad/glad.o

BINARY=build/main

OSFLAG:=
ifeq ($(OS), Windows_NT)
	CC=gcc.exe
	LIBRARIES=-Linclude -lopengl32 -lglfw3dll -lfreetype
endif


all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $(BINARY) $(OBJECTS) $(LIBRARIES)

%.o:%.c
	$(CC) $(CFLAGS) $(INCDIRS) -c -o $@ $^

ifeq ($(OS), Windows_NT)
clean::
	DEL $(BINARY) $(OBJECTS)
else
clean::
	rm -rf $(BINARY) $(OBJECTS)
endif
