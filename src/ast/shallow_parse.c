#include"shallow_parse.h"
#include"../util.h"
#include"../data_structures/string_array.h"

#include<stddef.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>

void shallow_ast_node_create_const_variable_set_name(ShallowASTNode* node, const char* name) {
    assert(node != NULL);
    assert(name != NULL);
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    node->data.CreateConstVariable.name = clone_string(name);
}

void shallow_ast_node_number_constant_set_value(ShallowASTNode* node, double number) {
    assert(node != NULL);
    assert(node->type == ShallowASTNodeType_NumberConstant);
    node->data.NumberConstant.number = number;
}

double expression_get_number(const Expression* expression) {
    assert(expression->type == ExpressionType_Number);
    return expression->value.number;
}

const Expression* shallow_ast_node_create_const_variable_get_expression(const ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    return &node->data.CreateConstVariable.expression; 
}

void shallow_ast_node_create_const_variable_set_expression(ShallowASTNode* node, const Expression* expression) {
    assert(node != NULL);
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    assert(expression != NULL);
    node->data.CreateConstVariable.expression = expression_clone(expression);
}

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
    output.data.CreateConstVariable.expression.type = ExpressionType_Number;
    output.data.CreateConstVariable.expression.value.number = value;
    return output;
}

void shallow_ast_node_free(ShallowASTNode* node) {
    if(node == NULL) { return; } 
    if(node->type == ShallowASTNodeType_AccessObjectMember) {
        assert(node->data.AccessObjectMember.object_name != NULL);
        string_array_free(&node->data.AccessObjectMember.path_data);
        free(node->data.AccessObjectMember.object_name);
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
    else if(node->type == ShallowASTNodeType_DoubleEquals) {
        // Nothing needed to be done.
    }
    else if(node->type == ShallowASTNodeType_Comma) {
        // Nothing needed to be done.
    }
    else if(node->type == ShallowASTNodeType_Plus) {
        // Nothing needed to be done.
    }
    else if(node->type == ShallowASTNodeType_Minus) {
        // Nothing needed to be done.
    }
    else if(node->type == ShallowASTNodeType_NumberConstant) {
        // Nothing needed to be done.
    }
    else if(node->type == ShallowASTNodeType_CreateConstVariable) {
        free(node->data.CreateConstVariable.name);
        expression_free(&node->data.CreateConstVariable.expression);
    }
    else if(node->type == ShallowASTNodeType_Addition) {
        expression_free(&node->data.Addition.left);
        expression_free(&node->data.Addition.right);
    }
    else if(node->type == ShallowASTNodeType_Subtraction) {
        expression_free(&node->data.Subtraction.left);
        expression_free(&node->data.Subtraction.right);
    }
    else if(node->type == ShallowASTNodeType_CreateObjectProperty) {
        free(node->data.CreateObjectProperty.identifier);
    }
    else if(node->type == ShallowASTNodeType_CreateObject) {
        shallow_ast_node_array_free(
            node->data.CreateObject.tokens
        );
        free(node->data.CreateObject.tokens);
        //free(node->data.CreateObjectProperty.identifier);
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

void shallow_ast_node_access_object_member_add_to_path(ShallowASTNode* node, const char* path) {
    assert(node != NULL);
    assert(path != NULL);
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    string_array_push(
        &node->data.AccessObjectMember.path_data,
        path
    );
}

// Takes ownership
void shallow_ast_node_call_add_argument(ShallowASTNode* node, const ShallowASTNode* argument) {
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

void shallow_ast_node_access_object_member_set_parent(ShallowASTNode* node, const char* parent) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
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

void shallow_ast_node_access_identifier_set_name(ShallowASTNode* node, const char* name) {
    assert(node != NULL);
    assert(name != NULL);
    assert(node->type == ShallowASTNodeType_AccessIdentifier);
    node->data.AccessIdentifier.name = clone_string(name);
}

ShallowASTNode parser_shallow_get_call(LexerTokenArray* array, size_t* index) {
    ShallowASTNode output = { 0 };
    shallow_ast_node_set_type(&output, ShallowASTNodeType_Call);

    for(size_t i = *index+1 ; i < array->count ; i++) {
        *index = i;
        if(array->tokens[i].type == LexerTokenType_ParenEnd) {
            break;
        }
        else if(array->tokens[i].type == LexerTokenType_String) {
            ShallowASTNode node = { 0 };
            shallow_ast_node_set_type(&node, ShallowASTNodeType_StringConstant);
            shallow_ast_node_string_constant_set(&node, array->tokens[i].raw);
            shallow_ast_node_call_add_argument(&output, &node);
        }
        else if(array->tokens[i].type == LexerTokenType_Identifier) {
            if(array->tokens[i+1].type == LexerTokenType_Dot)  {
                ShallowASTNode object_access = shallow_ast_node_get_access_object_member(
                    array,
                    &i
                );
                shallow_ast_node_call_add_argument(
                    &output,
                    &object_access
                );
                //shallow_ast_node_free(&object_access);
            }
            else {
                ShallowASTNode identifier = { 0 };
                shallow_ast_node_set_type(&identifier, ShallowASTNodeType_AccessIdentifier);
                shallow_ast_node_access_identifier_set_name(&identifier, array->tokens[i].raw);
                shallow_ast_node_call_add_argument(
                    &output,
                    &identifier
                );
            }
        }
        else if(array->tokens[i].type == LexerTokenType_Comma) {
            // TODO: Add proper checking for this
        }
        else if(array->tokens[i].type == LexerTokenType_DoubleEquals) {
            ShallowASTNode node = { 0 };
            shallow_ast_node_set_type(&node, ShallowASTNodeType_DoubleEquals);
            shallow_ast_node_call_add_argument(
                &output,
                &node
            );
        }
        else if(array->tokens[i].type == LexerTokenType_NumberConstant) {
            ShallowASTNode node = { 0 };
            shallow_ast_node_set_type(&node, ShallowASTNodeType_NumberConstant);
            shallow_ast_node_number_constant_set_value(
                &node, 
                strtod(array->tokens[i].raw, NULL)
            );
            shallow_ast_node_call_add_argument(
                &output,
                &node
            );
        }
        else {
            fprintf(stderr, 
                "Unknown argument type\n"
                "\tType: %d\n"
                ,
                array->tokens[i].type
            );
            PANIC();
        }
    }

    return output;
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

ShallowASTNode shallow_ast_node_create_addition(LexerToken* left, LexerToken* right) {
    ShallowASTNode node = { 0 };
    node.type = ShallowASTNodeType_Addition;
    node.data.Addition.left = expression_create_from_lexer_token(left);
    node.data.Addition.right = expression_create_from_lexer_token(right);
    return node;
}

ShallowASTNode shallow_ast_node_create_create_object_property(LexerToken* identifier, LexerToken* value) {
    assert(identifier->type == LexerTokenType_Identifier);
    assert(value->type == LexerTokenType_NumberConstant);
    ShallowASTNode node = { 0 };
    node.type = ShallowASTNodeType_CreateObjectProperty;
    node.data.CreateObjectProperty.identifier = clone_string(identifier->raw);
    node.data.CreateObjectProperty.value.type = ValueType_Number;
    node.data.CreateObjectProperty.value.value.number = strtod(value->raw, NULL);
    return node;
}

ShallowASTNode shallow_ast_node_create_create_object(ShallowASTNodeArray* tokens) {
    ShallowASTNode token = { 0 };
    token.type = ShallowASTNodeType_CreateObject;
    token.data.CreateObject.tokens = calloc(1, sizeof(ShallowASTNodeArray));
    *token.data.CreateObject.tokens = shallow_ast_node_array_clone(tokens);
    return token;
}

ShallowASTNode shallow_ast_node_create_addition_from_expressions(Expression left, Expression right) {
    ShallowASTNode output = { 0 };
    output.type = ShallowASTNodeType_Addition;
    output.data.Addition.left = left;
    output.data.Addition.right = right;
    return output;
}

Expression expression_create_property_access(ShallowASTNode* node) {
    Expression output = { 0 };
    output.type = ExpressionType_PropertyAccess;
    output.value.property_access = shallow_ast_node_deep_copy(node);
    return output;
}

Expression expression_create_from_shallow_ast_node(const ShallowASTNode* node) {
    assert(node != NULL);
    if(node->type == ShallowASTNodeType_AccessIdentifier) {
        Expression output = { 0 };
        output.type = ExpressionType_Identifier;
        output.value.identifier = clone_string(node->data.AccessIdentifier.name);
        return output;
    }
    else if(node->type == ShallowASTNodeType_AccessObjectMember) {
        Expression output = { 0 };
        output.type = ExpressionType_PropertyAccess;
        output.value.property_access = shallow_ast_node_deep_copy(node);
        return output;
    }
    else {
        fprintf(stderr, "Todo: Add message\n");
        PANIC("");
    }
}

ShallowASTNode shallow_ast_node_create_subtraction_from_expressions(const Expression* left, const Expression* right) {
    ShallowASTNode output = { 0 };
    output.type = ShallowASTNodeType_Subtraction;
    output.data.Addition.left = expression_clone(left);
    output.data.Addition.right = expression_clone(right);
    return output;
}

ShallowASTNode shallow_ast_node_create_empty_create_object_prperty() {
    ShallowASTNode output = { 0 };
    output.type = ShallowASTNodeType_CreateObjectProperty;
    return output;
}

void shallow_ast_node_create_object_property_set_identifier(ShallowASTNode* node, const char* identifier) {
    assert(node != NULL);
    assert(node->type == ShallowASTNodeType_CreateObjectProperty);
    assert(node->data.CreateObjectProperty.identifier == NULL);
    node->data.CreateObjectProperty.identifier = clone_string(identifier);
}

void shallow_ast_node_create_object_property_set_value(ShallowASTNode* node, const Value* value) {
    assert(node != NULL);
    assert(node->type == ShallowASTNodeType_CreateObjectProperty);
    node->data.CreateObjectProperty.value = value_clone(value);
}

void shallow_ast_node_set_type(ShallowASTNode* node, enum ShallowASTNodeType type) {
    assert(node != NULL) ;
    assert(type != ShallowASTNodeType_InvalidToken);
    node->type = type;
}

ShallowASTNode shallow_ast_node_get_access_object_member(const LexerTokenArray* array, size_t* index) {

    assert(array != NULL);
    assert(index != NULL);

    ShallowASTNode output = { 0 };
    shallow_ast_node_set_type(&output, ShallowASTNodeType_AccessObjectMember);
    shallow_ast_node_access_object_member_set_parent(
        &output,
        array->tokens[*index].raw
    );

    for(size_t i = *index+1; i < array->count ; i+=2) {
        if(i+1 >= array->count) {
            break;
        }
        else if(array->tokens[i].type == LexerTokenType_Dot) {
            if(array->tokens[i+1].type == LexerTokenType_Identifier) {
                *index = i+2;
                shallow_ast_node_access_object_member_add_to_path(
                    &output, 
                    array->tokens[i+1].raw
                );
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }

    *index -= 1;
    assert(output.data.AccessObjectMember.path_data.count > 0);

    return output;

}

void shallow_ast_node_string_constant_set(ShallowASTNode* node, const char* string) {
    assert(node != NULL);
    assert(string != NULL);
    assert(node->type == ShallowASTNodeType_StringConstant);
    node->data.StringConstant.string = clone_string(string);
}

Expression expression_collect_multitoken(const LexerTokenArray* array, size_t* start_index) {

    assert(array != NULL);
    assert(start_index != NULL);
    assert(array->count > *start_index);
    assert(array->tokens != NULL);


    // Stores the amount of tokens that is in the current expression
    size_t token_count = 0;
    for(size_t i = *start_index ; i < array->count ; i++) {
        if(array->tokens[i].type == LexerTokenType_Semicolon) {
            break;
        }
        else {
            token_count += 1;
        }
    }

    // Clone the tokens, so they can be parsed.
    LexerTokenArray expression_tokens = { 0 };
    for(size_t i = *start_index ; i < *start_index + token_count ; i++) {
        LexerToken token = lexer_token_clone(&array->tokens[i]);
        lexer_token_array_push(&expression_tokens, &token);
    }

    Expression output = { 0 };
    output.type = ExpressionType_Multitoken;
    output.value.multitoken = calloc(1, sizeof(ShallowASTNodeArray));

    ShallowASTNodeArray* tokens = output.value.multitoken;

    // Parse the tokens
    for(size_t i = 0 ; i < expression_tokens.count ; i++) {
        if(expression_tokens.tokens[i].type == LexerTokenType_Identifier) {
            if(i + 1 < expression_tokens.count) {
                if(parser_shallow_is_access_object_member(&expression_tokens, i)) {
                    ShallowASTNode node = shallow_ast_node_get_access_object_member(
                        &expression_tokens,
                        &i
                    );
                    shallow_ast_node_array_push(tokens, &node);
                }
                else {
                    ShallowASTNode node = { 0 };
                    shallow_ast_node_set_type(&node, ShallowASTNodeType_AccessIdentifier);
                    shallow_ast_node_access_identifier_set_name(&node, expression_tokens.tokens[i].raw);
                    shallow_ast_node_array_push(tokens, &node);
                }
            }
            else {
                ShallowASTNode node = { 0 };
                shallow_ast_node_set_type(&node, ShallowASTNodeType_AccessIdentifier);
                shallow_ast_node_access_identifier_set_name(&node, expression_tokens.tokens[i].raw);
                shallow_ast_node_array_push(tokens, &node);
            }
        }
        else if(expression_tokens.tokens[i].type == LexerTokenType_Plus) {
            ShallowASTNode node = { 0 };
            shallow_ast_node_set_type(&node, ShallowASTNodeType_Plus);
            shallow_ast_node_array_push(tokens, &node);
        }
        else if(expression_tokens.tokens[i].type == LexerTokenType_Minus) {
            ShallowASTNode node = { 0 };
            shallow_ast_node_set_type(&node, ShallowASTNodeType_Minus);
            shallow_ast_node_array_push(tokens, &node);
        }
        else if(expression_tokens.tokens[i].type == LexerTokenType_DoubleEquals) {
            ShallowASTNode node = { 0 };
            shallow_ast_node_set_type(&node, ShallowASTNodeType_DoubleEquals);
            shallow_ast_node_array_push(tokens, &node);
        }
        else {
            fprintf(stderr, 
                "Recieved unhandled token type.\n"
                "Got: %d\n"
                "Index: %ld\n"
                ,
                expression_tokens.tokens[i].type,
                i
            );
            PANIC();
        }
    }

    *start_index += expression_tokens.count;
    lexer_token_array_free(&expression_tokens);

    return output;

}

ShallowASTNode shallow_ast_node_create_create_const_variable_object(char* name, ShallowASTNodeArray* tokens) {
    ShallowASTNode output = { 0 };
    shallow_ast_node_set_type(&output, ShallowASTNodeType_CreateConstVariable);
    output.data.CreateConstVariable.expression.type = ExpressionType_Object;
    output.data.CreateConstVariable.name = clone_string(name);
    output.data.CreateConstVariable.expression.value.object = calloc(1, sizeof(ShallowASTNode));
    *output.data.CreateConstVariable.expression.value.object = shallow_ast_node_create_create_object(tokens);
    return output;
}

ShallowASTNodeArray parse_shallow_parse(LexerTokenArray* lexer_token_array) {
    ShallowASTNodeArray array = { 0 };

    for(size_t i = 0 ; i < lexer_token_array->count ; i++) {
        if(lexer_token_array->tokens[i].type == LexerTokenType_Identifier) {

            ShallowASTNode identifier = { 0 };
            if(parser_shallow_is_access_object_member(lexer_token_array, i)) {
                identifier = shallow_ast_node_get_access_object_member(
                    lexer_token_array,
                    &i
                );
                //i += identifier.data.AccessObjectMember.path_data.count * 2;
            }
            else {
                identifier = shallow_ast_node_create_access_identifier(
                    lexer_token_array->tokens[i].raw
                );
                i += 1;
            }

            if(i < lexer_token_array->count) {
                if(lexer_token_array->tokens[i].type == LexerTokenType_Plus) {
                    assert(i+1 < lexer_token_array->count);
                    assert(
                        lexer_token_array->tokens[i+1].type == LexerTokenType_NumberConstant ||
                        lexer_token_array->tokens[i+1].type == LexerTokenType_Identifier
                    );
                    assert(parser_shallow_is_access_object_member(lexer_token_array, i) == false);
                    Expression left = expression_create_from_shallow_ast_node(&identifier);
                    Expression right = expression_create_from_lexer_token(&lexer_token_array->tokens[i+1]);
                    ShallowASTNode node = shallow_ast_node_create_addition_from_expressions(
                        left,
                        right
                    );
                    shallow_ast_node_array_push(&array, &node);
                    i += 1;
                    shallow_ast_node_free(&identifier);
                }
                else if(lexer_token_array->tokens[i].type == LexerTokenType_Minus) {
                    assert(i+1 < lexer_token_array->count);
                    assert(
                        lexer_token_array->tokens[i+1].type == LexerTokenType_NumberConstant ||
                        lexer_token_array->tokens[i+1].type == LexerTokenType_Identifier
                    );
                    Expression left = expression_create_from_shallow_ast_node(&identifier);
                    Expression right = expression_create_from_lexer_token(&lexer_token_array->tokens[i+1]);
                    ShallowASTNode node = shallow_ast_node_create_subtraction_from_expressions(
                        &left,
                        &right
                    );
                    expression_free(&left);
                    expression_free(&right);
                    shallow_ast_node_array_push(&array, &node);
                    i += 1;
                    shallow_ast_node_free(&identifier);
                }
                else if(lexer_token_array->tokens[i].type == LexerTokenType_Colon) {
                    assert(identifier.type == ShallowASTNodeType_AccessIdentifier);
                    assert(i+1 < lexer_token_array->count);
                    assert(lexer_token_array->tokens[i+1].type == LexerTokenType_NumberConstant);
                    ShallowASTNode node = shallow_ast_node_create_empty_create_object_prperty(); 
                    const Value value = value_create_from_double(
                        strtod(lexer_token_array->tokens[i+1].raw, NULL)
                    );
                    shallow_ast_node_create_object_property_set_identifier(
                        &node, 
                        shallow_ast_node_access_identifier_get_name(&identifier)
                    );
                    shallow_ast_node_create_object_property_set_value(
                        &node,
                        &value
                    );
                    shallow_ast_node_array_push(&array, &node);
                    shallow_ast_node_free(&identifier);

                    i += 1;
                }
                else {
                    shallow_ast_node_array_push(&array, &identifier);
                }
            }
            else {
                shallow_ast_node_array_push(&array, &identifier);
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
        else if(lexer_token_array->tokens[i].type == LexerTokenType_Comma) {
            ShallowASTNode token = { 0 };
            token.type = ShallowASTNodeType_Comma;
            shallow_ast_node_array_push(&array, &token);
        }
        else if(lexer_token_array->tokens[i].type == LexerTokenType_KeywordConst) {
            assert(i+3 < lexer_token_array->count);
            assert(lexer_token_array->tokens[i+1].type == LexerTokenType_Identifier);
            assert(lexer_token_array->tokens[i+2].type == LexerTokenType_Equals);
            assert(
                lexer_token_array->tokens[i+3].type == LexerTokenType_NumberConstant ||
                lexer_token_array->tokens[i+3].type == LexerTokenType_Identifier ||
                lexer_token_array->tokens[i+3].type == LexerTokenType_CurlyBracketStart
                // TODO: Add more valid states here
            );

            // Find out if the assignment is multiple tokens or a single token
            if(lexer_token_array->tokens[i+3].type != LexerTokenType_CurlyBracketStart) {


                size_t token_count = 0;
                for(size_t j = i+3 ; j < lexer_token_array->count ; j++) {
                    enum LexerTokenType type = lexer_token_array->tokens[j].type;
                    if(type == LexerTokenType_Newline || type == LexerTokenType_Semicolon) {
                        break;
                    }
                    else {
                        token_count += 1;
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
                else if(lexer_token_array->tokens[i+4].type == LexerTokenType_Dot) {
                    ShallowASTNode node = { 0 };
                    shallow_ast_node_set_type(&node, ShallowASTNodeType_CreateConstVariable);
                    shallow_ast_node_create_const_variable_set_name(
                        &node,
                        lexer_token_array->tokens[i+1].raw
                    );
                    i += 3;
                    Expression expression = expression_collect_multitoken(lexer_token_array, &i);
                    shallow_ast_node_create_const_variable_set_expression(&node, &expression);
                    shallow_ast_node_array_push(&array, &node);
                    expression_free(&expression);
                }
                else {
                    ShallowASTNode node = { 0 };
                    shallow_ast_node_set_type(&node, ShallowASTNodeType_CreateConstVariable);
                    shallow_ast_node_create_const_variable_set_name(
                        &node, 
                        lexer_token_array->tokens[i+1].raw
                    );
                    i += 3;
                    node.data.CreateConstVariable.expression = expression_collect_multitoken(
                        lexer_token_array, 
                        &i
                    );
                    shallow_ast_node_array_push(&array, &node);
                }
            }
            else if(lexer_token_array->tokens[i+3].type == LexerTokenType_CurlyBracketStart) {
                LexerTokenArray tokens = { 0 };
                for(size_t j = i+4 ; j < lexer_token_array->count ; j++) {
                    if(lexer_token_array->tokens[j].type == LexerTokenType_CurlyBracketEnd) {
                        break;
                    }
                    else {
                        LexerToken token = lexer_token_clone(&lexer_token_array->tokens[j]);
                        lexer_token_array_push(&tokens, &token);
                    }
                }
                fprintf(stderr, "%ld\n", i);
                ShallowASTNodeArray shallow_tokens = parse_shallow_parse(&tokens);
                ShallowASTNode node = shallow_ast_node_create_create_const_variable_object(
                    lexer_token_array->tokens[i+1].raw,
                    &shallow_tokens
                );
                i += 4 + tokens.count;
                shallow_ast_node_array_push(&array, &node);
                lexer_token_array_free(&tokens);
                shallow_ast_node_array_free(&shallow_tokens);
            }
            else {
                fprintf(stderr, "Err\n");
                PANIC("");
            }
        }
        else if(lexer_token_array->tokens[i].type == LexerTokenType_Minus) {
            ShallowASTNode node = { 0 };
            shallow_ast_node_set_type(&node, ShallowASTNodeType_Minus);
            shallow_ast_node_array_push(&array, &node);
        }
        else {
            fprintf(stderr,
                "Token:\n"
                "\tType: %d\n"
                "\tRaw: %s\n"
                "Parser:\n"
                "\tIndex: %ld\n"
                ,
                lexer_token_array->tokens[i].type,
                lexer_token_array->tokens[i].raw,
                i
            );
            PANIC("NOT IMPLEMENTED");
        }
    }

    return array;

}

void shallow_ast_node_array_free(ShallowASTNodeArray* array) {
    for(size_t i = 0 ; i < array->count ; i++) {
        shallow_ast_node_free(&array->nodes[i]);
    }
    free(array->nodes);
}

ShallowASTNode* shallow_ast_node_deep_copy(const ShallowASTNode* node) {
    if(node->type == ShallowASTNodeType_AccessObjectMember) {
        ShallowASTNode* output = malloc(sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_AccessObjectMember;

        output->data.AccessObjectMember.path_data = string_array_clone(
            &node->data.AccessObjectMember.path_data
        );
        output->data.AccessObjectMember.object_name = clone_string(
            node->data.AccessObjectMember.object_name
        );

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
        output->data.CreateConstVariable.expression = expression_clone(
            &node->data.CreateConstVariable.expression
        );
        return output;
    }
    else if(node->type == ShallowASTNodeType_CreateObject) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_CreateObject;
        output->data.CreateObject.tokens = calloc(1, sizeof(ShallowASTNodeArray));
        *output->data.CreateObject.tokens = shallow_ast_node_array_clone(node->data.CreateObject.tokens);
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
    else if(node->type == ShallowASTNodeType_CreateObjectProperty) {
        // TODO: Add support for more types than just numbers
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_CreateObjectProperty;
        output->data.CreateObjectProperty.identifier = clone_string(node->data.CreateObjectProperty.identifier);
        output->data.CreateObjectProperty.value.type = ValueType_Number;
        output->data.CreateObjectProperty.value.value.number = node->data.CreateObjectProperty.value.value.number;
        return output;
    }
    else if(node->type == ShallowASTNodeType_Comma) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        output->type = ShallowASTNodeType_Comma;
        return output;
    }
    else if(node->type == ShallowASTNodeType_Plus) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        shallow_ast_node_set_type(output, ShallowASTNodeType_Plus);
        return output;
    }
    else if(node->type == ShallowASTNodeType_Minus) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        shallow_ast_node_set_type(output, ShallowASTNodeType_Minus);
        return output;
    }
    else if(node->type == ShallowASTNodeType_Comma) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        shallow_ast_node_set_type(output, ShallowASTNodeType_Comma);
        return output;
    }
    else if(node->type == ShallowASTNodeType_DoubleEquals) {
        ShallowASTNode* output = calloc(1, sizeof(ShallowASTNode));
        shallow_ast_node_set_type(output, ShallowASTNodeType_DoubleEquals);
        return output;
    }
    else {
        fprintf(stderr, "deep copy is not implemented for: (ShallowASTNodeType) %d\n", node->type);
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

size_t shallow_ast_node_access_object_member_get_path_count(const ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    return node->data.AccessObjectMember.path_data.count;
}


const char* shallow_ast_node_access_object_member_get_path_part(const ShallowASTNode* node, size_t index) {
    assert(node->type == ShallowASTNodeType_AccessObjectMember);
    assert(node->data.AccessObjectMember.path_data.count > index);
    return node->data.AccessObjectMember.path_data.strings[index];
}

char* shallow_ast_node_string_constant_get_string(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_StringConstant);
    return node->data.StringConstant.string;
}

const char* shallow_ast_node_create_const_variable_get_name(const ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    assert(node->data.CreateConstVariable.name != NULL);
    return node->data.CreateConstVariable.name;
}

char* shallow_ast_node_access_identifier_get_name(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_AccessIdentifier);
    assert(node->data.AccessIdentifier.name != NULL);
    return node->data.AccessIdentifier.name;
}

double shallow_ast_node_number_constant_get_value(const ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_NumberConstant);
    return node->data.NumberConstant.number;
}

ShallowASTNodeArray shallow_ast_node_array_clone(const ShallowASTNodeArray* array) {
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

Expression shallow_ast_node_create_const_variable_multitoken_get_expression(ShallowASTNode* node) {
    assert(node->type == ShallowASTNodeType_CreateConstVariable);
    //assert(node->data.CreateConstVariable.type == ExpressionType_Multitoken);
    return node->data.CreateConstVariable.expression;
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

Expression expression_clone(const Expression* expression) {
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
    else if(expression->type == ExpressionType_Object) {
        Expression output = { 0 };
        output.type = ExpressionType_Object;
        output.value.object = shallow_ast_node_deep_copy(expression->value.object);
        return output;
    }
    else if(expression->type == ExpressionType_Multitoken) {
        Expression output = { 0 };
        output.type = ExpressionType_Multitoken;
        output.value.multitoken = calloc(1, sizeof(ShallowASTNodeArray));
        *output.value.multitoken = shallow_ast_node_array_clone(expression->value.multitoken);
        return output;
    }
    else {
        fprintf(stderr,
            "Expression couldn't be cloned.\n"
            "Type: %d\n"
            ,
            expression->type
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
    else if(expression->type == ExpressionType_Multitoken) {
        shallow_ast_node_array_free(expression->value.multitoken);
        free(expression->value.multitoken);
    }
    else if(expression->type == ExpressionType_Object) {
        shallow_ast_node_free(expression->value.object);
        free(expression->value.object);
    }
    else {
        fprintf(stderr,
            "Failed to free Expression\n"
            "Type: %d\n"
            ,
            expression->type
        );
        PANIC();
    }
}
