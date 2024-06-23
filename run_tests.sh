#!/bin/sh
set -xe
./build.sh
flags="-Wall -Wextra -Werror -std=c99 -pedantic -fsanitize=address -g"
gcc $flags -o build/test_lexer build/*.o test/lexer.c
 ./build/test_lexer

 echo "All tests passed!"
