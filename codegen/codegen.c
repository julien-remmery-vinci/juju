#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "../utils/utils.h"

#define MAX_REGISTER_SIZE 20
#define DEFAULT_BUFFER_SIZE 1000

int has_return = 0;
exp stored_exps[1000];
int stored_exps_index = 0;

char* func_def_code(exp e)
{
    int n_locals = 0;
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* buffer;
    code[0] = '\0';
    sprintf(code, "%s:\n", e->u.func_def.id);
    strcat(code, "\tpushq %rbp\n");
    strcat(code, "\tmovq %rsp, %rbp\n");
    for (int i = 0; i < e->u.func_def.program->nbExp; i++)
    {
        if(e->u.func_def.program->expressions[i]->type == VARIABLE)
        {
            buffer = var_declaration_code(e->u.func_def.program->expressions[i], &n_locals);
            strncat(code, buffer, strlen(buffer));
        }
        if(e->u.func_def.program->expressions[i]->type == FUNCTION_CALL)
        {
            buffer = func_call_code(e->u.func_def.program->expressions[i]);
            strncat(code, buffer, strlen(buffer));
        }
        else if(e->u.func_def.program->expressions[i]->type == AFFECT)
        {
            buffer = affect_code(e->u.func_def.program->expressions[i], &n_locals);
            strncat(code, buffer, strlen(buffer));
        }
        else if(e->u.func_def.program->expressions[i]->type == RESERVED && e->u.func_def.program->expressions[i]->u.reserved.reserved == RETURN)
        {
            has_return = 1;
            buffer = return_code(e->u.func_def.program->expressions[i]);
            strncat(code, buffer, strlen(buffer));
        }
    }
    if (!has_return && strcmp(e->u.func_def.id, "main") == 0)
    {
        add_default_return(&code);
    }
    strcat(code, "\tpopq %rbp\n");
    strcat(code, "\tret\n");

    free(buffer);
    
    return code;
}

char* func_call_code(exp e)
{
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    code[0] = '\0';
    sprintf(code, "\tcall %s\n", e->u.func_call.id);
    return code;
}

char* get_register(exp e)
{
    char* reg = buffer_alloc(MAX_REGISTER_SIZE);
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
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    code[0] = '\0';
    (*n_locals)++;
    e->u.var.reg = buffer_alloc(MAX_REGISTER_SIZE);
    sprintf(e->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
    stored_exps[stored_exps_index] = e;
    stored_exps_index++;
    return code;
}

char* add_code(exp e1, exp e2) {
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* reg = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* reg1 = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* reg2 = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* addcode = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* addcode1 = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* addcode2 = buffer_alloc(DEFAULT_BUFFER_SIZE);

    if (e1->type == INT && e2->type == INT) {
        sprintf(code, "\tmovl $%d, %%eax\n\taddl $%d, %%eax\n", e1->u.i, e2->u.i);
    } else if (e1->type == ID && e2->type == ADD) {
        reg = get_register(e1);
        addcode = add_code(e2->u.add.e1, e2->u.add.e2);
        sprintf(code, "%s\taddl %s, %%eax\n", addcode, reg);
    } else if (e1->type == ADD && e2->type == ID) {
        reg = get_register(e2);
        addcode = add_code(e1->u.add.e1, e1->u.add.e2);
        sprintf(code, "%s\taddl %s, %%eax\n", addcode, reg);
    } else if (e1->type == ADD && e2->type == INT) {
        addcode = add_code(e1->u.add.e1, e1->u.add.e2);
        sprintf(code, "%s\taddl $%d, %%eax\n", addcode, e2->u.i);
    } else if (e1->type == INT && e2->type == ADD) {
        addcode = add_code(e2->u.add.e1, e2->u.add.e2);
        sprintf(code, "%s\taddl $%d, %%eax\n", addcode, e1->u.i);
    } else if (e1->type == ADD && e2->type == ADD) {
        addcode1 = add_code(e1->u.add.e1, e1->u.add.e2);
        addcode2 = add_code(e2->u.add.e1, e2->u.add.e2);
        sprintf(code, "%s%s\taddl %%eax, %%ebx\n", addcode1, addcode2);
    } else if (e1->type == ID && e2->type == ID) {
        reg1 = get_register(e1);
        reg2 = get_register(e2);
        sprintf(code, "\tmovl %s, %%eax\n\taddl %s, %%eax\n", reg1, reg2);
    } else if (e1->type == ID && e2->type == INT) {
        reg = get_register(e1);
        sprintf(code, "\tmovl %s, %%eax\n\taddl $%d, %%eax\n", reg, e2->u.i);
    } else if (e1->type == INT && e2->type == ID) {
        reg = get_register(e2);
        sprintf(code, "\tmovl $%d, %%eax\n\taddl %s, %%eax\n", e1->u.i, reg);
    }

    if(reg != NULL) free(reg);
    if (reg1 != NULL) free(reg1);
    if (reg2 != NULL) free(reg2);
    if (addcode != NULL) free(addcode);
    if (addcode1 != NULL) free(addcode1);
    if (addcode2 != NULL) free(addcode2); 

    return code;
}

char* affect_code(exp e, int* n_locals)
{
    char* code = malloc(1000*sizeof(char));
    char* buffer;
    code[0] = '\0';
    if(e->u.affect.e2->type == FUNCTION_CALL)
    {
        (*n_locals)++;
        buffer = func_call_code(e->u.affect.e2);
        sprintf(code, "%s\tmovl %%eax, -%d(%%rbp)\n", buffer, 4*(*n_locals));
        e->u.affect.e1->u.var.reg = buffer_alloc(MAX_REGISTER_SIZE);
        sprintf(e->u.affect.e1->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
        stored_exps[stored_exps_index] = e->u.affect.e1;
        stored_exps_index++;
    }
    else if(e->u.affect.e2->type == ADD)
    {
        (*n_locals)++;
        buffer = add_code(e->u.affect.e2->u.add.e1, e->u.affect.e2->u.add.e2);
        sprintf(code, "%s\tmovl %%eax, -%d(%%rbp)\n", buffer, 4*(*n_locals));
        e->u.affect.e1->u.var.reg = buffer_alloc(MAX_REGISTER_SIZE);
        sprintf(e->u.affect.e1->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
        stored_exps[stored_exps_index] = e->u.affect.e1;
        stored_exps_index++;
    }
    else if(e->u.affect.e1->type == ID && e->u.affect.e2->type == ID)
    {
        buffer = get_register(e->u.affect.e2);
        replace_register(e->u.affect.e1, buffer);
    }
    else if(e->u.affect.e1->type == ID && e->u.affect.e2->type == INT)
    {
        buffer = get_register(e->u.affect.e1);
        sprintf(code, "\tmovl $%d, %s\n", e->u.affect.e2->u.i, buffer);
    }
    else if(e->u.affect.e1->type == VARIABLE)
    {
        (*n_locals)++;
        sprintf(code, "\tmovl $%d, -%d(%%rbp)\n", e->u.affect.e2->u.i, 4*(*n_locals));
        e->u.affect.e1->u.var.reg = buffer_alloc(MAX_REGISTER_SIZE);
        sprintf(e->u.affect.e1->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
        stored_exps[stored_exps_index] = e->u.affect.e1;
        stored_exps_index++;
    }

    // free(buffer);

    return code;
}

char* return_code(exp e)
{
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* buffer;
    code[0] = '\0';
    if(e->u.reserved.e->type == ADD) 
    {
        buffer = add_code(e->u.reserved.e->u.add.e1, e->u.reserved.e->u.add.e2);
        sprintf(code, "%s", buffer);
    }
    else if(e->u.reserved.e->type == ID)
    {
        buffer = buffer_alloc(MAX_REGISTER_SIZE);
        for (int i = 0; i < stored_exps_index; i++)
        {
            if (strcmp(stored_exps[i]->u.var.id, e->u.reserved.e->u.id) == 0)
            {
                strcpy(buffer, stored_exps[i]->u.var.reg);
            }
        }
        sprintf(code, "\tmovl %s, %%eax\n", buffer);
    }
    else if(e->u.reserved.e->type == INT)
    {
        sprintf(code, "\tmovl $%d, %%eax\n", e->u.reserved.e->u.i);
    }

    free(buffer);

    return code;
}

void add_default_return(char** code)
{
    strcat(*code, "\tmovl $0, %eax\n");
}

char* builtin_print_code(exp e)
{
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);

    return code;
}

void codegen(program program)
{
    FILE* output = fopen("out.s", "w");
    checkNull(output, "fopen error");
    char* buffer;

    fprintf(output, ".text\n.globl main\n\n");
    for (int i = 0; i < program->nbExp; i++)
    {
        exp e = program->expressions[i];
        if(e->type == FUNCTION_DEF)
        {
            buffer = func_def_code(e);
            fprintf(output, "%s", buffer);
            memset(buffer, 0, strlen(buffer));
        }
    }

    fclose(output);
    free(buffer);
}