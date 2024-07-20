
#include"runtime.h"
#include"../util.h"
#include<assert.h>
#include<stdio.h>
#include<string.h>

void runtime_start(ASTNodeArray* array) {

    RuntimeScope global_scope = runtime_scope_create_global();
    runtime_scope_set_ast_nodes(&global_scope, array);

    printf("============ Starting runtime! ============\n");
    runtime_scope_run(&global_scope);
    runtime_scope_free(&global_scope);
    printf("============ Stopping runtime! ============\n");

}

