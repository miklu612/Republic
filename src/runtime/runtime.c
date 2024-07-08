
#include"runtime.h"
#include"../util.h"
#include<assert.h>
#include<stdio.h>
#include<string.h>

RuntimeVariable* runtime_get_variable(Runtime* runtime, char* name) {
    RuntimeVariable* variable = runtime_variable_array_get(&runtime->variables, name);
    if(variable == NULL) {
        fprintf(stderr, "Variable '%s' doesn't exist\n", name);
        PANIC();
    }
    return variable;
}

void runtime_function_call(Runtime* runtime, ShallowASTNode* call, ShallowASTNode* object) {
    assert(call->type == ShallowASTNodeType_Call);
    if(object->type == ShallowASTNodeType_AccessObjectMember) {
        if(shallow_ast_node_access_object_member_compare_parent(object, "console")) {
            assert(shallow_ast_node_access_object_member_get_path_count(object) == 1);
            //assert(shallow_ast_node_call_get_argument_count(call) == 1);
            const char* path = shallow_ast_node_access_object_member_get_path_part(object, 0);
            if(strcmp(path, "log") == 0) {
                ShallowASTNode* arg = shallow_ast_node_call_get_argument(call, 0);
                if(arg->type == ShallowASTNodeType_StringConstant) {
                    const char* string = shallow_ast_node_string_constant_get_string(arg);
                    printf("%s\n", string);
                }
                else if(arg->type == ShallowASTNodeType_AccessIdentifier) {
                    char* name = shallow_ast_node_access_identifier_get_name(arg);
                    RuntimeVariable* variable = runtime_get_variable(runtime, name);
                    if(variable->value.type == ValueType_Number) {
                        printf("%f\n", variable->value.value.number);
                    }
                    else {
                        fprintf(stderr, "Logging is not implemented for variable type: %d\n", variable->value.type);
                        PANIC("");
                    }
                }
                else {
                    fprintf(stderr, "Log not implemented for: (ShallowASTNodeType) %d\n", arg->type);
                    PANIC("");
                }
            }
            else if(strcmp(path, "assert") == 0) {

                fprintf(stderr, "%d\n", call->data.Call.arguments.nodes[0].type);
                fprintf(stderr, "%ld\n", call->data.Call.arguments.count);
                assert(call->data.Call.arguments.count == 3);
                assert(call->data.Call.arguments.nodes[0].type == ShallowASTNodeType_AccessIdentifier);
                assert(call->data.Call.arguments.nodes[1].type == ShallowASTNodeType_DoubleEquals);
                assert(call->data.Call.arguments.nodes[2].type == ShallowASTNodeType_NumberConstant);


                const RuntimeVariable* variable = runtime_get_variable(
                    runtime, 
                    call->data.Call.arguments.nodes[0].data.AccessIdentifier.name
                );
                
                const double value = call->data.Call.arguments.nodes[2].data.NumberConstant.number;

                assert(variable != NULL);
                assert(variable->value.type == ValueType_Number);
                assert(variable->value.value.number == value);

            }
            else {
                fprintf(stderr, "Method '%s' is not implemented for object 'console'\n", path);
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
            const char* name = ast_node_create_const_variable_get_name(current);
            const Expression* expression = shallow_ast_node_create_const_variable_get_expression(
                current->data.CreateConstVariable.node
            );
            if(expression->type == ExpressionType_Number) {
                const double value = expression_get_number(expression);
                RuntimeVariable runtime_variable = runtime_variable_create_number(
                    name,
                    value
                );
                runtime_variable_array_push(&runtime.variables, &runtime_variable);
            }
            else if(expression->type == ExpressionType_Object) {
                RuntimeVariable variable = runtime_variable_create_object(current);
                runtime_variable_array_push(&runtime.variables, &variable);
            }
            else if(expression->type == ExpressionType_Multitoken) {
                RuntimeVariable variable = runtime_variable_create_multitoken(&runtime, current);
                runtime_variable_array_push(&runtime.variables, &variable);
            }
            else {
                fprintf(stderr,
                    "Unimplemented expression type\n"
                    "Type: %d\n"
                    ,
                    expression->type
                );
                PANIC();
            }

        }
        else {
            PANIC("Type not implemented");
        }
    }
    printf("============ Stopping runtime! ============\n");

    runtime_variable_array_log_variables(&runtime.variables);
    runtime_variable_array_free(&runtime.variables);

}

Value* runtime_get_object_property(Runtime* runtime, const ShallowASTNode* path) {
    assert(path->type == ShallowASTNodeType_AccessObjectMember);
    assert(shallow_ast_node_access_object_member_get_path_count(path) == 1);
    RuntimeVariable* var = runtime_variable_array_get(&runtime->variables, path->data.AccessObjectMember.object_name);
    assert(var != NULL);
    if(var->value.type != ValueType_Object) {
        fprintf(stderr,
            "Expected: ValueType_Object(%d)\n"
            "Got: %d\n"
            ,
            ValueType_Object,
            var->value.type
        );
        PANIC("");
    }
    return object_get_property(
        var->value.value.object,
        shallow_ast_node_access_object_member_get_path_part(path, 0)
    );
}

