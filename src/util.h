#ifndef util_h
#define util_h

#include<stdbool.h>
#include<stddef.h>
#include<assert.h>

#define TODO(message) assert(0 && "Todo: " message)
#define PANIC(...) assert(0)


bool is_letter(char character);
bool is_digit(char character);
char* clone_string(const char*);

// This function is used to create arrays for any kind of element.
void generic_push(void** ptr, size_t* ptr_count, size_t element_size, void* element);


#endif
