#ifndef value_h
#define value_h


struct Object;

enum ValueType {
    ValueType_Invalid,
    ValueType_Number,
    ValueType_Object,
    ValueType_Variable,
};

typedef struct Value {
    enum ValueType type;
    union {
        double number;
        struct Object* object;
    } value;
} Value;

Value value_create_from_double(double);
Value value_clone(const Value*);
void value_free(Value* value);
double value_get_number(const Value*);

#endif
