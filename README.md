# lawk
Perform operations on files, a la grep/awk ecc, but with a logic-like language.

## How to Build
```
cd src
make
```
and then
```
./lawk <filename> [OPTIONS]
```

## Command Line Arguments
- `-q --query`: specify a query (see examples below)
- `-h --help`: print help
- `-v --verbose`: verbose

## How to Use
By default, when called only with filename, it enters an interactive mode: you can specify a query and then ask it with return. 
After the query, you can ask another query and so on... 
To exit from the interactive mode, you have several options: `halt`, `exit`, or `quit`.
In interactive mode, you can print the help with `help`. 
With `list` you get a list of available predicates.

## Quick Tutorial
TODO

Since this program consider one line at the time, you cannot (by now), do something like line(I,L), add(I,1,I1), line(I1,L1). In general, you must have only one call to line/1,2 in a query.

## Syntax
Variables are denoted with:
- `+` input variable
- `-` output variable
- `--` ground (constant)
- `?` can be a combination of the three above

- `line/1` - `line(?Line)`: `Line` contains a file line
- `line/2` - `line(?Line,?Number)`: `Line` represents a file line and `Number` its line number (counting from 1)
- `write/n` prints the variable(s) with the specified format
- `length/2` - `length(+Line,?Len)`: `Len` represents the length of the line, true if the length of the line is equal to `Len`
- `even/1` - `even(+Number)`: true if `Number` is even
- `odd/1` - `odd(+Number)`: true if `Number` is odd
- `mod/3` - `mod(+Number,+Mod,-Res)`: `Res` is `Number` modulo `Mod`
- `lt/2` - `lt(+NumberA,+NumberB)`: true if `NumberA` is less than `NumberB` (arithmetic <)
- `gt/2` - `gt(+NumberA,+NumberB)`: true if `NumberA` is greater than `NumberB` (arithmetic >)
- `between/3` - `between(+Number,+Lower,+Upper)`: true if `Lower < Number < Upper`
- `occurrences/3` - `occurrences(+Line,+Char,?Occ)`: `Occ` is the number of occurrences of char `Char` in line `L`, `Occ` can be a number
- `startswith/2` - `startswith(+Line,+Str)`: succeeds if the line starts with `Str`
- `endswith/2` - `endswith(+Line,+Str)`: succeeds if the line ends with `Str`
- `reverse/2` - `reverse(+Line,?Rev)`: `Rev` is `Line` reversed, `Rev` can be a constant
- `append/3` - `append(+Line,?ToApp,?Res)`: append `ToApp` to `Line`, `ToApp` and `Res` can be constants
- `words/2,3` - `words(+Line,+Sep,?Count)`: `Count` is the number of words of `Line`, where words are separated by `Sep`. Can be used also as `word(+Line,?Count)`, with space as default separator
- `swap/4` - `swap(+Line,+PosA,+PosB,?Res)`: `Res` is `Line` where chars in `PosA` and `PosB` are swapped. `Res` can be ground
- `nth1/3` - `nth1(+Line,+Nth,?Res)`: `Res` is the `Nth` char of `Line`, `Res` can be ground
<!-- member/2 line that contains -->
<!-- eval/2 used to evaluate the expression represented by the line -->
<!-- number/1 checks that the element is a number -->
<!-- letter/1 checks that the element is a letter -->
<!-- lowerLetter/1 checks that the element is a lower case letter -->
<!-- upperLetter/1 checks that the element is an upper case letter -->
<!-- alpha/1 checks that the element is an alphanumeric value -->
<!-- match/2 apply pattern matching -->
sumlist/2 sum of the list

# Implemented Predicates with Examples
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
- Print the content of the lines reversed: `line(I,L), reverse(L,LRev), write(LRev)`. If LRev is ground then returns true if the string reversed is equal to it. 
- Check if the content of the line is the specified (print true or false): `line(4, line5)`
- Append a string to a line: `line(I,L), append(L,"ciao",LO), write(LO)`. `append(L,M,labc)` -> find what is missing to have `labc` starting from L, `append(L,"abc",labc)` -> true / false, `append(L, "abc", LO)` -> `LO = Labc`  
- Print the index of the line that is equal to abc: `line(I,"abc"),write(I)`
- Perform arithmetic operations: `line(I,L),add(I,2,V),mul(V,I,V2),write(V2)`
- Find the nth character of a line: `line(L),nth1(L,2,V),write(V)`
- Find the nth word of a line: `line(L),nth1_word(L,2,V),write(V)`
- Find the nth word of a line, where elements are separated by a certain character (`_` in this example): `line(L),nth1_word(L,2,"_",V),write(V)`
- Swap the second and the fourth character (counting from 1): `line(L),swap(L,2,4,S),write(S)`

Priority (still to implement)
- Replace all the occurrences of a string with another: `line(L),replace(L,\"a\",\"b\",R),write(R)`
- Find first/last 10 chars: `line(L),first(L,10,R),write(R)` similarly with `last/2`

Queue (still to implement)
- Find all the lines that contain at least one number: `line(L),member(L,N),number(N), write(L)`
- Find all the lines that contain at least one number greater than 3: `line(L),member(L,N),gt(N,3), write(L)`
- Find and replace
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

## Current Limitations (or Bugs)
- all the predicates does not work when a string is expected but a number is found. For example: "line(I,L),nth1(L,2,\"45\"),write(I)" or "line(I,L),nth1(L,2,45),write(I)", 45 is treated as a number so it is not catched in the if. TODO: store the int in the list, so remove the union
- a full test module is still missing (but is as fundamental as boring to write)

## How to Contribute
In the way you prefer: issues (also suggesting features) or pull requests

## But Why?
The idea for this software came from [this video](https://www.youtube.com/watch?v=kGQNeeRp4sM)