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
    else if(node->type == ASTNodeType_FunctionDeclaration) {
        ast_node_array_free(node->data.FunctionDeclaration.body);
        free(node->data.FunctionDeclaration.body);
        free(node->data.FunctionDeclaration.identifier);
        string_array_free(&node->data.FunctionDeclaration.arguments);
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

ASTNode ast_node_clone(const ASTNode* node) {
    assert(node != NULL);
    if(node->type == ASTNodeType_FunctionDeclaration) {
        ASTNode output = { 0 };
        output.type = ASTNodeType_FunctionDeclaration;
        output.data.FunctionDeclaration.identifier = clone_string(
            node->data.FunctionDeclaration.identifier
        );
        output.data.FunctionDeclaration.arguments = string_array_clone(
            &node->data.FunctionDeclaration.arguments
        );
        output.data.FunctionDeclaration.body = calloc(1, sizeof(ASTNodeArray));
        *output.data.FunctionDeclaration.body = ast_node_array_clone(
            node->data.FunctionDeclaration.body
        );
        return output;
    }
    else if(node->type == ASTNodeType_CreateConstVariable) {
        ASTNode output = { 0 };
        output.type = ASTNodeType_CreateConstVariable;
        output.data.CreateConstVariable.node = shallow_ast_node_deep_copy(
            node->data.CreateConstVariable.node
        );
        return output;
    }
    else if(node->type == ASTNodeType_FunctionCall) {
        ASTNode output = { 0 };
        output.type = ASTNodeType_FunctionCall;
        output.data.FunctionCall.call = shallow_ast_node_deep_copy(
            node->data.FunctionCall.call
        );
        output.data.FunctionCall.object = shallow_ast_node_deep_copy(
            node->data.FunctionCall.object
        );
        return output;
    }
    else {
        fprintf(stderr,
            "ast_node_clone is not implemented for (ASTNodeType) %d\n"
            ,
            node->type
        );
        PANIC();
    }
}

ASTNodeArray ast_node_array_clone(const ASTNodeArray* array) {
    assert(array != NULL);
    ASTNodeArray output = { 0 };
    for(size_t i = 0 ; i < array->count ; i++) {
        ASTNode node = ast_node_clone(&array->nodes[i]);
        ast_node_array_push(&output, &node);
    }
    return output;
}

const char* ast_node_function_declaration_get_identifier(const ASTNode* node) {
    assert(node != NULL);
    assert(node->type == ASTNodeType_FunctionDeclaration);
    return node->data.FunctionDeclaration.identifier;
}
