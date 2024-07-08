#ifndef string_array_h
#define string_array_h

#include<stddef.h>

typedef struct StringArray {
    char** strings;
    size_t count;
} StringArray;

void string_array_push(StringArray*, const char*);
void string_array_free(StringArray*);
StringArray string_array_clone(const StringArray*);

#endif
