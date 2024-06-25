#ifndef shallow_parse_h
#define shallow_parse_h

#include"../lexer/lexer.h"

enum ShallowASTNodeType {
    ShallowASTNodeType_AccessObjectMember,
    ShallowASTNodeType_Call,
    ShallowASTNodeType_StringConstant,
    ShallowASTNodeType_Semicolon,
};

// These are "shallow" ast nodes. This means they are more simple than actual
// ast nodes, but this is for a reason. First we want to create a general
// overview of the code and then try to make sense of it.
typedef struct ShallowASTNode_struct {
    enum ShallowASTNodeType type;
    union {
	
	// object_name - The parent object that you are trying to get the data
	//               from.
	//
	// path_data   - The path that you are trying to access. 
	//
	// Example:
	//   {object_name}.{path_data[0]}.{path_data[1]}
	struct {
	    char* object_name;
	    struct {
		char** names;
		size_t count;
	    } path_data;
	} AccessObjectMember;

	struct {
	    struct {
		struct ShallowASTNode_struct* nodes;
		size_t count;
	    } arguments;
	} Call;

	struct {
	    char* string;
	} StringConstant;

    } data;
} ShallowASTNode;

typedef struct {
    ShallowASTNode* nodes;
    size_t count;
} ShallowASTNodeArray;

ShallowASTNode shallow_ast_node_create_string_constant(char* string);
void shallow_ast_node_free(ShallowASTNode* node);
void shallow_ast_node_array_push(ShallowASTNodeArray* array, ShallowASTNode* node);
void shallow_ast_node_access_object_member_add_to_path(ShallowASTNode* node, char* path);
void shallow_ast_node_call_add_argument(ShallowASTNode* node, ShallowASTNode* argument);
void shallow_ast_node_access_object_member_set_parent(ShallowASTNode* node, char* parent);
ShallowASTNode shallow_ast_node_create_access_object_member(LexerToken* parent);
ShallowASTNode parser_shallow_get_access_object_member(LexerTokenArray* array, size_t index);
ShallowASTNode shallow_ast_node_create_call();
ShallowASTNode parser_shallow_get_call(LexerTokenArray* array, size_t* index);
bool parser_shallow_is_access_object_member(LexerTokenArray* lexer_token_array, size_t index);
void shallow_ast_node_print(ShallowASTNode* node);
void shallow_ast_node_array_print(ShallowASTNodeArray* array);
ShallowASTNodeArray parse_shallow_parse(LexerTokenArray* lexer_token_array);
void shallow_ast_node_array_free(ShallowASTNodeArray* array);
ShallowASTNode* shallow_ast_node_deep_copy(ShallowASTNode*);

ShallowASTNode* shallow_ast_node_call_get_argument(ShallowASTNode*, size_t index);
size_t shallow_ast_node_call_get_argument_count(ShallowASTNode*);

bool shallow_ast_node_access_object_member_compare_parent(ShallowASTNode*, char* identifier);
size_t shallow_ast_node_access_object_member_get_path_count(ShallowASTNode*);
char* shallow_ast_node_access_object_member_get_path_part(ShallowASTNode*, size_t index);

char* shallow_ast_node_string_constant_get_string(ShallowASTNode*);

#endif
