#ifndef function_h
#define function_h

#include"../data_structures/string_array.h"
#include"../ast/ast.h"

typedef struct RuntimeFunction {
    char* identifier;
    StringArray arguments;
    ASTNodeArray body;
} RuntimeFunction;

typedef struct RuntimeFunctionArray {
    size_t count;
    RuntimeFunction* functions;
} RuntimeFunctionArray;

void runtime_function_array_push_internals(RuntimeFunctionArray*, RuntimeFunction*);
const RuntimeFunction* runtime_function_array_get(const RuntimeFunctionArray*, const ShallowASTNode*);
void runtime_function_array_free(RuntimeFunctionArray*);

RuntimeFunction runtime_function_create(const ASTNode* node);
void runtime_function_free(RuntimeFunction*);
const char* runtime_function_get_identifier(const RuntimeFunction*);
size_t runtime_function_get_argument_count(const RuntimeFunction*);

#endif
