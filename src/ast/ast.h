#ifndef ast_h
#define ast_h

#include<stdlib.h>
#include"shallow_parse.h"

enum ASTNodeType {
    ASTNodeType_FunctionCall,
};


// EVERY FUNCTION RELATED TO THIS WILL TAKE OWNERSHIP UNLESS STATED
// OTHERWISE!!!
typedef struct ASTNode_struct {
    enum ASTNodeType type;

    union {

	
	// ASTNodeType_FunctionCall
	// function_call:
	//   accepted types - ShallowASTNodeType_Call
	struct {
	    ShallowASTNode* call;
	    ShallowASTNode* object;
	} FunctionCall;

    } data;

} ASTNode;

typedef struct {
    ASTNode* nodes;
    size_t count;
} ASTNodeArray;

void ast_node_free(ASTNode*);
void ast_node_array_push(ASTNodeArray*, ASTNode*);
void ast_node_array_free(ASTNodeArray*);
void ast_node_array_print(ASTNodeArray*);
ASTNode ast_node_create_function_call(ShallowASTNode* object, ShallowASTNode* call);

#endif
