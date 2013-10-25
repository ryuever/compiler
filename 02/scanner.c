#include <stdio.h>
#include <stdlib.h>
#include "token.h"

/* for test */
static char s[] = " a+b a+=b a-b a-=b a*b a*=b a/b a/=b a%b a%=b ";

static Token *id(int);
static Token *cmt(int);
static Token *lit(int);
static Token *op(int);

static Token *gettoken0(void);
static int hexval(int);

Token *gettoken() {
    Token *t;

    t = gettoken0();
    if (t && t->sym == ID) {
        kwentry *e = kwlookup(t->text);
        if (e) t->sym = e->sym;
    }
    return  t;
}

static Token *gettoken0() {
    int ch;
    Token *t = &tok;

    clear_lexeme();
    ch = nextch();
    if (ch == EOF) return (Token*)0;

    switch (codeof(ch)) {
        case '+': case '-': case '*': case'/': case '%': case '=':
            t = op(ch); break;
        case 'Z':
            t = id(ch); break;
        case '0': case '9':
            t = lit(ch); break;
        case '@': printf("[line %d, pos %d]", getlineno(), getlinepos());
            t->sym = ' '; break;
        default:
            t->sym = ch;  break;
    }
    return t;
}

static Token *id(int ch) {
    Token *t = &tok;

    outch(ch);
    while (true) {
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

    switch (ch) {
       case '/':
          outch(ch);
          ch2 = nextch();
          if (ch2 == '*' || ch2 == '/') {
             return cmt(ch2);
          }
          backch(ch2);
          break;
       case '+':
       case '-':
       case '*':
       case '%':
       case '=':
          outch(ch);
          break;
       default: 
          t->sym == ERROR;
          t->ival = 0;
          return t;
    }

    t->sym = OP;
    t->ival = ch;
    outch(0);
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
        if (ch == 'x') {
            b = 16; 
	    outch(ch);
        } else 
            backch(ch);
    }

    while (true) {
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
          default: break;
       }
       backch(ch); 
       break;
    }

    t->sym = LIT;
    t->ival = v;
    outch(0);
    return t;
}

/*
   If you keep all texts in COMMENT, it may exceed the length of MAX_LEXEME.
 */

static Token *cmt(int ch) {
    Token *t = &tok;
    clear_lexeme();

    switch (ch) {
      case '/':
        while (true) {
          ch = nextch();
          if (ch == '\n' || ch == '\r') break;
          outch(ch);
        }
        break;

      case '*':
        while (true) {
          ch = nextch();
          if (ch == '/' && prevch() == '*')
               { delete_prev(); break;} /* erase '*' */
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

int main() {
    Token *t;

    initline();

    while (true) {
       if ((t = gettoken()) == 0) break;
       if (t->sym < ID) {
           if (isspace(t->sym))
             printf("%c", t->sym);
	   else
             printf("SEP<%c> ", t->sym);
       } else switch (t->sym) {
          case ID : printf("ID<%s> ", t->text); break;
          case LIT: printf("LIT<%s>(%d) ", t->text, t->ival); break;
          case OP:  printf("OP<%s>(%d) ", t->text, t->ival); break;
          case CMT: printf("CMT<%s> ", t->text); break;
          case tIF: printf("IF<>"); break;
          case tELSE: printf("ELSE<>"); break;
          case tWHILE: printf("WHILE<>"); break;
          case tDO: printf("DO<>"); break;
          case tFOR: printf("FOR<>"); break;
          case tCALL: printf("CALL<>"); break;
          case tRETURN: printf("RETURN<>"); break;
          case tBREAK: printf("BREAK<>"); break;
          case tCONTINUE: printf("CONTINUE<>"); break;
          case tSWITCH: printf("SWITCH<>"); break;
          case tCASE: printf("CASE<>"); break;
          case tDEFAULT: printf("DEFAULT<>"); break;
          default:  printf("ERROR<%s> ", t->text); break;
       }
    }
    printf("\n");
    return 0;
}
