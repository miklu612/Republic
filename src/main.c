#include<stdio.h>
#include<stdlib.h>

#include"lexer/lexer.h"


int main(int argc, char** argv) {

    (void) argc;
    (void) argv;

    LexerTokenArray token_array = lexer_lex_code("console.log(\"Hola\");");
    lexer_token_array_print(&token_array);
    lexer_token_array_free(&token_array);

    return 0;

}
