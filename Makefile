# spxe Makefile

CC=gcc
SRC=examples/gameoflife.c
STD=-std=c99
WFLAGS=-Wall -Wextra
OPT=-O2
INC=-I.
LIB=-lglfw
SCRIPT=build.sh

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
    LIB += -framework OpenGL
else
    LIB += -lm -lGL -lGLEW
endif

CFLAGS=$(CC) $(STD) $(OPT) $(WFLAGS) $(INC) $(LIB)

a.out: $(SRC)
	$(CFLAGS) $^ -o $@

clean: $(SCRIPT)
	./$^ $@

install: $(SCRIPT)
	./$^ $@

uninstall: $(SCRIPT)
	./$^ $@
