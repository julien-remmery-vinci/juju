#ifndef PROGRAM_H
#define PROGRAM_H

#include "expressions.h"

/*
    Definition of the program type
*/
typedef struct program* program;

/*
    Struct containing :
        - expressions : an array of expressions
        - nbExp : the number of elements in the array
*/
struct program{
    exp* expressions;
    int nbExp;
};

/*
    Struct containing the elements of an epression
        - type : the expression type
        - one of the possible expressions values :
            - i : an integer
            - id : an identifier, name of a variable
            - dataType : the data type corresponding to the expression
            - reserved : representing a reserved word
                - reserved : an enum value representing the reserved type
                - id : the corresponding reserved word
                - e : the expression following the reserved word
            - var : a variable
                - dataType : the data type of the variable
                - id : the name of the variable
                - reg : the register the variable is storred in (used during codegen phase, subject to changes)
            - operation : an operation between two expressions
                - operation : an enum value representing the operation type
                - e1 : the first expression of the operation
                - e2 : the second expression of the operation
            - affect : an affection to a variable (subject to changes for merging with above operation)
                - e1 : a variable identifier
                - e2 : the affected expression
            - func_def : a function definition
                - program : the struct containing the expressions in the function definition
                - params : the parameters of the function
                - returnType : the returning type of the function
                - id : the name of the function
            - func_call : a function call expression
                - params : the params of the function call
                - id : the name of the function to be called
*/
struct exp {
    enum expType type;
    union {
        int i;
        char* id;
        enum dataType dataType;
        struct {enum reserved reserved; char* id; exp e;} reserved;
        struct {enum dataType dataType; char* id; char* reg;} var;
        struct {enum operation operation; exp e1; exp e2;} operation;
        struct {exp e1; exp e2;} affect;
        struct {program program; program params; enum dataType returnType; char* id;} func_def;
        struct {program params; char* id;} func_call;
    } u;
};

#endif