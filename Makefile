# spxe Makefile

EXE=a.out
HEADER=spxe.h
EXAMPLES=examples
SRC=$(EXAMPLES)/hello.c
SCRIPT=build.sh

CC=gcc
STD=-std=c99
OPT=-O2
WFLAGS=-Wall -Wextra -pedantic
INC=-I.
LIB=-lglfw

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
    OPNGL=-framework OpenGL
else
    OPNGL=-lGL -lGLEW
endif

CFLAGS=$(STD) $(OPT) $(WFLAGS) $(INC) $(LIB) $(OPNGL)

$(EXE): $(SRC)

$(EXAMPLES)/*.c: $(HEADER)
	$(CC) $@ -o $(EXE) $(CFLAGS)

clean:
	$(RM) $(EXE)

install: $(SCRIPT)
	./$< $@

uninstall: $(SCRIPT)
	./$< $@
