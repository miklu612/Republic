#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include"lexer/lexer.h"
#include"ast/ast.h"
#include"ast/parser.h"
#include"runtime/runtime.h"

char* read_file(char* path) {
    FILE* file = fopen(path, "r");
    if(file == NULL) {
	return NULL;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* text = calloc(sizeof(char), (size+1));
    fread(text, sizeof(char), size, file);

    fclose(file);
    return text;
    
}

int main(int argc, char** argv) {

    for(int i = 1 ; i < argc ; i++) {
	char* text = read_file(argv[i]);
	if(text != NULL) {
	    printf("%s\n", text);
	    LexerTokenArray token_array = lexer_lex_code(text);
	    ASTNodeArray ast_nodes = parser_parse_lexer_token_array(&token_array);
	    runtime_start(&ast_nodes);
	    ast_node_array_free(&ast_nodes);
	    lexer_token_array_free(&token_array);
	    free(text);
	}
    }

    return 0;

}
