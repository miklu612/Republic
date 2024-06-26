#include"formatter.h"

#include<string.h>
#include<stddef.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>

char* formatter_format_code(char* code) {

    const size_t len = strlen(code);
    char* output = calloc(sizeof(char), strlen(code) + 1);
    output[0] = '\0';

    bool in_comment = false;

    for(size_t i = 0 ; i < len ; i++) {
	if(code[i] == '/' && code[i+1] == '/' && !in_comment) {
	    printf("In comment\n");
	    in_comment = true;
	}
	else if(in_comment) {
	    if(code[i] == '\n') {
		in_comment = false;
		output[strlen(output)+1] = '\0';
	    	output[strlen(output)] = code[i];
	    }
	}
	else {
	    output[strlen(output)+1] = '\0';
	    output[strlen(output)] = code[i];
	}
    }

    return output;


}

