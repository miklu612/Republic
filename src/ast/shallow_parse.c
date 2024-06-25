#include"shallow_parse.h"
#include"../util.h"

#include<stddef.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>

// This does not take ownership
ShallowASTNode shallow_ast_node_create_string_constant(char* string) {
    ShallowASTNode shallow_ast_node = { 0 };
    shallow_ast_node.type = ShallowASTNodeType_StringConstant;
    shallow_ast_node.data.StringConstant.string = clone_string(string);
    return shallow_ast_node;
}

void shallow_ast_node_free(ShallowASTNode* node) {
    if(node->type == ShallowASTNodeType_AccessObjectMember) {
	char*** names = &node->data.AccessObjectMember.path_data.names;
	char** object_name = &node->data.AccessObjectMember.object_name;
	size_t* count = &node->data.AccessObjectMember.path_data.count;
	if(*names != NULL) {
	    for(size_t i = 0 ; i < *count ; i++) {
		free((*names)[i]);
	    }
	    free(*names);
	}
	if(object_name != NULL) {
	    free(*object_name);
	}
    }
    else if(node->type == ShallowASTNodeType_Call) {
	ShallowASTNode** nodes = &node->data.Call.arguments.nodes;
	size_t* count = &node->data.Call.arguments.count;
	if(*nodes != NULL) {
	    for(size_t i = 0 ; i < *count ; i++) {
		shallow_ast_node_free(&(*nodes)[i]);
	    }
	    free(*nodes);
	}
    }
    else if(node->type == ShallowASTNodeType_StringConstant) {
	free(node->data.StringConstant.string);
    }
    else if(node->type == ShallowASTNodeType_Semicolon) {
	// Nothing needed to be done.
    }
    else {
	fprintf(stderr, "State: %d\n", node->type);
	PANIC("Not implemented");
    }
}

// Takes ownership of `node`
void shallow_ast_node_array_push(ShallowASTNodeArray* array, ShallowASTNode* node) {
    (void) array;
    (void) node;
    if(array->nodes == NULL) {
	array->nodes = malloc(sizeof(ShallowASTNode));
	array->count = 0;
    }
    else {
	array->nodes = realloc(array->nodes, (array->count+1) * sizeof(ShallowASTNode));
    }
    array->nodes[array->count] = *node;
    array->count += 1;
}

void shallow_ast_node_access_object_member_add_to_path(ShallowASTNode* node, char* path) {
    char*** names = &node->data.AccessObjectMember.path_data.names;
    size_t* count = &node->data.AccessObjectMember.path_data.count;
    if(*names == NULL) {
	*names = malloc(sizeof(char*));
	*count = 0;
    }
    else {
	*names = realloc(*names, (*count+1) * sizeof(char*));
    }
    (*names)[*count] = clone_string(path);
    *count += 1;
}

// Takes ownership
void shallow_ast_node_call_add_argument(ShallowASTNode* node, ShallowASTNode* argument) {
    ShallowASTNode** nodes = &node->data.Call.arguments.nodes;
    size_t* count = &node->data.Call.arguments.count;
    if(*nodes == NULL) {
	*nodes = malloc(sizeof(ShallowASTNode));
	*count = 0;
    }
    else {
	*nodes = realloc(*nodes, (*count+1) * sizeof(ShallowASTNode));
    }
    (*nodes)[*count] = *argument;
    *count += 1;
}

void shallow_ast_node_access_object_member_set_parent(ShallowASTNode* node, char* parent) {
    node->data.AccessObjectMember.object_name = clone_string(parent);
}


ShallowASTNode shallow_ast_node_create_access_object_member(LexerToken* parent) {
    assert(parent->type == LexerTokenType_Identifier);
    ShallowASTNode shallow_ast_node = { 0 };
    shallow_ast_node.type = ShallowASTNodeType_AccessObjectMember;
    shallow_ast_node_access_object_member_set_parent(
	&shallow_ast_node,
	parent->raw
    );
    return shallow_ast_node;
}

ShallowASTNode parser_shallow_get_access_object_member(LexerTokenArray* array, size_t index) {
    assert(array->tokens[index].type == LexerTokenType_Identifier);
    ShallowASTNode shallow_ast_node = shallow_ast_node_create_access_object_member(
	&array->tokens[index] 
    );
    //fprintf(stderr, "%s\n", shallow_ast_node.data.AccessObjectMember.object_name);
    fprintf(stderr, "%s\n", array->tokens[index].raw);
    //shallow_ast_node.data.AccessObjectMember.object_name = clone_string(array->tokens[index].raw);
    for(size_t i = index+1 ; i+1 < array->count ; i+=2) {
	if(array->tokens[i].type != LexerTokenType_Dot) {
	    break;
	}
	else if(array->tokens[i+1].type != LexerTokenType_Identifier) {
	    break;
	}
	else {
	    shallow_ast_node_access_object_member_add_to_path(
		&shallow_ast_node,
		array->tokens[i+1].raw
	    );
	}
    }
    return shallow_ast_node;
}

ShallowASTNode shallow_ast_node_create_call() {
    ShallowASTNode node = { 0 };
    node.type = ShallowASTNodeType_Call;
    return node;
}

ShallowASTNode parser_shallow_get_call(LexerTokenArray* array, size_t* index) {
    ShallowASTNode shallow_ast_node = shallow_ast_node_create_call();
    for(size_t i = *index+1 ; i < array->count ; i++) {
	if(array->tokens[i].type == LexerTokenType_String) {
	    ShallowASTNode argument = shallow_ast_node_create_string_constant(
		array->tokens[i].raw
	    );
	    shallow_ast_node_call_add_argument(&shallow_ast_node, &argument);
	}
	else if(array->tokens[i].type == LexerTokenType_ParenEnd) {
	    *index = i;
	    break;
	}
	else {
	    fprintf(stderr, "TODO: Add message here %d\n", __LINE__);
	    exit(-1);
	}
    }
    return shallow_ast_node;
}

bool parser_shallow_is_access_object_member(LexerTokenArray* lexer_token_array, size_t index) {

    if(index+2 >= lexer_token_array->count) {
	return false;
    }

    size_t identifier_count = 0;
    for(size_t j = index+1 ; j+1 < lexer_token_array->count ; j+=2) {
	if(lexer_token_array->tokens[j].type != LexerTokenType_Dot) {
	    break;
	}
	else if(lexer_token_array->tokens[j+1].type != LexerTokenType_Identifier) {
	    break;
	}
	else {
	    identifier_count += 1;
	}
    }

    if(identifier_count == 0) {
	return false;
    }
    else {
	return true;
    }

}

void shallow_ast_node_print(ShallowASTNode* node) {
    printf(
	"ShallowASTNode:\n"
	"\tType: %d\n",
	node->type
    );
}

void shallow_ast_node_array_print(ShallowASTNodeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
	shallow_ast_node_print(&array->nodes[i]);
    }
}

ShallowASTNodeArray parse_shallow_parse(LexerTokenArray* lexer_token_array) {
    ShallowASTNodeArray array = { 0 };

    for(size_t i = 0 ; i < lexer_token_array->count ; i++) {
	if(lexer_token_array->tokens[i].type == LexerTokenType_Identifier) {
	    if(parser_shallow_is_access_object_member(lexer_token_array, i)) {
		ShallowASTNode token = parser_shallow_get_access_object_member(
		    lexer_token_array,
		    i
		);
		shallow_ast_node_array_push(&array, &token);

		// Do dumb math to get the next index based on the the
		// `parser_shallow_get_Access_object_member`'s algorithm
		i += token.data.AccessObjectMember.path_data.count * 2;
	    }
	}
	else if(lexer_token_array->tokens[i].type == LexerTokenType_ParenStart) {
	    ShallowASTNode token = parser_shallow_get_call(lexer_token_array, &i);
	    shallow_ast_node_array_push(&array, &token);
	}
	else if(lexer_token_array->tokens[i].type == LexerTokenType_Semicolon) {
	    ShallowASTNode token = { 0 };
	    token.type = ShallowASTNodeType_Semicolon;
	    shallow_ast_node_array_push(&array, &token);
	}
	else {
	    fprintf(stderr,
		"Token:\n"
		"\tType: %d\n"
		"\tRaw: %s\n",
		lexer_token_array->tokens[i].type,
		lexer_token_array->tokens[i].raw
	    );
	    PANIC("NOT IMPLEMENTED");
	}
    }

    return array;
}

void shallow_ast_node_array_free(ShallowASTNodeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
	shallow_ast_node_free(&array->nodes[i]);
    }
    free(array->nodes);
}

ShallowASTNode* shallow_ast_node_deep_copy(ShallowASTNode* node) {
    if(node->type == ShallowASTNodeType_AccessObjectMember) {
	ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
	output->type = ShallowASTNodeType_AccessObjectMember;

	char** names = malloc(
	    node->data.AccessObjectMember.path_data.count * sizeof(char*)
	);

	for(size_t i = 0 ; i < node->data.AccessObjectMember.path_data.count ; i++) {
	    char* name = clone_string(node->data.AccessObjectMember.path_data.names[i]);
	    names[i] = name;
	}

	char* object_name = node->data.AccessObjectMember.object_name;
	fprintf(stderr, "%s\n", object_name);

	output->data.AccessObjectMember.path_data.names = names;
	output->data.AccessObjectMember.path_data.count = node->data.AccessObjectMember.path_data.count;
	output->data.AccessObjectMember.object_name = clone_string(node->data.AccessObjectMember.object_name);

	return output;

    }
    else if(node->type == ShallowASTNodeType_Call) {
	ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
	memset(output, 0, sizeof(ShallowASTNode));
	output->type = ShallowASTNodeType_Call;
	for(size_t i = 0 ; i < node->data.Call.arguments.count ; i++) {
	    // It just works
	    ShallowASTNode* child = shallow_ast_node_deep_copy(&node->data.Call.arguments.nodes[i]);
	    shallow_ast_node_call_add_argument(output, child);
	    free(child);
	    
	}
	return output;
    }
    else if(node->type == ShallowASTNodeType_StringConstant) {
	ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
	memset(output, 0, sizeof(ShallowASTNode));
	output->type = ShallowASTNodeType_StringConstant;
	output->data.StringConstant.string = clone_string(node->data.StringConstant.string);
	return output;
    }
    else {
	fprintf(stderr, "deep copy is not implemented for: %d\n", node->type);
	PANIC("");
    }
}

ShallowASTNode* shallow_ast_node_call_get_argument(ShallowASTNode* node, size_t index) {
    assert(node->type == ShallowASTNodeType_Call);
    assert(node->data.Call.arguments.count > index);
    return &node->data.Call.arguments.nodes[index];
}

size_t shallow_ast_node_call_get_argument_count(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_Call);
    return node->data.Call.arguments.count;
}


bool shallow_ast_node_access_object_member_compare_parent(ShallowASTNode* node, char* identifier) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    return strcmp(node->data.AccessObjectMember.object_name, identifier) == 0;
}

size_t shallow_ast_node_access_object_member_get_path_count(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    return node->data.AccessObjectMember.path_data.count;
}


char* shallow_ast_node_access_object_member_get_path_part(ShallowASTNode* node, size_t index) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    assert(node->data.AccessObjectMember.path_data.count > index);
    return node->data.AccessObjectMember.path_data.names[index];
}

char* shallow_ast_node_string_constant_get_string(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_StringConstant);
    return node->data.StringConstant.string;
}
