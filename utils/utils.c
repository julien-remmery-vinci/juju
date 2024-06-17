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

bool issemicolon(char* str)
{
    return str[0] == ';' ? true : false;
}

bool isopenparenthese(char* str)
{
    return str[0] == '(' ? true : false;
}

bool iscloseparenthese(char* str)
{
    return str[0] == ')' ? true : false;
}

bool isopenbrace(char* str)
{
    return str[0] == '{' ? true : false;
}

bool isclosebrace(char* str)
{
    return str[0] == '}' ? true : false;
}

bool isopenbracket(char* str)
{
    return str[0] == '[' ? true : false;
}

bool isclosebracket(char* str)
{
    return str[0] == ']' ? true : false;
}