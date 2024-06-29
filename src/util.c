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

char* clone_string(char* text_to_copy) {
    char* output = malloc((strlen(text_to_copy)+1) * sizeof(char));
    memset(output, 0, (strlen(text_to_copy)+1) * sizeof(char));
    memcpy(output, text_to_copy, strlen(text_to_copy));
    return output;
}
