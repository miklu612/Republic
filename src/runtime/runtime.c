
#include"runtime.h"
#include"../util.h"
#include<assert.h>
#include<stdio.h>
#include<string.h>

void runtime_function_call(Runtime* runtime, ShallowASTNode* call, ShallowASTNode* object) {
    (void) runtime;
    assert(call->type == ShallowASTNodeType_Call);
    if(object->type == ShallowASTNodeType_AccessObjectMember) {
	if(shallow_ast_node_access_object_member_compare_parent(object, "console")) {
	    assert(shallow_ast_node_access_object_member_get_path_count(object) == 1);
	    assert(shallow_ast_node_call_get_argument_count(call) == 1);
	    const char* path = shallow_ast_node_access_object_member_get_path_part(object, 0);
	    assert(strcmp(path, "log") == 0);
	    ShallowASTNode* arg = shallow_ast_node_call_get_argument(call, 0);
	    assert(arg->type == ShallowASTNodeType_StringConstant);
	    const char* string = shallow_ast_node_string_constant_get_string(arg);
	    printf("%s\n", string);
	}
	else {
	    fprintf(stderr, "Only console object is supported for now\n");
	    PANIC("");
	}
    }
    else {
	fprintf(stderr, "%d\n", object->type);
    }
}

void runtime_start(ASTNodeArray* array) {
    Runtime runtime = { 0 };
    
    printf("============ Starting runtime! ============\n");
    for(size_t i = 0 ; i < array->count ; i++) {
	ASTNode* current = &array->nodes[i];
	if(current->type == ASTNodeType_FunctionCall) {
	    ShallowASTNode* call = current->data.FunctionCall.call;
	    ShallowASTNode* object = current->data.FunctionCall.object;
	    runtime_function_call(&runtime, call, object);
	}
	else {
	    PANIC("aaaaaaaaaaaa");
	}
    }
}



