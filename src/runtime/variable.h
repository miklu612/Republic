#ifndef runtime_variable_h
#define runtime_variable_h

#include"../ast/shallow_parse.h"
#include"object.h"
#include"value.h"

struct Runtime;

enum VariableDefType {
    VariableDefType_Let,
    VariableDefType_Var,
    VariableDefType_Const,
};


typedef struct {
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

RuntimeVariable runtime_variable_create_object(const ASTNode* node);
RuntimeVariable runtime_variable_create_multitoken(struct Runtime*, const ASTNode*);

// Takes ownership
void runtime_variable_array_push(RuntimeVariableArray*, RuntimeVariable*);
void runtime_variable_array_free(RuntimeVariableArray*);
void runtime_variable_array_log_variables(RuntimeVariableArray*);
RuntimeVariable* runtime_variable_array_get(RuntimeVariableArray*, char* name);

#endif
