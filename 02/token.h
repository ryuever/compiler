#ifndef _TOKEN_H_
#define _TOKEN_H_

#define MAX_LEXEME 255
#define MAX_LINE   1000

enum { false=0, true=1 };

enum tokentype {ID=256, LIT, OP, CMT,
tIF, tELSE, tSWITCH, tCASE, tDEFAULT,
tBREAK, tCONTINUE, tWHILE, tDO, tFOR, tCALL, tRETURN,
tEND, ERROR=-1 };

typedef struct Token {
    enum tokentype sym;
    char text[MAX_LEXEME+1];
    int  index;
    int  ival;
    char *sval;
} Token ;

extern Token tok;
Token *gettoken(void);

typedef struct Line {
    int no;
    int pos;
    int backed;		/* ch backed by backch */
    char buf[MAX_LINE];
} Line;

extern Line line;

void initline(void);
int nextch(void);
void backch(int);
int prevch(void);
void clear_lexeme(void);

void outch(int);
void delete_prev(void);

int getlineno(void);
int getlinepos(void);

typedef struct kwentry {
    char *text;
    int sym;
} kwentry;

kwentry *kwlookup(const char*);

#endif
