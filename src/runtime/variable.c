#include"variable.h"
#include"../util.h"

#include<stdio.h>

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

