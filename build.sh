#!/bin/bash

cc=gcc
exe=a.out
name=spxe

lib=lib$name
header=$name.h
obj=$name.o
libs=(-lglfw)
std=-std=c89
opt=-O2

path=/usr/local
incpath=$path/include
libpath=$path/lib

cflags=(
    $std
    $opt
    -Wall
    -Wextra
    -pedantic
)

if echo "$OSTYPE" | grep -q "darwin"; then
    libs+=(-framework OpenGL)
    dlib=(-dynamiclib)
    suffix=dylib
elif echo "$OSTYPE" | grep -q "linux"; then
    libs+=(-lm -lGL -lGLEW)
    dlib=(-fPIC -shared)
    suffix=so
fi
    
cmd() {
    echo "$@" && $@
}

compile() {    
    cmd $cc ${cflags[*]} -I. ${libs[*]} $1 -o $exe 
}

object() {
    cmd $cc ${cflags[*]} -x c -DSPXE_APPLICATION -c $header -o $obj
}

shared() {
    cmd $cc $opt ${libs[*]} ${dlib[*]} $obj -o $lib.$suffix
}

static() {
    cmd ar -cr $lib.a $obj
}

cleanf() {
    [ -f $1 ] && cmd rm -f $1
}

clean() {
    cleanf $exe
    cleanf $obj
    cleanf $lib.a
    cleanf $lib.$suffix
}

install() {
    [ "$EUID" -ne 0 ] && echo "run with 'sudo' to install" && exit
    [ -f $obj ] || object
    [ -f $lib.$suffix ] || shared
    [ -f $lib.a ] || static
    cmd cp $header $incpath
    cmd cp $lib.$suffix $libpath
    cmd cp $lib.a $libpath
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "run with 'sudo' to uninstall" && exit
    cleanf $incpath/$header
    cleanf $libpath/$lib.$suffix
    cleanf $libpath/$lib.a
    return 0
}

usage() {
    echo "$0 usage:"
    echo -e "<source>\t: compile <source> file including $header"
    echo -e "shared\t\t: build $name as a shared library: $lib.$suffix"
    echo -e "static\t\t: build $name as a static library: $lib.a"
    echo -e "all\t\t: build $name as shared and static libraries"
    echo -e "help\t\t: print usage information and available commands"
    echo -e "clean\t\t: delete local builds and executables"
    echo -e "install\t\t: install $name in $path (run with sudo)"
    echo -e "uninstall\t: uninstall $name from $path (run with sudo)"
}

(( $# < 1 )) && usage && exit

case "$1" in
    "shared")
        object && shared;;
    "static")
        object && static;;
    "all")
        object && shared && static;;
    "help")
        usage;;
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        [ -f $1 ] && compile $1 || echo "$0: file '$1' not found";;
esac
