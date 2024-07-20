#ifndef runtime_variable_h
#define runtime_variable_h

#include"../ast/shallow_parse.h"
#include"../ast/ast.h"
#include"object.h"
#include"value.h"

struct Runtime;
struct RuntimeScope;

enum VariableDefType {
    VariableDefType_Let,
    VariableDefType_Var,
    VariableDefType_Const,
};


typedef struct RuntimeVariable {
    char* name;
    enum VariableDefType def_type;
    Value value;
} RuntimeVariable;

typedef struct {
    size_t count;
    RuntimeVariable* variables;
} RuntimeVariableArray;

RuntimeVariable runtime_variable_create();
RuntimeVariable runtime_variable_create_number(const char* name, double value);
void runtime_variable_set_name(RuntimeVariable*, const char* name);
void runtime_variable_set_value_number(RuntimeVariable*, double value);
void runtime_variable_free(RuntimeVariable*);

double runtime_variable_get_number(const RuntimeVariable*);

RuntimeVariable runtime_variable_create_object(const ASTNode* node);
RuntimeVariable runtime_variable_create_multitoken(struct RuntimeScope*, const ASTNode*);

// Takes ownership
void runtime_variable_array_push(RuntimeVariableArray*, RuntimeVariable*);
void runtime_variable_array_free(RuntimeVariableArray*);
void runtime_variable_array_log_variables(RuntimeVariableArray*);
RuntimeVariable* runtime_variable_array_get(RuntimeVariableArray*, const char* name);

#endif
