#!/bin/bash

exe=a.out
header=spxe.h
installpath=/usr/local/include

cc=gcc
std=-std=c99
opt=-O2
inc=-I.
lib=-lglfw

wflag=(
    -Wall
    -Wextra
    -pedantic
)

linux=(
    -lm
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
        echo "This OS is not supported by this build script yet..." && exit
    fi
    
    cmd $cc $1 -o $exe $std $opt ${wflag[*]} $inc $lib ${os[*]}
}

cleanf() {
    [ -f $1 ] && cmd rm -f $1
}

clean() {
    cleanf $exe
    return 0
}

install() {
    [ "$EUID" -ne 0 ] && echo "Run with 'sudo' to install" && exit
    cmd cp $header $installpath
    echo "Successfully installed $name"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "Run with 'sudo' to uninstall" && exit
    cleanf $installpath/$header
    echo "Successfully uninstalled $name"
    return 0
}

fail() {
    echo "file '$1' was not found"
}

usage() {
    echo "Enter a file to compile with $header"
    echo "Use 'install' to install spxe.h in $installpath"
    echo "Use 'clean' to remove local builds"
}

(( $# < 1 )) && usage && exit

case "$1" in
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    "run")
        shift && [ -f $1 ] && compile $1 && shift && ./$exe $@ || fail $1;;
    *)
        [ -f $1 ] && compile $1 || fail $1;;
esac
