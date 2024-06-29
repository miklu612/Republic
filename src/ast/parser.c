
// Note for anyone working on this parser. This thing is a glorified state
// machine. The ShallowASTNode struct is just a struct that has a state and a
// bunch of different data structs that are filled depending on the state.
//
// TODO: Move ShallowASTNode stuff in to its own file.
// TODO: Implement deep parsing



#include"parser.h"
#include"ast.h"
#include"shallow_parse.h"
#include"../util.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>



void ast_node_array_free(ASTNodeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
	ast_node_free(&array->nodes[i]);
    }
    free(array->nodes);
}

ASTNode ast_node_create_function_call(ShallowASTNode* object, ShallowASTNode* call) {
    ASTNode node = { 0 };
    node.type = ASTNodeType_FunctionCall;

    node.data.FunctionCall.object = shallow_ast_node_deep_copy(object);
    node.data.FunctionCall.call = shallow_ast_node_deep_copy(call);

    return node;
}

ASTNode ast_node_create_create_const_variable(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    ASTNode output = { 0 };
    output.type = ASTNodeType_CreateConstVariable;
    output.data.CreateConstVariable.node = shallow_ast_node_deep_copy(node);
    return output;
}

void ast_node_array_push(ASTNodeArray* array, ASTNode* node) {
    if(array->nodes == NULL) {
	array->nodes = malloc(sizeof(ASTNode));
	array->count = 0;
    }
    else {
	array->nodes = realloc(array->nodes, (array->count+1) * sizeof(ASTNode));
    }
    array->nodes[array->count] = *node;
    array->count += 1;
}



ASTNodeArray parser_parse_lexer_token_array(LexerTokenArray* lexer_token_array) {

    ShallowASTNodeArray shallow_array = parse_shallow_parse(lexer_token_array);
    ASTNodeArray ast_node_array = { 0 };

    for(size_t i = 0 ; i < shallow_array.count ; i++) {
	ShallowASTNode* c_node = &shallow_array.nodes[i];
	if(c_node->type == ShallowASTNodeType_AccessObjectMember) {
	    if(i+1 < shallow_array.count) {
		ShallowASTNode* next_node = &shallow_array.nodes[i+1];
		if(next_node->type == ShallowASTNodeType_Call) {
		    ASTNode node = ast_node_create_function_call(
			c_node,
			next_node
		    );
		    ast_node_array_push(&ast_node_array, &node);
		    i += 1;
		}
	    }
	}
        else if(c_node->type == ShallowASTNodeType_CreateConstVariable) {
            ASTNode node = ast_node_create_create_const_variable(&shallow_array.nodes[i]);
            ast_node_array_push(&ast_node_array, &node);
        }
	else if(c_node->type == ShallowASTNodeType_Semicolon) {
	    printf("Semicolons not supported.\n");
	}
	else {
	    fprintf(stderr,
		"Failure:\n"
		"\tType: %d\n",
		c_node->type
	    );
	    PANIC("Parsing error");
	}
    }

    shallow_ast_node_array_print(&shallow_array);
    shallow_ast_node_array_free(&shallow_array);


    return ast_node_array;

}
