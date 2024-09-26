#ifndef CODEGEN_H
#define CODEGEN_H

#include "../include/program.h"

char* func_def_code(exp e);
char* func_call_code(exp e);
char* add_code(exp e1, exp e2);
char* get_register(exp e);
void replace_register(exp e, char* reg);
char* var_declaration_code(exp e, int* n_locals);
char* affect_code(exp e, int* n_locals);
char* return_code(exp e);
void add_default_return(char** code);
char* builtin_print_code(exp e);
void codegen(program program);

#endif