#!/bin/sh

set -xe

mkdir -p build

flags="-Wall -Wextra -Werror -std=c99 -pedantic -fsanitize=address -g"

gcc $flags -c -o build/lexer.o src/lexer/lexer.c
gcc $flags -c -o build/util.o src/util.c
gcc $flags -c -o build/ast.o src/ast/ast.c
gcc $flags -c -o build/parser.o src/ast/parser.c
gcc $flags -c -o build/shallow_parse.o src/ast/shallow_parse.c
gcc $flags -o build/js build/*.o src/main.c


