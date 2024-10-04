#ifndef _PARSER_H
#define _PARSER_H

#include "../include/tokens.h"
#include "../include/error.h"
#include "../include/program.h"

/*
    Parse a function call expression
    Args : 
        - program : Non NULL program struct
        - tokens : NON NULL Tokens struct
        - i : a reference to the current index position in tokens array
        - prevID : name of the preceding identifier
    Return : the parsed function call expression
*/
exp parse_call(program program, Tokens tokens, int* i, char* prevID);

/*
    Create a new program struct
        - expressions is set to NULL
        - nbExp is set to 
    Return : the created program struct
*/
program new_program();

/*
    NOT SUPPORTED ANYMORE
    Returns the number of child nodes in provided expression
    Args :
        - e : the expression to count child nodes on
    Return : an integer representing the number of child nodes
*/
// int numNodes(exp e);

/*
    Get the data type represented by the provided string
    Args :
        - str : string to look for datatype on
    Return a dataType enum containing the corresponding data type :
        - DATATYPE_NONE if no data type is found in the string
*/
enum dataType getDataType(char* str);

/*
    Get the reserved word represented by the provided string
    Args :
        - str : string to look for reserved words on
    Return a reserved enum containing the reseved word type :
        - RESERVED_NONE if no word is found in the string
*/
enum reserved getReserved(char* str);

/*
    Get the operation represented by the provided string
    Args :
        - str : string to look for operator on
    Return an operation enum containing the operation type :
        - OPERATIONS_NONE if no operator is found in the string
*/
enum operation getOperation(char* str);

/*
    Create a new expression struct
    Return : the new expression struct
*/
exp new_exp();

/*
    Get the previous expression in provided program struct
    Args :
        - p : a program struct
    Return :
        - NULL if nbExp is set to 0
        - the preceding expression if nbExp > 0
*/
exp prev_exp(program p);

/*
    Add an expression to a program
    Args :
        - p : a non NULL program struct
        - e : the expression to be added
        - offset : the emplacement to add the expression to
            example : 
                -1 to erase preceding expression
                0 to add to the next available spot in the expressions array
*/
void add_exp(program p, exp e, int offset);

/*
    Get the next expression found in the tokens
    Args :
        - tokens : a non NULL tokens struct
        - i : a reference to the current index position in tokens array
    Return : the next expression found
*/
exp next_exp(Tokens tokens, int* i);

/*
    Handle a given expression based on its type
    Args :
        - program : a non NULL program struct
        - tokens : a non NULL Tokens struct
        - i : a reference to the current index position in tokens array
        - e : the expression to be handled
*/
void handle_exp(program program, Tokens tokens, int* i, exp e);

/*
    Parse the array of tokens into a program struct containing the corresponding expressions
    Args :
        - tokens : a non NULL Tokens struct
*/
Error parse(Tokens tokens);

#endif