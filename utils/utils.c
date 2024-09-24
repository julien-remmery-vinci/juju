#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
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
    for (int i = 0; i < program.nbExp; i++)
    {
        print_exp(program.expressions[i]);
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
        case(ADD):
            // printf("add: ");
            print_exp(e->u.add.e1);
            printf(" + ");
            print_exp(e->u.add.e2);
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
            for (int i = 0; i < e->u.func_def.params.nbExp; i++)
            {
                print_exp(e->u.func_def.params.expressions[i]);
                if(i + 1 < e->u.func_def.params.nbExp) {printf(", ");}
            }
            printf(") {\n");
            for (int i = 0; i < e->u.func_def.program.nbExp; i++)
            {
                printf("    ");
                print_exp(e->u.func_def.program.expressions[i]);
                printf(";\n");
            }
            printf("}");
            break;
        case(FUNCTION_CALL):
            printf("%s(", e->u.func_call.id);
            for (int i = 0; i < e->u.func_call.params.nbExp; i++)
            {
                print_exp(e->u.func_call.params.expressions[i]);
                if(i + 1 < e->u.func_call.params.nbExp) {printf(", ");}
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