#ifndef util_h
#define util_h

#include<stdbool.h>
#include<assert.h>

#define TODO(message) assert(0 && "Todo: " message)
#define PANIC(...) assert(0)


bool is_letter(char character);
bool is_digit(char character);
char* clone_string(const char*);


#endif
