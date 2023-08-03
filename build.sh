#!/bin/bash

exe=a.out
header=spxe.h
installpath=/usr/local/include

cc=gcc
std=-std=c89
opt=-O2
inc=-I.
lib=-lglfw

wflag=(
    -Wall
    -Wextra
    -pedantic
)

linux=(
    -lm     # used by examples/pendulum and examples/mandelbrot.c
    -lGL
    -lGLEW
)

mac=(
    -framework OpenGL
)

cmd() {
    echo "$@" && $@
}

compile() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        os=${mac[*]}
    elif echo "$OSTYPE" | grep -q "linux"; then
        os=${linux[*]}
    else
        echo "this OS is not supported by this build script yet..."
    fi
    
    cmd $cc $1 -o $exe $std $opt ${wflag[*]} $inc $lib ${os[*]}
}

cleanf() {
    [ -f $1 ] && cmd rm -f $1
}

install() {
    [ "$EUID" -ne 0 ] && echo "run with 'sudo' to install" && exit
    cmd cp $header $installpath
    echo "successfully installed $header"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "run with 'sudo' to uninstall" && exit
    cleanf $installpath/$header
    echo "successfully uninstalled $header"
    return 0
}

fail() {
    echo "$0: file '$1' not found"
}

usage() {
    echo "$0 usage:"
    echo -e "<source>\t: Compile <source> file with $header"
    echo -e "help\t\t: Print usage information and available commands"
    echo -e "clean\t\t: Delete compiled executable"
    echo -e "install\t\t: Install $header in $installpath (run with sudo)"
    echo -e "uninstall\t: Unnstall $header from $installpath (run with sudo)"
}

(( $# < 1 )) && usage && exit

case "$1" in
    "help")
        usage;;
    "clean")
        cleanf $exe;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    *)
        [ -f $1 ] && compile $1 || fail $1;;
esac
