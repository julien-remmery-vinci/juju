#ifndef TOKENS_H
#define TOKENS_H

/*
    Default max number of tokens (subject to change, make it dynamic)
*/
#define MAX_TOKENS 1000

/*
    Struct containing a token
*/
typedef struct {
    char* token;
} Token;

/*
    Tokens type and struct definition
        - tokens : the array of tokens
        - nbTokens : the number of tokens in the array
*/
typedef struct {
    Token tokens[MAX_TOKENS];
    int nbTokens;
} Tokens;

#endif