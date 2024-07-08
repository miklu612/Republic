#ifndef runtime_h
#define runtime_h

#include"../ast/ast.h"
#include"variable.h"


typedef struct Runtime {
    char buffer;
    RuntimeVariableArray variables;
} Runtime;

void runtime_start(ASTNodeArray*);
Value* runtime_get_object_property(Runtime*, const ShallowASTNode* path);
double runtime_get_value_with_node(Runtime* runtime, const ShallowASTNode* node);

#endif
