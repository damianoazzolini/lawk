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

void print_parser_error(char* str, int pos, char* message) {
    int i;
    printf("Syntax error %d\n%s\n", pos, str);

    for (i = 0; i < (pos) && i < (int)strlen(str); i++) {
        printf("-");
    }

    printf("^\n");
    printf("%s\n", message);
    exit(PARSER_ERROR);
}

void print_unbalanced_parentheses_error(char* str, char* filename, int line) {
    int i;
    int n = 0;
    // a(a,d(e))
    // -1---2-21
    printf("Unbalanced parentheses %s: %d\n%s\n", filename, line, str);

    for (i = 0; i < (int)strlen(str); i++) {
        if (str[i] == '(') {
            printf("%d", n);
            n++;
        }
        else if (str[i] == ')') {
            n--;
            printf("%d", n);
        }
        else {
            printf("-");
        }
    }

    exit(UNBALANCED_PARENTHESES_ERROR);
}

void print_realloc_error(const char* str) {
    printf("%s", str);
    exit(REALLOC_ERROR_EXIT);
}
