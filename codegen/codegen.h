#ifndef CODEGEN_H
#define CODEGEN_H

#include "../include/program.h"

#define MAX_REGISTER_SIZE 20
#define DEFAULT_BUFFER_SIZE 1000

int get_stack_size(program program);
char* get_exp_code(exp e, int* n_locals);
char* func_def_code(exp e);
char* func_call_code(exp e);
char* operation_code(exp e);
char* get_register(exp e);
void replace_register(exp e, char* reg);
void var_declaration(exp e, int* n_locals);
char* affect_code(exp e, int* n_locals);
char* return_code(exp e);
void add_default_return(char** code);
char* builtin_print_code(exp e);
void codegen(program program);

#endif