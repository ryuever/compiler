CC = gcc -g

all: scanner

scanner : scanner.o token.o
	$(CC) $(CFLAGS) -o scanner scanner.o token.o
scanner.o : scanner.c token.h
token.o : token.c token.h

clean:
	-rm scanner *.o out? core*
