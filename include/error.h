#ifndef ERROR_H
#define ERROR_H

#include "tokens.h"
#include "program.h"

/*
    Enum containing error types
*/
enum ErrorType {
    ERROR_NONE,
    ARGS_ERROR
};

/*
    Struct containg the waring messages and their count
*/
typedef struct {
    char** messages;
    int n;
} Warnings;

/*
    Struct containg the error messages and their count
*/
typedef struct {
    char** messages;
    int n;
} Errors;

/*
    Struct representing the output of a function
    Containing :
        - the error type : default to ERROR_NONE
        - a struct containing the warnings encountered during process
        - a struct containing the errors encountered during process
        - a union containg the result of the function
*/
typedef struct {
    enum ErrorType error_type;
    Warnings warnings; 
    Errors errors;
    union {
        Tokens tokens;
        program program;
    };
} Error;

#endif