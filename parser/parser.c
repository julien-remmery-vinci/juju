#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "parser.h"
#include "../lexer/lexer.h"
#include "../utils/utils.h"

program new_program() {
    program p = malloc(sizeof(*p));
    if (p == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    p->expressions = NULL;
    p->nbExp = 0;
    return p;
}

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

enum dataType isDatatype(char* str)
{
    enum dataType type;
    if(strcmp(str, "int") == 0) {
        type = DATATYPE_INTEGER;
        return type;
    }
    type = DATATYPE_NONE;
    return type;
}

enum reserved isReserved(char* str)
{
    enum reserved r;
    if(strcmp(str, "return") == 0) {
        r = RETURN;
        return r;
    }
    r = NONE;
    return r;
}

enum operation isoperator(char* str)
{
    enum operation o = OPERATIONS_NONE;
    switch(str[0])
    {
        case('+'):
            return (o = OPERATIONS_ADD);
        case('-'):
            return (o = OPERATIONS_SUBSTRACT);
        case('*'):
            return (o = OPERATIONS_MULTIPLY);
        case('/'):
            return (o = OPERATIONS_DIVIDE);
        case('='):
            return (o = OPERATIONS_AFFECT);
        default:
            return o;
    }
}

exp new_exp()
{
    exp new = malloc(sizeof(*new));
    return new;
}

exp prev_exp(program p)
{
    if(p->nbExp == 0) return NULL;
    return p->expressions[p->nbExp -1];
}

void add_exp(program p, exp e, int offset)
{
    if(offset == 0)
    {
        if(p->expressions == NULL) {p->expressions = (exp*)malloc((p->nbExp + 1)*sizeof(exp));}
        else {
            p->expressions = (exp*)realloc(p->expressions, (p->nbExp + 1)*sizeof(exp));
        }
        if (p->expressions == NULL) {
            print_error("error while allocating memory");
            exit(EXIT_FAILURE);
        }
        p->expressions[p->nbExp] = e;
        p->nbExp++;
    }
    else
    {
        p->expressions[p->nbExp + offset] = e;
    }
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

    enum dataType type = isDatatype(token);
    if(type != DATATYPE_NONE)
    {
        next->type = DATATYPE;
        next->u.dataType = type;
        return next;
    }

    enum reserved reserved = isReserved(token);
    if(reserved != NONE)
    {
        // TODO : add reserved words handling
        next->type = RESERVED;
        next->u.reserved.reserved = RETURN;
        next->u.reserved.id = buffer_alloc(strlen(token) + 1);
        strncpy(next->u.reserved.id, token, strlen(token));
        return next;
    }

    enum operation operation = isoperator(token);
    if(operation != OPERATIONS_NONE)
    {
        if(operation == OPERATIONS_AFFECT)
        {
            next->type = AFFECT;
        }
        else
        {
            next->type = OPERATION;
            next->u.operation.operation = operation;
        }
        return next;
    }

    next->type = ID;
    next->u.id = buffer_alloc(strlen(token) + 1);
    strncpy(next->u.id, token, strlen(token));

    if((*i) - 1 < tokens.nbTokens)
    {
        char* prevToken = tokens.tokens[(*i) - 2].token;
        if(isDatatype(prevToken))
        {
            return next;
        }
    }

    if((*i) + 1 < tokens.nbTokens)
    {
        char* nextToken = tokens.tokens[(*i)].token;
        if(isopenparenthese(nextToken))
        {
            program program = new_program();
            exp call = parse_call(program, tokens, i, token);
            (*i)++;
            free_exp(next);
            return call;
        }
    }

    return next;
}

void handle_exp(program program, Tokens tokens, int* i, exp e)
{
    if(e->type == DATATYPE || e->type == INT)
    {
        add_exp(program, e, 0);
        return;
    }
    if(e->type == RESERVED)
    {
        if(e->u.reserved.reserved == RETURN)
        {
            exp next = next_exp(tokens, i);
            e->u.reserved.e = next;
            add_exp(program, e, 0);
            return;
        }
    }

    exp prev = prev_exp(program);
    if(e->type == OPERATION)
    {
        if(prev->type == AFFECT)
        {
            e->u.operation.e1 = prev->u.affect.e2;
            e->u.operation.e2 = next_exp(tokens, i);
            prev->u.affect.e2 = e;
        }
        else if(prev->type == RESERVED && prev->u.reserved.reserved == RETURN)
        {
            e->u.operation.e1 = prev->u.reserved.e;
            e->u.operation.e2 = next_exp(tokens, i);
            prev->u.reserved.e = e;
        }
        else
        {
            e->u.operation.e1 = prev;
            e->u.operation.e2 = next_exp(tokens, i);
            add_exp(program, e, -1);
        }
        return;
    }
    if(e->type == AFFECT)
    {
        e->u.affect.e1 = prev;
        e->u.affect.e2 = next_exp(tokens, i);
        add_exp(program, e, -1);
        return;
    }

    if(prev != NULL && e->type == ID && program->nbExp > 0 && prev->type == DATATYPE)
    {
        exp new = new_exp();
        new->type = VARIABLE;
        new->u.var.dataType = prev->u.dataType;
        new->u.var.id = buffer_alloc(strlen(e->u.id) + 1);
        strncpy(new->u.var.id, e->u.id, strlen(e->u.id));
        new->u.var.reg = NULL;
        free_exp(prev);
        free_exp(e);
        add_exp(program, new, -1);
        return;
    }
    add_exp(program, e, 0);
}

exp parse_call(program program, Tokens tokens, int* i, char* prevID)
{
    exp prev;
    exp call = new_exp();
    call->type = FUNCTION_CALL;
    call->u.func_call.id = buffer_alloc(strlen(prevID) + 1);
    strncpy(call->u.func_call.id, prevID, strlen(prevID));
    call->u.func_call.params->expressions = NULL;
    call->u.func_call.params->nbExp = 0;
    (*i)++;
    char* token = tokens.tokens[*i].token;
    while(!iscloseparenthese(token))
    {
        prev = prev_exp(call->u.func_call.params);
        if(isopenparenthese(token) && prev->type == ID)
        {
            exp new_call = parse_call(program, tokens, i, prev->u.id);
            add_exp(call->u.func_call.params, new_call, -1);
            (*i)++;
            token = tokens.tokens[*i].token;
            free_exp(prev);
            continue;
        }
        if(iscomma(token))
        {
            (*i)++;
            token = tokens.tokens[*i].token;
            continue;
        }
        exp next = next_exp(tokens, i);
        if(next == NULL) break;
        handle_exp(call->u.func_call.params, tokens, i, next);
        token = tokens.tokens[*i].token;
    }
    free_exp(prev);
    return call;
}

Error parse(Tokens tokens)
{
    Error error = new_error();
    program program = new_program();
    int i = 0;
    int max = tokens.nbTokens;
    exp e, prev;
    while(i < max)
    {
        char* token = tokens.tokens[i].token;
        if(isopenparenthese(token))
        {
            i++;
            token = tokens.tokens[i].token;
            exp function_exp = new_exp();
            prev = prev_exp(program);
            if(prev->type == VARIABLE)
            {
                function_exp->type = FUNCTION_DEF;
                function_exp->u.func_def.returnType = prev->u.var.dataType;
                function_exp->u.func_def.id = buffer_alloc(strlen(prev->u.var.id) + 1);
                strncpy(function_exp->u.func_def.id, prev->u.var.id, strlen(prev->u.var.id));
                function_exp->u.func_def.program = new_program();
                function_exp->u.func_def.params = new_program();
                function_exp->u.func_def.params->expressions = NULL;
                function_exp->u.func_def.params->nbExp = 0;
                free_exp(prev);
                while(!iscloseparenthese(token))
                {
                    if(iscomma(token))
                    {
                        i++;
                        token = tokens.tokens[i].token;
                        continue;
                    }
                    // handle function params -> store params
                    exp e = next_exp(tokens, &i);
                    if(e == NULL) break;
                    handle_exp(function_exp->u.func_def.params, tokens, &i, e);
                    token = tokens.tokens[i].token;
                }
                i++;
                token = tokens.tokens[i].token;
                if(!isopenbrace(token)) 
                {
                    print_error("expected function implementation after definition");
                    exit(EXIT_FAILURE);    
                }
                i++;
                // function impl
                while(!isclosebrace(token))
                {
                    token = tokens.tokens[i].token;
                    prev = prev_exp(function_exp->u.func_def.program);
                    
                    if (issemicolon(token))
                    {
                        i++;
                        token = tokens.tokens[i].token;
                        continue;
                    }
                    // handle affect before
                    if(isopenparenthese(token) && prev->type == ID)
                    {
                        exp call = parse_call(program,tokens, &i, prev->u.id);
                        add_exp(function_exp->u.func_def.program, call, -1);
                        i++;
                        token = tokens.tokens[i].token;
                        free_exp(prev);
                        continue;
                    }
                    if(prev != NULL && prev->type == RESERVED)
                    {
                        if(isopenparenthese(token) && prev->u.reserved.e->type == ID)
                        {
                            exp call = parse_call(program,tokens, &i, prev->u.reserved.e->u.id);
                            prev->u.reserved.e = call;
                            i++;
                            token = tokens.tokens[i].token;
                            continue;
                        }
                    }

                    e = next_exp(tokens, &i);
                    if(e == NULL) break;
                    handle_exp(function_exp->u.func_def.program, tokens, &i, e);
                }
                i++;
            }

            add_exp(program, function_exp, -1);
        }
        else
        {
            e = next_exp(tokens, &i);
            if(e == NULL) break;
            handle_exp(program, tokens, &i, e);
        }
    }
    error.program = program;
    return error;
}