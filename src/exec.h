#pragma once
#include "parser.h"

FILE* outstream;

typedef struct line {
	int number;
	int len;
	char* content;
} line;

double exec_command(FILE *fp, term_list* tl, reference_list* rl);