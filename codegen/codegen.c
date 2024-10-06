#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "../utils/utils.h"

exp stored_exps[1000];
int stored_exps_index = 0;
int has_format = 0;

int get_stack_size(program program)
{
    int n = 0;
    for (int i = 0; i < program->nbExp; i++)
    {
        if (program->expressions[i]->type == VARIABLE)
        {
            if(program->expressions[i]->u.var.dataType == INT)
            {
                n+=4;
            }
        }
        else if(program->expressions[i]->type == AFFECT)
        {
            if(program->expressions[i]->u.affect.e1->type == VARIABLE)
            {
                n+=4;
            }
        }
    }
    return n;
}

char* func_def_code(exp e)
{
    int has_vars = 0;
    int has_return = 0;
    int n_locals = 0;
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* buffer = NULL;
    code[0] = '\0';
    sprintf(code, "%s:\n", e->u.func_def.id);
    strcat(code, "\tpushq %rbp\n");
    strcat(code, "\tmovq %rsp, %rbp\n");
    int stack_size = get_stack_size(e->u.func_def.program);
    if(stack_size > 0)
    {
        has_vars = 1;
        // Align 
        int aligned_stack_size = (stack_size + 15) & ~15;
        sprintf(code + strlen(code), "\tsubq $%d, %%rsp\n", aligned_stack_size);
    }
    for (int i = 0; i < e->u.func_def.program->nbExp; i++)
    {
        exp tmp = e->u.func_def.program->expressions[i];
        if(tmp->type == RESERVED && e->u.reserved.reserved == RETURN)
        {
            has_return = 1;
        }
        buffer = get_exp_code(tmp, &n_locals);
        strcat(code, buffer);
        if(buffer != NULL) free(buffer);
    }
    if (!has_return && strcmp(e->u.func_def.id, "main") == 0)
    {
        add_default_return(&code);
    }
    if(has_vars)
    {
        strcat(code, "\tleave\n");
        strcat(code, "\tret\n");
    }
    else
    {
        strcat(code, "\tpopq %rbp\n");
        strcat(code, "\tret\n");
    }
    
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

void var_declaration(exp e, int* n_locals)
{
    (*n_locals)++;
    e->u.var.reg = buffer_alloc(MAX_REGISTER_SIZE);
    sprintf(e->u.var.reg, "-%d(%%rbp)", 4*(*n_locals));
    stored_exps[stored_exps_index] = e;
    stored_exps_index++;
}

char* get_operation_instruction(exp e)
{
    char* instruction;
    switch (e->u.operation.operation)
    {
    case OPERATIONS_ADD:
        instruction = "addl";
        break;
    case OPERATIONS_SUBSTRACT:
        instruction = "subl";
        break;
    case OPERATIONS_MULTIPLY:
        instruction = "imull";
        break;
    case OPERATIONS_DIVIDE:
        instruction = "idivl";
        break;
    default:
        break;
    }
    return instruction;
}

char* operation_code(exp e) {
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* instruction = get_operation_instruction(e);

    char* reg = NULL;
    char* reg1 = NULL;
    char* reg2 = NULL;
    char* op_code = NULL;
    char* op_code1 = NULL;
    char* op_code2 = NULL;

    exp e1 = e->u.operation.e1;
    exp e2 = e->u.operation.e2;

    if (e1->type == INT && e2->type == INT) {
        sprintf(code, "\tmovl $%d, %%eax\n\t%s $%d, %%eax\n", e1->u.i, instruction, e2->u.i);
    } else if (e1->type == ID && e2->type == OPERATION) {
        reg = get_register(e1);
        op_code = operation_code(e2);
        sprintf(code, "%s\t%s %s, %%eax\n", op_code, instruction, reg);
    } else if (e1->type == ADD && e2->type == ID) {
        reg = get_register(e2);
        op_code = operation_code(e1);
        sprintf(code, "%s\t%s %s, %%eax\n", op_code, instruction, reg);
    } else if (e1->type == OPERATION && e2->type == INT) {
        op_code = operation_code(e1);
        sprintf(code, "%s\t%s $%d, %%eax\n", op_code, instruction, e2->u.i);
    } else if (e1->type == INT && e2->type == OPERATION) {
        op_code = operation_code(e2);
        sprintf(code, "%s\t%s $%d, %%eax\n", op_code, instruction, e1->u.i);
    } else if (e1->type == OPERATION && e2->type == OPERATION) {
        op_code1 = operation_code(e1);
        op_code2 = operation_code(e2);
        sprintf(code, "%s%s\t%s %%eax, %%ebx\n", op_code1, op_code2, instruction);
    } else if (e1->type == ID && e2->type == ID) {
        reg1 = get_register(e1);
        reg2 = get_register(e2);
        sprintf(code, "\tmovl %s, %%eax\n\t%s %s, %%eax\n", reg1, instruction, reg2);
    } else if (e1->type == ID && e2->type == INT) {
        reg = get_register(e1);
        sprintf(code, "\tmovl %s, %%eax\n\t%s $%d, %%eax\n", reg, instruction, e2->u.i);
    } else if (e1->type == INT && e2->type == ID) {
        reg = get_register(e2);
        sprintf(code, "\tmovl $%d, %%eax\n\t%s %s, %%eax\n", e1->u.i, instruction, reg);
    }

    if(reg != NULL) free(reg);
    if (reg1 != NULL) free(reg1);
    if (reg2 != NULL) free(reg2);
    if (op_code != NULL) free(op_code);
    if (op_code1 != NULL) free(op_code1);
    if (op_code2 != NULL) free(op_code2); 

    return code;
}

void write_register(exp e, int n_locals)
{
    e->u.var.reg = buffer_alloc(MAX_REGISTER_SIZE);
    sprintf(e->u.var.reg, "-%d(%%rbp)", 4*n_locals);
    stored_exps[stored_exps_index] = e;
    stored_exps_index++;
}

char* affect_code(exp e, int* n_locals)
{
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* buffer = NULL;
    code[0] = '\0';
    if(e->u.affect.e2->type == FUNCTION_CALL)
    {
        (*n_locals)++;
        buffer = func_call_code(e->u.affect.e2);
        sprintf(code, "%s\tmovl %%eax, -%d(%%rbp)\n", buffer, 4*(*n_locals));
        write_register(e->u.affect.e1, *n_locals);
    }
    else if(e->u.affect.e2->type == OPERATION)
    {
        (*n_locals)++;
        buffer = operation_code(e->u.affect.e2);
        sprintf(code, "%s\tmovl %%eax, -%d(%%rbp)\n", buffer, 4*(*n_locals));
        write_register(e->u.affect.e1, *n_locals);
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
        write_register(e->u.affect.e1, *n_locals);
    }

    if(buffer != NULL) free(buffer);

    return code;
}

char* return_code(exp e)
{
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    char* buffer = NULL;
    code[0] = '\0';
    if(e->u.reserved.e->type == OPERATION) 
    {
        buffer = operation_code(e->u.reserved.e);
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

    if(buffer != NULL) free(buffer);

    return code;
}

void add_default_return(char** code)
{
    sprintf(*code + strlen(*code), "\tmovl $%d, %%eax\n", DEFAULT_RETURN_CODE);
}

char* builtin_print_code(exp e)
{
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    code[0] = '\0';
    char* buffer = NULL;
    has_format = 1;
    if(e->u.func_call.params->expressions[0]->type == ID)
    {
        buffer = get_register(e->u.func_call.params->expressions[0]);
        sprintf(code, "\tmovl %s, %%esi\n\tleaq format(%%rip), %%rdi\n\tmovl $0, %%eax\n\tcall printf\n", buffer);
    }
    else if(e->u.func_call.params->expressions[0]->type == INT)
    {
        sprintf(code, "\tmovl $%d, %%eax\n\tmovl %%eax, %%esi\n\tleaq format(%%rip), %%rdi\n\tmovl $0, %%eax\n\tcall printf\n", e->u.func_call.params->expressions[0]->u.i);
    }
    else if(e->u.func_call.params->expressions[0]->type == OPERATION)
    {
        buffer = operation_code(e->u.func_call.params->expressions[0]);
        sprintf(code, "%s\tmovl %%eax, %%esi\n\tleaq format(%%rip), %%rdi\n\tmovl $0, %%eax\n\tcall printf\n", buffer);
    }
    if(buffer != NULL) free(buffer);
    return code;
}

char* get_exp_code(exp e, int* n_locals)
{
    char* code = buffer_alloc(DEFAULT_BUFFER_SIZE);
    code[0] = '\0';
    char* buffer = NULL;
    switch(e->type)
    {
        case FUNCTION_DEF:
            buffer = func_def_code(e);
            strncpy(code, buffer, strlen(buffer));
            break;
        case FUNCTION_CALL:
            if(strcmp(e->u.func_call.id, "print") == 0)
            {
                buffer = builtin_print_code(e);
                strncpy(code, buffer, strlen(buffer));
            }
            else
            {
                buffer = func_call_code(e);
                strncpy(code, buffer, strlen(buffer));
            }
            break;
        case OPERATION:
            buffer = operation_code(e);
            strncpy(code, buffer, strlen(buffer));
            break;
        case AFFECT:
            buffer = affect_code(e, n_locals);
            strncpy(code, buffer, strlen(buffer));
            break;
        case RESERVED:
            if(e->u.reserved.reserved == RETURN)
            {
                buffer = return_code(e);
                strncpy(code, buffer, strlen(buffer));
            }
            break;
    }
    if(buffer != NULL) free(buffer);
    return code;
}

void codegen(program program)
{
    FILE* output = fopen("out.s", "w");
    checkNull(output, "fopen error");
    char* buffer = NULL;

    fprintf(output, ".section .text\n.globl main\n\n");
    for (int i = 0; i < program->nbExp; i++)
    {
        exp e = program->expressions[i];
        buffer = get_exp_code(e, 0);
        fprintf(output, "%s\n", buffer);
        free(buffer);
    }
    if(has_format)
    {
        fputs("\n.section .data\nformat:\n\t.asciz \"%d\\n\"\n", output);
    }

    fclose(output);
}