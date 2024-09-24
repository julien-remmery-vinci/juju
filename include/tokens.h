#ifndef TOKENS_H
#define TOKENS_H

#define MAX_TOKENS 1000

typedef struct {
    char* token;
} Token;

typedef struct {
    Token tokens[MAX_TOKENS];
    int nbTokens;
} Tokens;

#endif