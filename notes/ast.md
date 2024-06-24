# Ast


```c
typedef struct {

    ASTNodeType type;

    union data {

        struct {
            char* identifier;
            struct {
                ASTNode* children;
                size_t count;
            } arguments;
        } function_call;

        struct {
            char* identifier;
            struct {
                ASTNode* children;
                size_t count;
            } value;
        } variable_declaration;

        struct {
            double value; 
        } number_constant;

    };

} ASTNode;
```
