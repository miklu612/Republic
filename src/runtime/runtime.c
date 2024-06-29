
#include"runtime.h"
#include"../util.h"
#include<assert.h>
#include<stdio.h>
#include<string.h>

RuntimeVariable* runtime_get_variable(Runtime* runtime, char* name) {
    RuntimeVariable* variable = runtime_variable_array_get(&runtime->variables, name);
    assert(variable != NULL);
    return variable;
}

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
            if(arg->type == ShallowASTNodeType_StringConstant) {
                const char* string = shallow_ast_node_string_constant_get_string(arg);
                printf("%s\n", string);
            }
            else if(arg->type == ShallowASTNodeType_AccessIdentifier) {
                char* name = shallow_ast_node_access_identifier_get_name(arg);
                RuntimeVariable* variable = runtime_get_variable(runtime, name);
                if(variable->type == VariableType_Number) {
                    printf("%f\n", variable->value.number);
                }
                else {
                    fprintf(stderr, "Logging is not implemented for variable type: %d\n", variable->type);
                    PANIC("");
                }
            }
            else {
                fprintf(stderr, "Log not implemented for: (ShallowASTNodeType) %d\n", arg->type);
                PANIC("");
            }
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

void runtime_create_const_variable(Runtime* runtime, ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    char* name = shallow_ast_node_create_const_variable_get_name(node);
    (void) name;
    (void) runtime;
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
        else if(current->type == ASTNodeType_CreateConstVariable) {
            char* name = ast_node_create_const_variable_get_name(current);
            const double value = ast_node_create_const_variable_get_number(current);
            RuntimeVariable runtime_variable = runtime_variable_create_number(name, value);
            runtime_variable_array_push(&runtime.variables, &runtime_variable);
        }
        else {
            PANIC("Type not implemented");
        }
    }
    printf("============ Stopping runtime! ============\n");

    runtime_variable_array_log_variables(&runtime.variables);
    runtime_variable_array_free(&runtime.variables);

}



