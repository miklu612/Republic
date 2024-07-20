#include"value.h"
#include"object.h"
#include"../util.h"
#include<stdlib.h>
#include<assert.h>
#include<stdio.h>

Value value_clone(const Value* value) {
    assert(value != NULL);
    if(value->type == ValueType_Number) {
        Value output = { 0 };
        output.type = ValueType_Number;
        output.value.number = value->value.number;
        return output;
    }
    else {
        fprintf(stderr, "Todo: Add message\n");
        PANIC("");
    }
}

void value_free(Value* value) {
    if(value->type == ValueType_Number) {
        // Nothing to do
    }
    else if(value->type == ValueType_Object) {
        object_free(value->value.object);
        free(value->value.object);
    }
    else {
        fprintf(stderr, "Todo: Add message\n");
        PANIC("");
    }
}

Value value_create_from_double(double number) {
    Value output = { 0 };
    output.type = ValueType_Number;
    output.value.number = number;
    return output;
}

double value_get_number(const Value* value) {
    assert(value != NULL);
    if(value->type == ValueType_Number) {
        return value->value.number;
    }
    else {
        fprintf(stderr,
            "Expected value type to be ValueType_Number.\n"
            "Got: (ValueType_Number) %d\n"
            ,
            value->type
        );
        PANIC();
    }
}
