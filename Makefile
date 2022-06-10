# Made with the help of this example
# https://github.com/gwu-cs-os/evening_os_hour/blob/master/f19/10.2-makefiles/03_featureful_makefile/Makefile


CC=gcc
INCDIRS=-Iinclude
CODEDIRS=src include/glad
CFLAGS=-Wall -Wextra -g -MP -MD
LIBRARIES=-Linclude -lGL -lglfw3 -lX11 -lm -lfreetype -ldl -lpthread

CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))
OBJECTS=$(patsubst %.c,%.o,$(CFILES))
DEPFILES=$(patsubst %.c,%.d,$(CFILES))

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
	$(CC) $(CFLAGS) $(INCDIRS) -c -o $@ $<

ifeq ($(OS), Windows_NT)
clean::
	DEL $(subst /,\,$(BINARY) $(OBJECTS) $(DEPFILES))
run: $(BINARY)
	$(subst /,\,$(BINARY))
$(BINARY): $(OBJECTS)
	IF not exist "build" (mkdir build) 
	$(CC) -o $(BINARY) $(OBJECTS) $(LIBRARIES)
else
clean::
	rm -rf $(BINARY) $(OBJECTS) $(DEPFILES)
run: $(BINARY)
	$(BINARY)
$(BINARY): $(OBJECTS)
	if [ ! -d "build" ]; then mkdir build; fi;
	$(CC) -o $(BINARY) $(OBJECTS) $(LIBRARIES)
endif

fast::
	$(CC) -O3 -o $(BINARY) $(INCDIRS) $(CFILES) $(LIBRARIES)


# include the dependencies
-include $(DEPFILES)