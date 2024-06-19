#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>

#define MAX_TOKENS 1000

typedef struct {
    char* token;
} Token;

typedef struct {
    Token tokens[MAX_TOKENS];
    int nbTokens;
} Tokens;

void print_usage(char** argv);

void print_error(char* message);

void checkNull(void* arg, char* message);

void checkValidSource(Tokens tokens);

int iscomma(char* str);

int issemicolon(char* str);

int isopenparenthese(char* str);

int iscloseparenthese(char* str);

int isopenbrace(char* str);

int isclosebrace(char* str);

int isopenbracket(char* str);

int isclosebracket(char* str);

int isnum(char* str);

#endif