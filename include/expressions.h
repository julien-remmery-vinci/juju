#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

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
    OPERATION,
    AFFECT,
    DATATYPE,
    VARIABLE,
    FUNCTION_DEF,
    FUNCTION_CALL,
    // handle reserved words ex: return, if, for, while, do..while, ...
    RESERVED
};

enum operation {
    OPERATIONS_NONE,
    OPERATIONS_ADD,
    OPERATIONS_SUBSTRACT,
    OPERATIONS_MULTIPLY,
    OPERATIONS_DIVIDE,
    OPERATIONS_AFFECT
    // add missing operators -> implement too
};

#endif