#include"object.h"
#include"../util.h"

#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<stdio.h>

void object_property_array_push(ObjectPropertyArray* array, const char* name, const Value* value) {
    assert(array != NULL);
    assert(name != NULL);
    assert(value != NULL);
    if(array->properties == NULL) {
        array->properties = calloc(1, sizeof(ObjectProperty));
        array->count = 0;
    }
    else {
        array->properties = realloc(
            array->properties, 
            (array->count+1) * sizeof(ObjectProperty)
        );
    }

    array->properties[array->count].identifier = clone_string(name);
    array->properties[array->count].value = value_clone(value);
    array->count += 1;

}

void object_add_property(Object* object, const char* name, const Value* value) {
    assert(object != NULL);
    assert(name != NULL);
    assert(value != NULL);
    object_property_array_push(&object->properties, name, value);
}

Object* object_create(const ShallowASTNode* node) {

    assert(node != NULL);

    if(node->type != ShallowASTNodeType_CreateObject) {
        fprintf(stderr,
            "Expected: ShallowASTNodeType_CreateObject (%d)\n"
            "Got: %d\n"
            ,
            ShallowASTNodeType_CreateObject,
            node->type
        );
        PANIC("");
    }

    Object* object = calloc(1, sizeof(Object));

    const ShallowASTNodeArray* tokens = node->data.CreateObject.tokens;
    for(size_t i = 0 ; i < tokens->count ; i += 2) {
        const ShallowASTNode* token = &tokens->nodes[i];
        assert(token != NULL);
        if(token->type != ShallowASTNodeType_CreateObjectProperty) {
            fprintf(stderr,
                "Expected: ShallowASTNodeType_CreateObjectProperty (%d)\n"
                "Got: %d\n"
                ,
                ShallowASTNodeType_CreateObjectProperty,
                token->type
            );
            PANIC("");
        }

        object_add_property(
            object,
            token->data.CreateObjectProperty.identifier,
            &token->data.CreateObjectProperty.value
        );

    }

    return object;
}

void object_property_free(ObjectProperty* property) {
    free(property->identifier);
    value_free(&property->value);
}

void object_property_array_free(ObjectPropertyArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
        object_property_free(&array->properties[i]);
    }
    free(array->properties);
}

void object_free(Object* object) {
    object_property_array_free(&object->properties);
}

Value* object_get_property(Object* object, const char* name) {
    return object_property_array_get(&object->properties, name);
}

Value* object_property_array_get(ObjectPropertyArray* array, const char* name) {
    for(size_t i = 0 ; i < array->count ; i++) {
        if(strcmp(array->properties[i].identifier, name) == 0) {
            return &array->properties[i].value;
        }
    }
    fprintf(stderr, "Todo: Add message\n");
    PANIC("");
}
