#!/bin/bash

cc=gcc
std=-std=c99
wflag=-Wall
opt=-O2
inc=-I.

lib=(
    -lglfw
)

linux=(
    -lm
    -lGL
    -lGLEW
)

mac=(
    -framework OpenGL
    #-mmacos-version-min=10.10
)

compile() {
    if echo "$OSTYPE" | grep -q "darwin"; then
        echo "$cc $std $wflag $opt $inc ${lib[*]} ${mac[*]} $1"
        $cc $std $wflag $opt $inc ${lib[*]} ${mac[*]} $1
    elif echo "$OSTYPE" | grep -q "linux"; then
        echo "$cc $std $wflag $opt $inc ${lib[*]} ${linux[*]} $1"
        $cc $std $wflag $opt $inc ${lib[*]} ${linux[*]} $1
    else
        echo "This OS is not supported by this build script yet..."
    fi
}

cleanf() {
    [ -f $1 ] && rm $1 && echo "deleted $1"
}

clean() {
    cleanf a.out
    return 0;
}

install() {
    [ "$EUID" -ne 0 ] && echo "Run with 'sudo' to install" && exit

    cp spxe.h /usr/local/include/

    echo "Successfully installed $name"
    return 0
}

uninstall() {
    [ "$EUID" -ne 0 ] && echo "Run with 'sudo' to uninstall" && exit
    
    cleanf /usr/local/include/spxe.h
    
    echo "Successfully uninstalled $name"
    return 0
}

fail() {
    echo "File '$1' was not found." && exit 
}

if (( $# < 1 )); then 
    echo "Enter a file to compile with spxe.h"
    echo "Use 'install' to install spxe.h in /usr/local/"
    echo "Use 'clean' to remove local builds"
    exit
fi

case "$1" in
    "clean")
        clean;;
    "install")
        install;;
    "uninstall")
        uninstall;;
    "run")
        shift
        [ -f $1 ] && compile $1 && ./a.out || fail $1;;
    *)
        [ -f $1 ] && compile $1 || fail $1;;
esac
