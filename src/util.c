#include"util.h"

#include<string.h>
#include<stdlib.h>


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
        *ptr = realloc(*ptr, element_size * *ptr_count);
    }


    memcpy(ptr[*ptr_count], element, element_size);
    *ptr_count += 1;

}
