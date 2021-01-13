#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "defines.h"

void print_memory_error(char* str) {
    printf("%s\n", str);
}

void print_singleton_warning(char* var_name) {
    printf("Singleton variable: %s\n", var_name);
}

void print_parser_error(char* str, int pos, const char* message) {
    int i;
    printf("Syntax error at position %d\n%s\n", pos, str);

    for (i = 0; i < (pos) && i < (int)strlen(str); i++) {
        printf("-");
    }

    printf("^\n");
    printf("%s\n", message);
    exit(PARSER_ERROR);
}

void print_unbalanced_parentheses_error() {
    printf("Unbalanced parentheses\n");
    exit(UNBALANCED_PARENTHESES_ERROR);
}

void print_missing_quote() {
    printf("Missing quote\n");
    exit(MISSING_QUOTE);
}

void print_realloc_error(const char* str) {
    printf("%s", str);
    exit(REALLOC_ERROR_EXIT);
}
