#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

#define MAX_REGISTER_SIZE 20

int has_return = 0;
exp stored_exps[1000];
int stored_exps_index = 0;

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

char* func_def_code(exp e)
{
    int n_locals = 0;
    char* code = malloc(1000*sizeof(char));
    code[0] = '\0';
    sprintf(code, "%s:\n", e->u.func_def.id);
    strcat(code, "\tpushq %rbp\n");
    strcat(code, "\tmovq %rsp, %rbp\n");
    for (int i = 0; i < e->u.func_def.program.nbExp; i++)
    {
        if(e->u.func_def.program.expressions[i]->type == VARIABLE)
        {
            strcat(code, var_declaration_code(e->u.func_def.program.expressions[i], &n_locals));
        }
        if(e->u.func_def.program.expressions[i]->type == FUNCTION_CALL)
        {
            strcat(code, func_call_code(e->u.func_def.program.expressions[i]));
        }
        else if(e->u.func_def.program.expressions[i]->type == AFFECT)
        {
            strcat(code, affect_code(e->u.func_def.program.expressions[i], &n_locals));
        }
        else if(e->u.func_def.program.expressions[i]->type == RESERVED && e->u.func_def.program.expressions[i]->u.reserved.reserved == RETURN)
        {
            has_return = 1;
            strcat(code, return_code(e->u.func_def.program.expressions[i]));
        }
    }
    if (!has_return && strcmp(e->u.func_def.id, "main") == 0)
    {
        add_default_return(&code);
    }
    strcat(code, "\tpopq %rbp\n");
    strcat(code, "\tret\n");
    
    return code;
}

char* func_call_code(exp e)
{
    char* code = malloc(1000*sizeof(char));
    code[0] = '\0';
    sprintf(code, "\tcall %s\n", e->u.func_call.id);
    return code;
}

char* get_register(exp e)
{
    char* reg = malloc(MAX_REGISTER_SIZE*sizeof(char));
    for (int i = 0; i < stored_exps_index; i++)
    {
        if (strcmp(stored_exps[i]->u.var.id, e->u.id) == 0)
        {
            strcpy(reg, stored_exps[i]->u.var.reg);
        }
    }
    return reg;
}

void replace_register(exp e, char* reg)
{
    // TODO: clear the register of the replaced variable for future use
    for (int i = 0; i < stored_exps_index; i++)
    {
        if (strcmp(stored_exps[i]->u.var.id, e->u.id) == 0)
        {
            strcpy(stored_exps[i]->u.var.reg, reg);
        }
    }
}

char* var_declaration_code(exp e, int* n_locals)
{
    char* code = malloc(1000*sizeof(char));
    code[0] = '\0';
    (*n_locals)++;
    e->u.var.reg = malloc(MAX_REGISTER_SIZE*sizeof(char));
    sprintf(e->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
    stored_exps[stored_exps_index] = e;
    stored_exps_index++;
    return code;
}

char* add_code(exp e1, exp e2) {
    static char code[1024];
    memset(code, 0, sizeof(code));

    if (e1->type == INT && e2->type == INT) {
        sprintf(code, "\tmovl $%d, %%eax\n\taddl $%d, %%eax\n", e1->u.i, e2->u.i);
    } else if (e1->type == ID && e2->type == ADD) {
        char* reg = get_register(e1);
        char* addcode = add_code(e2->u.add.e1, e2->u.add.e2);
        sprintf(code, "%s\taddl %s, %%eax\n", addcode, reg);
    } else if (e1->type == ADD && e2->type == ID) {
        char* reg = get_register(e2);
        char* addcode = add_code(e1->u.add.e1, e1->u.add.e2);
        sprintf(code, "%s\taddl %s, %%eax\n", addcode, reg);
    } else if (e1->type == ADD && e2->type == INT) {
        char* addcode = add_code(e1->u.add.e1, e1->u.add.e2);
        sprintf(code, "%s\taddl $%d, %%eax\n", addcode, e2->u.i);
    } else if (e1->type == INT && e2->type == ADD) {
        char* addcode = add_code(e2->u.add.e1, e2->u.add.e2);
        sprintf(code, "%s\taddl $%d, %%eax\n", addcode, e1->u.i);
    } else if (e1->type == ADD && e2->type == ADD) {
        char* addcode1 = add_code(e1->u.add.e1, e1->u.add.e2);
        char* addcode2 = add_code(e2->u.add.e1, e2->u.add.e2);
        sprintf(code, "%s%s\taddl %%eax, %%ebx\n", addcode1, addcode2);
    } else if (e1->type == ID && e2->type == ID) {
        char* reg1 = get_register(e1);
        char* reg2 = get_register(e2);
        sprintf(code, "\tmovl %s, %%eax\n\taddl %s, %%eax\n", reg1, reg2);
    } else if (e1->type == ID && e2->type == INT) {
        char* reg = get_register(e1);
        sprintf(code, "\tmovl %s, %%eax\n\taddl $%d, %%eax\n", reg, e2->u.i);
    } else if (e1->type == INT && e2->type == ID) {
        char* reg = get_register(e2);
        sprintf(code, "\tmovl $%d, %%eax\n\taddl %s, %%eax\n", e1->u.i, reg);
    }

    return code;
}

char* affect_code(exp e, int* n_locals)
{
    char* code = malloc(1000*sizeof(char));
    code[0] = '\0';
    if(e->u.affect.e2->type == FUNCTION_CALL)
    {
        (*n_locals)++;
        char* callcode = func_call_code(e->u.affect.e2);
        sprintf(code, "%s\tmovl %%eax, -%d(%%rbp)\n", callcode, 4*(*n_locals));
        e->u.affect.e1->u.var.reg = malloc(MAX_REGISTER_SIZE*sizeof(char));
        sprintf(e->u.affect.e1->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
        stored_exps[stored_exps_index] = e->u.affect.e1;
        stored_exps_index++;
    }
    else if(e->u.affect.e2->type == ADD)
    {
        (*n_locals)++;
        char* addcode = add_code(e->u.affect.e2->u.add.e1, e->u.affect.e2->u.add.e2);
        sprintf(code, "%s\tmovl %%eax, -%d(%%rbp)\n", addcode, 4*(*n_locals));
        e->u.affect.e1->u.var.reg = malloc(MAX_REGISTER_SIZE*sizeof(char));
        sprintf(e->u.affect.e1->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
        stored_exps[stored_exps_index] = e->u.affect.e1;
        stored_exps_index++;
    }
    else if(e->u.affect.e1->type == ID && e->u.affect.e2->type == ID)
    {
        replace_register(e->u.affect.e1, get_register(e->u.affect.e2));
    }
    else if(e->u.affect.e1->type == ID && e->u.affect.e2->type == INT)
    {
        char* reg = get_register(e->u.affect.e1);
        sprintf(code, "\tmovl $%d, %s\n", e->u.affect.e2->u.i, reg);
    }
    else if(e->u.affect.e1->type == VARIABLE)
    {
        (*n_locals)++;
        sprintf(code, "\tmovl $%d, -%d(%%rbp)\n", e->u.affect.e2->u.i, 4*(*n_locals));
        e->u.affect.e1->u.var.reg = malloc(MAX_REGISTER_SIZE*sizeof(char));
        sprintf(e->u.affect.e1->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
        stored_exps[stored_exps_index] = e->u.affect.e1;
        stored_exps_index++;
    }
    return code;
}

char* return_code(exp e)
{
    char* code = malloc(1000*sizeof(char));
    code[0] = '\0';
    if(e->u.reserved.e->type == ADD) 
    {
        char* addcode = add_code(e->u.reserved.e->u.add.e1, e->u.reserved.e->u.add.e2);
        sprintf(code, "%s", addcode);
    }
    else if(e->u.reserved.e->type == ID)
    {
        char* reg = malloc(MAX_REGISTER_SIZE*sizeof(char));
        for (int i = 0; i < stored_exps_index; i++)
        {
            if (strcmp(stored_exps[i]->u.var.id, e->u.reserved.e->u.id) == 0)
            {
                strcpy(reg, stored_exps[i]->u.var.reg);
            }
        }
        sprintf(code, "\tmovl %s, %%eax\n", reg);
    }
    else if(e->u.reserved.e->type == INT)
    {
        printf("RETURN CODE USING INTEGER\n");
        sprintf(code, "\tmovl $%d, %%eax\n", e->u.reserved.e->u.i);
    }
    return code;
}

void add_default_return(char** code)
{
    strcat(*code, "\tmovl $0, %eax\n");
}

char* builtin_print_code(exp e)
{
    char* code = malloc(1000*sizeof(char));

    return code;
}

char* codegen(program program)
{
    char* code = malloc(1000*sizeof(char));
    code[0] = '\0';

    sprintf(code, ".text\n.globl main\n\n");
    for (int i = 0; i < program.nbExp; i++)
    {
        exp e = program.expressions[i];
        if(e->type == FUNCTION_DEF)
        {
            strcat(code, func_def_code(e));
        }
    }
    
    return code;
}