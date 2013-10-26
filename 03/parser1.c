#include <stdio.h>
#include <stdlib.h>
#include "token.h"

static void stmt(void);
static void expr(void);
static void var(void);
static void con(void);

/*
   simplest grammar

   stmt ::= var OP expr 
   expr ::= var | con
   var  ::= ID
   con  ::= LIT
*/

int main() {
    initline();
    gettoken();
  
    stmt();

    printf("\n");
    return 0;
}

static void stmt() {
    Token *t = &tok;

    var() ;

    if (t->sym == OP) 
        gettoken();
    else
        parse_error("expected op"); 

    expr();
    printf("stmt::=var OP expr\n");
}

static void var() {
    Token *t = &tok;

    if (t->sym == ID) {
        gettoken();
        printf("var::=ID\n");
    } else 
        parse_error("expected ID");
}

static void con() {
    Token *t = &tok;

    if (t->sym == LIT) {
        gettoken();
        printf("con::=LIT\n");
    } else 
        parse_error("expected LIT");
}

static void expr() {
    Token *t = &tok;

    switch (t->sym) {
        case ID : var(); printf("expr::=var\n"); break;
        case LIT: con(); printf("expr::=con\n"); break;
        default: parse_error("expected ID or LIT"); break;
    }
}
