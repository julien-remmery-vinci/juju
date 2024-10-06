#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "utils.h"

#include "../include/program.h"
#include "../include/error.h"
#include "../include/tokens.h"

void print_usage(char** argv)
{
    printf("Usage:\n");
    printf("    : %s <file>\n", argv[0]);
}

void print_warning(char* message)
{
    printf("Warn : %s\n", message);
}

void print_error(char* message)
{
    printf("Error : %s\n", message);
}

void checkNull(void* arg, char* message)
{
    if(arg == NULL) 
    {
        print_error(message);
        exit(EXIT_FAILURE);
    }
}

void checkValidSource(Tokens tokens)
{
    if(tokens.nbTokens == 0)
    {
        printf("Invalid source.\n");
        exit(EXIT_FAILURE);
    }
}

int iscomma(char* str) {return *str == ',';}

int issemicolon(char* str) {return *str == ';';}

int isopenparenthese(char* str) {return *str == '(';}

int iscloseparenthese(char* str) {return *str == ')';}

int isopenbrace(char* str) {return *str == '{';}

int isclosebrace(char* str) {return *str == '}';}

int isopenbracket(char* str) {return *str == '[';}

int isclosebracket(char* str) {return *str == ']';}

int isnum(char* str)
{
    while(*str != '\0')
    {
        if(!isdigit(*str)) {return false;}
        str++;
    }
    return true;
}

void checkErrorAndWarnings(Error error)
{
    if(error.error_type == ERROR_NONE) return;

    for (int i = 0; i < error.warnings.n; i++)
    {
        if(error.warnings.messages[i] == NULL) {print_error("missing warning message.\n");}
        else print_warning(error.warnings.messages[i]);
    }

    for (int i = 0; i < error.errors.n; i++)
    {
        if(error.errors.messages[i] == NULL) {print_error("missing error message.\n");}
        else print_error(error.errors.messages[i]);
    }
}

void print_program(program program)
{
    printf("----------------\nprinting program\n----------------\n");
    for (int i = 0; i < program->nbExp; i++)
    {
        print_exp(program->expressions[i]);
        printf("\n");
    }
    printf("---------------\nend of printing\n---------------\n");
}

void print_exp(exp e) 
{
    switch(e->type)
    {
        case(AFFECT):
            print_exp(e->u.affect.e1);
            printf(" = ");
            print_exp(e->u.affect.e2);
            break;
        case(OPERATION):
            print_exp(e->u.operation.e1);
            switch(e->u.operation.operation)
            {
                case OPERATIONS_ADD:
                    printf(" + ");
                    break;
                case OPERATIONS_SUBSTRACT:
                    printf(" - ");
                    break;
                case OPERATIONS_MULTIPLY:
                    printf(" * ");
                    break;
                case OPERATIONS_DIVIDE:
                    printf(" / ");
                    break;
            }
            print_exp(e->u.operation.e2);
            break;
        case(INT):
            printf("%d", e->u.i);
            break;
        case(ID):
            printf("%s", e->u.id);
            break;
        case(VARIABLE):
            switch (e->u.var.dataType)
            {
                case DATATYPE_INTEGER:
                    printf("int %s", e->u.var.id);
                    break;
            }
            break;
        case(RESERVED):
            switch(e->u.reserved.reserved)
            {
                case(RETURN):
                    printf("return ");
                    print_exp(e->u.reserved.e);
                    break;
            }
            break;
        case(FUNCTION_DEF):
            printf("%s %s(", e->u.func_def.returnType == 1 ? "int" : "", e->u.func_def.id);
            for (int i = 0; i < e->u.func_def.params->nbExp; i++)
            {
                print_exp(e->u.func_def.params->expressions[i]);
                if(i + 1 < e->u.func_def.params->nbExp) {printf(", ");}
            }
            printf(") {\n");
            for (int i = 0; i < e->u.func_def.program->nbExp; i++)
            {
                printf("    ");
                print_exp(e->u.func_def.program->expressions[i]);
                printf(";\n");
            }
            printf("}");
            break;
        case(FUNCTION_CALL):
            printf("%s(", e->u.func_call.id);
            for (int i = 0; i < e->u.func_call.params->nbExp; i++)
            {
                print_exp(e->u.func_call.params->expressions[i]);
                if(i + 1 < e->u.func_call.params->nbExp) {printf(", ");}
            }
            printf(")");
            break;
        case(DATATYPE):
            switch(e->u.dataType)
            {
                case DATATYPE_INTEGER:
                    printf("int");
                    break;
            }
            break;
        default:
            printf("not yet implemented exp type printing: %d\n", e->type);
            break;
    }
}

void free_exp(exp e) {
    if (e == NULL) return;

    switch (e->type) {
        case DATATYPE:
        case INT:
            break;
        case ID:
            free(e->u.id);
            break;
        case VARIABLE:
            free(e->u.var.id);
            free(e->u.var.reg);
            break;
        case OPERATION:
            free_exp(e->u.operation.e1);
            free_exp(e->u.operation.e2);
            break;
        case AFFECT:
            free_exp(e->u.affect.e1);
            free_exp(e->u.affect.e2);
            break;
        case RESERVED:
            free(e->u.reserved.id);
            free_exp(e->u.reserved.e);
            break;
        case FUNCTION_DEF:
            free(e->u.func_def.id);
            for (int i = 0; i < e->u.func_def.params->nbExp; i++) {
                free_exp(e->u.func_def.params->expressions[i]);
            }
            free(e->u.func_def.params->expressions);
            free(e->u.func_def.params);
            for (int i = 0; i < e->u.func_def.program->nbExp; i++) {
                free_exp(e->u.func_def.program->expressions[i]);
            }
            free(e->u.func_def.program->expressions);
            free(e->u.func_def.program);
            break;
        case FUNCTION_CALL:
            free(e->u.func_call.id);
            for (int i = 0; i < e->u.func_call.params->nbExp; i++) {
                free_exp(e->u.func_call.params->expressions[i]);
            }
            free(e->u.func_call.params->expressions);
            free(e->u.func_call.params);
            break;
        default:
            printf("Not yet implemented exp type freeing: %d\n", e->type);
            break;
    }

    free(e);
}

void free_program(program p) {
    if (p == NULL) return; // Check for NULL pointer

    for (int i = 0; i < p->nbExp; i++) {
        free_exp(p->expressions[i]);
    }
    free(p->expressions); // Free the array of expressions
    free(p);
}

Error new_error()
{
    Error error;
    error.error_type = ERROR_NONE;
    error.warnings.messages = NULL;
    error.warnings.n = 0;
    error.errors.messages = NULL;
    error.errors.n = 0;
    return error;
}

void free_error(Error *error) {
    if (error == NULL) return; // Check for NULL pointer

    // Free warning messages
    for (int i = 0; i < error->warnings.n; i++) {
        free(error->warnings.messages[i]);
    }
    free(error->warnings.messages);

    // Free error messages
    for (int i = 0; i < error->errors.n; i++) {
        free(error->errors.messages[i]);
    }
    free(error->errors.messages);
}


void free_tokens(Tokens tokens)
{
    for (int i = 0; i < tokens.nbTokens; i++)
    {
        if(tokens.tokens[i].token != NULL) 
        {
            free(tokens.tokens[i].token);
            tokens.tokens[i].token = 0;
        }
    }
    // TODO : uncomment when tokens array will be alloced dynamically
    // free(tokens.tokens);
}

char* buffer_alloc(int size)
{
    char* buff = malloc(size*sizeof(char));
    if(buff == NULL) 
    {
        print_error("malloc error");
        exit(EXIT_FAILURE);
    }
    memset(buff, 0, size);
    return buff;
}

void concat_text(char** buffer, int* buffer_size, char* concat)
{
    int concat_len = strlen(concat);
    if(*buffer == NULL)
    {
        *buffer = malloc(concat_len + 1);
        *buffer_size = concat_len + 1;
        strcpy(*buffer, concat);
        return;
    }
    int buffer_len = strlen(*buffer);
    if(*buffer_size >= buffer_len + concat_len)
    {
        strcat(*buffer, concat);
        return;
    }
    *buffer = realloc(*buffer, buffer_len + concat_len + 1);
    strcat(*buffer, concat);
    *buffer_size = buffer_len + concat_len;
}