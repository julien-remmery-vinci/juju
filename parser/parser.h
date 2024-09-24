#ifndef _PARSER_H
#define _PARSER_H

#include "../include/tokens.h"
#include "../include/error.h"
#include "../include/program.h"

Error parse(Tokens tokens);

#endif