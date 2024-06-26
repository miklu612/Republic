#include"lexer.h"
#include"formatter.h"
#include"../util.h"

#include<stdlib.h>
#include<assert.h>
#include<stdio.h>
#include<string.h>


enum LexerState {
    // This state means that the lexer is still trying to figure out what the 
    // current token might be. You have to get out of this state in a single
    // cycle. Otherwise you have made an error, since you can just guess
    // what a token might be with the first character of it.
    //
    // If you can't guess what the next lexer state is, throw an error. This 
    // makes it easier to debug things.
    LexerState_None,

    // This means that the lexer is currently collecting an identifier. This
    // identifier can be a variable, function, keyword, and anything else
    // that is an identifier.
    LexerState_Identifier,

    // This state will collect characters until it hits another quotation mark.
    LexerState_QuoteString,

    // This state will collect a number. Basically the same syntax as a float,
    // since js doesn't have integers and floats, but numbers
    LexerState_Number,

};


// Returned when you want to ask for a character and that character's existence
// is not known for certain
typedef struct {
    bool valid;
    char character;
} CharReqResult;

typedef struct {
    size_t index;
    enum LexerState state;
    char* code;
} Lexer;

// Dangerous function. Make sure you are pushing into a string that has enough
// space for the character.
void str_push_character(char* string, char character) {
    string[strlen(string)+1] = '\0';
    string[strlen(string)] = character;
}

void lexer_skip_increment(Lexer* lexer) {
    lexer->index -= 1;
}

void lexer_end_identifier_state(Lexer* lexer) {
    lexer_skip_increment(lexer);
    lexer->state = LexerState_None;
}

void lexer_end_quote_string_state(Lexer* lexer) {
    lexer->state = LexerState_None;
}

char lexer_get_character(Lexer* lexer) {
    return lexer->code[lexer->index];
}

void lexer_token_array_push(LexerTokenArray* array, LexerToken* token) {

    if(array->tokens == NULL) {
	array->tokens = malloc(sizeof(LexerToken));
	array->count = 0;
    }
    else {
	array->tokens = realloc(array->tokens, (array->count+1) * sizeof(LexerToken));
    }

    assert(array->tokens != NULL);

    array->tokens[array->count] = *token;
    array->count += 1;

}

bool is_identifier(char* token) {
    for(size_t i = 0 ; i < strlen(token) ; i++) {
	if(is_letter(token[i]) == false) {
	    return false;
	}
    }
    return true;
}

bool is_keyword(char* string) {
    if(strcmp(string, "if") == 0) {
	return true;
    }
    else if(strcmp(string, "const") == 0) {
	return true;
    }
    else {
	return false;
    }
}

enum LexerTokenType identify_token_type(char* token) {
    if(is_identifier(token)) {
	if(is_keyword(token)) {
	    return LexerTokenType_Keyword;
	}
	else {
	    return LexerTokenType_Identifier;
	}
    }
    else if(strlen(token) == 1) {
	if(token[0] == '.') {
	    return LexerTokenType_Dot;
	}
	else if(token[0] == '(') {
	    return LexerTokenType_ParenStart;
	}
	else if(token[0] == ')') {
	    return LexerTokenType_ParenEnd;
	}
	else if(token[0] == '{') {
	    return LexerTokenType_CurlyBracketStart;
	}
	else if(token[0] == '}') {
	    return LexerTokenType_CurlyBracketEnd;
	}
	else if(token[0] == ';') {
	    return LexerTokenType_Semicolon;
	}
	else if(token[0] == '=') {
	    return LexerTokenType_Equals;
	}
    }
    else if(strlen(token) == 2) {
	if(strcmp(token, "==") == 0) {
	    return LexerTokenType_DoubleEquals;
	}
    }
    PANIC("Couldn't identify token");
    return LexerTokenType_Keyword;
}

void lexer_token_set_raw(LexerToken* token, char* raw_token) {

    token->raw = malloc((strlen(raw_token)+1) * sizeof(char));
    assert(token->raw != NULL);
    memset(token->raw, 0, strlen(raw_token)+1);
    memcpy(token->raw, raw_token, strlen(raw_token) * sizeof(char));

}


LexerToken lexer_token_create(char* raw_token) {

    LexerToken token = { 0 };

    token.type = identify_token_type(raw_token);
    lexer_token_set_raw(&token, raw_token);


    return token;

}

LexerToken lexer_token_create_string(char* string) {

    LexerToken token = { 0 };

    token.type = LexerTokenType_String;
    lexer_token_set_raw(&token, string);

    return token;

}

LexerToken lexer_token_create_custom(enum LexerTokenType type, char* raw) {
    LexerToken token = { 0 };
    token.type = type;
    lexer_token_set_raw(&token, raw);
    return token;
}

LexerToken lexer_token_create_number(char* number) {

    LexerToken token = { 0 };

    token.type = LexerTokenType_NumberConstant;
    lexer_token_set_raw(&token, number);

    return token;

}

CharReqResult lexer_get_next_character(Lexer* lexer, char* code) {

    if(lexer->index + 1 < strlen(code)) {
	CharReqResult result = { 0 };
	result.valid = true;
	result.character = code[lexer->index + 1];
	return result;
    }
    else {
	CharReqResult result = { 0 };
	result.valid = false;
	result.character = '\0';
	return result;
    }

}

LexerTokenArray lexer_lex_code(char* code) {

    (void) code;

    char* new_code = formatter_format_code(code);
    LexerTokenArray token_array = { 0 };

    const size_t current_token_size = 1024;
    char* current_token = malloc(current_token_size * sizeof(char));
    assert(current_token != NULL);
    memset(current_token, 0, current_token_size * sizeof(char));

    Lexer lexer = { 0 };
    lexer.state = LexerState_None;
    lexer.index = 0;
    lexer.code = new_code;

    for(lexer.index = 0 ; lexer.index < strlen(new_code) ; lexer.index++) {

	const char character = lexer_get_character(&lexer);
	(void) character;

	if(lexer.state == LexerState_None) {
	    if(is_letter(character)) {
		lexer.state = LexerState_Identifier;
		str_push_character(current_token, character);
	    }
	    else if(character == '.') {
		LexerToken token = lexer_token_create(".");
		lexer_token_array_push(&token_array, &token);
	    }
	    else if(character == '(') {
		LexerToken token = lexer_token_create("(");
		lexer_token_array_push(&token_array, &token);
	    }
	    else if(character == ')') {
		LexerToken token = lexer_token_create(")");
		lexer_token_array_push(&token_array, &token);
	    }
	    else if(character == '{') {
		LexerToken token = lexer_token_create("{");
		lexer_token_array_push(&token_array, &token);
	    }
	    else if(character == '}') {
		LexerToken token = lexer_token_create("}");
		lexer_token_array_push(&token_array, &token);
	    }
	    else if(character == ';') {
		LexerToken token = lexer_token_create(";");
		lexer_token_array_push(&token_array, &token);
	    }
	    else if(character == '"') {
		lexer.state = LexerState_QuoteString;
	    }
	    else if(character == '\n') {
		LexerToken token = lexer_token_create_custom(LexerTokenType_Newline, "\n");
		lexer_token_array_push(&token_array, &token);
	    }
	    else if(character == '\t') {
		continue;
	    }
	    else if(character == ' ') {
		continue;
	    }
	    else if(character == '=') {
		CharReqResult result = lexer_get_next_character(&lexer, new_code);
		if(result.valid) {
		    if(result.character == '=') {
			LexerToken token = lexer_token_create("==");
			lexer_token_array_push(&token_array, &token);
			lexer.index += 1;
		    }
		    else {
			LexerToken token = lexer_token_create("=");
			lexer_token_array_push(&token_array, &token);
		    }
		}
	    }
	    else if(is_digit(character)) {
		lexer.state = LexerState_Number;
		str_push_character(current_token, character);
	    }
	    else {
		fprintf(stderr, "Failed to identify: '%c'\n", character);
		PANIC("");
	    }
	}
	else if(lexer.state == LexerState_Identifier) {
	    if(is_letter(character)) {
		str_push_character(current_token, character);
	    }
	    else {
		LexerToken token = lexer_token_create(current_token);
		lexer_token_array_push(&token_array, &token);
		lexer_end_identifier_state(&lexer);
		current_token[0] = '\0';
	    }
	}
	else if(lexer.state == LexerState_QuoteString) {
	    if(character == '\"') {
		LexerToken token = lexer_token_create_string(current_token);
		lexer_token_array_push(&token_array, &token);
		lexer_end_quote_string_state(&lexer);
		current_token[0] = '\0';
	    }
	    else {
		str_push_character(current_token, character);
	    }
	}
	else if(lexer.state == LexerState_Number) {
	    if(is_digit(character)) {
		str_push_character(current_token, character);
	    }
	    else if(character == '.') {
		for(size_t i = 0 ; i < strlen(current_token) ; i++) {
		    if(current_token[i] == '.') {
			PANIC("Multiple . characters in number");
		    }
		}
		str_push_character(current_token, character);
	    }
	    else {
		LexerToken token = lexer_token_create_number(current_token);
		lexer_token_array_push(&token_array, &token);
		current_token[0] = '\0';
		lexer.state = LexerState_None;
		lexer.index -= 1;
	    }
	}
	else {
	    PANIC("Invalid Lexer State");
	}

    }

    free(current_token);
    free(new_code);


    return token_array;

}

void lexer_token_array_free(LexerTokenArray* array) {
    if(array->tokens != NULL) {
	for(size_t i = 0 ; i < array->count ; i++) {
	    if(array->tokens[i].raw != NULL) {
		free(array->tokens[i].raw);
	    }
	}
	free(array->tokens);
    }
}

void lexer_token_array_print(LexerTokenArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
	printf(
	    "{\n"
	    "\tvalue: \"%s\"\n"
	    "}\n",
	    array->tokens[i].raw
	);
    }
}

bool __test_lexer_token_array_compare(LexerTokenArray* expected, LexerTokenArray* got) {
    if(expected->count != got->count) {
	fprintf(stderr, 
	    "Test Mismatch Found:\n"
	    "\tType: Token Count\n"
	    "\tExpected: %ld\n"
	    "\tGot: %ld\n",
	    expected->count,
	    got->count
	);
	fprintf(stderr, "Dumping values...\n");
	fprintf(stderr, "Expected:\n");
	for(size_t i = 0 ; i < expected->count ; i++) {
	    fprintf(stderr, "\t%ld - %d - %s\n", 
		i,
		expected->tokens[i].type,
		expected->tokens[i].raw
	    );
	}
	fprintf(stderr, "Got:\n");
	for(size_t i = 0 ; i < got->count ; i++) {
	    fprintf(stderr, "\t%ld - %d - %s\n",
		i,
		got->tokens[i].type,
		got->tokens[i].raw
	    );
	}
	return false;
    }
    for(size_t i = 0 ; i < expected->count ; i++) {
	if(expected->tokens[i].type != got->tokens[i].type) {
	    fprintf(stderr, 
		"Test Mismatch Found:\n"
		"\tType: Token Type\n"
		"\tExpected: %d\n"
		"\tGot: %d\n"
		"\tExpected(raw): %s\n"
		"\tGot(raw): %s\n",
		expected->tokens[i].type,
		got->tokens[i].type,
		expected->tokens[i].raw,
		got->tokens[i].raw
	    );
	    return false;
	}
	if(strcmp(expected->tokens[i].raw, got->tokens[i].raw) != 0) {
	    fprintf(stderr, 
		"Test Mismatch Found:\n"
		"\tType: Raw Value\n"
		"\tExpected: %s\n"
		"\tGot: %s\n",
		expected->tokens[i].raw,
		got->tokens[i].raw
	    );
	    return false;
	}
    }
    return true;
}
