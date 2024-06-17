#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lexer.h"
#include "../utils/utils.h"

char* next_token(FILE* file) {
    bool found = false;
    char token[MAX_TOKENS] = {'\0'};
    memset(token, 0, MAX_TOKENS);
    int nbChar = 0;
    fpos_t last_pos;
    int ret;
    char* returned;
    while(!feof(file))
    {
        char c = getc(file);
        // not in switch for break use
        if(c == ' ') 
        {
            if(found) {break;}
            continue;
        }
        switch(c)
        {
            case(-1): return "";
            case('\n'): return "";
            case('('):
            case(')'):
            case('{'):
            case('}'):
            case(';'):
            case('='):
            case('+'):
            case('-'):
            case('*'):
            case('/'):
            case('^'):
                if(found) 
                {
                    ret = fsetpos(file, &last_pos);
                    if(ret < 0) {print_error("fsetpos error");}
                    returned = (char*)malloc(strlen(token) + 1);
                    checkNull(returned, "malloc error");
                    strcpy(returned, token);
                    return returned;
                }
                char* r = malloc(2);
                checkNull(r, "malloc error");
                r[0] = c;
                r[1] = '\0';
                return r;
        }
        token[nbChar] = c;
        nbChar++;
        found = true;
        ret = fgetpos(file, &last_pos);
        if(ret < 0) {print_error("fgetpos error");}
    }
    returned = (char*)malloc(strlen(token) + 1);
    checkNull(returned, "malloc error");
    strcpy(returned, token);
    return returned;
}

Tokens lex(FILE* file) 
{
    Tokens tokens;
    tokens.nbTokens = 0;

    while(!feof(file)) 
    {
        char* token = next_token(file);
        if(strcmp(token, "") == 0) {continue;}
        Token t;
        t.token = token;
        tokens.tokens[tokens.nbTokens] = t;
        tokens.nbTokens++;
    }
    return tokens;
}

// int main(int argc, char *argv[])
// {
//     if(argc < 2) 
//     {print_usage(argv);}

//     FILE* source = fopen(argv[1], "r");

//     Tokens tokens = lex(source);

//     fclose(source);

//     for (int i = 0; i < tokens.nbTokens; i++)
//     {printf("%s\n", tokens.tokens[i].token);}

//     printf("nb tokens : %d\n", tokens.nbTokens);    
// }
