#!/bin/bash
#compile script for cards
#compiles cards and creates executable named 'cards'
#defaults to using gcc, but uses clang if user supplies '-c' or '--clang' as argument to script. 
#use argumnet --help to bring up help info

#prints the help message to terminal.
printHelp() {
	printf "compile script for cards\n"
	printf "compiles rogie_clone and creates executable names 'cards'\n"
	printf "defaults to using gcc, but uses clang if user supplies '-c' to script.\n\n"
	printf "options:\n"
	printf " -c   --clang 	use clang instead of gcc\n"
	printf "-help  --help 	prints this page then exits\n"
}

#checks if GCC is installed, and if so it compiles the game with given flags.  if not installed then gives error message and exits.
useGCC() {
	(gcc -v >/dev/null 2>&1 && 
	printf "using GCC\n" &&
	gcc cards.c -orogueclone -Wall -Wextra -O2 -finline-functions -Wswitch-enum -lncurses -std=gnu11 -ocards ) ||
	printf "error, gcc seems to not be installed.\n"
}

#compiles game using Clang
useClang() {
	printf "using Clang\n"
	clang cards.c -Wall -Wextra -O2 -lncurses -finline-functions -Wswitch-enum -std=gnu11 -ocards   
}

main() {
	case "$1" in
		-help | --help)
			printHelp && exit
			;;
		-c | --clang)
			(clang -v >/dev/null 2>&1 && useClang) || (printf "falling back to GCC.\n" && useGCC)
			;;
		*)
			useGCC
			;;
	esac
}


main "$1"
