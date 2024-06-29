#ifndef runtime_h
#define runtime_h

#include"../ast/ast.h"
#include"variable.h"


typedef struct {
    char buffer;
    RuntimeVariableArray variables;
} Runtime;

void runtime_start(ASTNodeArray*);

#endif
