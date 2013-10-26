#include <stdio.h>
#include <stdlib.h>
#include "token.h"

static void stmt(void);
static void call(void);
static void arglist(void);
static void args(void);
static void args0(void);
static void arg(void);
static void expr(void);
static void var(void);
static void con(void);
static void name(void);

/*
   grammar with recursive call

   stmt ::= call (asn is omitted)
   call ::= name arglist
   arglist ::= e | '(' args ')'
   args ::= arg args0
   args0 ::= e | ',' arg args0
   arg ::= expr
   expr ::= var | con
   var ::= ID
   con ::= LIT
   name ::= ID
*/

int main() {
    initline();
    gettoken();
  
    stmt();

    printf("\n");
    return 0;
}

static void stmt() {
    call();
    printf("stmt::=call\n");
}

static void call() {
    Token *t = &tok;

    name();
    arglist();
    printf("call::=name arglist\n");
}

static void arglist() {
    Token *t = &tok;

    if (t->sym == ';') { /* FOLLOW(arglist) */
        printf("arglist::=e\n"); return;
    }

    if (t->sym == '(') 
        gettoken();
    else
        parse_error("expected (");

    args();

    if (t->sym == ')') 
        gettoken();
    else
        parse_error("expected )");

    printf("arglist::=( args )\n");
}

static void arg() { expr(); printf("arg::=expr\n"); }

static void args() {
    Token *t = &tok;

    arg();
    args0();
    printf("args::=arg args0\n");
}

static void args0() {
    Token *t = &tok;

    if (t->sym == ')') { /* FOLLOW(args0) */
        printf("args0::=e\n"); return;
    }

    if (t->sym == ',') 
        gettoken();
    else {
        parse_error("expected , or )");

/* terminate recursion when ) is missing */
        if (t->sym == ';') return;
	gettoken();
    }

    arg();
    args0();

    printf("args0::= , arg args0\n");
}

static void name() {
    Token *t = &tok;

    if (t->sym == ID) {
        gettoken();
        printf("name::=ID<%s>\n",t->text);
    } else 
        parse_error("expected ID");
}

static void var() {
    Token *t = &tok;

    if (t->sym == ID) {
        gettoken();
        printf("var::=ID<%s>\n",t->text);
    } else 
        parse_error("expected ID");
}

static void con() {
    Token *t = &tok;

    if (t->sym == LIT) {
        gettoken();
        printf("con::=LIT<%d>\n",t->ival);
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
