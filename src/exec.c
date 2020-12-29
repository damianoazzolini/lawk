#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#include "defines.h"
#include "exec.h"
#include "parser.h"

// line(I,L),mod(I,3,0), write(L)

// really primitive implementation
char* get_line(FILE* fp, int *len_l) {
	char* l = NULL;
	char ch;
	int len = 0;

	ch = fgetc(fp);
	
	if (ch == EOF || feof(fp)) {
		return NULL;
	}

	while (ch != '\n') {
		len++;
		l = realloc(l, len);
		l[len - 1] = ch;
		ch = fgetc(fp);
	}
	l = realloc(l, len + 1);
	l[len] = '\0';

	*len_l = len;

	return l;	
}

int associate_int(reference_list *rl, char* name, int value) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (strcmp(rl->list[i].name,name) == 0) {
			if (rl->list[i].t == INT || rl->list[i].t == UNSET) {
				rl->list[i].cont.int_val = value;
				rl->list[i].t = INT;
				return SUCCESS;
			}
			else {
				printf("Already set for: %s\n", rl->list[i].name);
				return FAILURE;
			}
		}
	}

	printf("Term not found\n");
	return FAILURE;
}

int associate_str(reference_list* rl, char* name, char *value) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (strcmp(rl->list[i].name, name) == 0) {
			if (rl->list[i].t == LIST || rl->list[i].t == UNSET) {
				rl->list[i].cont.list = value;
				rl->list[i].t = LIST;
				return SUCCESS;
			}
			else {
				printf("Already set for: %s\n", rl->list[i].name);
				return FAILURE;
			}
		}
	}

	printf("Term not found\n");
	return FAILURE;
}

// super mega long function
int apply_rule(line *l, term_t* t, char* line_name, char* number_name, reference_list* rl) {
	int i;

	if (strcmp(t->functor, "length") == 0) {
		if (isdigit(t->argument_list[1][0])) {
			// lenght(L,2)
			if (atoi(t->argument_list[1]) == l->len) {
				return SUCCESS;
			}
			else {
				return FAILURE;
			}
		}
		else {
			associate_int(rl, t->argument_list[1], l->len);
			return SUCCESS;
		}
	}
	else if (strcmp(t->functor, "write") == 0) {
		// todo: allow print in the specified format
		// fprintf(outstream,"Unable to print %s\n", t->argument_list[0]);
		// print_reference_list(rl);
		for (i = 0; i < rl->n_elements; i++) {
			if (strcmp(rl->list[i].name, t->argument_list[0]) == 0) {
				if (rl->list[i].t == INT) {
					printf("%d\n", rl->list[i].cont.int_val);
					return SUCCESS;
				}
				else if (rl->list[i].t == FLOAT) {
					printf("%lf\n", rl->list[i].cont.double_val);
					return SUCCESS;
				}
				else if (rl->list[i].t == LIST) {
					printf("%s\n", rl->list[i].cont.list);
					return SUCCESS;
				}
			}
		}
		
	}
	else if (strcmp(t->functor, "even") == 0 || strcmp(t->functor, "odd") == 0) {
		for (i = 0; i < rl->n_elements; i++) {
			if (strcmp(rl->list[i].name, t->argument_list[0]) == 0) {
				if (rl->list[i].t == INT) {
					if ((rl->list[i].cont.int_val % 2 == 0 && strcmp(t->functor, "even") == 0)  || (rl->list[i].cont.int_val % 2 != 0 && strcmp(t->functor, "odd") == 0)) {
						// printf("%d\n", rl->list[i].cont.int_val);
						return SUCCESS;
					}
					else {
						return FAILURE;
					}
				}
				else {
					printf("Unable to check even or odd for not int\n");
					return FAILURE;
				}
			}
		}
	}
	else if (strcmp(t->functor, "mod") == 0) {
		// mod(I,3,0): variable number number
	}
	else if (strcmp(t->functor, "gt") == 0) {

	}
}

double exec_command(FILE *fp, term_list* tl, reference_list* rl) {
	// loop trough term list
	int i;
	int line_extracted = 0;
	line l;
	int n_line_processed = 1;
	clock_t begin = clock();
	clock_t end;
	int index = 1;
	int result_int = -1;
	int num;

	if (strcmp("line", tl->list[0].functor) == 0) {
		assert(tl->list[0].arity == 1 || tl->list[0].arity == 2);
		assert(tl->list[0].argument_list[0] != NULL);
		assert(tl->list[0].argument_list[1] != NULL);

		if (isupper(tl->list[0].argument_list[0][0])) {
			l.content = get_line(fp,&l.len);
			l.number = n_line_processed;
			if (tl->list[0].arity == 2) {
				// line(I,L)
				associate_int(rl, tl->list[0].argument_list[0], l.number);
				associate_str(rl, tl->list[0].argument_list[1], l.content);
			}
			else {
				// line(L)
				associate_str(rl, tl->list[0].argument_list[0], l.content);
			}

			while (l.content != NULL) {
				// printf("%s\n", l.content);
				for (i = 1; i < tl->n_elements; i++) {
					if (apply_rule(&l, &tl->list[i], tl->list[0].argument_list[0], tl->list[0].argument_list[1], rl) < 0) {
						break;
					}
				}

				result_int = -1;
				n_line_processed++;
				free(l.content);

				l.content = get_line(fp,&l.len);
				l.number = n_line_processed;
				if (tl->list[0].arity == 2) {
					// line(I,L)
					associate_int(rl, tl->list[0].argument_list[0], l.number);
					associate_str(rl, tl->list[0].argument_list[1], l.content);
				}
				else {
					// line(L)
					associate_str(rl, tl->list[0].argument_list[0], l.content);
				}
			}
		}
		else {
			// here is line(1,N), so the first is number
			num = atoi(tl->list[0].argument_list[0]);
			if (num > 0) {
				l.content = get_line(fp, &l.len);
				l.number = n_line_processed;
				associate_str(rl, tl->list[0].argument_list[1], l.content);
				while (l.content != NULL && l.number != num) {
					n_line_processed++;
					free(l.content);

					l.content = get_line(fp, &l.len);
					l.number = n_line_processed;
					associate_str(rl, tl->list[0].argument_list[1], l.content);
				}
				if (l.content == NULL) {
					fprintf(outstream, "Line %d not found\n", num);
					exit(LINE_NOT_FOUND);
				}
				
				for (i = 1; i < tl->n_elements; i++) {
					if (apply_rule(&l, &tl->list[i], tl->list[0].argument_list[0], tl->list[0].argument_list[1], rl) < 0) {
						break;
					}
				}

			}
			else {
				fprintf(outstream, "Line should be a number %s\n", tl->list[0].argument_list[0]);
				exit(CONVERSION_STRING_INT_FAILURE);
			}
		}
		// TODO manage line(I,abc)

	}
	
	end = clock();

	// return execution time
	return (double)(end - begin) / CLOCKS_PER_SEC;
}