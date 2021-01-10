#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "defines.h"
#include "error.h"

int check_singleton(reference_list* rl) {
    int i;
    int count = 0;
    
    for (i = 0; i < rl->n_elements; i++) {
        if (rl->list[i].reference_count == 0) {
            print_singleton_warning(rl->list[i].name);
            count++;
        }
    }

    return count;
}

int check_arity(term_list* tl) {
    int i;
    for (i = 0; i < tl->n_elements; i++) {
        if (
            (strcmp(tl->list[i].functor,"line") == 0 && (tl->list[i].arity == 1 || tl->list[i].arity == 2)) ||
            (strcmp(tl->list[i].functor,"write") == 0 && tl->list[i].arity > 0) ||
            (strcmp(tl->list[i].functor,"even") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"odd") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"number") == 0 && tl->list[i].arity == 1)||
            (strcmp(tl->list[i].functor,"letter") == 0 && tl->list[i].arity == 1) ||
            (strcmp(tl->list[i].functor,"alpha") == 0 && tl->list[i].arity == 1) || 
            (strcmp(tl->list[i].functor,"length") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"mod") == 0 && tl->list[i].arity == 3) ||
            (strcmp(tl->list[i].functor,"gt") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"lt") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"between") == 0 && tl->list[i].arity == 3) || 
            (strcmp(tl->list[i].functor,"occurrences") == 0 && tl->list[i].arity == 3) || 
            (strcmp(tl->list[i].functor,"startswith") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"endswith") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"reverse") == 0 && tl->list[i].arity == 2) ||
            (strcmp(tl->list[i].functor,"words") == 0 && (tl->list[i].arity == 2 || tl->list[i].arity == 3)) ||
            (strcmp(tl->list[i].functor,"nth1") == 0 && (tl->list[i].arity == 3 || tl->list[i].arity == 4))  ||
            (strcmp(tl->list[i].functor,"add") == 0 && tl->list[i].arity == 3) ||
            (strcmp(tl->list[i].functor,"mul") == 0 && tl->list[i].arity == 3) ||
            (strcmp(tl->list[i].functor,"sub") == 0 && tl->list[i].arity == 3) ||
            (strcmp(tl->list[i].functor,"div") == 0 && tl->list[i].arity == 3) ||
            (strcmp(tl->list[i].functor,"replace") == 0 && tl->list[i].arity == 4) || 
            (strcmp(tl->list[i].functor,"member") == 0 && tl->list[i].arity == 2) 
            ) {
        }
        else {
            return FAILURE;
        }
    }

    return SUCCESS;
}

int reference_already_present(reference_list* r_l, char* name) {
    int i;

    assert(name != NULL);

    for (i = 0; i < r_l->n_elements; i++) {
        if (strcmp(r_l->list[i].name, name) == 0) {
            r_l->list[i].reference_count++;
            return i;
        }
    }

    return -1;
}

void add_reference(reference_list* r_l, char* name) {
    ref_t* temp = NULL;

    assert(name != NULL);

    if (reference_already_present(r_l, name) < 0) {
        r_l->n_elements++;
        temp = realloc(r_l->list, r_l->n_elements * sizeof(ref_t));
        if (temp == NULL) {
            print_realloc_error("Realloc error in parser.c - add_reference\n");
        }
        else {
            r_l->list = temp;
        }
        r_l->list[r_l->n_elements - 1].name = malloc(strlen(name) + 2);
        snprintf(r_l->list[r_l->n_elements - 1].name, strlen(name) + 1, "%s", name);
        r_l->list[r_l->n_elements - 1].t = UNSET;
        r_l->list[r_l->n_elements - 1].reference_count = 0;
    }    
}

void add_term_t(term_list* tl, char* name) {
    assert(name != NULL);

    tl->n_elements++;
    tl->list = realloc(tl->list, tl->n_elements * sizeof(term_t));

    tl->list[tl->n_elements - 1].arity = 0;
    tl->list[tl->n_elements - 1].functor = malloc(strlen(name) + 2);
    snprintf(tl->list[tl->n_elements - 1].functor, strlen(name) + 1, "%s", name);
    tl->list[tl->n_elements - 1].argument_list = NULL;
}

void add_subterm(term_list* tl, char* name) {
    tl->list[tl->n_elements - 1].arity++;
    tl->list[tl->n_elements - 1].argument_list = realloc(tl->list[tl->n_elements - 1].argument_list, sizeof(char*) * tl->list[tl->n_elements - 1].arity);
    tl->list[tl->n_elements - 1].argument_list[tl->list[tl->n_elements - 1].arity - 1] = malloc(strlen(name) + 2);
    snprintf(tl->list[tl->n_elements - 1].argument_list[tl->list[tl->n_elements - 1].arity - 1], strlen(name) + 1, "%s", name);
}

// TODO: handle floating point numbers (.)
// TODO: remove *index_str and use a local variable
int parse_command_rec(char* str, int* index_str, term_list *tl, reference_list *rl) {
    char var_name[VAR_NAME_LEN];
    int var_pos = 0;
    int n_par = 0;

    // i think this is not a definitive solution
    if (str[*index_str] == '\"') {
        printf("Unexpected quote\n");
        exit(COMMAND_ERROR);
    }

    while (str[*index_str] != '\0') {
        var_pos = 0;
        var_name[var_pos] = str[*index_str];
        var_pos++;

        if (isupper(str[*index_str])) {
            (*index_str)++;
            while (str[*index_str] != ',' && str[*index_str] != ')' && str[*index_str] != '\"' && str[*index_str] != '\0') {
                var_name[var_pos] = str[*index_str];
                var_pos++;
                (*index_str)++;
            }
            var_name[var_pos] = '\0';

            // here i have the variable name
            add_reference(rl,var_name);            
            // printf("%s\n", var_name);
            add_subterm(tl, var_name);
        }
        else if (islower(str[*index_str])) {
            // this is a constant or compound
            (*index_str)++;
            while (str[*index_str] != ',' && str[*index_str] != ')' && str[*index_str] != '(' && str[*index_str] != '\"' && str[*index_str] != '\0') {
                var_name[var_pos] = str[*index_str];
                var_pos++;
                (*index_str)++;
            }
            var_name[var_pos] = '\0';

            if (str[*index_str] == ',' || str[*index_str] == ')') {
                add_subterm(tl, var_name);
            }
            else {
                add_term_t(tl, var_name);
            }
        }
        else if (isdigit(str[*index_str])) {
            (*index_str)++;
            while (isdigit(str[*index_str]) && str[*index_str] != '\0') {
                var_name[var_pos] = str[*index_str];
                var_pos++;
                (*index_str)++;
            }
            var_name[var_pos] = '\0';

            add_subterm(tl, var_name);
        }
        else if (str[*index_str] == '"') {
            var_pos = 0;
            (*index_str)++;
            while (str[*index_str] != '"' && str[*index_str] != '\0') {
                var_name[var_pos] = str[*index_str];
                var_pos++;
                (*index_str)++;
            }
            if(str[*index_str] == '\0') {
                print_missing_quote();
            }
            var_name[var_pos] = '\0';
            add_subterm(tl, var_name);
        }

        // TODO: all the error stuff
        if (str[*index_str] == ',') {
            (*index_str)++; // digest ,
            // term_pos++;
            // increase_arity(stack, *index_stack);
        }
        else if (str[*index_str] == '(') {
            (*index_str)++;
            n_par++;
        }
        else if (str[*index_str] == ')') {
            (*index_str)++;
            n_par--;
        }
        else if (str[*index_str] == ' ') {
            (*index_str)++;
        }
        else {
            // fallback
            (*index_str)++;
        }
    }

    if(n_par != 0) {
        print_unbalanced_parentheses_error(str);
    }

    if(tl->n_elements == 0) {
        return FAILURE;
    }

    return SUCCESS;
}

void parse_command(char* command_string, term_list* tl, reference_list* rl) {
	int index_str = 0;
	// check if the string is well formed
	parse_command_rec(command_string, &index_str,tl,rl);
    // print_reference_list(rl);
    // print_term_list(tl);
    if(check_arity(tl) == FAILURE) {
        printf("Some predicates have wrong arity\n");
        exit(CHECK_ARITY_ERROR);
    }

    check_singleton(rl);
}


/*
print utilities
*/

void print_term_t(term_t* t) {
    int i;
    printf("Arity: %d, functor: %s, ", t->arity, t->functor);
    for (i = 0; i < t->arity; i++) {
        printf("| %s ",t->argument_list[i]);
    }
}

void print_term_list(term_list* tl) {
    int i;
    printf("TERM LIST\n");
    for (i = 0; i < tl->n_elements; i++) {
        printf("Index: %d\n", i);
        print_term_t(&tl->list[i]);
        printf("\n");
    }
    printf("\n");
}

void print_ref_t(ref_t* r) {
    printf("Name: %s\n", r->name);
    switch (r->t) {
    case UNSET:
        printf("Unset");
        break;
    case INT:
        printf("%d", r->cont.int_val);
        break;
    case FLOAT:
        printf("%lf", r->cont.double_val);
        break;
    case LIST:
        printf("%s", r->cont.list);
        break;
    default:
        printf("Unknown content\n");
        exit(UNKNOWN_CONTENT);
        break;
    }
}

void print_reference_list(reference_list* rl) {
    int i;

    printf("REFERENCE LIST\n");
    for (i = 0; i < rl->n_elements; i++) {
        print_ref_t(&rl->list[i]);
        printf("\n");
    }
    printf("\n");
}

/*
free
*/
void free_term_t(term_t* t) {
    int i;
    
    for (i = 0; i < t->arity; i++) {
        free(t->argument_list[i]);
    }
    
    free(t->argument_list);
    free(t->functor);
}

void free_term_list(term_list* t) {
    int i;

    for (i = 0; i < t->n_elements; i++) {
        free_term_t(&t->list[i]);
    }

    free(t->list);
}

void free_ref_t(ref_t *t) {
    free(t->name);
    /*
    if (t->t == LIST) {
        free(t->cont.list);
    }
    */
}

void free_reference_list(reference_list* rl) {
    int i;
    for (i = 0; i < rl->n_elements; i++) {
        free_ref_t(&rl->list[i]);
    }
    free(rl->list);
}