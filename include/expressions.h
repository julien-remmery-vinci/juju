#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

/*
    Definition of exp type
*/
typedef struct exp* exp;

/*
    Enum data types
*/
enum dataType {
    DATATYPE_NONE,
    DATATYPE_INTEGER
};

/*
    Enum of reserved words
*/
enum reserved {
    RESERVED_NONE,
    RETURN,
    IF,
    WHILE,
    DO_WHILE,
    FOR
};

/*
    Enum of expressions types
*/
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

/*
    Enum of operations types
*/
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