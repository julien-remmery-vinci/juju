#ifndef _LEXER_H
#define _LEXER_H

#include <stdio.h>
#include "../include/error.h"

/*  
    Create a Tokens struct 
        - nbTokens is set to 0
    Return : the created struct   
*/  
Tokens new_tokens();

/*  
    Get the next token in the file
    Args : 
        - FILE* opened with 'r' option
    Return the next token found in file :
        - NULL if no token found 
*/
char* next_token(FILE* file);

/*  
    Retrieve an array of tokens from a given file
    Args : 
        - FILE* opened with 'r' option
    Return Error struct with error type set to corresponding error :
        - ARGS_ERROR : provided file is NULL 
        - ERROR_NONE : no error during lexing
*/
Error lex(FILE* file);

#endif