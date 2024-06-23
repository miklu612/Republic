#!/bin/sh

set -xe

mkdir -p build

flags="-Wall -Wextra -Werror -std=c99 -pedantic -fsanitize=address -g"

gcc $flags -c -o build/lexer.o src/lexer/lexer.c
gcc $flags -c -o build/util.o src/util.c
gcc $flags -o build/js build/*.o src/main.c


