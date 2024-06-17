#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "../lexer/lexer.h"
#include "../utils/utils.h"

typedef struct exp* exp;

enum dataType {
    DATATYPE_NONE,
    DATATYPE_INTEGER
};

enum expType {
    INT,
    ID,
    ADD,
    AFFECT,
    DATATYPE,
    VARIABLE,
    FUNCTION_DEF,
    FUNCTION_CALL
    // handle reserved words ex: return, if, for, while, do..while, ...
};

enum operations {
    OPERATIONS_NONE,
    OPERATIONS_ADD,
    OPERATIONS_AFFECT
    // add missing operators -> implement too
};

typedef struct {
    exp* expressions;
    int nbExp;
} program;

struct exp {
    enum expType type;
    union {
        int i;
        char* id;
        enum dataType dataType;
        struct {enum dataType dataType; char* id;} var;
        struct {exp e1; exp e2;} add;
        struct {exp e1; exp e2;} affect;
        struct {program program; enum dataType returnType; char* id; exp* params;} func_def;
        struct {char* id; char** params;} func_call;
    } u;
};

int numNodes(exp e)
{
    switch(e->type)
    {
        case INT: return 1;
        case ID: return 1;
        case ADD: return 1 + numNodes(e->u.add.e1) + numNodes(e->u.add.e2);
        default: perror("Impossible");
    }
}

enum dataType getDatatype(char* str)
{
    enum dataType type;
    if(strcmp(str, "int") == 0) {
        type = DATATYPE_INTEGER;
        return type;
    }
    type = DATATYPE_NONE;
    return type;
}

bool isnum(char* str)
{
    while(*str != '\0')
    {
        if(!isdigit(*str)) {return false;}
        str++;
    }
    return true;
}

enum operations isoperator(char* str)
{
    enum operations o = OPERATIONS_NONE;
    switch(str[0])
    {
        case('+'):
            return (o = OPERATIONS_ADD);
        case('='):
            return (o = OPERATIONS_AFFECT);
        default:
            return o;
    }
}

exp new_exp()
{
    return malloc(sizeof(exp));
}

exp prev_exp(program p)
{
    if(p.nbExp == 0) return NULL;
    return p.expressions[p.nbExp -1];
}

exp next_exp(Tokens tokens, int* i)
{
    if((*i) + 1 >= tokens.nbTokens) return NULL;
    exp next = new_exp();
    char* token = tokens.tokens[(*i)++].token;
    if(isnum(token))
    {
        next->type = INT;
        next->u.i = atoi(token);
        return next;
    }

    enum dataType type = getDatatype(token);
    if(type != DATATYPE_NONE)
    {
        next->type = DATATYPE;
        next->u.dataType = type;
        return next;
    }

    enum operations operation = isoperator(token);
    if(operation != OPERATIONS_NONE)
    {
        if(operation == OPERATIONS_ADD)
        {
            next->type = ADD;
        }
        else if(operation == OPERATIONS_AFFECT)
        {
            next->type = AFFECT;
        }
        return next;
    }

    next->type = ID;
    next->u.id = token;
    return next;
}

void add_exp(program* p, exp e, int offset)
{
    if(offset == 0)
    {
        if(p->expressions == NULL) {(*p).expressions = (exp*)malloc((p->nbExp + 1)*sizeof(exp));}
        else {(*p).expressions = (exp*)realloc(p->expressions, (p->nbExp + 1)*sizeof(exp));}
        p->expressions[p->nbExp++] = e;
    }
    else
    {
        p->expressions[p->nbExp + offset] = e;
    }
}

void print_exp(exp e) 
{
    switch(e->type)
    {
        case(VARIABLE):
            switch (e->u.var.dataType)
            {
                case DATATYPE_INTEGER:
                    printf("int %s", e->u.var.id);
                    break;
            }
            break;
        case(AFFECT):
            print_exp(e->u.affect.e1);
            printf(" = ");
            print_exp(e->u.affect.e2);
            break;
        case(ADD):
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
        case(FUNCTION_DEF):
            printf("%d\n", e->u.func_def.returnType);
            printf("%d %s () {\n", e->u.func_def.returnType, e->u.func_def.id);
            for (int i = 0; i < e->u.func_def.program.nbExp; i++)
            {
                print_exp(e->u.func_def.program.expressions[i]);
                printf("\n");
            }
            printf("\n}");
            break;
        default:
            printf("not yet implemented exp type printing: %d\n", e->type);
            break;
    }
}

void print_program(program program)
{
    for (int i = 0; i < program.nbExp; i++)
    {
        print_exp(program.expressions[i]);
        printf("\n");
    }
}

program parse(Tokens tokens)
{
    program program;
    program.nbExp = 0;
    int i = 0;
    int max = tokens.nbTokens;
    exp e, prev;
    while(i < max)
    {
        char* token = tokens.tokens[i].token;
        // printf("%d: %s\n", i, token);
        if(isopenparenthese(token))
        {
            exp function_exp = new_exp();
            prev = prev_exp(program);
            if(prev->type == VARIABLE)
            {
                function_exp->type = FUNCTION_DEF;
                // function_exp->u.func_def.returnType = prev->u.var.dataType;
                // function_exp->u.func_def.id = prev->u.var.id;
                while(!iscloseparenthese(token))
                {
                    // handle function params -> store params
                    i++;
                    token = tokens.tokens[i].token;
                }
                i++;
                token = tokens.tokens[i].token;
                if(!isopenbrace(token)) {print_error("expected function implementation after definition");}
                i++;
                // function impl
                while(!isclosebrace(token))
                {
                    token = tokens.tokens[i].token;
                    // printf("token: %s\n", token);
                    
                    if (issemicolon(token))
                    {
                        i++;
                        continue;
                    }

                    e = next_exp(tokens, &i);
                    if(e == NULL) break;

                    if(e->type == DATATYPE || e->type == INT)
                    {
                        add_exp(&function_exp->u.func_def.program, e, 0);
                        continue;
                    }

                    if(e->type == ADD)
                    {
                        prev = prev_exp(function_exp->u.func_def.program);
                        // checkNull(prev, "error prev exp is null");
                        //  diff ne montre aucune différence sur les 2 output ????
                        if(prev->type == AFFECT)
                        {
                            e->u.add.e1 = prev->u.affect.e2;
                            e->u.add.e2 = next_exp(tokens, &i);
                            prev->u.affect.e2 = e;
                        }
                        else
                        {
                            e->u.add.e1 = prev;
                            e->u.add.e2 = next_exp(tokens, &i);
                            add_exp(&function_exp->u.func_def.program, e, -1);
                        }
                        continue;
                    }

                    if(e->type == AFFECT)
                    {
                        prev = prev_exp(function_exp->u.func_def.program);
                        // checkNull(prev, "error prev exp is null");
                        if(prev->type == VARIABLE)
                        {
                            exp id = new_exp();
                            id->type = ID;
                            id->u.id = prev->u.var.id;
                            e->u.affect.e1 = id;
                            e->u.affect.e2 = next_exp(tokens, &i);
                            add_exp(&function_exp->u.func_def.program, e, 0);
                        }
                        else
                        {
                            e->u.affect.e1 = prev;
                            e->u.affect.e2 = next_exp(tokens, &i);
                            add_exp(&function_exp->u.func_def.program, e, -1);
                        }
                        continue;
                    }

                    prev = prev_exp(function_exp->u.func_def.program);
                    if(prev != NULL && e->type == ID && function_exp->u.func_def.program.nbExp > 0 && prev->type == DATATYPE)
                    {
                        exp new = new_exp();
                        new->type = VARIABLE;
                        new->u.var.dataType = prev->u.dataType;
                        new->u.var.id = e->u.id;
                        add_exp(&function_exp->u.func_def.program, new, -1);
                        continue;
                    }
                }
                // i++;
            }
            else if(prev->type == ID)
            {
                function_exp->type = FUNCTION_CALL;
                // handle function call -> params values passed
            }
            else {}

            add_exp(&program, function_exp, -1);

            continue;
        }

        e = next_exp(tokens, &i);
        if(e == NULL) break;

        if(e->type == DATATYPE || e->type == INT)
        {
            add_exp(&program, e, 0);
            continue;
        }

        prev = prev_exp(program);

        if(prev != NULL && e->type == ID && program.nbExp > 0 && prev->type == DATATYPE)
        {
            exp new = new_exp();
            new->type = VARIABLE;
            new->u.var.dataType = prev->u.dataType;
            new->u.var.id = e->u.id;
            add_exp(&program, new, -1);
            continue;
        }

        add_exp(&program, e, 0);
    }
    return program;
}

int main(int argc, char *argv[])
{
    if(argc < 2) 
    {print_usage(argv);}

    FILE* source = fopen(argv[1], "r");
    checkNull(source, "fopen error");
    Tokens tokens = lex(source);
    printf("Lexing done\n");
    fclose(source);
    // for (int i = 0; i < tokens.nbTokens; i++)
    // {printf("%s\n", tokens.tokens[i].token);}
    // printf("nb tokens : %d\n", tokens.nbTokens);

    if(tokens.nbTokens == 0)
    {
        printf("Invalid source.\n");
        exit(EXIT_FAILURE);
    }

    program program = parse(tokens);
    printf("Parsing done\n");
    print_program(program);
}