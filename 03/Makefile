CC = gcc -g

all: parser1 parser2 parser3 parser4 scanner

parser1: parser1.o scanner.o token.o
	$(CC) $(CFLAGS) -o parser1 parser1.o scanner.o token.o
parser2: parser2.o scanner.o token.o
	$(CC) $(CFLAGS) -o parser2 parser2.o scanner.o token.o
parser3: parser3.o scanner.o token.o ast.o 
	$(CC) $(CFLAGS) -o parser3 parser3.o scanner.o token.o ast.o
parser4: parser4.o scanner.o token.o ast.o 
	$(CC) $(CFLAGS) -o parser4 parser4.o scanner.o token.o ast.o

scanner: scanner.c token.o 
	$(CC) $(CFLAGS) -o scanner -DTEST_SCANNER scanner.c token.o

parser1.o : parser1.c token.h
parser2.o : parser2.c token.h
parser3.o : parser3.c token.h ast.h 
parser4.o : parser4.c token.h ast.h 
token.o : token.c token.h
ast.o : ast.c ast.h

clean:
	-rm scanner parser? *.o out? core*
