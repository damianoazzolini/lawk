#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
// #include <sys/io.h>
// #include <sys/mman.h>


#include "defines.h"
#include "parser.h"
#include "exec.h"

// findall(L,(line(I,L),even(I)),Lines)

// TODO: create a structure to save the relation
// name -> type and then in the term set the index of this
// reference

// length(L,N), geq(N,3)
// 0, L, list
// 1, N, int
// 2, 3, int const

// arity: 2
// functor: length
// references: 0, 1 

// arity: 2
// functor: geq
// references: 2, 3

// findall(L,(line(I,L),even(I)),Lines)

// functor: findall - arity: 3
// list:
	// functor L, arity 0, ref 0, term_list null 
	// functor line

FILE *open_file(const char *filename) {
	FILE *fp = NULL;

	fp = fopen(filename,"r");
	if(fp == NULL) {
		// TODO: ask another name, maybe look in the folder to find something
		exit(FILE_NOT_FOUND);
	}

	return fp;
}

char *read_command() {
	// char* command = "line(1,L), write(L)"; // OK
	// char* command = "line(1,L), length(L,N), write(N)"; // OK
	// char* command = "line(L), length(L,6), write(L)"; // OK
	// char *command = "line(I,L), even(I), write(L)"; // OK
	// char *command = "line(I,L), odd(I), write(L)"; // OK
	// char *command = "line(I,L),mod(I,3,0), write(L)"; // OK
	// char *command = "line(I,L), lt(I,3), write(L)"; // OK 
	// char *command = "line(I,L), gt(I,3), write(L)"; // OK 
	// char *command = "line(I,L), length(L,N), lt(N,3), write(L)"; // OK
	// char *command = "line(I,L),between(I,5,10), write(L)"; // OK
	// char *command = "line(L),occurrences(L,\"l\",N),write(N)"; // OK
	// char *command = "line(L),occurrences(L,\"li\",N),write(N)"; // OK
	// char *command = "line(L),startswith(L,\"lin\"), write(L)"; // OK
	// char *command = "line(L),endswith(L,\"a2\"), write(L)"; // OK
	char *command = "line(L),words(L,N),write(N)"; // OK
	// char* command = "line(L),member(L,\"abc\"),write(L)";
	
	
	// char *command = "line(I,L), write(\"ciso\")"; // OK
	// char *command = "line(L),words(L,\"_\",N),write(N)"; // NEED to modify parser, otherwise loop, now there is a fallback
	
	return command;
}

// copied from https://stackoverflow.com/questions/20460670/reading-a-file-to-string-with-mmap
/*
void read_file(FILE* fp) {
    struct stat s;
    int status = fstat(fp, &s);
    int size;
    unsigned char* f;

    size = s.st_size;

    // f = (char*)mmap(0, size, PROT_READ, MAP_PRIVATE, fp, 0);
    f = (char*)mmap(0, size, 1, 2, fp, 0);
    for (int i = 0; i < size; i++) {
        char c;

        c = f[i];
        putchar(c);
    }

}
*/

// TODO: store history of the commands

int main() {


    reference_list ref_list;
	term_list t_list;

	double exec_time = 0.0;

	// todo: check if file exists

	ref_list.list = NULL;
	ref_list.n_elements = 0;

	t_list.n_elements = 0;
	t_list.list = NULL;

	// TODO: while loop
	char *command_in = read_command();

	printf("Command: %s\n", command_in);
	
	parse_command(command_in, &t_list, &ref_list);

	FILE* fp = open_file("test.txt");

	outstream = stdout;
	
	exec_time = exec_command(fp, &t_list, &ref_list);

	fclose(fp);

	printf("Executed in %lfs\n", exec_time);

	

	// free(command);

	return SUCCESS;
}