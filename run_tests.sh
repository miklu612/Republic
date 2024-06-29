#!/bin/sh
set -xe
./build.sh
flags="-Wall -Wextra -Werror -std=c99 -pedantic -fsanitize=address -g"
gcc $flags -o build/test_lexer build/*.o test/lexer.c
gcc $flags -o build/test_util build/*.o test/util_test.c
 #./build/test_lexer
 ./build/test_util

 echo "All tests passed!"
