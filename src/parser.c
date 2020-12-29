#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "defines.h"
#include "error.h"

// set the element name to t
void propagate_type(reference_list *rl, char* name, type t) {

}

// line(Index,Line): check that index is number and Line is a variable
// line(Line)
/*
int check_line(term_t *t) {
    int error = 0;
    if (t->arity >= 1) {
        printf("Expected arity of line/1, line/2 is 1 or 2, found: %d\n", t->arity);
        error++;
    }
    else {
        assert(t->sub_term_list[0].functor != NULL);
        if (t->arity == 2 && atoi(t->sub_term_list[0].functor) < 0) {
            printf("Expected integer as first argument of line/2, found: %s\n", t->sub_term_list[0].functor);
            error++;
        }
        if ((t->arity == 2 && !isupper(t->sub_term_list[1].functor[0]) || t->arity == 1 && !isupper(t->sub_term_list[0].functor[0]))) {
            printf("Expected variable, found: %s ", t->sub_term_list[0].functor);
            error++;
            if (islower(t->sub_term_list[1].functor[0])) {
                printf("(constant)\n");
            }
            else if (isdigit(t->sub_term_list[1].functor[0])) {
                printf("(number)\n");
            }
            else {
                printf("\n");
            }
        }
    }

    return error;
}

int check_variable_1(term_t* t) {
    int error = 0;
    if (t->arity != 1) {
        printf("Expected arity 1, found: %d\n", t->arity);
        error++;
    }
    else {
        if (!isupper(t->sub_term_list[0].functor[0])) {
            printf("Expected variable as argument, found: %s\n", t->sub_term_list[0].functor);
            error++;
        }
    }

    return error;
}
*/

// check that the direction of the arguments is correct
/*
DONE
line/1, line/2
even/1
odd/1
number/1
letter/1
lowerLetter/1
upperLetter/1
alpha/1

TODO
findall/3
gt/2
lt/2
eval/2
startswith/2
endswith/2
member/2
forall/2
match/2
length/2
sumlist/2
nth1/4
*/
/*
void check_mode(term_list* tl) {
    int i;
    int error = 0;
    for (i = 0; i < tl->n_elements; i++) {
        if (strcmp(tl->list[i].functor, "line") == 0) {
            error = error + check_line(&tl->list[i]);
        }
        else if (
            strcmp(tl->list[i].functor, "even") == 0 || 
            strcmp(tl->list[i].functor, "odd") == 0 ||
            strcmp(tl->list[i].functor, "number") == 0 ||
            strcmp(tl->list[i].functor, "letter") == 0 ||
            strcmp(tl->list[i].functor, "lowerletter") == 0 ||
            strcmp(tl->list[i].functor, "upperletter") == 0 ||
            strcmp(tl->list[i].functor, "alpha") == 0) {
            error = error + check_variable_1(&tl->list[i]);
        }
        else {
            printf("Functor not found\n");
            exit(UNKOWN_FUNCTOR);
        }
    }

    if (error > 0) {
        exit(MODE_ERROR);
    }
}
*/

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


int parse_command_rec(char* str, int* index_str, term_list *tl, reference_list *rl) {
    char var_name[VAR_NAME_LEN];
    int var_pos = 0;
    int n_par = 0; // number of parentheses to indicate whether we are in a term or not

    assert(*index_str <= (int) strlen(str));

    while (str[*index_str] != '\0') {
        var_pos = 0;
        var_name[var_pos] = str[*index_str];
        var_pos++;

        if (isupper(str[*index_str])) {
            (*index_str)++;
            while (str[*index_str] != ',' && str[*index_str] != ')') {
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
            while (str[*index_str] != ',' && str[*index_str] != ')' && str[*index_str] != '(') {
                var_name[var_pos] = str[*index_str];
                var_pos++;
                (*index_str)++;
            }
            var_name[var_pos] = '\0';
            // here i have the compound name
            // printf("%s\n",var_name);     
            add_term_t(tl, var_name);
        }
        else if (isdigit(str[*index_str])) {
            (*index_str)++;
            while (isdigit(str[*index_str])) {
                var_name[var_pos] = str[*index_str];
                var_pos++;
                (*index_str)++;
            }
            var_name[var_pos] = '\0';
            if (str[*index_str] != ',' && str[*index_str] != ')') {
                print_parser_error(str, *index_str, "Expected , or ) after a number");
            }
            add_subterm(tl, var_name);
        }
        else {
            // this is the case of * ? regex
        }

        // TODO: all the error stuff
        if (str[*index_str] == ',') {
            (*index_str)++; // digest ,
            // term_pos++;
            // increase_arity(stack, *index_stack);
        }
        else if (str[*index_str] == '(') {
            (*index_str)++;
        }
        else if (str[*index_str] == ')') {
            (*index_str)++;
            // (*n_parentheses)--;
            if (str[*index_str + 1] == ',') {
                // term_pos++;
            }
            
            // printf("already increased for: %d\n", current_index_functor);
            // (*index_parsed)--;
            // parse_command_rec(str, index_str, t_l, r_l,  n_parentheses,term_index, "findall", 2);
        }
        else if (str[*index_str] == ' ') {
            (*index_str)++;
        }
    }

    return 0;
}

void parse_command(char* command_string, term_list* tl, reference_list* rl) {
	int n_par = 0; // parentheses
	int term_nesting = 0;
    int index_str = 0;
    int n_parentheses = 0;
	// check if the string is well formed
	parse_command_rec(command_string, &index_str,tl,rl);
    // print_reference_list(rl);
    // print_term_list(tl);
    check_singleton(rl);

}


/*
print utilities
*/

void print_term_t(term_t* t) {
    int i;
    printf("Arity: %d, functor: %s, ", t->arity, t->functor);
    for (i = 0; i < t->arity; i++) {
        printf("%s ",t->argument_list[i]);
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
        free(&t->argument_list[i]);
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
    if (t->t == LIST) {
        free(t->cont.list);
    }
}

void free_reference_list(reference_list* rl) {
    int i;
    for (i = 0; i < rl->n_elements; i++) {
        free_ref_t(&rl->list[i]);
    }
    free(rl->list);
}