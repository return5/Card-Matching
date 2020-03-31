#!/bin/sh
#compile script for cards
#compiles cards and creates executable named 'cards'
#defaults to using gcc, but uses clang if user supplies '-c' or '--clang' as argument to script. 
#use argumnet --help to bring up help info

#prints the help message to terminal.
printHelp() {
    printf "%s\n" "compile script for cards"
    printf "%s\n" "compiles cards and creates executable names 'cards'"
    printf "%s\n\n" "defaults to using gcc, but uses clang if user supplies '-c' to script."
    printf "%s\n\t" "options:"
    printf "%s\n\t" "-c   --clang   use clang instead of gcc"
    printf "%s\n" "-help  --help   prints this page then exits"
}

#use GCC to compile the game
useGCC() {
    printf "%s\n" "using GCC"
    gcc -Wall -Wextra -O2 -Iheaders -finline-functions -Wswitch-enum -lncurses -std=gnu11 -ocards
}

#compiles game using Clang
useClang() {
    printf "%s\n" "using Clang"
    clang -Iheaders -Wall -Wextra -O2 -lncurses -finline-functions -Wswitch-enum -std=gnu11 -ocards   
}

main() {
    case "$1" in                #check argument of positional parameter 1
        -help | --help)         #if argument passed to script is the help flag
            printHelp && exit   #run the help function then exit
            ;;
        -c | --clang)           #if argument passed is the clang flag
            #check if clang is installed, if it is, call the useClang function, other wise fallback to GCC
            (clang -v >/dev/null 2>&1 && useClang) || (printf "%s\n" "falling back to GCC." && (gcc -v >/dev/null 2>&1 && useGCC || printf "error, make sure GCC is installed.\n"))   
            ;;
        *)
            #checks to see if gcc is installed, and if so call useGCC, otherwise print error
            (gcc -v >/dev/null 2>&1 && useGCC) || printf "%S\n" "error, make sure GCC is installed."
            ;;
    esac
}

#call main function and pass it positonal paramter 1
main "$1"
