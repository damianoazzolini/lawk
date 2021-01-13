#define _CRT_SECURE_NO_DEPRECATE // used for visual studio for strcpy and strncpy. TODO: replace with snprintf
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
// #ifdef _WIN32
// really primitive implementation
// since getline is not available in windows
char* get_line(FILE* fp, size_t *len_l) {
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

	*len_l = (size_t)len; // TODO: abominio, use *len_l

	return l;	
}
// #endif


// You must free the result if result is non-NULL.
// here to make more readable the mega if else in the other function
// https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c

char* str_replace(char* source, char* find, char* replace) {
	// DOES NOT WORK
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
	for (count = 0; (tmp = strstr(ins, replace)); ++count) {
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
	exit(TERM_NOT_FOUND);
}

// used to free the memory allocated in, for example,
// reverse, nth ...
void free_ref_t_to_free(reference_list* rl) {
	int i;
	for (i = 0; i < rl->n_elements; i++) {
		if (rl->list[i].to_free == 1 && rl->list[i].t == LIST) {
			if(rl->list[i].cont.list != NULL) {
				free(rl->list[i].cont.list);
			}
			rl->list[i].cont.list = NULL;
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

// used in nth
int find_indexes_string(char *str, char ch, int pos, int *start, int *end) {
	int count = 0;
	int prev_start = 0;
	int j = 0;
	
	while (count != pos && str[j] != '\0') {
		if (str[j] == ch) {
			count++;
			prev_start = *start;
			*end = j;
			// to eat a replication of the separator
			while (str[j] == ch) {
				j++;
			}
			*start = j;
		}
		else {
			j++;
		}
	}
	if (str[j] == '\0' && str[j - 1] != ch) {
		count++;
		prev_start = *start;
		*end = j;
	}

	*start = prev_start;

	return count;
}

// super mega long function
int apply_rule(line *l, term_t* t, reference_list* rl) {
	int i, j;
	int v1, v2;
	int count = 0;
	char* tmp = NULL;
	char ch;
	int start = 0, end;
	char* ts1 = NULL, * ts2 = NULL;

	if (strcmp(t->functor, "length") == 0) {
		if (isdigit(t->argument_list[1][0])) {
			// lenght(L,2)
			if ((size_t)atoi(t->argument_list[1]) == l->len) {
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
				((strcmp(t->functor, "between") == 0) && (rl->list[i].cont.int_val > atoi(t->argument_list[1])) && (rl->list[i].cont.int_val < atoi(t->argument_list[2])))
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
		// credo che devo implementare una funzione apposta per member, nel ciclo pi� esterno
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

		for (j = 0; j < (int)strlen(rl->list[i].cont.list); j++) {
			if (rl->list[i].cont.list[j] == ch) {
				if (j != (int)strlen(rl->list[i].cont.list) - 1) {
					count++;
				}
			}
			while (rl->list[i].cont.list[j] == ch && j < (int)strlen(rl->list[i].cont.list)) {
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
			for (j = 0; j < (int)strlen(rl->list[i].cont.list); j++) {
				if (rl->list[i].cont.list[j] == t->argument_list[1][0]) {
					count++;
				}
			}
		}
		else {
			// string
			tmp = rl->list[i].cont.list;
			while ((tmp = strstr(tmp, t->argument_list[1]))) {
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
		// reverse(Line,sorted)
		i = find_matching_index(rl, t->argument_list[0]);
		if (rl->list[i].t == LIST) {
			tmp = malloc(strlen(rl->list[i].cont.list) + 2);
			for (j = (int)strlen(rl->list[i].cont.list); j > 0; j--) {
				tmp[strlen(rl->list[i].cont.list) - j] = rl->list[i].cont.list[j - 1];
			}
			tmp[strlen(rl->list[i].cont.list)] = '\0';
			if(isupper(t->argument_list[1][0])) {
				// reverse(Line,Sorted)
				associate_str_copy(rl, t->argument_list[1], tmp);
				free(tmp);
				tmp = NULL;
				return SUCCESS;
			}
			else if(islower(t->argument_list[1][0])) {
				// reverse(Line,sorted)
				if(strcmp(tmp,t->argument_list[1]) == 0) {
					free(tmp);
					return SUCCESS;
				}
				else {
					free(tmp);
					return FAILURE;
				}
			}
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
		if (islower(t->argument_list[1][0]) && islower(t->argument_list[2][0])) {
			// both ground
			ts1 = t->argument_list[1];
			ts2 = t->argument_list[2];
		}
		else if (islower(t->argument_list[1][0]) && (isupper(t->argument_list[2][0]) || isdigit(t->argument_list[2][0]))) {
			ts1 = t->argument_list[1];
			if (isupper(t->argument_list[2][0])) {
				j = find_matching_index(rl, t->argument_list[2]);
				ts2 = rl->list[j].cont.list;
			}
			else if (isdigit(t->argument_list[2][0])) {
				ts2 = t->argument_list[2];
			}
		}
		else if (islower(t->argument_list[2][0]) && (isupper(t->argument_list[1][0]) || isdigit(t->argument_list[1][0]))) {
			ts2 = t->argument_list[2];
			if (isupper(t->argument_list[1][0])) {
				j = find_matching_index(rl, t->argument_list[1]);
				ts1 = rl->list[j].cont.list;
			}
			else if (isdigit(t->argument_list[1][0])) {
				ts1 = t->argument_list[1];
			}
		}

		tmp = str_replace(rl->list[i].cont.list, ts1, ts2);
		if (tmp != NULL) {
			associate_str_copy(rl, t->argument_list[3], tmp);
			free(tmp);
			tmp = NULL;
			return SUCCESS;
		}
		else {
			return FAILURE;
		}

		// replace(Line,F,R,result) -> what F and R should be to obtain result?
	}
	else if (strcmp(t->functor, "match") == 0) {
		// TODO: implement
		// regex
	}
	else if (strcmp(t->functor, "append") == 0) {
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
					// printf("true\n");
					free(tmp);
					tmp = NULL;
					return SUCCESS;
				}
				else {
					// printf("false\n");
					free(tmp);
					tmp = NULL;
					return FAILURE;
				}
			}
			else if (isupper(t->argument_list[1][0]) && islower(t->argument_list[2][0])) {
				// append(L,M,labc) -> find what is missing to have labc starting from L
				if (strlen(rl->list[i].cont.list) > strlen(t->argument_list[2])) {
					// printf("false\n");
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
					// printf("false\n");
					return FAILURE;
				}
				tmp = malloc(strlen(t->argument_list[2]) - j + 2);
				for (i = 0; i < (int)strlen(t->argument_list[2]) - j; i++) {
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
	else if(strcmp(t->functor, "first") == 0 || strcmp(t->functor, "last") == 0)  {
		// last(L,N,V): V are the last N chars of L
		// last(L,N,ground): ground are the last N chars of L -> success/failure
		// last(L,ground,V): V are the last ground chars of L
		// last(L,groundN,ground): ground are the last groundN chars of L
		i = find_matching_index(rl, t->argument_list[0]);
		if(isupper(t->argument_list[1][0]) && isupper(t->argument_list[2][0])) {
			// last(L,N,V): V are the last N chars of L
		}
		else if(isupper(t->argument_list[1][0]) && islower(t->argument_list[2][0])) {
			// last(L,N,ground): ground are the last N chars of L -> success/failure
		}
		else if(islower(t->argument_list[1][0]) && isupper(t->argument_list[2][0])) {
			// last(L,ground,V): V are the last ground chars of L
	
		}
		else if(islower(t->argument_list[1][0]) && islower(t->argument_list[2][0])) {
			// last(L,groundN,ground): ground are the last groundN chars of L
		}
		else {
			printf("%s error\n", strcmp(t->functor, "first") == 0 ? "first" : "last");
			exit(COMMAND_ERROR);
		}
	}
	else if(strcmp(t->functor, "nth1") == 0) {
		// nth1(Line,Index,Char)
		// nth1(Line,3,Char)
		i = find_matching_index(rl, t->argument_list[0]);
		if(isdigit(t->argument_list[1][0])) {
			// nth1(Line,3,Char)
			if(atoi(t->argument_list[1]) > (int)strlen(rl->list[i].cont.list)) {
				printf("Unable to access index %d of line\n",atoi(t->argument_list[1]));
				exit(OUT_OF_RANGE);
			}
			else {
				tmp = malloc(2);
				tmp[0] = rl->list[i].cont.list[atoi(t->argument_list[1])];
				tmp[1] = '\0';
				associate_str_copy(rl, t->argument_list[2], tmp);
				free(tmp);
				return SUCCESS;
			}
		}
		else if(isupper(t->argument_list[1][0])) {
			// nth1(Line,Index,Char)
			j = find_matching_index(rl, t->argument_list[1]);
				if(rl->list[j].t == INT) {
					if(rl->list[j].cont.int_val > (int)strlen(rl->list[i].cont.list)) {
					printf("Unable to access index %d of line\n",rl->list[j].cont.int_val);
					exit(OUT_OF_RANGE);
				}
				else {
					tmp = malloc(2);
					tmp[0] = rl->list[i].cont.list[rl->list[j].cont.int_val];
					tmp[1] = '\0';
					associate_str_copy(rl, t->argument_list[2], tmp);
					free(tmp);
					return SUCCESS;
				}
			}
		}
	}
	else if (strcmp(t->functor, "nth1_word") == 0) {
		// nth1(L,2,E) -> E is the second element
		// nth1(L,2,"abc") -> true when abc is the second element
		i = find_matching_index(rl, t->argument_list[0]);
		// strsep
		
		if ((t->arity == 3 && (isdigit(t->argument_list[1][0]) && isupper(t->argument_list[2][0]))) || (t->arity == 4 && (isdigit(t->argument_list[1][0]) && isupper(t->argument_list[3][0])))) {
			// nth1(L, 2, E)->E is the second element
			// TODO: does not work with number
			count = 0;
			start = 0; 
			end = 0;
			j = 0;
			if (t->arity == 4) {
				ch = t->argument_list[2][0];
			}
			else {
				ch = ' ';
			}
			count = find_indexes_string(rl->list[i].cont.list, ch, atoi(t->argument_list[1]), &start, &end);
			
			if (count == atoi(t->argument_list[1])) {
				// found it
				tmp = malloc(end - start + 2);
				// strncpy(tmp, rl->list[i].cont.list + prev_start, end - prev_start);
				snprintf(tmp, end - start + 1, "%s", rl->list[i].cont.list + start);
				// tmp[end - prev_start] = '\0';
				if (t->arity == 3) {
					associate_str_copy(rl, t->argument_list[2], tmp);
				}
				else {
					associate_str_copy(rl, t->argument_list[3], tmp);
				}
				free(tmp);
				return SUCCESS;
			}
			else {
				printf("Unable to access nth\n");
				exit(NTH_ERROR);
			}
		}
		else if ((t->arity == 3 && (isdigit(t->argument_list[1][0]) && islower(t->argument_list[2][0]))) || (t->arity == 4 && (isdigit(t->argument_list[1][0]) && islower(t->argument_list[3][0])))) {
			// nth1(L,2,"abc") 
			count = 0;
			start = 0; 
			end = 0;
			j = 0;
			if (t->arity == 4) {
				ch = t->argument_list[2][0];
			}
			else {
				ch = ' ';
			}
			count = find_indexes_string(rl->list[i].cont.list, ch, atoi(t->argument_list[1]), &start, &end);
			if (count == atoi(t->argument_list[1])) {
				// found it
				tmp = malloc(end - start + 2);
				// strncpy(tmp, rl->list[i].cont.list + prev_start, end - prev_start);
				snprintf(tmp, end - start + 1, "%s", rl->list[i].cont.list + start);
				// tmp[end - prev_start] = '\0';
				if ((t->arity == 3 && strcmp(t->argument_list[2],tmp) == 0) || (t->arity == 4 && strcmp(t->argument_list[3], tmp) == 0)) {
					free(tmp);
					return SUCCESS;
				}
				else {
					free(tmp);
					return FAILURE;
				}
			}
			else {
				printf("Unable to access nth\n");
				exit(NTH_ERROR);
			}
		}

	}
	else if (strcmp(t->functor, "add") == 0 || strcmp(t->functor, "mul") == 0 || strcmp(t->functor, "sub") == 0 || strcmp(t->functor, "div") == 0) {
		// add(L,2,3) -> true or false
		// add(L,2,R) -> R = L + 2
		// add(L,V,R) -> R = L + V
		i = find_matching_index(rl, t->argument_list[0]);
		
		if (rl->list[i].t != INT && rl->list[i].t != FLOAT) {
			printf("Expected int or float\n");
			exit(EXPECTED_NUMBER);
		}

		j = rl->list[i].cont.int_val; // this works only for int
		
		if (isdigit(t->argument_list[1][0]) && isdigit(t->argument_list[2][0])) {
			// add(L,2,3) -> true or false
			v1 = atoi(t->argument_list[1]);
			v2 = atoi(t->argument_list[2]);
			if (
				(strcmp(t->functor, "add") == 0 && j + v1 == v2) ||
				(strcmp(t->functor, "mul") == 0 && j * v1 == v2) ||
				(strcmp(t->functor, "sub") == 0 && j - v1 == v2) ||
				(strcmp(t->functor, "div") == 0 && j / v1 == v2)
				) {
				return SUCCESS;
			}
			else {
				return FAILURE;
			}
		}
		else if (isdigit(t->argument_list[1][0]) && isupper(t->argument_list[2][0])) {
			// add(L,2,R) -> R = L + 2
			v1 = atoi(t->argument_list[1]);
			if (strcmp(t->functor, "add") == 0) {
				associate_int(rl, t->argument_list[2], j + v1);
			}
			else if (strcmp(t->functor, "mul") == 0) {
				associate_int(rl, t->argument_list[2], j * v1);
			}
			else if (strcmp(t->functor, "sub") == 0) {
				associate_int(rl, t->argument_list[2], j - v1);
			}
			else if (strcmp(t->functor, "div") == 0) {
				associate_int(rl, t->argument_list[2], j / v1);
			}
			else {
				printf("Operation inexistent\n");
				exit(UNKNOWN_FUNCTOR);
			}

		}
		else if (isupper(t->argument_list[1][0]) && isupper(t->argument_list[2][0])) {
			// add(L,V,R) -> R = L + V
			i = find_matching_index(rl, t->argument_list[1]);

			if (rl->list[i].t != INT && rl->list[i].t != FLOAT) {
				printf("Expected int or float\n");
				exit(EXPECTED_NUMBER);
			}

			v1 = rl->list[i].cont.int_val; // this works only for int

			if (strcmp(t->functor, "add") == 0) {
				associate_int(rl, t->argument_list[2], j + v1);
			}
			else if (strcmp(t->functor, "mul") == 0) {
				associate_int(rl, t->argument_list[2], j * v1);
			}
			else if (strcmp(t->functor, "sub") == 0) {
				associate_int(rl, t->argument_list[2], j - v1);
			}
			else if (strcmp(t->functor, "div") == 0) {
				associate_int(rl, t->argument_list[2], j / v1);
			}
			else {
				printf("Operation inexistent\n");
				exit(UNKNOWN_FUNCTOR);
			}
		}
	}
	else {
		printf("Predicate not found\n");
		exit(PREDICATE_NOT_FOUND);
	}

	return FAILURE;
}

double exec_command(FILE *fp, term_list* tl, reference_list* rl, FILE *outstream) {
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
			// #ifdef _WIN32
			l.content = get_line(fp,&l.len);
			// #else 
			// getline(&l.content,&l.len,fp);
			// #endif
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

				// #ifdef _WIN32
				l.content = get_line(fp,&l.len);
				// #else 
				// getline(&l.content,&l.len,fp);
				// #endif
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
				// #ifdef _WIN32
				l.content = get_line(fp,&l.len);
				// #else 
				// getline(&l.content,&l.len,fp);
				// #endif
				l.number = n_line_processed;
				associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
				while (l.content != NULL && l.number != num) {
					n_line_processed++;
					free(l.content);

					// #ifdef _WIN32
					l.content = get_line(fp,&l.len);
					// #else 
					// getline(&l.content,&l.len,fp);
					// #endif
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
				free(l.content);

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
				// #ifdef _WIN32
				l.content = get_line(fp,&l.len);
				// #else 
				// getline(&l.content,&l.len,fp);
				// #endif
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

					// #ifdef _WIN32
					l.content = get_line(fp,&l.len);
					// #else 
					// getline(&l.content,&l.len,fp);
					// #endif
					l.number = n_line_processed;
					
					associate_int(rl, tl->list[0].argument_list[0], l.number);
				}
			
			}
			else if(tl->list[0].arity == 2 && isdigit(tl->list[0].argument_list[0][0]) && islower(tl->list[0].argument_list[1][0])) {
				// line(1,abc) -> return true or false
				
				num = atoi(tl->list[0].argument_list[0]);
				if (num > 0) {
					// #ifdef _WIN32
					l.content = get_line(fp,&l.len);
					// #else 
					// getline(&l.content,&l.len,fp);
					// #endif
					l.number = n_line_processed;
					// associate_str_reference(rl, tl->list[0].argument_list[1], l.content);
					while (l.content != NULL && l.number != num) {
						n_line_processed++;
						free(l.content);

						// #ifdef _WIN32
						l.content = get_line(fp,&l.len);
						// #else 
						// getline(&l.content,&l.len,fp);
						// #endif
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
					
					free(l.content);


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
		printf("Expected line1/2\n");
	}
	
	end = clock();

	// return execution time
	return (double)(end - begin) / CLOCKS_PER_SEC;
}