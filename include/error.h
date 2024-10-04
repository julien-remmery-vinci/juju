#ifndef ERROR_H
#define ERROR_H

#include "tokens.h"
#include "program.h"

enum ErrorType {
    ERROR_NONE,
    ARGS_ERROR
};

typedef struct {
    char** messages;
    int n;
} Warnings;

typedef struct {
    char** messages;
    int n;
} Errors;

// typedef struct Error* Error;

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