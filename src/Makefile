CFLAGS= -Wall -Wextra -pedantic
CFLAGSPEDANTIC= -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wwrite-strings -Wunreachable-code -Winit-self -fsanitize=address -g
COMMONFLAGS= -Os
CC=gcc

COMPILE=${CC} ${CFLAGS} ${CFLAGSPEDANTIC} ${COMMONFLAGS}

lawk: main.o parser.o error.o exec.o
	${COMPILE} -o lawk main.o parser.o exec.o error.o -lc
	
main.o: main.c parser.h defines.h exec.h
	${COMPILE} -c main.c

parser.o: parser.c parser.h error.h defines.h
	${COMPILE} -c parser.c

exec.o: exec.c parser.h error.h defines.h
	${COMPILE} -c exec.c

error.o: error.c error.h
	${COMPILE} -c error.c

clean:
	rm lawk *.o

# check:
# 	@echo "tests"