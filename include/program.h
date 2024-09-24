#ifndef PROGRAM_H
#define PROGRAM_H

#include "expressions.h"

typedef struct{
    exp* expressions;
    int nbExp;
} program;

struct exp {
    enum expType type;
    union {
        int i;
        char* id;
        enum dataType dataType;
        struct {enum reserved reserved; char* id; exp e;} reserved;
        struct {enum dataType dataType; char* id; char* reg;} var;
        struct {exp e1; exp e2;} add;
        struct {exp e1; exp e2;} affect;
        struct {program program; program params; enum dataType returnType; char* id;} func_def;
        struct {program params; char* id;} func_call;
    } u;
};

#endif