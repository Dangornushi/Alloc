#!/bin/bash

gcc -std=c11 -o alloc main.c

runtest() {
	output=$(./alloc "$1")
	if [ "$output" != "$2" ]; then
		echo "$1: $2 expected, but got $output"
		exit 1
	fi
	echo "$1 => $output"
}

echo '=== basic ==='
runtest 0 0
runtest 1 1
runtest 12 12

echo '=== arithmetic operators ==='
runtest '+ 1 2' 3
runtest '+ 100 2' 102
runtest '+ 100 2000' 2100

runtest '- 100 2' 98
runtest '- 100 2000' -1900

runtest '* 12 2' 24

runtest '/ 20 5' 4

runtest '+ + + 1 2 3 4' 10

runtest '+ 2 * 4 3' 14


echo '=== functions ==='


echo "OK"
