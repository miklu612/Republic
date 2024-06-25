#ifndef runtime_h
#define runtime_h

#include"../ast/ast.h"

typedef struct {
    char buffer;
} Runtime;

void runtime_start(ASTNodeArray*);

#endif
