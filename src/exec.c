#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>

#include "defines.h"
#include "exec.h"
#include "parser.h"

/* TODO:


*/
// really primitive implementation
char* get_line(FILE* fp, int *len_l) {
	char* l = NULL;
	char ch;
	int len = 0;

	ch = fgetc(fp);
	
	if (ch == EOF || feof(fp)) {
		return NULL;
	}

	while (ch != '\n' && ch != EOF  && !feof(fp)) {
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

// You must free the result if result is non-NULL.
// here to make more readable the mega if else in the other function
// https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
/*
char* str_replace(char* source, char* find, char* replace) {
	char* result; // the return string
	char* ins;    // the next insert point
	char* tmp;    // varies
	int len_rep;  // length of rep (the string to remove)
	int len_with; // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep
	int count;    // number of replacements

	// sanity checks and initialization
	if (source == NULL || replace == NULL || find == NULL) {
		return NULL;
	}
	len_rep = strlen(find);
	if (len_rep == 0)
		return NULL; // empty rep causes infinite loop during count
	len_with = strlen(replace);

	// count the number of replacements needed
	ins = source;
	for (count = 0; tmp = strstr(ins, replace); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = malloc(strlen(source) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(source, find);
		len_front = ins - source;
		tmp = strncpy(tmp, source, len_front) + len_front;
		tmp = strcpy(tmp, replace) + len_with;
		source += len_front + len_rep; // move to next "end of rep"
	}
	// strcpy(tmp, source);
	snprintf(tmp, strlen(source) + (len_with - len_rep) * count + 1, "%s", source);
	return result;
}
*/

// TODO: it requires also the modification of the parser, to read a float
/*
int associate_float() {

}
*/

int associate_int(reference_list *rl, char* name, int value) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (strcmp(rl->list[i].name,name) == 0) {
			if (rl->list[i].t == INT || rl->list[i].t == UNSET) {
				rl->list[i].cont.int_val = value;
				rl->list[i].t = INT;
				rl->list[i].to_free = 0;
				return SUCCESS;
			}
			else {
				printf("Already set for: %s\n", rl->list[i].name);
				return FAILURE;
			}
		}
	}

	printf("Term %s not found %d\n",name,value);
	return FAILURE;
}


// TODO: merge with the next function
int associate_str_reference(reference_list* rl, char* name, char *value) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (strcmp(rl->list[i].name, name) == 0) {
			if (rl->list[i].t == LIST || rl->list[i].t == UNSET) {
				rl->list[i].cont.list = value;
				rl->list[i].t = LIST;
				rl->list[i].to_free = 0;
				return SUCCESS;
			}
			else {
				printf("Already set for: %s\n", rl->list[i].name);
				return FAILURE;
			}
		}
	}

	printf("Term %s not found - %s\n",name,value);
	return FAILURE;
}

int associate_str_copy(reference_list* rl, char* name, char* value) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (strcmp(rl->list[i].name, name) == 0) {
			if (rl->list[i].t == LIST || rl->list[i].t == UNSET) {
				rl->list[i].cont.list = malloc(strlen(value) + 2);
				snprintf(rl->list[i].cont.list, strlen(value) + 1, "%s", value);
				rl->list[i].to_free = 1;
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

// used to free the memory allocated in, for example
// reverse, nth ...
void free_ref_t_to_free(reference_list* rl) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (rl->list[i].to_free == 1 && rl->list[i].t == LIST) {
			free(rl->list[i].cont.list);
			rl->list[i].to_free = 0;
		}
	}
}

int find_matching_index(reference_list* rl, char* name) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (strcmp(rl->list[i].name, name) == 0) {
			return i;
		}
	}

	printf("%s not found\n", name);
	exit(INDEX_NOT_FOUND);
}

// super mega long function
int apply_rule(line *l, term_t* t, reference_list* rl) {
	int i, j;
	int count = 0;
	char* tmp = NULL;
	char ch;
	
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
		// TODO: allow print in the specified format
		// fprintf(outstream,"Unable to print %s\n", t->argument_list[0]);
		// print_reference_list(rl);
		for (j = 0; j < t->arity; j++) {
			if (isupper(t->argument_list[j][0])) {
				i = find_matching_index(rl, t->argument_list[j]);
				if (rl->list[i].t == INT) {
					printf("%d", rl->list[i].cont.int_val);
				}
				else if (rl->list[i].t == FLOAT) {
					printf("%lf", rl->list[i].cont.double_val);
				}
				else if (rl->list[i].t == LIST) {
					printf("%s", rl->list[i].cont.list);
				}
			}
			else {
				printf("%s",t->argument_list[j]);
			}
		}
		printf("\n");
		return SUCCESS;
	}
	else if (strcmp(t->functor, "even") == 0 || strcmp(t->functor, "odd") == 0 || strcmp(t->functor, "mod") == 0) {
		i = find_matching_index(rl, t->argument_list[0]);
		if (rl->list[i].t == INT) {
			if (
				(rl->list[i].cont.int_val % 2 == 0 && strcmp(t->functor, "even") == 0)  || 
				(rl->list[i].cont.int_val % 2 != 0 && strcmp(t->functor, "odd") == 0)
					||
				(rl->list[i].cont.int_val % atoi(t->argument_list[1]) == atoi(t->argument_list[2]) && strcmp(t->functor, "mod") == 0)
					) {
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
	else if (strcmp(t->functor, "gt") == 0 || strcmp(t->functor, "lt") == 0 || strcmp(t->functor, "between") == 0) {
		i = find_matching_index(rl, t->argument_list[0]);
		if (rl->list[i].t == INT) {
			if (
				(rl->list[i].cont.int_val < atoi(t->argument_list[1]) && strcmp(t->functor, "lt") == 0) ||
				(rl->list[i].cont.int_val > atoi(t->argument_list[1]) && strcmp(t->functor, "gt") == 0) ||
				(strcmp(t->functor, "between") == 0) && (rl->list[i].cont.int_val > atoi(t->argument_list[1])) && (rl->list[i].cont.int_val < atoi(t->argument_list[2]))
				) {
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
	else if (strcmp(t->functor, "member") == 0) {
		// member(L,N): loop trough all the char of L to find something equal to N
		// member(L,"abc"): loop trough all the char of L to find something equal to N
		// member(L,1): same as above
		// also need to associate N to the value, associate_str()
		// TODO: implement using strstr
		// credo che devo implementare una funzione apposta per member, nel ciclo più esterno
	}
	else if (strcmp(t->functor, "number") == 0) {
		// number(N): check if the char is number
		// TODO: check
		i = find_matching_index(rl, t->argument_list[0]);
		if (rl->list[i].t == INT && rl->list[i].cont.int_val) {
			return SUCCESS;
		}
		else {
			return FAILURE;
		}
	}
	else if (strcmp(t->functor, "words") == 0) {
		// words(Line,NWords): NWords is the number of words of the line
		i = find_matching_index(rl, t->argument_list[0]);

		if (t->arity == 2) {
			ch = ' ';
		}
		else {
			ch = t->argument_list[1][0];
		}

		for (j = 0; j < strlen(rl->list[i].cont.list); j++) {
			if (rl->list[i].cont.list[j] == ch) {
				if (j != strlen(rl->list[i].cont.list) - 1) {
					count++;
				}
			}
			while (rl->list[i].cont.list[j] == ch && j < strlen(rl->list[i].cont.list)) {
				j++;
			}
		}

		count++;

		if (t->arity == 2) {
			associate_int(rl, t->argument_list[1], count);
		}
		else {
			associate_int(rl, t->argument_list[2], count);
		}

		return SUCCESS;		
	}
	else if (strcmp(t->functor, "occurrences") == 0) {
		// occurrences(Line,CharOrString,NOcc)
		i = find_matching_index(rl, t->argument_list[0]);

		if (strlen(t->argument_list[1]) == 1) {
			// single char
			for (j = 0; j < strlen(rl->list[i].cont.list); j++) {
				if (rl->list[i].cont.list[j] == t->argument_list[1][0]) {
					count++;
				}
			}
		}
		else {
			// string
			tmp = rl->list[i].cont.list;
			while (tmp = strstr(tmp, t->argument_list[1])) {
				count++;
				tmp++;
			}			
		}

		associate_int(rl, t->argument_list[2], count);
		return SUCCESS;
	}
	else if (strcmp(t->functor, "startswith") == 0) {
		i = find_matching_index(rl, t->argument_list[0]);

		if (strncmp(t->argument_list[1], rl->list[i].cont.list, strlen(t->argument_list[1])) == 0) {
			return SUCCESS;
		}
		else {
			return FAILURE;
		}
	}
	else if (strcmp(t->functor, "endswith") == 0) {		
		i = find_matching_index(rl, t->argument_list[0]);
		
		if (strlen(rl->list[i].cont.list) > strlen(t->argument_list[1]) && strcmp(rl->list[i].cont.list + strlen(rl->list[i].cont.list) - strlen(t->argument_list[1]), t->argument_list[1]) == 0) {
			return SUCCESS;
		}
		else {
			return FAILURE;
		}
	}
	else if (strcmp(t->functor, "reverse") == 0) {
		// reverse(Line,Sorted)
		i = find_matching_index(rl, t->argument_list[0]);
		if (rl->list[i].t == LIST) {
			tmp = malloc(strlen(rl->list[i].cont.list) + 2);
			for (j = strlen(rl->list[i].cont.list); j > 0; j--) {
				tmp[strlen(rl->list[i].cont.list) - j] = rl->list[i].cont.list[j - 1];
			}
			tmp[strlen(rl->list[i].cont.list)] = '\0';
			associate_str_copy(rl, t->argument_list[1], tmp);
			free(tmp);
			tmp = NULL;
			return SUCCESS;
		}
		else {
			printf("Unable to reverse\n");
			exit(REVERSE_FAILURE);
		}	
	}
	else if (strcmp(t->functor, "replace") == 0) {
		// TODO: implement
		// replace(Line,"find","replace",LOut)
		i = find_matching_index(rl, t->argument_list[0]);

		// replace(Line,F,R,result) -> what F and R should be to obtain result?
	}
	else if (strcmp(t->functor, "match") == 0) {
		// TODO: implement
		// regex
	}
	else if (strcmp(t->functor, "append") == 0) {
		// TODO: implement
		// append(L,"abc",LO) -> LO = Labc
		// append(L,"abc",labc) -> true / false
		// append(L,M,labc) -> find what is missing to have labc starting from L
		i = find_matching_index(rl, t->argument_list[0]);
		if (rl->list[i].t == LIST) {
			if (islower(t->argument_list[1][0]) && isupper(t->argument_list[2][0])) {
				// append(L, "abc", LO)->LO = Labc
				tmp = malloc(strlen(rl->list[i].cont.list) + strlen(t->argument_list[1]) + 2);
				snprintf(tmp, strlen(rl->list[i].cont.list) + strlen(t->argument_list[1]) + 1, "%s%s", rl->list[i].cont.list, t->argument_list[1]);
				associate_str_copy(rl, t->argument_list[2], tmp);
				free(tmp);
				tmp = NULL;
				return SUCCESS;
			}
			else if (islower(t->argument_list[1][0]) && islower(t->argument_list[2][0])) {
				// append(L,"abc",labc) -> true / false
				tmp = malloc(strlen(rl->list[i].cont.list) + strlen(t->argument_list[1]) + 2);
				snprintf(tmp, strlen(rl->list[i].cont.list) + strlen(t->argument_list[1]) + 1, "%s%s", rl->list[i].cont.list, t->argument_list[1]);
				if (strcmp(tmp, t->argument_list[2]) == 0) {
					printf("true\n");
					free(tmp);
					tmp = NULL;
					return SUCCESS;
				}
				else {
					printf("false\n");
					free(tmp);
					tmp = NULL;
					return FAILURE;
				}
			}
			else if (isupper(t->argument_list[1][0]) && islower(t->argument_list[2][0])) {
				// append(L,M,labc) -> find what is missing to have labc starting from L
				if (strlen(rl->list[i].cont.list) > strlen(t->argument_list[2])) {
					printf("false\n");
					return FAILURE;
				}
				if (strcmp(rl->list[i].cont.list,t->argument_list[2]) == 0) {
					// printf("false\n");
					// strings are equal
					associate_str_copy(rl, t->argument_list[1], "");
					return SUCCESS;
				}
				j = 0;
				while ((rl->list[i].cont.list[j] == t->argument_list[2][j]) && rl->list[i].cont.list[j] != '\0' && t->argument_list[2][j] != '\0') {
					j++;
				}
				// if we do not terminate the input string then they cannot be equal
				// in anyway
				if (rl->list[i].cont.list[j] != '\0') {
					printf("false\n");
					return FAILURE;
				}
				tmp = malloc(strlen(t->argument_list[2]) - j + 2);
				for (i = 0; i < strlen(t->argument_list[2]) - j; i++) {
					tmp[i] = t->argument_list[2][j + i];
				}
				tmp[i] = '\0';
				associate_str_copy(rl, t->argument_list[1], tmp);
				free(tmp);
				tmp = NULL;
				return SUCCESS;
			}
		}
		else {
			printf("Unable to append\n");
			exit(APPEND_FAILURE);
		}

	}
	else if (strcmp(t->functor, "nth1") == 0) {
		// TODO: implement
		// nth1(L,2,E) -> E is the second element
		// nth1(L,2,"abc") -> true when abc is the second element
		i = find_matching_index(rl, t->argument_list[0]);
		// strsep
	}
	else if (strcmp(t->functor, "add") == 0 || strcmp(t->functor, "mul") == 0 || strcmp(t->functor, "sub") == 0 || strcmp(t->functor, "div") == 0) {
		// TODO: implement
		// add(L,2,3) -> true or false
		// add(L,2,R) -> R = L + 2
		// add(L,V,R) -> R = L + V
		i = find_matching_index(rl, t->argument_list[0]);
		// strsep
	}
	else {
		printf("Predicate not found\n");
		exit(PREDICATE_NOT_FOUND);
	}
}

double exec_command(FILE *fp, term_list* tl, reference_list* rl) {
	// loop trough term list
	int i;
	line l;
	int n_line_processed = 1;
	clock_t begin = clock();
	clock_t end;
	int num;

	// TODO: remove line from here and do in the function
	// TODO: refactor: most part of the cases are similar and check the if else

	if (strcmp("line", tl->list[0].functor) == 0) {
		assert(tl->list[0].arity == 1 || tl->list[0].arity == 2);
		assert(tl->list[0].argument_list[0] != NULL);
		// assert(tl->list[0].argument_list[1] != NULL);

		if ((isupper(tl->list[0].argument_list[0][0]) && tl->list[0].arity == 2 && isupper(tl->list[0].argument_list[1][0])) || (isupper(tl->list[0].argument_list[0][0]) && tl->list[0].arity == 1) ) {
			l.content = get_line(fp,&l.len);
			l.number = n_line_processed;
			if (tl->list[0].arity == 2) {
				// line(I,L)
				associate_int(rl, tl->list[0].argument_list[0], l.number);
				associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
			}
			else {
				// line(L)
				associate_str_reference(rl, tl->list[0].argument_list[0], l.content);
			}

			while (l.content != NULL) {
				// printf("%s\n", l.content);
				for (i = 1; i < tl->n_elements; i++) {
					if (apply_rule(&l, &tl->list[i], rl) < 0) {
						break;
					}
				}

				// TODO: check if something is to free in reference
				free_ref_t_to_free(rl);

				n_line_processed++;
				free(l.content);

				l.content = get_line(fp,&l.len);
				l.number = n_line_processed;
				if (tl->list[0].arity == 2) {
					// line(I,L)
					associate_int(rl, tl->list[0].argument_list[0], l.number);
					associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
				}
				else {
					// line(L)
					associate_str_reference(rl, tl->list[0].argument_list[0], l.content);
				}
			}
		}
		else if(tl->list[0].arity == 2 && isdigit(tl->list[0].argument_list[0][0]) && isupper(tl->list[0].argument_list[1][0])) {
			// here is line(1,N), so the first is number
			num = atoi(tl->list[0].argument_list[0]);
			if (num > 0) {
				l.content = get_line(fp, &l.len);
				l.number = n_line_processed;
				associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
				while (l.content != NULL && l.number != num) {
					n_line_processed++;
					free(l.content);

					l.content = get_line(fp, &l.len);
					l.number = n_line_processed;
					associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
				}
				if (l.content == NULL) {
					fprintf(outstream, "Line %d not found\n", num);
					exit(LINE_NOT_FOUND);
				}
				
				for (i = 1; i < tl->n_elements; i++) {
					if (apply_rule(&l, &tl->list[i], rl) < 0) {
						break;
					}
				}

				// check if something is to free in reference, flag to_free that indicates
				// that the string has been mallocd instead of reference
				free_ref_t_to_free(rl);
			}
			else {
				fprintf(outstream, "Line should be a number %s\n", tl->list[0].argument_list[0]);
				exit(CONVERSION_STRING_INT_FAILURE);
			}
		}
		else {
			if (tl->list[0].arity == 2 && isupper(tl->list[0].argument_list[0][0])) {
				// line(I,abc)
				l.content = get_line(fp, &l.len);
				l.number = n_line_processed;
				associate_int(rl, tl->list[0].argument_list[0], l.number);
				
				while (l.content != NULL) {
					// printf("%s\n", l.content);
					if (strcmp(l.content, tl->list[0].argument_list[1]) == 0) {
						for (i = 1; i < tl->n_elements; i++) {
							if (apply_rule(&l, &tl->list[i], rl) < 0) {
								break;
							}
						}
					}
					

					// TODO: check if something is to free in reference
					free_ref_t_to_free(rl);

					n_line_processed++;
					free(l.content);

					l.content = get_line(fp, &l.len);
					l.number = n_line_processed;
					
					associate_int(rl, tl->list[0].argument_list[0], l.number);
				}
			
			}
			else if(tl->list[0].arity == 2 && isdigit(tl->list[0].argument_list[0][0]) && islower(tl->list[0].argument_list[1][0])) {
				// line(1,abc) -> return true or false
				
				num = atoi(tl->list[0].argument_list[0]);
				if (num > 0) {
					l.content = get_line(fp, &l.len);
					l.number = n_line_processed;
					// associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
					while (l.content != NULL && l.number != num) {
						n_line_processed++;
						free(l.content);

						l.content = get_line(fp, &l.len);
						l.number = n_line_processed;
						// associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
					}
					if (l.content == NULL) {
						fprintf(outstream, "Line %d not found\n", num);
						exit(LINE_NOT_FOUND);
					}

					if (strcmp(l.content, tl->list[0].argument_list[1]) == 0) {
						if (tl->n_elements == 1) {
							printf("true\n");
						}
						else {
							for (i = 1; i < tl->n_elements; i++) {
								if (apply_rule(&l, &tl->list[i], rl) < 0) {
									break;
								}
							}
						}
					}
					else if (strcmp(l.content, tl->list[0].argument_list[1]) != 0 && tl->n_elements == 1) {
						printf("false\n");
					}
					

					// check if something is to free in reference, flag to_free that indicates
					// that the string has been mallocd instead of reference
					free_ref_t_to_free(rl);
				}
				else {
					fprintf(outstream, "Line should be a number %s\n", tl->list[0].argument_list[0]);
					exit(CONVERSION_STRING_INT_FAILURE);
				}
			}
			else {
				printf("case not covered\n");
			}
		}

	}
	else {
		printf("Expected a line\n");
	}
	
	end = clock();

	// return execution time
	return (double)(end - begin) / CLOCKS_PER_SEC;
}