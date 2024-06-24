#ifndef ast_h
#define ast_h

#include<stdlib.h>

enum ASTNodeType {
    ASTNodeType_ObjectAccess,
    ASTNodeType_FunctionCall,
};

typedef struct ASTNode_struct {
    enum ASTNodeType type;

    union {

	// ASTNodeType_ObjectAccess
	struct {
	    char* name;
	    struct ASTNode_struct* next_action;
	    struct ASTNode_struct* next;
	} object_access;
	
	// ASTNodeType_FunctionCall
	struct {
	    struct {
		struct ASTNode_struct* arg;
		size_t count;
	    } arguments;
	    struct ASTNode_struct* function;
	    struct ASTNode_struct* next;
	} function_call;

    } arg;

} ASTNode;

typedef struct {
    ASTNode* nodes;
    size_t count;
} ASTNodeArray;

void ast_node_free(ASTNode*);
void ast_node_array_free(ASTNodeArray*);

#endif
