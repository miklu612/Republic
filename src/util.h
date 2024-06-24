#ifndef util_h
#define util_h

#include<stdbool.h>
#include<assert.h>

#define TODO(message) assert(0 && "Todo: " message)
#define PANIC(message) assert(0 && "Panic: " message)


bool is_letter(char character);
bool is_digit(char character);
char* clone_string(char*);


#endif
