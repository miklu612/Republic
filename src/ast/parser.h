#ifndef parser_h
#define parser_h

#include"ast.h"
#include"../lexer/lexer.h"

ASTNodeArray parser_parse_lexer_token_array(LexerTokenArray*);
ASTNodeArray parser_parse_shallow_ast_node_array(const ShallowASTNodeArray*);

#endif
