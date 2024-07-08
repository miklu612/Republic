#include"string_array.h"
#include"../util.h"

#include<assert.h>
#include<stdlib.h>


void string_array_push(StringArray* array, const char* text) {
    assert(array != NULL);
    assert(text != NULL);

    if(array->count == 0) {
        array->strings = calloc(1, sizeof(char*));
        array->count = 0;
    }
    else {
        array->strings = realloc(
            array->strings, 
            (array->count+1) * sizeof(char*)
        );
    }

    assert(array->strings != NULL);
    array->strings[array->count] = clone_string(text);
    array->count += 1;

}

void string_array_free(StringArray* array) {
    assert(array != NULL);
    for(size_t i = 0 ; i < array->count ; i++) {
        free(array->strings[i]);
    }
    free(array->strings);
}


StringArray string_array_clone(const StringArray* array) {
    assert(array != NULL);
    StringArray output = { 0 };
    for(size_t i = 0 ; i < array->count ; i++) {
        string_array_push(&output, array->strings[i]);
    }
    return output;
}
