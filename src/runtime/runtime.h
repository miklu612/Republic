#ifndef runtime_h
#define runtime_h

#include"../ast/ast.h"
#include"variable.h"
#include"scope.h"


typedef struct Runtime {
    char buffer;
    RuntimeScope global_scope;
} Runtime;

void runtime_start(ASTNodeArray*);
Value* runtime_get_object_property(Runtime*, const ShallowASTNode* path);
double runtime_get_value_with_node(Runtime* runtime, const ShallowASTNode* node);

#endif
