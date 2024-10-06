#ifndef _UTILS_H
#define _UTILS_H

#include <stdlib.h>
#include "../parser/parser.h"
#include "../include/tokens.h"
#include "../include/error.h"
#include "../include/program.h"

void print_usage(char** argv);

void print_warning(char* message);

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

void checkErrorAndWarnings(Error error);

void print_program(program program);

void print_exp(exp e);

void free_exp(exp e);

void free_program(program program);

Error new_error();

void free_error(Error* error);

void free_tokens(Tokens tokens);

char* buffer_alloc(int size);

void concat_text(char** buffer, int* buffer_size, char* concat);

#endif