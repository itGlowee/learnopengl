CC=gcc
INCDIRS=-Iinclude
CFLAGS=-Wall -Wextra -g 
LIBRARIES=-Linclude -lGL -lglfw3 -lX11 -lm -lfreetype -ldl -lpthread

CFILES=src/main.c include/glad/glad.c
OBJECTS=src/main.o include/glad/glad.o

BINARY=build/main

OSFLAG:=
ifeq ($(OS), Windows_NT)
	CC=gcc.exe
	LIBRARIES=-Linclude -lopengl32 -lglfw3dll -lfreetype
	BINARY := $(BINARY).exe
	CFLAGS += -DWINDOWS=1
else
	CFLAGS += -DLINUX=1
endif

all: $(BINARY)

%.o:%.c
	$(CC) $(CFLAGS) $(INCDIRS) -c -o $@ $^

ifeq ($(OS), Windows_NT)
clean::
	DEL $(subst /,\,$(BINARY) $(OBJECTS))
run: $(BINARY)
	$(subst /,\,$(BINARY))
$(BINARY): $(OBJECTS)
	IF not exist "build" (mkdir build) 
	$(CC) -o $(BINARY) $(OBJECTS) $(LIBRARIES)
else
clean::
	rm -rf $(BINARY) $(OBJECTS)
run: $(BINARY)
	$(BINARY)
$(BINARY): $(OBJECTS)
	if [ ! -d "build" ]; then mkdir build; fi;
	$(CC) -o $(BINARY) $(OBJECTS) $(LIBRARIES)
endif

fast::
	$(CC) -O3 -o $(BINARY) $(INCDIRS) $(CFILES) $(LIBRARIES)
