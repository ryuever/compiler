#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "token.h"
#include "ast.h"

static AST stmts(void);
static AST stmt(void);
static AST asn(void);
static AST call(void);
static AST arglist(void);
static AST args(void);
static AST args0(void);
static AST arg(void);
static AST expr(void);
static AST var(void);
static AST con(void);
static AST name(void);

/*
   grammar with AST (recursive)

   stmts ::= e | stmt ';' stmts 
   stmt ::= asn | call
   asn  ::=var OP expr
   call ::= name arglist
   name ::= ID
   arglist ::= e | '(' args ')'
   args ::= arg args0
   args0 ::= e | ',' arg args0
   arg ::= expr
   expr ::= var | con
   var ::= ID
   con ::= LIT
*/

static AST ast_root;

int main() {
    initline();
    init_AST();

    gettoken();
    ast_root = stmts();

    printf("\n");
    print_AST(ast_root);
    printf("\n");
    return 0;
}

static AST stmts() {
    Token *t = &tok;

    AST a;
    AST a1=0, a2=0;

    if (t->sym == ID || t->sym == tCALL) { /* FIRST(stmt) */
        a1 = stmt();

        if (t->sym == ';')
            gettoken();
        else
            parse_error("expected ;");

        a2 = stmts(); 
    }
    a = make_AST(nSTMTS, a1, a2, 0, 0);
    return a;
}

static AST stmt() {
    Token *t = &tok;
    AST a;
    AST a1=0;

    switch (t->sym) {
      case ID:
        a1 = asn();
	break;
      case tCALL:
        gettoken();
	a1 = call();
	break;
      default:
        break;
    }
    a = make_AST(nSTMT, a1, 0, 0, 0);
    return a;
}

static AST asn() {
    Token *t = &tok;
    AST a;
    AST a1=0, a2=0, a3=0;
    int op;

    a1 = var();

    if (t->sym == OP) {
        op = t->ival;
        gettoken();
        a2 = make_AST_op(op);
    } else
        parse_error("expected OP");

    a3 = expr();
    a = make_AST(nASN, a1, a2, a3, 0);
    return a;
}

static AST call() {
    Token *t = &tok;
    AST a;
    AST a1=0, a2=0;

    a1 = name();
    a2 = arglist();
    a = make_AST(nCALL, a1, a2, 0, 0);;
    return a;
}

static AST arglist() {
    Token *t = &tok;
    AST a;
    AST a1=0;

    if (t->sym == ';') { /* FOLLOW(arglist) */
        return make_AST(nARGLIST, 0, 0, 0, 0);
    }

    if (t->sym == '(')
        gettoken();
    else
        parse_error("expected (");

    a1 = args();

    if (t->sym == ')')
        gettoken();
    else
        parse_error("expected )");

    a = make_AST(nARGLIST, a1, 0, 0, 0);
    return a;
}

static AST arg() {
    AST a;
    AST a1=0;

    a1 = expr(); 
    a = make_AST(nARG, a1, 0, 0, 0);
    return a;
}

static AST args() {
    Token *t = &tok;
    AST a;
    AST a1=0, a2=0;

    a1 = arg();
    a2 = args0();
    a = make_AST(nARGS, a1, a2, 0, 0);
    return a;
}

static AST args0() {
    Token *t = &tok;
    AST a;
    AST a1=0, a2=0;

    if (t->sym != ')') { /* FOLLOW(args0) */

      if (t->sym == ',')
         gettoken();
      else {
         parse_error("expected , or )");

	/* terminate recursion when ) is missing */
        if (t->sym == ';')
          goto out;

        gettoken();

      }

      a1 = arg();
      a2 = args0();
    }

out:
    a = make_AST(nARGS, a1, a2, 0, 0);
    return a ;
}

static AST id(int type) {
    Token *t = &tok;
    AST a;
    AST a1=0;

    if (t->sym == ID) {
	char *s = strdup(t->text);
        gettoken();
        a1 = make_AST_id(s);
    } else 
        parse_error("expected ID");
    
    a = make_AST(type, a1, 0, 0, 0); 
    return a;
}

static AST name() { return id(nNAME); }
static AST var()  { return id(nVAR);  }

static AST con() {
    Token *t = &tok;
    AST a;
    AST a1=0;

    if (t->sym == LIT) {
	char *s = strdup(t->text);
        int v = t->ival;
        gettoken();
        a1 = make_AST_lit(s,v);
    } else 
        parse_error("expected LIT");

    a = make_AST(nCON, a1, 0, 0, 0);
    return a;
}

static AST expr() {
    Token *t = &tok;
    AST a;
    AST a1=0;

    switch (t->sym) {
      case ID : 
        a1 = var(); 
	break;
      case LIT: 
        a1 = con(); 
	break;
      default: 
	parse_error("expected ID or LIT"); 
	skiptoken(';');
	break;
    }
    a = make_AST(nEXPR, a1, 0, 0, 0);
    return a;
}
