# lawk
Perform operations on files, a la grep/awk ecc, but with a logic-like language.

## Examples
Variable are indicated with:
- `+` input variable
- `-` output variable
- `--` ground (constant)
- `?` can be a combination of the three above

- `line/1` - `line(?Line)` represents a file line
- line/2 represents a file line and its line number (starting from 1): line(Number,Content)
- write/n print the variable(s) with the specified format
- length/2 - length(+Line,?Len) check the length of the line, true if the lenght of the line is equal to Len, which can also be an integer
- even/1 - even(+Number) checks that the number is evem
- odd/1 - odd(+Number) checks that the number is odd
- mod/3 - mod(+Number,-Mod,-Res)
- lt/2 less than (arithmetic <)
- gt/2 greater than (arithmetic >)
- between/3
- occurrences/3
- startswith/2 line that starts with
- endswith/2 line that ends with
- reverse/2
- append/3
- words/3

member/2 line that contains
eval/2 used to evaluate the expression represented by the line
number/1 checks that the element is a number
letter/1 checks that the element is a letter
lowerLetter/1 checks that the element is a lower case letter
upperLetter/1 checks that the element is an upper case letter
alpha/1 checks that the element is an alphanumeric value
forall/2 apply a pattern to all the elements
match/2 apply pattern matching
sumlist/2 sum of the list
nth1/4 index list element separator character

# Implemented Predicates
- Print the first line: `line(1,L), write(L)`
- Print the length of line 1: `line(1,L), length(L,N), write(N)`
- Print all the even positioned lines: `line(I,L),even(I), write(L)`
- Print all the lines with length 3: `line(L),length(L,3), write(L)`
- Print all lines at step of 3: `line(I,L),mod(I,3,0), write(L)`
- Print all the lines with length greater than 3: `line(L),length(L,N),gt(N,3),write(L)` 
- Print all the lines of a file between 5 and 10: `line(I,L),between(I,5,10), write(L)`
- Print the first 5 lines (head -n 5): `line(I,L), lt(I,5), write(L)`
- Count the occurrences of a char in a line: `line(L),occurrences(L,"c",N),write(N)`
- Count the occurrences of a substring in a line: `line(L),occurrences(L,"abc",N),write(N)`
- Find all the lines that start with a certain pattern (abc): `line(L),startswith(L,"abc"), write(L)`
- Find all the lines that end with a certain pattern (abc): `line(L),endswith(L,"abc"), write(L)`
- Count the words of all lines, where words are separated by a space: `line(L),words(L,N),write(N)`
- Count the words of all lines, where words are separated by a certain character (`_` in this example): `line(L),words(L,"_",N),write(N)`
- Print all the even positioned lines and index separated by a space: `line(I,L),even(I),write(L," ",I)`
- Print the content of the lines reversed: `line(I,L), reverse(L,LRev), write(LRev)`
- Check if the content of the line is the specified (print true or false): `line(4, line5)`
- Append a string to a line: `line(I,L), append(L,"ciao",LO), write(LO)`. append(L,M,labc) -> find what is missing to have labc starting from L, append(L,"abc",labc) -> true / false, append(L, "abc", LO)->LO = Labc  
- Print the index of the line that is equal to abc: `line(I,"abc"),write(I)`
- Perform arithmetic operations: `line(I,L),add(I,2,V),mul(V,I,V2),write(V2)`
- Find the nth element of a line: `line(L),nth1(L,2,V),write(V)`
- Find the nth element of a line, where elements are separated by a certain character (`_` in this example): `line(L),nth1(L,2,"_",V),write(V)`
- Replace all the occurrences of a string with another: `line(L),replace(L,\"a\",\"b\",R),write(R)`
Priority
- Find all the lines that contain at least one number: `line(L),member(L,N),number(N), write(L)`
- Find all the lines that contain at least one number greater than 3: `line(L),member(L,N),gt(N,3), write(L)`
- Find and replace

Queue
- Find all the lines that contain a certain pattern (abc): `line(L),member(L,"abc"),write(L)`
- Find all the lines that contain a certain pattern: `line(L),match(L,"*a"),write(L)`

<!-- You can also operate on file:
- Count file line: `file(F),count_lines(F,N),writeln(N)`
- Count the occurrences of a char in the file: `file(F),count_chars(F,c,N),writeln(N)`
- Sum of all the length of all the lines: `file(F),count_chars(F,N),writeln(N)`
- Count duplicated lines: `file(F),count_duplicated(F,N),writeln(N)` 
- Count duplicated lines and print those: `file(F),count_duplicated(F,N,L),writeln(N,L)` 

To check
- Find all the lines that contain only numbers: findall(L,(line(L),forall(member(L,N),number(N))),Lines). <-- check
- Find all the lines that, when evaluated, results to a certain value (e.g. 5): findall(L,(line(L),eval(L,5)),Lines).
- HOW TO HANDLE NEGATION?

If you want to count lines, you need to activate count mode with:
`?- count.`
- Count the words of the line
- Count the words of a line, but words are separated by a certain char
- Count the occurrences of a char in a line

- line(I,L), even(I), count(L,C), write(C)
oppure
- ?- count.
- ?- line(I,L), even(I).
- Count the lines that satisfy a particular condition:  -->

## Current limitations
- all the predicates does not work when a string is expected but a number is found. For example: "line(I,L),nth1(L,2,\"45\"),write(I)" or "line(I,L),nth1(L,2,45),write(I)", 45 is treated as a number so it is not catched in the if. TODO: store the int in the list, so remove the union
