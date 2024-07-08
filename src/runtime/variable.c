
#include"runtime.h"
#include"variable.h"
#include"../util.h"

#include<stdio.h>
#include<string.h>

void runtime_variable_set_name(RuntimeVariable* variable, const char* name) {
    variable->name = clone_string(name);
}

void runtime_variable_set_value_number(RuntimeVariable* variable, double value) {
    variable->value.type = ValueType_Number;
    variable->value.value.number = value;
}

RuntimeVariable runtime_variable_create() {
    return (RuntimeVariable) { 0 };
}

RuntimeVariable runtime_variable_create_number(const char* name, double value) {
    RuntimeVariable var = runtime_variable_create();
    runtime_variable_set_name(&var, name);
    runtime_variable_set_value_number(&var, value);
    return var;
}

void runtime_variable_array_push(RuntimeVariableArray* array, RuntimeVariable* variable) {

    if(array->variables == NULL) {
        array->variables = calloc(1, sizeof(RuntimeVariable));
        array->count = 0;
    }
    else {
        array->variables = realloc(array->variables, (array->count+1) * sizeof(RuntimeVariable));
    }

    array->variables[array->count] = *variable;
    array->count += 1;

}

void runtime_variable_free(RuntimeVariable* variable) {
    free(variable->name);
    value_free(&variable->value);
}

void runtime_variable_array_free(RuntimeVariableArray* array) {
    if(array->variables != NULL) {
        for(size_t i = 0 ; i < array->count ; i++) {
            runtime_variable_free(&array->variables[i]);
        }
        free(array->variables);
    }
}

void runtime_variable_array_log_variables(RuntimeVariableArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
        printf(
            "Variable:\n"
            "\tName: %s\n"
            ,
            array->variables[i].name
        );
        if(array->variables[i].value.type == ValueType_Number) {
            printf("\tValue: %f\n", array->variables[i].value.value.number);
        }
        else {
            printf("\tValue: <PrintFormatNotImplemnted>\n");
        }
    }
}

RuntimeVariable* runtime_variable_array_get(RuntimeVariableArray* array, char* name) {
    for(size_t i = 0 ; i < array->count ; i++)  {
        if(strcmp(array->variables[i].name, name) == 0) {
            return &array->variables[i];
        }
    }
    return NULL;
}


RuntimeVariable runtime_variable_create_object(const ASTNode* node) {

    assert(node != NULL);
    assert(node->type == ASTNodeType_CreateConstVariable);

    const ShallowASTNode* shallow_node = node->data.CreateConstVariable.node;
    assert(shallow_node != NULL);
    assert(shallow_node->type == ShallowASTNodeType_CreateConstVariable);
    assert(shallow_node->data.CreateConstVariable.expression.type == ExpressionType_Object);

    RuntimeVariable variable = { 0 };
    variable.value.type = ValueType_Object;
    variable.value.value.object = object_create(shallow_node->data.CreateConstVariable.expression.value.object);
    variable.name = clone_string(ast_node_create_const_variable_get_name(node));

    return variable;

}


RuntimeVariable runtime_variable_create_multitoken(Runtime* runtime, const ASTNode* node) {
    assert(node != NULL);
    assert(node->type == ASTNodeType_CreateConstVariable);


    const ShallowASTNode* s_node = node->data.CreateConstVariable.node;
    assert(s_node->type == ShallowASTNodeType_CreateConstVariable);
    const Expression* expression = &s_node->data.CreateConstVariable.expression;
    assert(expression->type == ExpressionType_Multitoken);

    if(expression->value.multitoken->count == 3) {
        RuntimeVariable variable = { 0 };
        variable.value.type = ValueType_Number;
        assert(expression->value.multitoken->count == 3);
        assert(
            expression->value.multitoken->nodes[1].type == ShallowASTNodeType_Plus ||
            expression->value.multitoken->nodes[1].type == ShallowASTNodeType_Minus
        );

        runtime_variable_set_name(&variable, s_node->data.CreateConstVariable.name);
        double left = runtime_get_value_with_node(runtime, &expression->value.multitoken->nodes[0]);
        double right = runtime_get_value_with_node(runtime, &expression->value.multitoken->nodes[2]);

        if(expression->value.multitoken->nodes[1].type == ShallowASTNodeType_Plus) {
            variable.value.value.number = left + right;
        }
        else if(expression->value.multitoken->nodes[1].type == ShallowASTNodeType_Minus) {
            variable.value.value.number = left - right;
        }
        else {
            PANIC();
        }
        return variable;
    }
    else if(expression->value.multitoken->count == 1) {
        assert(expression->value.multitoken->nodes[0].type == ShallowASTNodeType_AccessObjectMember);
        RuntimeVariable variable = { 0 };
        variable.value.type = ValueType_Number;
        runtime_variable_set_name(&variable, s_node->data.CreateConstVariable.name);
        variable.value.value.number = runtime_get_value_with_node(
            runtime,
            &expression->value.multitoken->nodes[0]
        );
        return variable;
    }
    else {
        PANIC();
    }


}
