#include"util.h"

#include<string.h>
#include<stdlib.h>
#include<stdint.h>


bool is_letter(char character) {
    if(character >= 'a' && character <= 'z') {
        return true;
    }
    else if(character >= 'A' && character <= 'Z') {
        return true;
    }
    else if(character == '_') {
        return true;
    }
    else {
        return false;
    }
}

bool is_digit(char character) {
    return character <= '9' && character >= '0';
}

char* clone_string(const char* text_to_copy) {
    char* output = calloc(strlen(text_to_copy)+1, sizeof(char));
    assert(output != NULL);
    memcpy(output, text_to_copy, strlen(text_to_copy));
    return output;
}

#include<stdio.h>
void generic_push(void** ptr, size_t* ptr_count, size_t element_size, void* element) {

    assert(ptr != NULL);
    assert(ptr_count != NULL);
    assert(element_size > 0);
    assert(element != NULL);

    if(*ptr == NULL) {
        *ptr = calloc(1, element_size);
        *ptr_count = 0;
    }
    else {
        *ptr = realloc(*ptr, element_size * ((*ptr_count)+1));
    }

    // We have to recast the inner ptr to a uint8_t pointer, since the C
    // standard forbids pointer arithmetics on void pointer. Why uint8_t
    // pointer? Because it has a size of 1. Or it should.
    assert(sizeof(uint8_t) == 1);
    uint8_t* inner_ptr = *ptr;
    memcpy(((inner_ptr)+(element_size * (*ptr_count))), element, element_size);
    *ptr_count += 1;

}
