
#define LEXER_TESTING_MODE
#include"../src/lexer/lexer.h"

#include<string.h>
#include<stdio.h>

typedef struct {
    char* input;
    LexerTokenArray expected;
} LexerTest;

void lexer_token_array_push_const_string(LexerTokenArray* array, char* string, enum LexerTokenType type) {

    const size_t size = (strlen(string)+1) * sizeof(char);
    char* allocated_string = malloc(size);
    memset(allocated_string, 0, size);
    memcpy(allocated_string, string, strlen(string));

    LexerToken token = { 0 };
    token.type = type;
    token.raw = allocated_string;

    lexer_token_array_push(array, &token);

}

bool run_test(LexerTest* test) {
    LexerTokenArray got = lexer_lex_code(test->input);
    if(__test_lexer_token_array_compare(&test->expected, &got) == true) {
	lexer_token_array_free(&got);
	return true;
    }
    else {
	lexer_token_array_free(&got);
	return false;
    }
}

void test_0() {

    LexerTest test = { 0 };

    test.input = "console.log(\"Hello, World!\");";

    test.expected.count = 7;
    lexer_token_array_push_const_string(&test.expected, "console", LexerTokenType_Identifier);
    lexer_token_array_push_const_string(&test.expected, ".", LexerTokenType_Dot);
    lexer_token_array_push_const_string(&test.expected, "log", LexerTokenType_Identifier);
    lexer_token_array_push_const_string(&test.expected, "(", LexerTokenType_ParenStart);
    lexer_token_array_push_const_string(&test.expected, "Hello, World!", LexerTokenType_String);
    lexer_token_array_push_const_string(&test.expected, ")", LexerTokenType_ParenEnd);
    lexer_token_array_push_const_string(&test.expected, ";", LexerTokenType_Semicolon);

    if(!run_test(&test)) {
	fprintf(stderr, "Failed test: 0\n");
	exit(-1);
    }

    lexer_token_array_free(&test.expected);


}

void test_1() {

    LexerTest test = { 0 };

    test.input = "if(a == 1) { console.log(\"A is equal to 2\"); }";

    test.expected.count = 7;
    lexer_token_array_push_const_string(&test.expected, "if", LexerTokenType_Keyword);
    lexer_token_array_push_const_string(&test.expected, "(", LexerTokenType_ParenStart);
    lexer_token_array_push_const_string(&test.expected, "a", LexerTokenType_Identifier);
    lexer_token_array_push_const_string(&test.expected, "==", LexerTokenType_DoubleEquals);
    lexer_token_array_push_const_string(&test.expected, "1", LexerTokenType_NumberConstant);
    lexer_token_array_push_const_string(&test.expected, ")", LexerTokenType_ParenEnd);
    lexer_token_array_push_const_string(&test.expected, "{", LexerTokenType_CurlyBracketStart);
    lexer_token_array_push_const_string(&test.expected, "console", LexerTokenType_Identifier);
    lexer_token_array_push_const_string(&test.expected, ".", LexerTokenType_Dot);
    lexer_token_array_push_const_string(&test.expected, "log", LexerTokenType_Identifier);
    lexer_token_array_push_const_string(&test.expected, "(", LexerTokenType_ParenStart);
    lexer_token_array_push_const_string(&test.expected, "A is equal to 2", LexerTokenType_String);
    lexer_token_array_push_const_string(&test.expected, ")", LexerTokenType_ParenEnd);
    lexer_token_array_push_const_string(&test.expected, ";", LexerTokenType_Semicolon);
    lexer_token_array_push_const_string(&test.expected, "}", LexerTokenType_CurlyBracketEnd);

    if(!run_test(&test)) {
	fprintf(stderr, "Failed test: 0\n");
	exit(-1);
    }

    lexer_token_array_free(&test.expected);


}


int main() {
    test_0();
    test_1();
    return 0;
}

