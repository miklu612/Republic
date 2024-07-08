#include"ast.h"
#include"../util.h"

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
    else if(node->type == ASTNodeType_CreateConstVariable) {
        if(node->data.CreateConstVariable.node != NULL) {
            shallow_ast_node_free(node->data.CreateConstVariable.node);
            free(node->data.CreateConstVariable.node);
        }
        else {
            fprintf(stderr, "Todo: Add message\n");
            PANIC("");
        }
    }
    else {
        fprintf(stderr, "Free not implemented for %d\n", node->type);
        PANIC("Free not implemented");
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

const char* ast_node_create_const_variable_get_name(const ASTNode* node) {
    assert(node->type == ASTNodeType_CreateConstVariable);
    return shallow_ast_node_create_const_variable_get_name(node->data.CreateConstVariable.node);
}

double ast_node_create_const_variable_get_number(ASTNode* node) {
    assert(node->type == ASTNodeType_CreateConstVariable);
    return expression_get_number(&node->data.CreateConstVariable.node->data.CreateConstVariable.expression);
}

