#ifndef ast_h
#define ast_h

#include<stdlib.h>
#include"shallow_parse.h"

enum ASTNodeType {
    ASTNodeType_Invalid,
    ASTNodeType_FunctionCall,
    ASTNodeType_CreateConstVariable,
    ASTNodeType_FunctionDeclaration,
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

        struct {
            ShallowASTNode* node;
        } CreateConstVariable;

        struct {
            char* identifier;
            StringArray arguments;
            struct ASTNodeArray* body;
        } FunctionDeclaration;

    } data;

} ASTNode;

typedef struct ASTNodeArray {
    ASTNode* nodes;
    size_t count;
} ASTNodeArray;

void ast_node_free(ASTNode*);
void ast_node_array_push(ASTNodeArray*, ASTNode*);
void ast_node_array_free(ASTNodeArray*);
void ast_node_array_print(ASTNodeArray*);
ASTNodeArray ast_node_array_clone(const ASTNodeArray*);

ASTNode ast_node_create_function_call(ShallowASTNode* object, ShallowASTNode* call);


const char* ast_node_create_const_variable_get_name(const ASTNode*);
double ast_node_create_const_variable_get_number(ASTNode*);
enum ExpressionType ast_node_create_const_variable_get_expression_type(ASTNode*);
ASTNode ast_node_clone(const ASTNode*);

const char* ast_node_function_declaration_get_identifier(const ASTNode*);



#endif
