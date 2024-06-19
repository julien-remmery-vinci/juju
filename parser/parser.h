#ifndef _PARSER_H
#define _PARSER_H

typedef struct exp* exp;

enum dataType {
    DATATYPE_NONE,
    DATATYPE_INTEGER
};

enum reserved {
    NONE,
    RETURN,
    IF,
    WHILE,
    DO_WHILE,
    FOR
};

enum expType {
    INT,
    ID,
    ADD,
    AFFECT,
    DATATYPE,
    VARIABLE,
    FUNCTION_DEF,
    FUNCTION_CALL,
    // handle reserved words ex: return, if, for, while, do..while, ...
    RESERVED
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
        struct {enum reserved reserved; char* id; exp e;} reserved;
        struct {enum dataType dataType; char* id;} var;
        struct {exp e1; exp e2;} add;
        struct {exp e1; exp e2;} affect;
        struct {program program; program params; enum dataType returnType; char* id;} func_def;
        struct {program params; char* id;} func_call;
    } u;
};

int numNodes(exp e);

#endif