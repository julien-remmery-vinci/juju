#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

void print_usage(char** argv)
{
    printf("Usage:\n");
    printf("    : %s <file>\n", argv[0]);
    exit(EXIT_FAILURE);
}

void print_error(char* message)
{
    printf("Error : %s\n", message);
    exit(EXIT_FAILURE);
}

void checkNull(void* arg, char* message)
{
    if(arg == NULL) 
    {
        print_error(message);
    }
}

int issemicolon(char* str) {return *str == ';';}

int isopenparenthese(char* str) {return *str == '(';}

int iscloseparenthese(char* str) {return *str == ')';}

int isopenbrace(char* str) {return *str == '{';}

int isclosebrace(char* str) {return *str == '}';}

int isopenbracket(char* str) {return *str == '[';}

int isclosebracket(char* str) {return *str == ']';}