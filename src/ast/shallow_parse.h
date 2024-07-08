#ifndef shallow_parse_h
#define shallow_parse_h

#include"../lexer/lexer.h"
#include"../runtime/value.h"
#include"../data_structures/string_array.h"


// TODO: Add a Expression array for arguments.
// TODO: Add a function to evaluate expressions.


// ShallowASTNodeType_InvalidToken is not supposed to be used as a real token
// type. It represents an unset node type, which is an error.
enum ShallowASTNodeType {
    ShallowASTNodeType_InvalidToken,
    ShallowASTNodeType_AccessObjectMember,
    ShallowASTNodeType_AccessIdentifier,
    ShallowASTNodeType_Call,
    ShallowASTNodeType_StringConstant,
    ShallowASTNodeType_Semicolon,
    ShallowASTNodeType_CreateConstVariable,
    ShallowASTNodeType_ConditionalCheck,
    ShallowASTNodeType_NumberConstant,
    ShallowASTNodeType_Addition,
    ShallowASTNodeType_Subtraction,
    ShallowASTNodeType_CreateObjectProperty,
    ShallowASTNodeType_CreateObject,
    ShallowASTNodeType_Comma,
    ShallowASTNodeType_Plus,
    ShallowASTNodeType_DoubleEquals,
    ShallowASTNodeType_Minus,
};

enum ConditionalCheckType {
    ConditionalCheckType_Equals,
};

enum ExpressionType {
    ExpressionType_Number,
    ExpressionType_Identifier,
    ExpressionType_PropertyAccess,
    ExpressionType_Multitoken,
    ExpressionType_Object,
};

struct ShallowASTNodeArray;
struct ShallowASTNode;

typedef struct Expression {
    enum ExpressionType type;
    union {
        char* identifier;
        double number;
        struct ShallowASTNode* property_access;
        struct ShallowASTNodeArray* multitoken;
        struct ShallowASTNode* object;
    } value;
} Expression;

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
            StringArray path_data;
        } AccessObjectMember;

        struct {
            // TODO: Change this into ShallowASTNodeArray
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
            Expression expression;
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
            Expression left;
            Expression right;
        } Addition;

        struct {
            Expression left;
            Expression right;
        } Subtraction;

        struct {
            struct ShallowASTNodeArray* tokens;
        } CreateObject;

        struct {
            char* identifier;
            Value value;
        } CreateObjectProperty;

    } data;
} ShallowASTNode;

typedef struct ShallowASTNodeArray {
    ShallowASTNode* nodes;
    size_t count;
} ShallowASTNodeArray;

Expression expression_create_from_lexer_token(LexerToken*);
Expression expression_create_from_shallow_ast_node(const ShallowASTNode*);
Expression expression_clone(const Expression*);
Expression expression_collect_multitoken(const LexerTokenArray* array, size_t* start_index);
double expression_get_number(const Expression*);
void expression_free(Expression*);


void shallow_ast_node_free(ShallowASTNode* node);
void shallow_ast_node_array_push(ShallowASTNodeArray* array, ShallowASTNode* node);
ShallowASTNode shallow_ast_node_create_access_object_member(LexerToken* parent);
ShallowASTNode parser_shallow_get_access_object_member(LexerTokenArray* array, size_t index);
ShallowASTNode shallow_ast_node_create_call();
ShallowASTNode parser_shallow_get_call(LexerTokenArray* array, size_t* index);
bool parser_shallow_is_access_object_member(LexerTokenArray* lexer_token_array, size_t index);
void shallow_ast_node_print(ShallowASTNode* node);
void shallow_ast_node_array_print(ShallowASTNodeArray* array);
ShallowASTNodeArray parse_shallow_parse(LexerTokenArray* lexer_token_array);
void shallow_ast_node_array_free(ShallowASTNodeArray* array);
ShallowASTNode* shallow_ast_node_deep_copy(const ShallowASTNode*);

// Call related functions
ShallowASTNode* shallow_ast_node_call_get_argument(ShallowASTNode*, size_t index);
size_t shallow_ast_node_call_get_argument_count(ShallowASTNode*);
void shallow_ast_node_call_add_argument(ShallowASTNode* node, const ShallowASTNode* argument);

// Access Object Member related functions
ShallowASTNode shallow_ast_node_get_access_object_member(const LexerTokenArray* array, size_t* i);
bool shallow_ast_node_access_object_member_compare_parent(ShallowASTNode*, char* identifier);
size_t shallow_ast_node_access_object_member_get_path_count(const ShallowASTNode*);
const char* shallow_ast_node_access_object_member_get_path_part(const ShallowASTNode*, size_t index);
void shallow_ast_node_access_object_member_add_to_path(ShallowASTNode* node, const char* path);
void shallow_ast_node_access_object_member_set_parent(ShallowASTNode* node, const char* parent);

char* shallow_ast_node_string_constant_get_string(ShallowASTNode*);

// Create Const Variable related functions
const char* shallow_ast_node_create_const_variable_get_name(const ShallowASTNode*);
void shallow_ast_node_create_const_variable_set_name(ShallowASTNode*, const char*);
ShallowASTNode shallow_ast_node_create_create_const_variable_number(char* name, double value);
ShallowASTNode shallow_ast_node_create_create_const_variable_multitoken(char* name);
double shallow_ast_node_create_const_variable_number_get_value(ShallowASTNode*);
void shallow_ast_create_const_variable_multitoken_push(ShallowASTNode*, ShallowASTNode*);
enum ExpressionType shallow_ast_node_create_const_variable_get_expression_type(ShallowASTNode*);
Expression shallow_ast_node_create_const_variable_multitoken_get_expression(ShallowASTNode*);
void shallow_ast_node_create_const_variable_set_expression(ShallowASTNode*, const Expression*);
const Expression* shallow_ast_node_create_const_variable_get_expression(const ShallowASTNode*);
ShallowASTNode shallow_ast_node_create_create_const_variable_object(char* name, ShallowASTNodeArray* tokens);

ShallowASTNode* shallow_ast_node_create_empty();

ShallowASTNode shallow_ast_node_create_access_identifier(char* name);
char* shallow_ast_node_access_identifier_get_name(ShallowASTNode*);
void shallow_ast_node_access_identifier_set_name(ShallowASTNode*, const char*);

ShallowASTNode shallow_ast_node_conditional_check_create(ShallowASTNode* left, ShallowASTNode* right, enum ConditionalCheckType type);

double shallow_ast_node_number_constant_get_value(ShallowASTNode*);
void shallow_ast_node_number_constant_set_value(ShallowASTNode*, double number);

ShallowASTNode shallow_ast_node_create_addition(LexerToken* left, LexerToken* right);
ShallowASTNode shallow_ast_node_create_addition_from_expressions(Expression left, Expression right);

ShallowASTNode shallow_ast_node_create_subtraction(LexerToken* left, LexerToken* right);
ShallowASTNode shallow_ast_node_create_subtraction_from_expressions(const Expression* left, const Expression* right);

ShallowASTNodeArray shallow_ast_node_array_clone(const ShallowASTNodeArray*);

ShallowASTNode shallow_ast_node_create_create_object_property(LexerToken* identifier, LexerToken* value);
ShallowASTNode shallow_ast_node_create_empty_create_object_property();
void shallow_ast_node_create_object_property_set_identifier(ShallowASTNode*, const char* identifier);

ShallowASTNode shallow_ast_node_create_create_object(ShallowASTNodeArray* tokens);


void shallow_ast_node_set_type(ShallowASTNode*, enum ShallowASTNodeType);

ShallowASTNode shallow_ast_node_create_string_constant(char* string);
void shallow_ast_node_string_constant_set(ShallowASTNode*, const char* string);

#endif
