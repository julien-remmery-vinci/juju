#ifndef _LEXER_H
#define _LEXER_H

#include <stdio.h>

#define MAX_TOKENS 1000

typedef struct {
    char* token;
} Token;

typedef struct {
    Token tokens[MAX_TOKENS];
    int nbTokens;
} Tokens;

Tokens lex(FILE* file);

#endif