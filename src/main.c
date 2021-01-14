#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <assert.h>
// #include <fcntl.h>
// #include <sys/types.h>
// #include <sys/stat.h>
/*
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
*/
// #include <sys/io.h>
// #include <sys/mman.h>
// #include <unistd.h>
// #include <pwd.h>

#include "defines.h"
#include "parser.h"
#include "exec.h"

const char *program_version = "0.0.1";
const char *query_usage = "Usage: lawk <file> [arguments]"; 
// const char *history_file = ".lawk_history"; 
const char *predicates_list[] = {
	"line/1,2: line",
	"write/n: print the content of the variable(s)",
	"length(L,N): N is length of L",
	"even(I): checks if I is even",
	"odd(I): checks if I is odd",
	"mod(A,B,C): C = A mod B",
	"gt(A,B): true if A > B",
	"between(A,B,C): true if A is between B and C",
	"lt(A,B): true if A < B",
	"occurrences(A,B,C): count the occurrences of B in A and save in C",
	"startswith(A,B): true if A starts with B",
	"endswith(A,B): true if A ends with B",
	"words(A,B): B is the number of words in A (separated by space)",
	"words(A,B,C): C is the number of words in A (separated by B)",
	"reverse(A,B): true B is A reversed",
	"append(A,B,C): C is B appended to A, C can be ground",
	"add(A,B,C): C is A + B",
	"mul(A,B,C): C is A * B",
	"sub(A,B,C): C is A - B",
	"div(A,B,C): C is A \\ B",
	"nth1(A,B,C): C is the B-nth char of A counting from 1, C can be ground",
	"nth1_word(A,B,C): C is the nth word of A, B can be ground",
	"nth1_word(A,B,C,D): D is the nth word of A, where words are separated by char C, D can be ground",
	"replace(A,B,C,D): D is A where char B is replaced by char C"
}; 

typedef struct command_line_arguments {
	char *filename;
	char *query;
	int interactive;
	int verbose;
} command_line_arguments;

void init_command_line_arguments(command_line_arguments *cla) {
	cla->filename = NULL;
	cla->query = NULL;
	cla->interactive = 0;
	cla->verbose = 0;
}

void print_help() {
	printf("lawk version %s\n", program_version);
	printf("Analizing files with a logic-like language\n");
	printf("Repository: https://github.com/damianoazzolini/lawk\n");
	printf("%s\n",query_usage);
	printf("Query examples:\n");
	printf("- Print the first line: `line(1,L), write(L)`\n");
	printf("- Count the occurrences of a char in a line: `line(L),occurrences(L,\"c\",N),write(N)`\n");
	printf("- Count the words of all lines, where words are separated by a space: `line(L),words(L,N),write(N)`\n");
	printf("For a full list of examples, see: https://github.com/damianoazzolini/lawk/README.md\n");
	printf("Report bugs as GitHub issues\n");
}

void print_detail_predicates() {
	int i;
	int n_predicates = 24;
	for(i = 0; i < n_predicates; i++) {
		printf("%s\n",predicates_list[i]);
	}
}

// --help -h
// --interactive -i (unused)
// --query -q
void parse_arguments(int argc, char** argv, command_line_arguments *cla) {
	int i;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--query") == 0) {
			if (i == argc - 1) {
				printf("Expected query");
				printf("%s\n",query_usage);
				exit(MISSING_QUERY);
			}
			else {
				cla->query = argv[i + 1];
				i++;
			}
		}
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			print_help();
			exit(PRINT_HELP);
		}
		else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
			cla->interactive = 1;
		}
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
			cla->verbose = 1;
		}
		else {
			cla->filename = argv[i];
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
	// ---- NOT OK
	// char* command = "line(L),replace(L,\"a\",\"b\",R),write(R)"; // OK
	// char* command = "line(I,L),nth1(L,2,\"45\"),write(I)"; 
	// char* command = "line(I,L),nth1(L,2,45),write(I)";
	// char* command = "line(L),member(L,\"abc\"),write(L)";
	
	char *command = malloc(256);
	// size_t n;
	printf("?- ");
	// fgets cannot manage arrow keys
	if(fgets(command, 256, stdin) == NULL) {
		free(command);
		command = NULL;
	}
	else {
		command[strlen(command) - 1] = '\0';
	}
	return command;
}

// TODO: store history of the commands
int main(int argc, char **argv) {
	command_line_arguments cla;
    reference_list ref_list;
	term_list t_list;
	FILE* fp, *outstream;
	char *command_in;
	double exec_time = 0.0;
	// char **commands_history = NULL;
	// int n_commands = 0;
	// const char *homedir;

	init_command_line_arguments(&cla);

	ref_list.list = NULL;
	ref_list.n_elements = 0;

	t_list.n_elements = 0;
	t_list.list = NULL;

	parse_arguments(argc, argv, &cla);

	if (cla.filename == NULL) {
		printf("Missing filename\n");
		printf("%s\n",query_usage);
		exit(MISSING_FILENAME);
	}

	// TODO: store command history
	// if ((homedir = getenv("HOME")) == NULL) {
    // 	homedir = getpwuid(getuid())->pw_dir;
	// }

	// printf("home: %s\n",homedir);

	// printf("Query: %s\n",query);

	// printf("filename: %s\n",filename);
	fp = open_file(cla.filename);

	
	if(cla.query == NULL) {
		command_in = read_command();
	}
	else {
		command_in = cla.query;
	}
		
	while(command_in != NULL && strcmp(command_in,"halt") != 0 && strcmp(command_in,"quit") != 0 && strcmp(command_in,"exit") != 0 ) {
		// printf("Command: %s\n", command_in);
		if(strcmp(command_in,"help") == 0) {
			print_help();
		}
		else if (strcmp(command_in,"nl") == 0) {
			printf("\n");
		}
		else if (strcmp(command_in,"list") == 0) {
			print_detail_predicates();
		}
		else {
			// commands_history = realloc(commands_history,sizeof(char *) * (n_commands + 1));
			// commands_history[i] = malloc(strlen(command_in) + 2);
			// snprintf(commands_history[i],strlen(command_in) + 1,"%s",command_in);
			// n_commands++;

			parse_command(command_in, &t_list, &ref_list);

			outstream = stdout;
			
			exec_time = exec_command(fp, &t_list, &ref_list, outstream);

			if(cla.verbose == 1) {
				printf("Executed in %lfs\n", exec_time < 0 ? 0 : exec_time);
			}

			free_reference_list(&ref_list);
			free_term_list(&t_list);
		}
		
		if(command_in == cla.query) {
			command_in = NULL;
			break;
		}
		else {
			free(command_in);
			command_in = NULL;
			command_in = read_command();
		}

		rewind(fp);
	}

	fclose(fp);
	if(command_in != NULL) {
		free(command_in);
	}
	if(cla.query == NULL) {
		printf("Bye\n");
	}

	return SUCCESS;
}