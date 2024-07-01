#ifndef shallow_parse_h
#define shallow_parse_h

#include"../lexer/lexer.h"

enum ShallowASTNodeType {
    ShallowASTNodeType_AccessObjectMember,
    ShallowASTNodeType_AccessIdentifier,
    ShallowASTNodeType_Call,
    ShallowASTNodeType_StringConstant,
    ShallowASTNodeType_Semicolon,
    ShallowASTNodeType_CreateConstVariable,
    ShallowASTNodeType_ConditionalCheck,
    ShallowASTNodeType_NumberConstant,
    ShallowASTNodeType_Addition,
};

enum ConditionalCheckType {
    ConditionalCheckType_Equals
};

enum ExpressionType {
    ExpressionType_Number,
    ExpressionType_Identifier,
    ExpressionType_Multitoken,
};

struct ShallowASTNodeArray;

// These are "shallow" ast nodes. This means they are more simple than actual
// ast nodes, but this is for a reason. First we want to create a general
// overview of the code and then try to make sense of it.
typedef struct ShallowASTNode {
    enum ShallowASTNodeType type;
    union {
    
        // object_name - The parent object that you are trying to get the data
        //               from.
        //
        // path_data   - The path that you are trying to access. 
        //
        // Example:
        //   {object_name}.{path_data[0]}.{path_data[1]}
        struct {
            char* object_name;
            struct {
                char** names;
                size_t count;
            } path_data;
        } AccessObjectMember;

        struct {
            struct {
                struct ShallowASTNode* nodes;
                size_t count;
            } arguments;
        } Call;

        struct {
            char* string;
        } StringConstant;

        struct {
            char* name;
            enum ExpressionType type;
            union {
                double number;
                struct ShallowASTNodeArray* multitoken;
            } value;
        } CreateConstVariable;

        struct {
            char* name;
        } AccessIdentifier;

        struct {
            struct ShallowASTNode* left;
            struct ShallowASTNode* right;
            enum ConditionalCheckType type;
        } ConditionalCheck;

        struct {
            double number;
        } NumberConstant;

        struct {

            struct {
                enum ExpressionType type;
                union {
                    char* identifier;
                    double number;
                } value;
            } left;

            struct {
                enum ExpressionType type;
                union {
                    char* identifier;
                    double number;
                } value;
            } right;

        } Addition;

    } data;
} ShallowASTNode;

typedef struct ShallowASTNodeArray {
    ShallowASTNode* nodes;
    size_t count;
} ShallowASTNodeArray;

ShallowASTNode shallow_ast_node_create_string_constant(char* string);
void shallow_ast_node_free(ShallowASTNode* node);
void shallow_ast_node_array_push(ShallowASTNodeArray* array, ShallowASTNode* node);
void shallow_ast_node_access_object_member_add_to_path(ShallowASTNode* node, char* path);
void shallow_ast_node_call_add_argument(ShallowASTNode* node, ShallowASTNode* argument);
void shallow_ast_node_access_object_member_set_parent(ShallowASTNode* node, char* parent);
ShallowASTNode shallow_ast_node_create_access_object_member(LexerToken* parent);
ShallowASTNode parser_shallow_get_access_object_member(LexerTokenArray* array, size_t index);
ShallowASTNode shallow_ast_node_create_call();
ShallowASTNode parser_shallow_get_call(LexerTokenArray* array, size_t* index);
bool parser_shallow_is_access_object_member(LexerTokenArray* lexer_token_array, size_t index);
void shallow_ast_node_print(ShallowASTNode* node);
void shallow_ast_node_array_print(ShallowASTNodeArray* array);
ShallowASTNodeArray parse_shallow_parse(LexerTokenArray* lexer_token_array);
void shallow_ast_node_array_free(ShallowASTNodeArray* array);
ShallowASTNode* shallow_ast_node_deep_copy(ShallowASTNode*);

ShallowASTNode* shallow_ast_node_call_get_argument(ShallowASTNode*, size_t index);
size_t shallow_ast_node_call_get_argument_count(ShallowASTNode*);

bool shallow_ast_node_access_object_member_compare_parent(ShallowASTNode*, char* identifier);
size_t shallow_ast_node_access_object_member_get_path_count(ShallowASTNode*);
char* shallow_ast_node_access_object_member_get_path_part(ShallowASTNode*, size_t index);

char* shallow_ast_node_string_constant_get_string(ShallowASTNode*);

char* shallow_ast_node_create_const_variable_get_name(ShallowASTNode*);

ShallowASTNode shallow_ast_node_create_create_const_variable_number(char* name, double value);
ShallowASTNode shallow_ast_node_create_create_const_variable_multitoken(char* name);
double shallow_ast_node_create_const_variable_number_get_value(ShallowASTNode*);
void shallow_ast_create_const_variable_multitoken_push(ShallowASTNode*, ShallowASTNode*);
enum ExpressionType shallow_ast_node_create_const_variable_get_expression_type(ShallowASTNode*);
ShallowASTNodeArray* shallow_ast_node_create_const_variable_multitoken_get_expression(ShallowASTNode*);

ShallowASTNode* shallow_ast_node_create_empty();

ShallowASTNode shallow_ast_node_create_access_identifier(char* name);
char* shallow_ast_node_access_identifier_get_name(ShallowASTNode*);

ShallowASTNode shallow_ast_node_conditional_check_create(ShallowASTNode* left, ShallowASTNode* right, enum ConditionalCheckType type);

double shallow_ast_node_number_constant_get_value(ShallowASTNode*);

ShallowASTNode shallow_ast_node_create_addition(LexerToken* left, LexerToken* right);

ShallowASTNodeArray shallow_ast_node_array_clone(ShallowASTNodeArray*);

#endif
