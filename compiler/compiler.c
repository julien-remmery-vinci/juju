#include <stdlib.h>
#include <stdio.h>
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../utils/utils.h"
#include "../codegen/codegen.h"

int main(int argc, char *argv[])
{
    if(argc < 2) 
    {
        print_usage(argv);
        exit(EXIT_FAILURE);
    }

    FILE* source = fopen(argv[1], "r");
    checkNull(source, "fopen error");

    Error lex_result = lex(source);
    if(lex_result.error_type != ERROR_NONE)
    {
        print_error("error while lexing file");
        exit(EXIT_FAILURE);
    }
    printf("Lexing done without errors\n");
    fclose(source);

    checkValidSource(lex_result.tokens);
    printf("Source is valid\n");

    Error parsing_result = parse(lex_result.tokens);
    if(parsing_result.error_type != ERROR_NONE)
    {
        print_error("error while parsing file");
        exit(EXIT_FAILURE);
    }
    printf("Parsing done without errors\n");
    print_program(parsing_result.program);

    char* codegen_result = codegen(parsing_result.program);
    FILE* output = fopen("out.s", "w");
    checkNull(output, "fopen error");
    fprintf(output, "%s", codegen_result);
    fclose(output);
    printf("Code generation done without errors\n");
}