#pragma once
#include "parser.h"

FILE* outstream;

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

double exec_command(FILE *fp, term_list* tl, reference_list* rl);
char* get_line(FILE* fp, int* len_l);