#include"util.h"



bool is_letter(char character) {
    if(character >= 'a' && character <= 'z') {
	return true;
    }
    else if(character >= 'A' && character <= 'Z') {
	return true;
    }
    else {
	return false;
    }
}

bool is_digit(char character) {
    return character <= '9' && character >= '0';
}
