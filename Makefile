# spxe Makefile

EXE=a.out
NAME=spxe
HEADER=$(NAME).h
OBJ=$(NAME).o
LIB=lib$(NAME)

CC=gcc
STD=-std=c89
OPT=-O2
WFLAGS=-Wall -Wextra -pedantic
LIBS=-lglfw

EXAMPLES=examples
SRC=$(EXAMPLES)/hello.c
SCRIPT=build.sh

OS=$(shell uname -s)
ifeq ($(OS),Darwin)
    LIBS+=-framework OpenGL
    DLIB=-dynamiclib
    SUFFIX=dylib
else
    LIBS+=-lm -lGL -lGLEW
    DLIB=-fPIC -shared
    SUFFIX=so
endif

CFLAGS=$(STD) $(OPT) $(WFLAGS)

$(EXE): $(SRC)

$(EXAMPLES)/%.c: $(HEADER)
	$(CC) $(CFLAGS) -I. $(LIBS) $@ -o $(EXE) 

.PHONY: shared static clean install uninstall

all: shared static

shared: $(LIB).$(SUFFIX)

static: $(LIB).a

$(LIB).$(SUFFIX): $(OBJ)
	$(CC) $(OPT) $(LIBS) $(DLIB) $^ -o $@  

$(LIB).a: $(OBJ)
	ar -cr $@ $^

$(OBJ): $(HEADER)
	$(CC) $(CFLAGS) -x c -DSPXE_APPLICATION -c $^ -o $@ 

clean:
	$(RM) $(EXE)
	$(RM) $(OBJ)
	$(RM) $(LIB).$(SUFFIX)
	$(RM) $(LIB).a

install: $(SCRIPT)
	./$< $@

uninstall: $(SCRIPT)
	./$< $@
