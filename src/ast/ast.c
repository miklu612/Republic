#include"ast.h"

#include<stdio.h>

void ast_node_free(ASTNode* node) {
    if(node->type == ASTNodeType_FunctionCall) {
        ShallowASTNode* call = node->data.FunctionCall.call;
        ShallowASTNode* object = node->data.FunctionCall.object;
        if(call != NULL) {
	   shallow_ast_node_free(call);
    	   free(call);
        }
        if(object != NULL) {
	   shallow_ast_node_free(object);
    	   free(object);
        }
    }
}

void ast_node_array_print(ASTNodeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++){
	printf(
	    "ASTNode:\n"
	    "\tType: %d\n"
	    ,
	    array->nodes[i].type
	);
    }
}

