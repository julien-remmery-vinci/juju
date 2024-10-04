#ifndef _LEXER_H
#define _LEXER_H

#include <stdio.h>
#include "../include/error.h"

/*  
    Create and return a new Tokens struct     
*/  
Tokens new_tokens();

/*  
    Args : FILE* opened with 'r' option
    Return the next token found in file :
        - NULL if no token found 
*/
char* next_token(FILE* file);

/*  
    Args : FILE* opened with 'r' option
    Return Error struct with error type set to corresponding error :
        - ARGS_ERROR : provided file is NULL 
        - ERROR_NONE : 
*/
Error lex(FILE* file);

#endif