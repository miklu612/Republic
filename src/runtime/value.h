#ifndef value_h
#define value_h

struct Object;

enum ValueType {
    ValueType_ERROR_VALUE,
    ValueType_Number,
    ValueType_Object,
};

typedef struct Value {
    enum ValueType type;
    union {
        double number;
        struct Object* object;
    } value;
} Value;

Value value_clone(const Value*);
void value_free(Value* value);

#endif
