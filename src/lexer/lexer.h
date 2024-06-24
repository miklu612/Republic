#ifndef lexer_h
#define lexer_h

#include<stdlib.h>
#include<stdbool.h>

enum LexerTokenType {
    LexerTokenType_Keyword,
    LexerTokenType_Dot,
    LexerTokenType_ParenStart,
    LexerTokenType_ParenEnd,
    LexerTokenType_String,
    LexerTokenType_Semicolon,
    LexerTokenType_Identifier,
    LexerTokenType_DoubleEquals,
    LexerTokenType_Equals,
    LexerTokenType_CurlyBracketStart,
    LexerTokenType_CurlyBracketEnd,
    LexerTokenType_NumberConstant,
};


// This is used to store individual lexer tokens that are
// found in the given code.
//
// raw  -  The value in the source code
typedef struct LexerToken_struct {
    char* raw;
    enum LexerTokenType type;
} LexerToken;

typedef struct {
    size_t count;
    LexerToken* tokens;
} LexerTokenArray;



LexerTokenArray lexer_lex_code(char* code);

// This function will push a token into the lexer token array. Ownership will
// be transfered to the array, so don't free it.
void lexer_token_array_push(LexerTokenArray*, LexerToken*);
void lexer_token_array_free(LexerTokenArray*);
void lexer_token_array_print(LexerTokenArray*);
bool __test_lexer_token_array_compare(LexerTokenArray*, LexerTokenArray*);

// This function will create a token based on the string that is given to it.
// This will not take ownership of the string.
LexerToken lexer_token_create(char* token);
LexerToken test_lexer_token_create_string(char* string);

#endif
