#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "../utils/utils.h"

Tokens new_tokens() 
{
    Tokens tokens;
    tokens.nbTokens = 0;
    return tokens;
}

char* next_token(FILE* file) {
    char* token = NULL;
    fpos_t last_pos;
    int ret = 0;

    while(!feof(file))
    {
        char c = getc(file);
        // not in switch for break use
        if(c == ' ') 
        {
            if(token != NULL) {break;}
            continue;
        }
        switch(c)
        {
            case(-1):
            case('\n'):
                free(token); 
                return "";
            case('('):
            case(')'):
            case('{'):
            case('}'):
            case(';'):
            case(','):
            case('='):
            case('+'):
            case('-'):
            case('*'):
            case('/'):
            case('^'):
                // if token is not empty, we need to go back one char
                if(token != NULL) 
                {
                    ret = fsetpos(file, &last_pos);
                    if(ret < 0) {print_error("fsetpos error");}
                    return token;
                }
                token = buffer_alloc(2);
                strncat(token, &c, 1);
                return token;
        }

        if(token == NULL){token = buffer_alloc(2);}
        else {token = (char*)realloc(token, strlen(token) + 2);}

        strncat(token, &c, 1);

        ret = fgetpos(file, &last_pos);
        if(ret < 0) 
        {
            print_error("fgetpos error");
            exit(EXIT_FAILURE);    
        }
    }
    return token;
}

Error lex(FILE* file) 
{
    Error error = new_error();

    Tokens tokens = new_tokens();

    while(!feof(file)) 
    {
        char* token = next_token(file);
        if(strcmp(token, "") == 0) {continue;}
        Token t;
        t.token = buffer_alloc(strlen(token) + 1);
        strcpy(t.token, token);
        free(token);
        tokens.tokens[tokens.nbTokens] = t;
        tokens.nbTokens++;
    }
    error.tokens = tokens;
    return error;
}
