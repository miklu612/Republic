#!/bin/sh

set -xe

mkdir -p build

flags="-Wall -Wextra -Werror -std=c99 -pedantic -fsanitize=address -g"

gcc $flags -c -o build/lexer.o src/lexer/lexer.c
gcc $flags -c -o build/formatter.o src/lexer/formatter.c
gcc $flags -c -o build/util.o src/util.c
gcc $flags -c -o build/ast.o src/ast/ast.c
gcc $flags -c -o build/parser.o src/ast/parser.c
gcc $flags -c -o build/shallow_parse.o src/ast/shallow_parse.c
gcc $flags -c -o build/runtime.o src/runtime/runtime.c
gcc $flags -c -o build/variable.o src/runtime/variable.c
gcc $flags -c -o build/object.o src/runtime/object.c
gcc $flags -c -o build/value.o src/runtime/value.c
gcc $flags -c -o build/scope.o src/runtime/scope.c
gcc $flags -c -o build/function.o src/runtime/function.c
gcc $flags -c -o build/string_array.o src/data_structures/string_array.c
gcc $flags -o build/js build/*.o src/main.c


