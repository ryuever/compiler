#include <stdio.h>
#include <stdlib.h>
#include "token.h"

static Token *id(int);
static Token *cmt(int);
static Token *lit(int);
static Token *op(int);
static Token *op_eq(int);  /* OP end with = (+=. -=, == etc) */
static Token *op_dup(int); /* OP which dups the same char (++, --) */

static Token *gettoken0(void);

Token *gettoken() {
    Token *t;

    do {
      t = gettoken0();
    } while (t && (isspace(t->sym) || t->sym == CMT));

    if (t && t->sym == ID) {
        kwentry *e = kwlookup(t->text);
        if (e) t->sym = e->sym;
    }
    return  t;
}

void skiptoken(int sym) {
    Token *t=&tok;

    while (t && t->sym != '\n') {
       if (t->sym == sym)  break;
       t = gettoken0();
    }
}

void skiptoken2(int sym1,int sym2) {
    Token *t=&tok;
    while (t && t->sym != '\n') {
       if (t->sym == sym1)  break;
       if (t->sym == sym2)  break;
       t = gettoken0();
    }
}

static Token *gettoken0() {
    int ch;
    Token *t = &tok;
    t->index = 0;

    ch = nextch();
    if (ch == EOF) { return (Token*)0; }

    switch (codeof(ch)) {
        case '+': case '-': case '*': case'/': case '%':
        case '<': case '>': case '=': case '!': case '&' : case '|':
            t = op(ch); break;
        case 'Z':
            t = id(ch); break;
        case '0': case '9':
            t = lit(ch); break;
        case '@': 
/* printf("[line %d, pos %d]", getlineno(), getlinepos()); */
            t->sym = ' '; break;
        default:
            t->sym = ch;  break;
    }
    return t;
}

static Token *id(int ch) {
    Token *t = &tok;

    outch(ch);
    while (1) {
       ch = nextch();
       switch (codeof(ch)) {
          case 'Z': case '0': case '9':
	     outch(ch);
             continue;
          default: break;
       }
       backch(ch); 
       break;
    }
    t->sym = ID;
    outch(0);
    return t;
}

static int hexval(int ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return ERROR;
}

static Token *op(int ch) {
    Token *t = &tok;
    int ch2;
    int s = 1;

    t->ival = ch;
    switch (ch) {
       case '/':
          outch(ch);
          ch2 = nextch();
          if (ch2 == '*' || ch2 == '/') {
             return cmt(ch2);
          } else if (ch2 == '=') {
             return op_eq(ch2);
          }
          break;

       case '-':
          s = -1;
          /* through to next case */
       case '+':
          outch(ch);
          ch2 = nextch();
	  if (ch == ch2)
	      return op_dup(ch2);

          switch (codeof(ch2)) {
             case '=': 
                  return op_eq(ch2);
             case '0': case '9':
                  t = lit(ch2);
                  t->ival *= s;
                  return t;
             default: 
		  break;
          }
          break;

       case '&':
       case '|':
       case '<':
       case '>':
          outch(ch);
          ch2 = nextch();
	  if (ch == ch2) 
	    return op_dup(ch2);
          switch (codeof(ch2)) {
             case '=':
                 return op_eq(ch2);
             default:
                 break;
          }
          break;

       case '*':
       case '%':
       case '!':
       case '=':
          outch(ch);
          t->ival = ch;
          ch2 = nextch();
          if (ch2 == '=') 
             return op_eq(ch2);
          break;
       default: 
          ch2 = ch; 
          break;
    }
    backch(ch2);

    t->sym = OP;
    outch(0);
    return t;
}

static Token *op_eq(int ch) {
    Token *t = &tok;

    outch(ch);
    outch(0);
    t->sym = OP;
    t->ival = WITHEQ(t->ival);
    return t;
}

static Token *op_dup(int ch) {
    Token *t = &tok;

    outch(ch);
    outch(0);
    t->sym = OP;
    t->ival = WITHDUP(t->ival);
    return t;
}

static Token *lit(int ch) {
    Token *t = &tok;
    int d = 0;
    int v = 0;
    int b = 10;

    outch(ch);
    v = hexval(ch);

    if (ch == '0') {
        b = 8;
        ch = nextch();
        if (ch == 'x' || ch == 'X') {
            b = 16; outch(ch);
        } else 
            backch(ch);
    }

    while (1) {
       ch = nextch();
       d  = hexval(ch);
       switch (codeof(ch)) {
          case '0': case '9':
             if (d < b) {
                 outch(ch);
                 v = v * b + d;
                 continue;
             }
          case 'Z':
             if (b == 16 && d < b) {
                 outch(ch);
                 v = v * b + d;
                 continue;
             }
       }
       backch(ch); 
       break;
    }

    t->sym = LIT;
    outch(0);
    t->ival = v;
    return t;
}

/*
   If you keep all texts in COMMENT, it may exceed the length of MAX_LEXEME.
 */
static int keep = 0;

static Token *cmt(int ch) {
    Token *t = &tok;
    clear_lexeme();

    switch (ch) {
      case '/':
        while (1) {
          ch = nextch();
          if (ch == '\n' || ch == '\r') break;
          if (keep) outch(ch);
        }
        break;

      case '*':
        while (1) {
          ch = nextch();
          if (ch == '/' && prevch() == '*')
               { delete_prev(); break; } /* erase '*' */
          outch(ch);
        }
        break;

      default:
        break;
    }

    t->sym = CMT;
    outch(0);
    return t;
}

#ifdef TEST_SCANNER
int main() {
    Token *t;


fprintf(stderr,"isspace(256)=%d\n", isspace(256));

    initline();

    while ((t = gettoken()) != 0) {

fprintf(stderr,"t=%d\n", t->sym);

       if (t->sym < ID) {
           if (isspace(t->sym))
             printf("%c", t->sym);
	   else
             printf("SEP<%c> ", t->sym);
       } else switch (t->sym) {
          case ID : printf("ID<%s> ", t->text); break;
          case LIT: printf("LIT<%s>(%d) ", t->text, t->ival); break;
          case OP:  printf("OP<%s>(%d) ", t->text, t->ival); break;
          case tIF: printf("IF<> "); break;
          case tELSE: printf("ELSE<> "); break;
          case tSWITCH: printf("SWITCH<> "); break;
          case tCASE: printf("CASE<> "); break;
          case tDEFAULT: printf("DEFAULT<> "); break;
          case tBREAK: printf("BREAK<> "); break;
          case tCONTINUE: printf("CONTINUE<> "); break;
          case tWHILE: printf("WHILE<> "); break;
          case tDO: printf("DO<> "); break;
          case tFOR: printf("FOR<> "); break;
          case tCALL: printf("CALL<> "); break;
          case tRETURN: printf("RETURN<> "); break;
          default:  printf("ERROR<%s> ", t->text); break;
       }
    }
    printf("\n");
    return 0;
}
#endif
