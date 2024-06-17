#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>
#include <stdbool.h>


void print_usage(char** argv);

void print_error(char* message);

void checkNull(void* arg, char* message);

bool issemicolon(char* str);

bool isopenparenthese(char* str);

bool iscloseparenthese(char* str);

bool isopenbrace(char* str);

bool isclosebrace(char* str);

bool isopenbracket(char* str);

bool isclosebracket(char* str);

#endif