#include"shallow_parse.h"
#include"../util.h"

#include<stddef.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>

// Used to quickly turn known lexer tokens into shallow tokens
ShallowASTNode shallow_ast_node_create_from_lexer_token(LexerToken* token) {

    if(token->type == LexerTokenType_Identifier) {
        ShallowASTNode output = shallow_ast_node_create_access_identifier(token->raw);
        return output;
    }

    else if(token->type == LexerTokenType_NumberConstant) {
        ShallowASTNode output = { 0 };
        output.type = ShallowASTNodeType_NumberConstant;
        output.data.NumberConstant.number = strtod(token->raw, NULL);
        return output;
    }

    else {
        fprintf(stderr, "shallow_ast_node_create_from_lexer is not implemented for this\n");
        PANIC("");
    }
    return (ShallowASTNode) { 0 };
}

ShallowASTNode* shallow_ast_node_create_empty() {
    ShallowASTNode* node = calloc(1, sizeof(ShallowASTNode));
    assert(node != NULL);
    return node;
}

// This does not take ownership
ShallowASTNode shallow_ast_node_create_string_constant(char* string) {
    ShallowASTNode shallow_ast_node = { 0 };
    shallow_ast_node.type = ShallowASTNodeType_StringConstant;
    shallow_ast_node.data.StringConstant.string = clone_string(string);
    return shallow_ast_node;
}

ShallowASTNode shallow_ast_node_create_create_const_variable_number(char* name, double value) {
    ShallowASTNode output = { 0 };
    output.type = ShallowASTNodeType_CreateConstVariable;
    output.data.CreateConstVariable.name = clone_string(name);
    output.data.CreateConstVariable.value.number = value;
    return output;
}

void shallow_ast_node_free(ShallowASTNode* node) {
    if(node->type == ShallowASTNodeType_AccessObjectMember) {
        char*** names = &node->data.AccessObjectMember.path_data.names;
        char** object_name = &node->data.AccessObjectMember.object_name;
        size_t* count = &node->data.AccessObjectMember.path_data.count;
        if(*names != NULL) {
            for(size_t i = 0 ; i < *count ; i++) {
                free((*names)[i]);
            }
            free(*names);
        }
        if(object_name != NULL) {
            free(*object_name);
        }
    }
    else if(node->type == ShallowASTNodeType_Call) {
        ShallowASTNode* nodes = node->data.Call.arguments.nodes;
        size_t count = node->data.Call.arguments.count;
        if(nodes != NULL) {
            for(size_t i = 0 ; i < count ; i++) {
                shallow_ast_node_free(&nodes[i]);
            }
            free(nodes);
        }
    }
    else if(node->type == ShallowASTNodeType_StringConstant) {
        free(node->data.StringConstant.string);
    }
    else if(node->type == ShallowASTNodeType_AccessIdentifier) {
        free(node->data.AccessIdentifier.name);
    }
    else if(node->type == ShallowASTNodeType_ConditionalCheck) {
        shallow_ast_node_free(node->data.ConditionalCheck.left);
        shallow_ast_node_free(node->data.ConditionalCheck.right);
        free(node->data.ConditionalCheck.left);
        free(node->data.ConditionalCheck.right);
    }
    else if(node->type == ShallowASTNodeType_Semicolon) {
        // Nothing needed to be done.
    }
    else if(node->type == ShallowASTNodeType_NumberConstant) {
        // Nothing needed to be done.
    }
    else if(node->type == ShallowASTNodeType_CreateConstVariable) {
        free(node->data.CreateConstVariable.name);
        if(node->data.CreateConstVariable.type == ExpressionType_Number) {
            // Nothing needed to be done.
        }
        else if(node->data.CreateConstVariable.type == ExpressionType_Multitoken) {
            shallow_ast_node_array_free(
                node->data.CreateConstVariable.value.multitoken
            );
            free(node->data.CreateConstVariable.value.multitoken);
        }
        else {
            PANIC("Not implemented");
        }
    }
    else if(node->type == ShallowASTNodeType_Addition) {
        if(node->data.Addition.left.type == ExpressionType_Identifier) {
            free(node->data.Addition.left.value.identifier);
        }
        else if(node->data.Addition.left.type == ExpressionType_Number) {

        }
        else {
            PANIC("Not implemented");
        }
        if(node->data.Addition.right.type == ExpressionType_Identifier) {
            free(node->data.Addition.right.value.identifier);
        }
        else if(node->data.Addition.right.type == ExpressionType_Number) {

        }
        else {
            PANIC("Not implemented");
        }
    }
    else if(node->type == ShallowASTNodeType_Subtraction) {
        expression_free(&node->data.Subtraction.left);
        expression_free(&node->data.Subtraction.right);
    }
    else {
        fprintf(stderr, "State: %d\n", node->type);
        PANIC("Not implemented");
    }
}

// Takes ownership of `node`
void shallow_ast_node_array_push(ShallowASTNodeArray* array, ShallowASTNode* node) {
    if(array->nodes == NULL) {
        array->nodes = malloc(sizeof(ShallowASTNode));
        array->count = 0;
    }
    else {
        array->nodes = realloc(array->nodes, (array->count+1) * sizeof(ShallowASTNode));
    }
    array->nodes[array->count] = *node;
    array->count += 1;
}

void shallow_ast_node_access_object_member_add_to_path(ShallowASTNode* node, char* path) {
    char*** names = &node->data.AccessObjectMember.path_data.names;
    size_t* count = &node->data.AccessObjectMember.path_data.count;
    if(*names == NULL) {
        *names = malloc(sizeof(char*));
        *count = 0;
    }
    else {
        *names = realloc(*names, (*count+1) * sizeof(char*));
    }
    (*names)[*count] = clone_string(path);
    *count += 1;
}

// Takes ownership
void shallow_ast_node_call_add_argument(ShallowASTNode* node, ShallowASTNode* argument) {
    ShallowASTNode** nodes = &node->data.Call.arguments.nodes;
    size_t* count = &node->data.Call.arguments.count;
    if(*nodes == NULL) {
        *nodes = malloc(sizeof(ShallowASTNode));
        *count = 0;
    }
    else {
        *nodes = realloc(*nodes, (*count+1) * sizeof(ShallowASTNode));
    }
    (*nodes)[*count] = *argument;
    *count += 1;
}

void shallow_ast_node_access_object_member_set_parent(ShallowASTNode* node, char* parent) {
    node->data.AccessObjectMember.object_name = clone_string(parent);
}


ShallowASTNode shallow_ast_node_create_access_object_member(LexerToken* parent) {
    assert(parent->type == LexerTokenType_Identifier);
    ShallowASTNode shallow_ast_node = { 0 };
    shallow_ast_node.type = ShallowASTNodeType_AccessObjectMember;
    shallow_ast_node_access_object_member_set_parent(
        &shallow_ast_node,
        parent->raw
    );
    return shallow_ast_node;
}

ShallowASTNode parser_shallow_get_access_object_member(LexerTokenArray* array, size_t index) {
    assert(array->tokens[index].type == LexerTokenType_Identifier);
    ShallowASTNode shallow_ast_node = shallow_ast_node_create_access_object_member(
        &array->tokens[index] 
    );
    //fprintf(stderr, "%s\n", shallow_ast_node.data.AccessObjectMember.object_name);
    fprintf(stderr, "%s\n", array->tokens[index].raw);
    //shallow_ast_node.data.AccessObjectMember.object_name = clone_string(array->tokens[index].raw);
    for(size_t i = index+1 ; i+1 < array->count ; i+=2) {
        if(array->tokens[i].type != LexerTokenType_Dot) {
            break;
        }
        else if(array->tokens[i+1].type != LexerTokenType_Identifier) {
            break;
        }
        else {
            shallow_ast_node_access_object_member_add_to_path(
                &shallow_ast_node,
                array->tokens[i+1].raw
            );
        }
    }
    return shallow_ast_node;
}

ShallowASTNode shallow_ast_node_create_call() {
    ShallowASTNode node = { 0 };
    node.type = ShallowASTNodeType_Call;
    return node;
}

ShallowASTNode shallow_ast_node_create_access_identifier(char* name) {
    ShallowASTNode node = { 0 };
    node.type = ShallowASTNodeType_AccessIdentifier;
    node.data.AccessIdentifier.name = clone_string(name);
    return node;
}

ShallowASTNode shallow_ast_node_conditional_check_create(ShallowASTNode* left, ShallowASTNode* right, enum ConditionalCheckType type) {
    ShallowASTNode node = { 0 };
    node.type = ShallowASTNodeType_ConditionalCheck;
    node.data.ConditionalCheck.type = type;
    node.data.ConditionalCheck.left = shallow_ast_node_deep_copy(left);
    node.data.ConditionalCheck.right = shallow_ast_node_deep_copy(right);
    return node;
}

ShallowASTNode parser_shallow_get_call(LexerTokenArray* array, size_t* index) {
    ShallowASTNode shallow_ast_node = shallow_ast_node_create_call();
    for(size_t i = *index+1 ; i < array->count ; i++) {
        if(array->tokens[i].type == LexerTokenType_String) {
            ShallowASTNode argument = shallow_ast_node_create_string_constant(
                array->tokens[i].raw
            );
            shallow_ast_node_call_add_argument(&shallow_ast_node, &argument);
        }
        else if(array->tokens[i].type == LexerTokenType_ParenEnd) {
            *index = i;
            break;
        }
        else if(array->tokens[i].type == LexerTokenType_Identifier) {
            // TODO: Fix potential buffer overflow. Thanks!
            if(array->tokens[i+1].type == LexerTokenType_DoubleEquals) {
                if(array->tokens[i+2].type == LexerTokenType_NumberConstant) {
                    ShallowASTNode left = shallow_ast_node_create_from_lexer_token(&array->tokens[i]);
                    ShallowASTNode right = shallow_ast_node_create_from_lexer_token(&array->tokens[i+2]);
                    ShallowASTNode argument = shallow_ast_node_conditional_check_create(
                            &left,
                            &right,
                            ConditionalCheckType_Equals
                    );
                    shallow_ast_node_call_add_argument(&shallow_ast_node, &argument);
                    shallow_ast_node_free(&left);
                    shallow_ast_node_free(&right);
                    i += 2;
                }
                else {
                    fprintf(stderr, "Conditional check is not implemented for this\n");
                    PANIC("");
                }
            }
            else {
                ShallowASTNode argument = shallow_ast_node_create_access_identifier(
                    array->tokens[i].raw
                );
                shallow_ast_node_call_add_argument(&shallow_ast_node, &argument);
            }
        }
        else {
            fprintf(stderr, "================\n");
            fprintf(stderr, 
                "Expected string or parenthesis\n"
                "\tGot: %d\n",
                array->tokens[i].type
            );
            PANIC("");
        }
    }
    return shallow_ast_node;
}

bool parser_shallow_is_access_object_member(LexerTokenArray* lexer_token_array, size_t index) {

    if(index+2 >= lexer_token_array->count) {
        return false;
    }

    size_t identifier_count = 0;
    for(size_t j = index+1 ; j+1 < lexer_token_array->count ; j+=2) {
        if(lexer_token_array->tokens[j].type != LexerTokenType_Dot) {
            break;
        }
        else if(lexer_token_array->tokens[j+1].type != LexerTokenType_Identifier) {
            break;
        }
        else {
            identifier_count += 1;
        }
    }

    if(identifier_count == 0) {
        return false;
    }
    else {
        return true;
    }

}

void shallow_ast_node_print(ShallowASTNode* node) {
    printf(
        "ShallowASTNode:\n"
        "\tType: %d\n",
        node->type
    );
}

void shallow_ast_node_array_print(ShallowASTNodeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
        shallow_ast_node_print(&array->nodes[i]);
    }
}

void shallow_ast_node_addition_set_left(ShallowASTNode* node, LexerToken* token) {
    assert(node->type == ShallowASTNodeType_Addition);
    if(token->type == LexerTokenType_Identifier) {
        node->data.Addition.left.type = ExpressionType_Identifier;
        node->data.Addition.left.value.identifier = clone_string(token->raw);
    }
    else if(token->type == LexerTokenType_NumberConstant) {
        node->data.Addition.left.type = ExpressionType_Number;
        node->data.Addition.left.value.number = strtod(token->raw, NULL);
    }
    else {
        fprintf(stderr, "TODO: Add message\n");
        PANIC("");
    }
}

void shallow_ast_node_addition_set_right(ShallowASTNode* node, LexerToken* token) {
    assert(node->type == ShallowASTNodeType_Addition);
    if(token->type == LexerTokenType_Identifier) {
        node->data.Addition.right.type = ExpressionType_Identifier;
        node->data.Addition.right.value.identifier = clone_string(token->raw);
    }
    else if(token->type == LexerTokenType_NumberConstant) {
        node->data.Addition.right.type = ExpressionType_Number;
        node->data.Addition.right.value.number = strtod(token->raw, NULL);
    }
    else {
        fprintf(stderr, "TODO: Add message\n");
        PANIC("");
    }
}

ShallowASTNode shallow_ast_node_create_addition(LexerToken* left, LexerToken* right) {
    ShallowASTNode node = { 0 };
    node.type = ShallowASTNodeType_Addition;
    shallow_ast_node_addition_set_left(&node, left);
    shallow_ast_node_addition_set_right(&node, right);
    return node;
}

ShallowASTNodeArray parse_shallow_parse(LexerTokenArray* lexer_token_array) {
    ShallowASTNodeArray array = { 0 };

    for(size_t i = 0 ; i < lexer_token_array->count ; i++) {
        if(lexer_token_array->tokens[i].type == LexerTokenType_Identifier) {
            if(parser_shallow_is_access_object_member(lexer_token_array, i)) {
                ShallowASTNode token = parser_shallow_get_access_object_member(
                    lexer_token_array,
                    i
                );
                shallow_ast_node_array_push(&array, &token);

                // Do dumb math to get the next index based on the the
                // `parser_shallow_get_Access_object_member`'s algorithm
                i += token.data.AccessObjectMember.path_data.count * 2;
            }
            else if(lexer_token_array->tokens[i+1].type == LexerTokenType_Plus) {
                ShallowASTNode token = shallow_ast_node_create_addition(
                    &lexer_token_array->tokens[i],
                    &lexer_token_array->tokens[i+2]
                );
                shallow_ast_node_array_push(&array, &token);
                i += 2;
            }
            else if(lexer_token_array->tokens[i+1].type == LexerTokenType_Minus) {
                ShallowASTNode token = shallow_ast_node_create_subtraction(
                    &lexer_token_array->tokens[i],
                    &lexer_token_array->tokens[i+2]
                );
                shallow_ast_node_array_push(&array, &token);
                i += 2;
            }
            else {
                fprintf(stderr,
                    "Error: Failed to identify identifier type\n"
                    "Token:\n"
                    "\tRaw: %s\n"
                    ,
                    lexer_token_array->tokens[i].raw
                );
                PANIC("");
            }
        }
        else if(lexer_token_array->tokens[i].type == LexerTokenType_ParenStart) {
            ShallowASTNode token = parser_shallow_get_call(lexer_token_array, &i);
            shallow_ast_node_array_push(&array, &token);
        }
        else if(lexer_token_array->tokens[i].type == LexerTokenType_Semicolon) {
            ShallowASTNode token = { 0 };
            token.type = ShallowASTNodeType_Semicolon;
            shallow_ast_node_array_push(&array, &token);
        }
        else if(lexer_token_array->tokens[i].type == LexerTokenType_Newline) {
            // TODO: Implement newlines
        }
        else if(lexer_token_array->tokens[i].type == LexerTokenType_KeywordConst) {
            assert(i+3 < lexer_token_array->count);
            assert(lexer_token_array->tokens[i+1].type == LexerTokenType_Identifier);
            assert(lexer_token_array->tokens[i+2].type == LexerTokenType_Equals);
            assert(
                lexer_token_array->tokens[i+3].type == LexerTokenType_NumberConstant ||
                lexer_token_array->tokens[i+3].type == LexerTokenType_Identifier
                // TODO: Add more valid states here
            );

            // Find out if the assignment is multiple tokens or a single token
            size_t token_count = 0;
            for(size_t j = i+3 ; j < lexer_token_array->count ; j++) {
                enum LexerTokenType type = lexer_token_array->tokens[j].type;
                if(type == LexerTokenType_Newline || type == LexerTokenType_Semicolon) {
                    break;
                }
                else {
                    token_count += 1;
                    //fprintf(stderr, "%ld\n", token_count);
                }
            }

            if(token_count == 1) {
                if(lexer_token_array->tokens[i+3].type == LexerTokenType_NumberConstant) {
                    ShallowASTNode node = shallow_ast_node_create_create_const_variable_number(
                        lexer_token_array->tokens[i+1].raw,
                        strtod(lexer_token_array->tokens[i+3].raw, NULL)
                    );
                    shallow_ast_node_array_push(&array, &node);
                    i += 3;
                }
                else {
                    fprintf(stderr,
                        "Error\n"
                        "\tDescription:\n"
                        "\t\tThe current type of assignemnt is not supported\n"
                        "\tType information:\n"
                        "\t\tType: (LexerTokenType) %d\n"
                        "\tWhat can I do to fix this?\n"
                        "\t\tNothing really. While this is functional code, it's not implemented\n"
                        "\t\tin this runtime yet.\n"
                        ,
                        lexer_token_array->tokens[i+3].type
                    );
                    PANIC("This assignment is not implemented");
                }
            }
            else {
                ShallowASTNode node = shallow_ast_node_create_create_const_variable_multitoken(
                        lexer_token_array->tokens[i+1].raw
                );

                LexerTokenArray tokens = { 0 };
                for(size_t j = i+3 ; j < lexer_token_array->count ; j++) {
                    enum LexerTokenType type = lexer_token_array->tokens[j].type;
                    if(type != LexerTokenType_Semicolon && type != LexerTokenType_Newline) {
                        LexerToken token = lexer_token_clone(&lexer_token_array->tokens[j]);
                        lexer_token_array_push(&tokens, &token);
                    }
                    else {
                        break;
                    }
                }
                node.data.CreateConstVariable.value.multitoken = calloc(1, sizeof(ShallowASTNodeArray));
                *node.data.CreateConstVariable.value.multitoken = parse_shallow_parse(&tokens);
                shallow_ast_node_array_push(&array, &node);
                i += 2 + tokens.count;
                printf("= '%s'\n", lexer_token_array->tokens[i].raw);
                lexer_token_array_free(&tokens);
            }

        }
        else {
            fprintf(stderr,
                "Token:\n"
                "\tType: %d\n"
                "\tRaw: %s\n",
                lexer_token_array->tokens[i].type,
                lexer_token_array->tokens[i].raw
            );
            PANIC("NOT IMPLEMENTED");
        }
    }

    return array;

}

void shallow_ast_create_const_variable_multitoken_push(ShallowASTNode* s_node, ShallowASTNode* l_token) {
    ShallowASTNode* copy = shallow_ast_node_deep_copy(l_token);
    shallow_ast_node_array_push(s_node->data.CreateConstVariable.value.multitoken, copy);
}

ShallowASTNode shallow_ast_node_create_create_const_variable_multitoken(char* name) {
    ShallowASTNode output = { 0 };
    output.type = ShallowASTNodeType_CreateConstVariable;
    output.data.CreateConstVariable.type = ExpressionType_Multitoken;
    output.data.CreateConstVariable.name = clone_string(name);
    return output;
}

void shallow_ast_node_array_free(ShallowASTNodeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
        shallow_ast_node_free(&array->nodes[i]);
    }
    free(array->nodes);
}

ShallowASTNode* shallow_ast_node_deep_copy(ShallowASTNode* node) {
    if(node->type == ShallowASTNodeType_AccessObjectMember) {
        ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_AccessObjectMember;

        char** names = malloc(
            node->data.AccessObjectMember.path_data.count * sizeof(char*)
        );

        for(size_t i = 0 ; i < node->data.AccessObjectMember.path_data.count ; i++) {
            char* name = clone_string(node->data.AccessObjectMember.path_data.names[i]);
            names[i] = name;
        }

        char* object_name = node->data.AccessObjectMember.object_name;
        fprintf(stderr, "%s\n", object_name);

        output->data.AccessObjectMember.path_data.names = names;
        output->data.AccessObjectMember.path_data.count = node->data.AccessObjectMember.path_data.count;
        output->data.AccessObjectMember.object_name = clone_string(node->data.AccessObjectMember.object_name);

        return output;

    }
    else if(node->type == ShallowASTNodeType_Call) {
        ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
        memset(output, 0, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_Call;
        for(size_t i = 0 ; i < node->data.Call.arguments.count ; i++) {
            // It just works
            ShallowASTNode* child = shallow_ast_node_deep_copy(&node->data.Call.arguments.nodes[i]);
            shallow_ast_node_call_add_argument(output, child);
            free(child);
            
        }
        return output;
    }
    else if(node->type == ShallowASTNodeType_StringConstant) {
        ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
        memset(output, 0, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_StringConstant;
        output->data.StringConstant.string = clone_string(node->data.StringConstant.string);
        return output;
    }
    else if(node->type == ShallowASTNodeType_CreateConstVariable) {
        ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
        memset(output, 0, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_CreateConstVariable;
        output->data.CreateConstVariable.name = clone_string(node->data.CreateConstVariable.name);
        if(node->data.CreateConstVariable.type == ExpressionType_Number) {
            output->data.CreateConstVariable.type = ExpressionType_Number;
            output->data.CreateConstVariable.value.number = node->data.CreateConstVariable.value.number;
        }
        else if(node->data.CreateConstVariable.type == ExpressionType_Multitoken) {
            output->data.CreateConstVariable.type = ExpressionType_Multitoken;
            output->data.CreateConstVariable.value.multitoken = calloc(1, sizeof(ShallowASTNodeArray));
            *output->data.CreateConstVariable.value.multitoken = shallow_ast_node_array_clone(
                node->data.CreateConstVariable.value.multitoken
            );
        }
        else {
            PANIC("CreateConstVariable: Type not implemented");
        }
        return output;
    }
    else if(node->type == ShallowASTNodeType_AccessIdentifier) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->data.AccessIdentifier.name = clone_string(node->data.AccessIdentifier.name);
        output->type = ShallowASTNodeType_AccessIdentifier;
        return output;
    }
    else if(node->type == ShallowASTNodeType_NumberConstant) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = node->type;
        output->data.NumberConstant.number = node->data.NumberConstant.number;
        return output;
    }
    else if(node->type == ShallowASTNodeType_ConditionalCheck) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_ConditionalCheck;
        fprintf(stderr,
            "%d\n%d\n",
            node->data.ConditionalCheck.left->type,
            node->data.ConditionalCheck.right->type
        );
        output->data.ConditionalCheck.left = shallow_ast_node_deep_copy(node->data.ConditionalCheck.left);
        output->data.ConditionalCheck.right = shallow_ast_node_deep_copy(node->data.ConditionalCheck.right);
        output->data.ConditionalCheck.type = node->data.ConditionalCheck.type;
        return output;
    }
    else if(node->type == ShallowASTNodeType_Addition) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_Addition;

        output->data.Addition.left.type = output->data.Addition.left.type;
        output->data.Addition.right.type = output->data.Addition.right.type;

        if(node->data.Addition.left.type == ExpressionType_Identifier) {
            output->data.Addition.left.type = ExpressionType_Identifier;
            output->data.Addition.left.value.identifier = clone_string(
                node->data.Addition.left.value.identifier
            );
        }
        else if(node->data.Addition.left.type == ExpressionType_Number) {
            output->data.Addition.left.type = ExpressionType_Number;
            output->data.Addition.left.value.number = node->data.Addition.left.value.number;
        }
        else {
            fprintf(stderr, "TODO: Add message\n");
            PANIC("");
        }

        if(node->data.Addition.right.type == ExpressionType_Identifier) {
            output->data.Addition.right.type = ExpressionType_Identifier;
            output->data.Addition.right.value.identifier = clone_string(
                node->data.Addition.right.value.identifier
            );
        }
        else if(node->data.Addition.right.type == ExpressionType_Number) {
            output->data.Addition.right.type = ExpressionType_Number;
            output->data.Addition.right.value.number = node->data.Addition.left.value.number;
        }
        else {
            fprintf(stderr, "TODO: Add message\n");
            PANIC("");
        }

        return output;
    }
    else if(node->type == ShallowASTNodeType_Semicolon) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_Semicolon;
        return output;
    }
    else if(node->type == ShallowASTNodeType_Subtraction) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_Subtraction;
        output->data.Subtraction.left = expression_clone(&node->data.Subtraction.left);
        output->data.Subtraction.right = expression_clone(&node->data.Subtraction.right);
        return output;
    }
    else {
        fprintf(stderr, "deep copy is not implemented for: %d\n", node->type);
        PANIC("");
    }
}

ShallowASTNode* shallow_ast_node_call_get_argument(ShallowASTNode* node, size_t index) {
    assert(node->type == ShallowASTNodeType_Call);
    assert(node->data.Call.arguments.count > index);
    return &node->data.Call.arguments.nodes[index];
}

size_t shallow_ast_node_call_get_argument_count(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_Call);
    return node->data.Call.arguments.count;
}


bool shallow_ast_node_access_object_member_compare_parent(ShallowASTNode* node, char* identifier) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    return strcmp(node->data.AccessObjectMember.object_name, identifier) == 0;
}

size_t shallow_ast_node_access_object_member_get_path_count(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    return node->data.AccessObjectMember.path_data.count;
}


char* shallow_ast_node_access_object_member_get_path_part(ShallowASTNode* node, size_t index) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    assert(node->data.AccessObjectMember.path_data.count > index);
    return node->data.AccessObjectMember.path_data.names[index];
}

char* shallow_ast_node_string_constant_get_string(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_StringConstant);
    return node->data.StringConstant.string;
}

char* shallow_ast_node_create_const_variable_get_name(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    assert(node->data.CreateConstVariable.name != NULL);
    return node->data.CreateConstVariable.name;
}

double shallow_ast_node_create_const_variable_number_get_value(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    assert(node->data.CreateConstVariable.type == ExpressionType_Number);
    return node->data.CreateConstVariable.value.number;
}

char* shallow_ast_node_access_identifier_get_name(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_AccessIdentifier);
    assert(node->data.AccessIdentifier.name != NULL);
    return node->data.AccessIdentifier.name;
}

double shallow_ast_node_number_constant_get_value(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_NumberConstant);
    return node->data.NumberConstant.number;
}

ShallowASTNodeArray shallow_ast_node_array_clone(ShallowASTNodeArray* array) {
    assert(array != NULL);
    ShallowASTNodeArray output = { 0 };
    output.count = array->count;
    for(size_t i = 0 ; i < array->count ; i++) {
        ShallowASTNode* clone = shallow_ast_node_deep_copy(&array->nodes[i]);
        shallow_ast_node_array_push(&output, clone);
        free(clone);
    }
    return output;
}


enum ExpressionType shallow_ast_node_create_const_variable_get_expression_type(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    return node->data.CreateConstVariable.type;
}

ShallowASTNodeArray* shallow_ast_node_create_const_variable_multitoken_get_expression(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    assert(node->data.CreateConstVariable.type == ExpressionType_Multitoken);
    return node->data.CreateConstVariable.value.multitoken;

}

ShallowASTNode shallow_ast_node_create_subtraction(LexerToken* left, LexerToken* right) {
    ShallowASTNode output = { 0 };
    output.type = ShallowASTNodeType_Subtraction;
    output.data.Subtraction.left = expression_create_from_lexer_token(left);
    output.data.Subtraction.right = expression_create_from_lexer_token(right);
    return output;
}

Expression expression_create_from_lexer_token(LexerToken* token) {
    if(token->type == LexerTokenType_NumberConstant) {
        Expression output = { 0 };
        output.type = ExpressionType_Number;
        output.value.number = strtod(token->raw, NULL);
        return output;
    }
    else if(token->type == LexerTokenType_Identifier) {
        Expression output = { 0 };
        output.type = ExpressionType_Identifier;
        output.value.identifier = clone_string(token->raw);
        return output;
    }
    else {
        fprintf(stderr, 
            "Failed to create an expression\n"
        );
        PANIC("");
    }
}

Expression expression_clone(Expression* expression) {
    if(expression->type == ExpressionType_Identifier) {
        Expression output = { 0 };
        output.type = ExpressionType_Identifier;
        output.value.identifier = clone_string(expression->value.identifier);
        return output;
    }
    else if(expression->type == ExpressionType_Number) {
        Expression output = { 0 };
        output.type = ExpressionType_Number;
        output.value.number = expression->value.number;
        return output;
    }
    else {
        fprintf(stderr,
            "TODO: Add message\n"
        );
        PANIC("");
    }
}

void expression_free(Expression* expression) {
    if(expression->type == ExpressionType_Identifier) {
        free(expression->value.identifier);
    }
    else if(expression->type == ExpressionType_Number) {
        // Nothing to do
    }
    else {
        fprintf(stderr,
            "TODO: Add message\n"
        );
        PANIC("");
    }
}
