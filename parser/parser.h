#ifndef _PARSER_H
#define _PARSER_H

typedef struct exp *exp;

enum expType {
    INT,
    ID,
    ADD,
    RETURN
};

struct exp {
    enum expType type;
    union {
        int i;
        char* id;
        struct {exp e1; exp e2;} add;
    } u;
};

void parse();

#endif