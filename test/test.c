#include <stdio.h>
#include "../src/exec.h"

int exec_test(char* test_name, char* command_in, char* expected_result) {

}

int main2() {
	FILE* file_out = "temp.tmp";
	FILE* file_in = "../src/test.txt";
	// parse_command(command_in, &t_list, &ref_list);

	char *commands[] = {
		"line(1,L), write(L)",
		"line(1,L), length(L,N), write(N)",
		"line(I,L),even(I), write(L)",
		"line(L),length(L,3), write(L)",
		"line(I,L),mod(I,3,0), write(L)",
		"line(L),length(L,N),gt(N,3),write(L)",
		"line(I,L),between(I,5,10), write(L)",
		"line(I,L), lt(I,5), write(L)",
		"line(L),occurrences(L,\"c\",N),write(N)",
		"line(L),occurrences(L,\"abc\",N),write(N)",
		"line(L),startswith(L,\"abc\"), write(L)",
		"line(L),endswith(L,\"abc\"), write(L)",
		"line(L),words(L,N),write(N)",
		"line(L),words(L,\"_\",N),write(N)",
		"line(I,L),even(I),write(L,\" \",I)",
		"line(I,L), reverse(L,LRev), write(LRev)",
		"line(4, line5)",
		"line(I,L), append(L,\"ciao\",LO), write(LO)",
		"append(L,\"abc\",\"labc\")",
		"append(L, \"abc\", LO)",
		"line(I,L),add(I,2,V),mul(V,I,V2),write(V2)",
		"line(I,L),nth1(L,1,V),write(V)",
		"line(I,L),nth1(L,2,V),write(V)",
		"line(L),nth1(L,1,\"_\",V),write(V)",
		"line(I,L),nth1(L,2,\"hh\"),write(I)",
		"line(L),replace(L,\"a\",\"b\",R),write(R)"
	};
	

	//outstream = stdout;

	// exec_time = exec_command(fp, &t_list, &ref_list);
}
