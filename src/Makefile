CFLAGS= -Wall -Wextra -pedantic
CFLAGSPEDANTIC= -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wwrite-strings -Wunreachable-code -Winit-self -fsanitize=address -g
COMMONFLAGS= -Os
CC=gcc

COMPILE=${CC} ${CFLAGS} ${CFLAGSPEDANTIC} ${COMMONFLAGS}

lawk: main.o
	${COMPILE} -o lawk main.o -lc
	
main.o: main.c parser.h
	${COMPILE} -c main.c


# lawk: main.o parser.o error.o
# 	${COMPILE} -o lawk main.o parser.o error.o -lc
	
# main.o: main.cpp parser.h error.h defines.h
# 	${COMPILE} -c main.cpp

# parser.o: parser.cpp parser.h error.h defines.h
# 	${COMPILE} -c parser.cpp

# error.o: error.cpppp error.h
# 	${COMPILE} -c error.cpp

# clean:
# 	rm lawk *.o *.so

# check:
# 	@echo "tests"