#pragma once
#include "parser.h"

typedef enum {COPY, REFERENCE} cr;

typedef struct line {
	int number;
	size_t len;
	char* content;
} line;

/*
char *built_in[] = {
	"line","file","length","write","even","odd","mod","gt","lt","between","number","words","occurrences","startswith","endswith","reverse"
};
*/

double exec_command(FILE *fp, term_list* tl, reference_list* rl, FILE *outstream);
char* get_line(FILE* fp, size_t* len_l);