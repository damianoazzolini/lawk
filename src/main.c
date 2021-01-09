#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
/*
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
*/
// #include <sys/io.h>
// #include <sys/mman.h>

#include <unistd.h>
// #include <sys/types.h>
#include <pwd.h>


#include "defines.h"
#include "parser.h"
#include "exec.h"

const char *program_version = "0.0.1";
const char *history_file = ".lawk_history"; 
// TODO: create a struct with global variables
char *filename = NULL;
int interactive = 0;
char *query = NULL;

void print_help() {
	printf("lawk version %s\n", program_version);
	printf("Analizing files with a logic-like language\n");
	printf("Repository: https://github.com/damianoazzolini/lawk\n");
	printf("Examples:\n");
	printf("- Print the first line: `line(1,L), write(L)`\n");
	printf("- - Count the occurrences of a char in a line: `line(L),occurrences(L,\"c\",N),write(N)`\n");
	printf("- Count the words of all lines, where words are separated by a space: `line(L),words(L,N),write(N)`\n");
	printf("For a full list of examples, see: https://github.com/damianoazzolini/lawk/README.md\n");
	printf("Report bugs as GitHub issues\n");
}

// --help -h
// --interactive -i
// --query -q
// lawq <file> [arguments]
void parse_arguments(int argc, char** argv) {
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--query") == 0) {
			if (i == argc - 1) {
				printf("Expected query");
				exit(MISSING_QUERY);
			}
			else {
				query = malloc(strlen(argv[i]) + 2);
				snprintf(query, strlen(argv[i]) + 1, "%s", argv[i]);
			}
		}
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_help();
			exit(PRINT_HELP);
		}
		else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
			interactive = 1;
		}
		else {
			filename = malloc(strlen(argv[i]) + 2);
			snprintf(filename, strlen(argv[i]) + 1, "%s", argv[i]);
		}
	}
}

FILE *open_file(const char *file) {
	FILE *fp = NULL;

	fp = fopen(file,"r");
	if(fp == NULL) {
		printf("File %s not found\n",file);
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
	// char *command = "line(L),words(L,N),write(\"ciao \",L, \" \", N)"; // OK
	// char* command = "line(I,L), reverse(L,LRev), write(LRev)"; // OK
	// char* command = "line(I, \"linea5\"), write(I)"; // OK
	// char* command = "line(4, \"linea5\")"; // OK
	// char* command = "line(L),append(L,\"abc\",LO), write(LO)"; // OK
	// char* command = "line(L),append(L,\"abc\",\"inea12abc\")"; // OK
	// char* command = "line(L),append(L,M,\"inea12abc\"),write(L, \"-> \",M)"; // OK
	// char* command = "line(L),append(L,M,\"inea12abc\"),write(L, \"-> \",M)"; // OK
	// char* command = "line(I,L),add(I,2,V),mul(V,I,V2),write(V2)"; // OK
	// char* command = "line(L),nth1(L,1,V),write(V)"; // OK 
	// char* command = "line(L),nth1(L,2,V),write(V)"; // OK
	// char* command = "line(I,L),nth1(L,2,\"hh\"),write(I)"; // OK
	// char* command = "line(I,L),nth1(L,2,\"hh\"),write(I)"; // OK
	// char* command = "line(I,L),nth1(L,2,\"hh\"),write(I)"; // OK
	// char* command = "line(L),replace(L,\"a\",\"b\",R),write(R)"; // OK


	// ---- NOT OK
	// char* command = "line(I,L),nth1(L,2,\"45\"),write(I)"; 
	// char* command = "line(I,L),nth1(L,2,45),write(I)";
	// char* command = "line(L),member(L,\"abc\"),write(L)";
	
	
	// char *command = "line(I,L), write(\"ciso\")"; // OK
	// char *command = "line(L),words(L,\"_\",N),write(N)"; // NEED to modify parser, otherwise loop, now there is a fallback
	char *command = malloc(256);
	// size_t n;
	printf("?- ");
	fgets(command, 256, stdin);
	command[strlen(command) - 1] = '\0';
	return command;
}

// TODO: store history of the commands

int main(int argc, char **argv) {
    reference_list ref_list;
	term_list t_list;
	FILE* fp, *outstream;
	char *command_in;
	const char *homedir;

	double exec_time = 0.0;

	ref_list.list = NULL;
	ref_list.n_elements = 0;

	t_list.n_elements = 0;
	t_list.list = NULL;

	parse_arguments(argc, argv);

	if (filename == NULL) {
		// filename = malloc(256);
		// printf("Insert filename: ");
		// scanf("%s",filename);
		printf("Missing filename\n");
		exit(MISSING_FILENAME);
	}

	// TODO: store command history
	// if ((homedir = getenv("HOME")) == NULL) {
    // 	homedir = getpwuid(getuid())->pw_dir;
	// }

	// printf("home: %s\n",homedir);

	// printf("filename: %s\n",filename);
	fp = open_file(filename);

	
	command_in = read_command();
	while(command_in != NULL && strcmp(command_in,"halt") != 0 && strcmp(command_in,"quit") != 0 && strcmp(command_in,"exit") != 0 ) {
		// char *command_in = "line(I,L),write(L)";

		printf("Command: %s\n", command_in);
		if(strcmp(command_in,"help") == 0) {
			print_help();
		}
		else if (strcmp(command_in,"nl") == 0) {
			printf("\n");
		}
		else {
			parse_command(command_in, &t_list, &ref_list);

			outstream = stdout;
			
			exec_time = exec_command(fp, &t_list, &ref_list, outstream);

			printf("Executed in %lfs\n", exec_time < 0 ? 0 : exec_time);

			free_reference_list(&ref_list);
			free_term_list(&t_list);
		}
		
		free(command_in);
		command_in = NULL;
		command_in = read_command();		
	}

	fclose(fp);
	free(command_in);
	free(filename);
	printf("Bye\n");
	return SUCCESS;
}