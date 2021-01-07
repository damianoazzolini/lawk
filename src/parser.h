#pragma once
typedef enum { UNSET, INT, FLOAT, LIST } type;

typedef union content {
	int int_val;
	double double_val;
	char* list;
} content;

typedef struct {
	char* name;
	type t;
	content cont;
	int reference_count;
	int to_free; // set to 1 if t is list and has been copied (associate_str_copy). needed to free in every loop
} ref_t;

typedef struct {
	int n_elements;
	ref_t* list;
} reference_list;

/*
typedef struct term_t {
	int arity;
	char* functor;
	// int n_subterm;
	struct term_t* sub_term_list;
} term_t;
*/

typedef struct term_t {
	int arity;
	char* functor;
	char** argument_list;
} term_t;

typedef struct term_list {
	int n_elements;
	term_t* list;
} term_list;

void parse_command(char* command_string, term_list *t_l, reference_list *r_l);

// print
void print_term_t(term_t* t);
void print_term_list(term_list* tl);
void print_reference_list(reference_list* rl);
void print_ref_t(ref_t* r);

// free
void free_term_t(term_t* t);
void free_term_list(term_list* t);
void free_ref_t(ref_t* t);
void free_reference_list(reference_list* rl);