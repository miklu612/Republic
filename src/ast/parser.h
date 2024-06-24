#ifndef parser_h
#define parser_h

#include"ast.h"
#include"../lexer/lexer.h"

ASTNodeArray parser_parse_lexer_token_array(LexerTokenArray*);

#endif
