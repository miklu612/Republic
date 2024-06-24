#include"ast.h"


void ast_node_free(ASTNode* node) {
    // TODO: Implement
    (void) node;
}

void ast_node_array_free(ASTNodeArray* ast_node_array) {
    if(ast_node_array->nodes != NULL) {
	for(size_t i = 0 ; i < ast_node_array->count ; i++) {
    	    ast_node_free(&ast_node_array->nodes[i]);
    	}
	free(ast_node_array->nodes);
    }
}

