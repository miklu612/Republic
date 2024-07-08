#ifndef object_h
#define object_h

#include"value.h"
#include"../ast/shallow_parse.h"

#include<stddef.h>

typedef struct ObjectProperty {
    char* identifier;
    Value value;
} ObjectProperty;

typedef struct ObjectPropertyArray {
    size_t count;
    ObjectProperty* properties;
} ObjectPropertyArray;

typedef struct Object {
    ObjectPropertyArray properties;
} Object;

void object_property_free(ObjectProperty*);

void object_property_array_push(ObjectPropertyArray*, const char* name, const Value* value);
void object_property_array_free(ObjectPropertyArray*);
Value* object_property_array_get(ObjectPropertyArray*, const char* name);

void object_free(Object*);
Object* object_create(const ShallowASTNode*);
void object_add_property(Object*, const char* name, const Value* value);
Value* object_get_property(Object*, const char* name);

#endif
