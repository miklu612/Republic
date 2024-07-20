#ifndef scope_h
#define scope_h

#include"variable.h"
#include"function.h"

typedef struct RuntimeScopeArray {
    struct RuntimeScope* scopes;
    size_t count;
} RuntimeScopeArray;

typedef struct RuntimeScope {
    RuntimeVariableArray variables;
    RuntimeScopeArray children;
    struct RuntimeScope* parent;
    ASTNodeArray ast_nodes;
    size_t references;
    RuntimeFunctionArray functions;
} RuntimeScope;

void runtime_scope_array_push(RuntimeScopeArray*, RuntimeScope* scope);
void runtime_scope_array_free(RuntimeScopeArray*);

void runtime_scope_add_child(RuntimeScope* scope, RuntimeScope* child);
void runtime_scope_free(RuntimeScope*);
void runtime_scope_run(RuntimeScope*);
Value runtime_scope_get_value(RuntimeScope*, const ShallowASTNode*);
void runtime_scope_add_variable(RuntimeScope*, RuntimeVariable* variable);
RuntimeScope runtime_scope_create_global();
void runtime_scope_set_ast_nodes(RuntimeScope*, const ASTNodeArray*);
RuntimeVariable* runtime_scope_get_variable(RuntimeScope*, const char* name);
const RuntimeFunction* runtime_scope_get_function(const RuntimeScope*, const ShallowASTNode*);

RuntimeScope runtime_scope_create_from_function(const RuntimeFunction*);

#endif
