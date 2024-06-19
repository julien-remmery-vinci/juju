#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>

void print_usage(char** argv);

void print_error(char* message);

void checkNull(void* arg, char* message);

int issemicolon(char* str);

int isopenparenthese(char* str);

int iscloseparenthese(char* str);

int isopenbrace(char* str);

int isclosebrace(char* str);

int isopenbracket(char* str);

int isclosebracket(char* str);

#endif