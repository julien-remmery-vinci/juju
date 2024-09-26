#ifndef _PARSER_H
#define _PARSER_H

#include "../include/tokens.h"
#include "../include/error.h"
#include "../include/program.h"

exp parse_call(program program, Tokens tokens, int* i, char* prevID);
program new_program();
int numNodes(exp e);
enum dataType isDatatype(char* str);
enum reserved isReserved(char* str);
enum operations isoperator(char* str);
exp new_exp();
exp prev_exp(program p);
void add_exp(program p, exp e, int offset);
exp next_exp(Tokens tokens, int* i);
void handle_exp(program program, Tokens tokens, int* i, exp e);
Error parse(Tokens tokens);

#endif