#ifndef CODEGEN_H
#define CODEGEN_H

#include "../include/program.h"

/*
    References to a register in this file should be represented by another term such as memory space (?)
    This is subject to changes as I learn how this work during coding this
*/

/*
    Default max register size for storring during codegen
*/
#define MAX_REGISTER_SIZE 20

/*
    Default allocated buffer size (subject to change, make it dynamic)
*/
#define DEFAULT_BUFFER_SIZE 1000

/*
    Default return code, success
*/
#define DEFAULT_RETURN_CODE 0

/*
    Get the expected stack size for a given program (expected to be part of a function definition)
    Args :
        - program : a non NULL program struct
    Returns : an integer representing the need space on the stack
*/
int get_stack_size(program program);

/*
    Get the assembly code representing an expression
    Args :
        - e : the expressing to generate code for
        - n_locals : a reference to the number of local defined variables
    Return : the generated code for the expression
*/
char* get_exp_code(exp e, int* n_locals);

/*
    Get the assembly code representing a function definition expression
    Args :
        - e : the function definition expression
    Return : the generated code for the expression
*/
char* func_def_code(exp e);

/*
    Get the assembly code representing a function call expression
    Args :
        - e : the function call expression
    Return : the generated code for the expression
*/
char* func_call_code(exp e);

/*
    Get the assembly code representing an operation expression
    Args :
        - e : the operation expression
    Return : the generated code for the expression
*/
char* operation_code(exp e);

/*
    Find in which register the variable represented by an expression is storred
    Args :
        - e : the expression representing the variable
    Return : the string representing the register
*/
char* get_register(exp e);

/*
    Replace the storred register in which a variable is storred
    Args :
        - e : the expression to replace the register for
        - reg : the new affected register
*/
void replace_register(exp e, char* reg);

/*
    Store a variable in a register for future use
    Args :
        - e : the expression representing the variable to store
        - n_locals : a reference to the number of locals defined variables
*/
void var_declaration(exp e, int* n_locals);

/*
    Get the assembly code representing an affectation expression
    Args :
        - e : the affectation expression
    Return : the generated code for the expression
*/
char* affect_code(exp e, int* n_locals);

/*
    Get the assembly code representing a return expression
    Args :
        - e : the return expression
    Return : the generated code for the expression
*/
char* return_code(exp e);

/*
    Get the assembly code representing the default return statement (DEFAULT_RETURN_CODE)
    Return : the generated code
*/
char* get_default_return();

/*
    Get the assembly code representing a builtin print expression
    Args :
        - e : the print expression
    Return : the generated code for the expression
*/
char* builtin_print_code(exp e);

/*
    Generates the assembly code for a given program struct
    Args :
        - program : a non NULL program struct
*/
void codegen(program program);

#endif