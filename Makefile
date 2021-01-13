CFLAGS= -Wall -Wextra -pedantic -g
# CFLAGSPEDANTIC= -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wwrite-strings -Wunreachable-code -Winit-self -fsanitize=address -g
CFLAGSPEDANTIC= -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wwrite-strings -Wunreachable-code -Winit-self -g
# COMMONFLAGS= -Os
CC=gcc

COMPILE=${CC} ${CFLAGS} ${CFLAGSPEDANTIC} ${COMMONFLAGS}


lawk: main.o parser.o error.o exec.o
	${COMPILE} -o lawk main.o parser.o exec.o error.o -lc
	
main.o: src/main.c src/parser.h src/defines.h src/exec.h
	${COMPILE} -c src/main.c

parser.o: src/parser.c src/parser.h src/error.h src/defines.h
	${COMPILE} -c src/parser.c

exec.o: src/exec.c src/parser.h src/error.h src/defines.h
	${COMPILE} -c src/exec.c

error.o: src/error.c src/error.h
	${COMPILE} -c src/error.c

clean:
	@rm lawk *.o

# check:
# 	@echo "tests"