
// Note for anyone working on this parser. This thing is a glorified state
// machine. The ShallowASTNode struct is just a struct that has a state and a
// bunch of different data structs that are filled depending on the state.
//
// TODO: Move ShallowASTNode stuff in to its own file.
// TODO: Implement deep parsing



#include"parser.h"
#include"shallow_parse.h"
#include"../util.h"

#include<stdio.h>






ASTNodeArray parser_parse_lexer_token_array(LexerTokenArray* lexer_token_array) {

    ShallowASTNodeArray shallow_array = parse_shallow_parse(lexer_token_array);

    ASTNodeArray ast_node_array = { 0 };

    (void) ast_node_array;
    (void) lexer_token_array;

    shallow_ast_node_array_print(&shallow_array);
    shallow_ast_node_array_free(&shallow_array);

    return ast_node_array;

}
