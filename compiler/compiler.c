#include <stdlib.h>
#include <stdio.h>
#include "../lexer/lexer.h"
#include "../parser/parser.h"
#include "../utils/utils.h"
#include "../codegen/codegen.h"

int main(int argc, char *argv[])
{
    // Check for source file argument
    // if (argc < 2) 
    // {
    //     print_usage(argv);
    //     exit(EXIT_FAILURE);
    // }

    // Open the source file
    FILE* source = fopen("test.ju", "r");
    checkNull(source, "fopen error");

    // Perform lexical analysis
    Error lex_result = lex(source);
    fclose(source); // Close file immediately after lexing

    // Check for errors in lexing
    if (lex_result.error_type != ERROR_NONE)
    {
        print_error("error while lexing file");
        free_error(&lex_result); // Free any allocated error resources
        exit(EXIT_FAILURE);
    }
    printf("Lexing done without errors\n");

    // Validate the tokens
    checkValidSource(lex_result.tokens);
    printf("Source is valid\n");

    // Perform parsing
    Error parsing_result = parse(lex_result.tokens);
    free_tokens(lex_result.tokens); // Free tokens after parsing

    // Check for errors in parsing
    if (parsing_result.error_type != ERROR_NONE)
    {
        print_error("error while parsing file");
        free_error(&parsing_result); // Free any allocated error resources
        exit(EXIT_FAILURE);
    }
    printf("Parsing done without errors\n");

    // Print the parsed program
    print_program(parsing_result.program);

    // Generate code from the parsed program
    codegen(parsing_result.program);
    printf("Code generation done without errors\n");

    // Free allocated resources
    free_program(parsing_result.program);
    free_error(&parsing_result); // Free parsing result error resources

    return 0; // Return success
}
