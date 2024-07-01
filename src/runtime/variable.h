#ifndef runtime_variable_h
#define runtime_variable_h

#include"../ast/shallow_parse.h"

struct Runtime;

enum VariableDefType {
    VariableDefType_Let,
    VariableDefType_Var,
    VariableDefType_Const,
};

enum VariableType {
    VariableType_Number
};

typedef struct {
    char* name;
    enum VariableDefType def_type;
    enum VariableType type;
    struct {
        double number;
    } value;
} RuntimeVariable;

typedef struct {
    size_t count;
    RuntimeVariable* variables;
} RuntimeVariableArray;

RuntimeVariable runtime_variable_create();
RuntimeVariable runtime_variable_create_multitoken(ShallowASTNodeArray*, struct Runtime*);
RuntimeVariable runtime_variable_create_number(char* name, double value);
void runtime_variable_set_name(RuntimeVariable*, char* name);
void runtime_variable_set_value_number(RuntimeVariable*, double value);
void runtime_variable_free(RuntimeVariable*);

// Takes ownership
void runtime_variable_array_push(RuntimeVariableArray*, RuntimeVariable*);
void runtime_variable_array_free(RuntimeVariableArray*);
void runtime_variable_array_log_variables(RuntimeVariableArray*);
RuntimeVariable* runtime_variable_array_get(RuntimeVariableArray*, char* name);

#endif
