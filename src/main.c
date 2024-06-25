#include<stdio.h>
#include<stdlib.h>

#include"lexer/lexer.h"
#include"ast/ast.h"
#include"ast/parser.h"
#include"runtime/runtime.h"


int main(int argc, char** argv) {

    (void) argc;
    (void) argv;

    LexerTokenArray token_array = lexer_lex_code("console.log(\"Hello, World!\");\nconsole.log(\"Hi\")");
    ASTNodeArray ast_nodes = parser_parse_lexer_token_array(&token_array);
    lexer_token_array_free(&token_array);
    ast_node_array_print(&ast_nodes);
    runtime_start(&ast_nodes);
    ast_node_array_free(&ast_nodes);

    return 0;

}
