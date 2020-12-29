Idea: reasoning about file, perform operations on files, a là grep/awk ecc but with a logic-like language.

Structure: written in C++, usage of mmap, interpreter of the commands (prolog like)

## Examples
Variable are indicated with:
- `+` input variable
- `-` output variable
- `--` ground (constant)
- `?` can be a combination of the three above

- `line/1` - `line(?Line)` represents a file line
line/2 represents a file line and its line number (starting from 1): line(Number,Content)
write/1 print the variable
length/2 - length(+Line,?Len) check the length of the line, true if the lenght of the line is equal to Len, which can also be an integer
even/1 - even(+Number) checks that the number is evem
odd/1 - odd(+Number) checks that the number is odd

mod/3 - mod(+Number,-Mod,-Res)

lt/2 less than (arithmetic <)
gt/2 greater than (arithmetic >)
startswith/2 line that starts with
endswith/2 line that ends with
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

Implemented
- Print the first line: `line(1,L), write(L)`
- Print the length of line 1: `line(1,L), length(L,N), write(N)`
- Print all the even positioned lines: `line(I,L),even(I), write(L)`
- Print all the lines with length 3: `line(L),length(L,3), write(L)`

Priority
- Print all lines at step of 3: `line(I,L),mod(I,3,0), write(L)`
- Print all the lines with length greater than 3: `line(L),length(L,N),gt(N,3),write(L)` 
- Find all the lines of a file between 5 and 10: `line(I,L),between(5,10,I), write(L)`
- Find the first 5 lines (head -n 5): `line(I,L), lt(I,5), write(L)`
- Find all the lines that contains at least one number: `line(L),member(L,N),number(N), write(L)`
- Count the words of a line: `line(1,L),words(L,N),write(N)`
- Count the occurrences of a char in a line: `line(L),occurrences(L,c,N),write(N)`

Queue
- Print the index of the line that is equal to abc: `line(I,abc)`
- Print all the even positioned lines and index: `line(I,L),even(I),write([L,I])`
- Find all the lines that start with a certain pattern (abc): `line(L),startswith(L,"abc"), writeln(L)`
- Find all the lines that end with a certain pattern (abc): `line(L),endswith(L,"abc"), write(L)`
- Find all the lines that contains a certain pattern (abc): `line(L),member(L,"abc"),write(L)`
- Find all the lines that contains a certain pattern: `line(L),match(L,"*a"),write(L)`
- Count the words of all lines, where words are separated by a certain character (`_` in this example): `line(L),words(L,'_',N),write(N)`


You can also operate on file:
- Count file line: `file(F),count_lines(F,N),writeln(N)`
- Count the occurrences of a char in the file: `file(F),count_chars(F,c,N),writeln(N)`
- Sum of all the length of all the lines: `file(F),count_chars(F,N),writeln(N)`
- Count the words of all lines: `line(L),words(L,N),write(N)` <-- check

To check
- Find all the second field of the list separated by a certain character: `line(L),nth1(2,L,E,' '),write(E)`
- Find all the lines that contains only numbers: findall(L,(line(L),forall(member(L,N),number(N))),Lines). <-- check
- Find all the lines that, when evaluated, results to a certain value (e.g. 5): findall(L,(line(L),eval(L,5)),Lines).

If you want to count lines, you need to activate count mode with:
`?- count.`
- Count the words of the line
- Count the words of a line, but words are separated by a certain char
- Count the occurrences of a char in a line  

- line(I,L), even(I), count(L,C), write(C)
oppure
- ?- count.
- ?- line(I,L), even(I).
- Count the lines that satisfy a particular condition: 