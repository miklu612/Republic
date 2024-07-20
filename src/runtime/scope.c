#include"scope.h"
#include"../util.h"

#include<assert.h>
#include<stdio.h>
#include<string.h>



void runtime_scope_array_push(RuntimeScopeArray* array, RuntimeScope* scope) {
    assert(array != NULL);
    assert(scope != NULL);
    scope->references += 1;
    generic_push(
        (void**) &array->scopes,
        &array->count,
        sizeof(RuntimeScope),
        scope
    );
}

void runtime_scope_add_child(RuntimeScope* scope, RuntimeScope* child) {
    assert(scope != NULL);
    assert(child != NULL);
    runtime_scope_array_push(
        &scope->children,
        child
    );
}

void runtime_scope_array_free(RuntimeScopeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
        runtime_scope_free(&array->scopes[i]);
    }
}

void runtime_scope_free(RuntimeScope* scope) {
    assert(scope != NULL);
    assert(scope->references > 0);
    scope->references -= 1;
    if(scope->references == 0) {
        runtime_variable_array_free(&scope->variables);
        runtime_scope_array_free(&scope->children);
        ast_node_array_free(&scope->ast_nodes);
        runtime_function_array_free(&scope->functions);
    }
}


RuntimeScope runtime_scope_create_global() {
    RuntimeScope scope = { 0 };
    scope.references = 1;
    return scope;
}

void runtime_scope_set_ast_nodes(RuntimeScope* scope, const ASTNodeArray* array) {
    assert(scope != NULL);
    assert(array != NULL);
    scope->ast_nodes = ast_node_array_clone(array);
}

void runtime_scope_add_variable(RuntimeScope* scope, RuntimeVariable* variable) {
    assert(scope != NULL);
    assert(variable != NULL);
    runtime_variable_array_push(&scope->variables, variable);
}

RuntimeVariable* runtime_scope_get_variable(RuntimeScope* scope, const char* name) {
    assert(scope != NULL);
    assert(name != NULL);
    RuntimeVariable* variable = runtime_variable_array_get(&scope->variables, name);
    assert(variable != NULL);
    return variable;
}

const RuntimeFunction* runtime_scope_get_function(const RuntimeScope* scope, const ShallowASTNode* node) {
    assert(scope != NULL);
    assert(node != NULL);
    return runtime_function_array_get(&scope->functions, node);
}

Value* runtime_scope_get_object_property(RuntimeScope* scope, const ShallowASTNode* path) {
    assert(path->type == ShallowASTNodeType_AccessObjectMember);
    assert(shallow_ast_node_access_object_member_get_path_count(path) == 1);
    RuntimeVariable* var = runtime_variable_array_get(&scope->variables, path->data.AccessObjectMember.object_name);
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


Value runtime_scope_get_value(RuntimeScope* scope, const ShallowASTNode* node) {

    assert(scope != NULL);
    assert(node != NULL);

    if(node->type == ShallowASTNodeType_NumberConstant) {
        double number = shallow_ast_node_number_constant_get_value(node);
        Value value = { 0 };
        value.type = ValueType_Number;
        value.value.number = number;
        return value;
    }
    else if(node->type == ShallowASTNodeType_AccessIdentifier) {
        RuntimeVariable* var = runtime_scope_get_variable(
            scope, 
            node->data.AccessIdentifier.name
        );
        assert(var->value.type == ValueType_Number);
        return var->value;
    }
    else if(node->type == ShallowASTNodeType_AccessObjectMember) {
      // TODO: Figure out a better way of handling values. This is just dumb,
      //       but the older architecture forces me to do this.
      Value *var_ptr = runtime_scope_get_object_property(scope, node);
      Value value = *var_ptr;
      assert(value.type == ValueType_Number);
      return value;
    }
    else {
        fprintf(stderr,
            "Type not implemented (ShallowASTNodeType) %d\n"
            ,
            node->type
        );
        PANIC();
    }

}

RuntimeScope runtime_scope_create_from_function(const RuntimeFunction* function) {

    assert(function != NULL);

    RuntimeScope scope = { 0 };

    scope.ast_nodes = ast_node_array_clone(&function->body);

    return scope;

}

void runtime_scope_function_call(RuntimeScope* scope, ShallowASTNode* call, ShallowASTNode* object) {
    assert(call->type == ShallowASTNodeType_Call);
    if(object->type == ShallowASTNodeType_AccessObjectMember) {
        if(shallow_ast_node_access_object_member_compare_parent(object, "console")) {
            assert(shallow_ast_node_access_object_member_get_path_count(object) == 1);
            const char* path = shallow_ast_node_access_object_member_get_path_part(object, 0);
            if(strcmp(path, "log") == 0) {
                ShallowASTNode* arg = shallow_ast_node_call_get_argument(call, 0);
                if(arg->type == ShallowASTNodeType_StringConstant) {
                    const char* string = shallow_ast_node_string_constant_get_string(arg);
                    printf("%s\n", string);
                }
                else if(arg->type == ShallowASTNodeType_AccessIdentifier) {
                    char* name = shallow_ast_node_access_identifier_get_name(arg);
                    RuntimeVariable* variable = runtime_scope_get_variable(scope, name);
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
                assert(call->data.Call.arguments.nodes[1].type == ShallowASTNodeType_DoubleEquals);

                Value v_left = runtime_scope_get_value(
                    scope, 
                    &call->data.Call.arguments.nodes[0]
                );

                Value v_right = runtime_scope_get_value(
                    scope, 
                    &call->data.Call.arguments.nodes[2]
                );

                const double left = value_get_number(&v_left);
                const double right = value_get_number(&v_right);

                assert(left == right);

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
    else if(object->type == ShallowASTNodeType_AccessIdentifier) {

        const RuntimeFunction* function = runtime_scope_get_function(
            scope,
            object
        );
        assert(function != NULL);

        RuntimeScope child_scope = runtime_scope_create_from_function(function);
        child_scope.references = 1;

        runtime_scope_run(&child_scope);
        assert(child_scope.references == 1);
        runtime_scope_free(&child_scope);

    }
    else {
        fprintf(stderr, "Unknown call type (ShallowASTNodeType) %d\n", object->type);
        PANIC();
    }
}


void runtime_scope_run(RuntimeScope* scope) {

    assert(scope != NULL);

    const ASTNodeArray* array = &scope->ast_nodes;
    
    for(size_t i = 0 ; i < array->count ; i++) {
        ASTNode* current = &array->nodes[i];
        if(current->type == ASTNodeType_FunctionCall) {
            ShallowASTNode* call = current->data.FunctionCall.call;
            ShallowASTNode* object = current->data.FunctionCall.object;
            runtime_scope_function_call(scope, call, object);
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
                runtime_scope_add_variable(scope, &runtime_variable);
            }
            else if(expression->type == ExpressionType_Object) {
                RuntimeVariable variable = runtime_variable_create_object(current);
                runtime_scope_add_variable(scope, &variable);
            }
            else if(expression->type == ExpressionType_Multitoken) {
                RuntimeVariable variable = runtime_variable_create_multitoken(scope, current);
                runtime_scope_add_variable(scope, &variable);
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
        else if(array->nodes[i].type == ASTNodeType_FunctionDeclaration) {

            RuntimeFunction function = { 0 };

            function.identifier = clone_string(
                ast_node_function_declaration_get_identifier(&array->nodes[i])
            );

            // TODO: Change access into function, so ownership semantics are
            //       clearer.
            function.arguments = string_array_clone(
                &array->nodes[i].data.FunctionDeclaration.arguments
            );

            // TODO: Change access into function, so ownership semantics are
            //       clearer.
            function.body = ast_node_array_clone(
                array->nodes[i].data.FunctionDeclaration.body
            );

            runtime_function_array_push_internals(
                &scope->functions,
                &function
            );
        }
        else {
            fprintf(stderr,
                "Unknown ASTNodeType received in runtime\n"
                "Type: (ASTNodeType) %d\n"
                ,
                array->nodes[i].type
            );
            PANIC();
        }
    }

}

