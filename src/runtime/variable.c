
#include"runtime.h"
#include"variable.h"
#include"../util.h"

#include<stdio.h>
#include<string.h>

void runtime_variable_set_name(RuntimeVariable* variable, char* name) {
    variable->name = clone_string(name);
}

void runtime_variable_set_value_number(RuntimeVariable* variable, double value) {
    variable->type = VariableType_Number;
    variable->value.number = value;
}

RuntimeVariable runtime_variable_create() {
    return (RuntimeVariable) { 0 };
}

RuntimeVariable runtime_variable_create_number(char* name, double value) {
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
    if(variable->type == VariableType_Number) {
        // Nothing needed to be done.
    }
    else {
        PANIC("Free for this is not implemented");
    }
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
        if(array->variables[i].type == VariableType_Number) {
            printf("\tValue: %f\n", array->variables[i].value.number);
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

RuntimeVariable runtime_variable_create_multitoken(ShallowASTNodeArray* array, Runtime* runtime) {
    (void) runtime;
    RuntimeVariable variable = { 0 };
    variable.type = VariableType_Number;
    for(size_t i = 0 ; i < array->count ; i++) {
        if(array->nodes[i].type == ShallowASTNodeType_Addition) {
            double left_value = 0;
            double right_value = 0;
            if(array->nodes[i].data.Addition.left.type == ExpressionType_Identifier) {
                RuntimeVariable* variable = runtime_variable_array_get(
                    &runtime->variables, 
                    array->nodes[i].data.Addition.left.value.identifier
                );
                assert(variable != NULL);
                assert(variable->type == VariableType_Number);
                left_value = variable->value.number;
            }
            else if(array->nodes[i].data.Addition.left.type == ExpressionType_Number) {
                left_value = array->nodes[i].data.Addition.left.value.number;
            }
            if(array->nodes[i].data.Addition.right.type == ExpressionType_Identifier) {
                RuntimeVariable* variable = runtime_variable_array_get(
                    &runtime->variables, 
                    array->nodes[i].data.Addition.right.value.identifier
                );
                assert(variable != NULL);
                assert(variable->type == VariableType_Number);
                right_value = variable->value.number;
            }
            else if(array->nodes[i].data.Addition.right.type == ExpressionType_Number) {
                right_value = array->nodes[i].data.Addition.left.value.number;
            }
            variable.value.number = left_value + right_value;
        }
        else {
            fprintf(stderr, "Todo: Add message\n");
            PANIC("");
        }
    }
    return variable;
}
