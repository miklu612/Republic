#include"function.h"
#include"../util.h"

#include<string.h>
#include<stdio.h>


void runtime_function_array_push_internals(RuntimeFunctionArray* array, RuntimeFunction* function) {

    assert(array != NULL);
    assert(function != NULL);

    generic_push(
        (void**) &array->functions,
        &array->count,
        sizeof(RuntimeFunction),
        (void*) function
    );



}


const RuntimeFunction* runtime_function_array_get(const RuntimeFunctionArray* array, const ShallowASTNode* node) {

    assert(array != NULL);
    assert(node != NULL);
    assert(node->type == ShallowASTNodeType_AccessIdentifier);

    const char* identifier = node->data.AccessIdentifier.name;

    for(size_t i = 0 ; i < array->count ; i++) {
        const RuntimeFunction* function = &array->functions[i];
        assert(function != NULL);
        assert(function->identifier != NULL);
        if(strcmp(function->identifier, identifier) == 0) {
            return function;
        }
    }

    fprintf(stderr, 
        "Couldn't find function '%s'\n"
        ,
        identifier
    );
    PANIC();

}

void runtime_function_free(RuntimeFunction* function) {
    assert(function != NULL);
    assert(function->identifier != NULL);
    free(function->identifier);
    string_array_free(&function->arguments);
    ast_node_array_free(&function->body);
}

void runtime_function_array_free(RuntimeFunctionArray* array) {
    assert(array != NULL);
    for(size_t i = 0 ; i < array->count ; i++) {
        runtime_function_free(&array->functions[i]);
    }
    free(array->functions);
}

