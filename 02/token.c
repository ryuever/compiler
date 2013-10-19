#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"

Token tok;
Line  line;

inline int getlineno()  { return line.no; }
inline int getlinepos() { return line.pos; }

inline void clear_lexeme() { tok.index = 0; }

// outch push a value to tok buffer waiting for the same style of character..
inline void outch(int ch) { tok.text[tok.index++] = ch; }
inline int prevch() { return (tok.index>0) ? (tok.text[tok.index-1]) : '\n'; }
inline void delete_prev() { --tok.index; }

void initline() {
    line.pos = 0;
    line.no = 0;
    line.backed = 0;
    line.buf[line.pos] = '\n'; /* dummy */
}

int nextch() {
  Line *p = &line;  // The second time, line has values.
  char *r;
  int ch;
  
  if (ch = p->backed) {
    p->backed = 0;
  } else {
    // the next statement is meanful only if you read the line buffer which is already given values.
    ch = p->buf[p->pos++];
    if (ch == '\n') {
      // once you read a new line, it will reset the pos to 0 in line.
      p->pos = 0;
      r = fgets(p->buf,MAX_LINE,stdin);    // read from stdin to p->buf <=> line->buf
      if (r == 0) { p->buf[0] = 0; return EOF; }
      ++p->no;
    }
  }
  return ch;
}

// "p->backed = 0 " means There are values in line buffer.
void backch(int ch) {
  Line *p = &line;
  // this step will restore the data and pos to line structure. because we read one more char through function nextch
  if (p->pos > 0) {
    // When read a different type of character in function nextch, this step will decrease the increment offset 
    // caused by nextch
    p->buf[--p->pos] = ch;
    p->backed = 0;
  } else {
    // I think it only could be '\n' or "EOF", when it come to the end of this line.
    // for "EOF", I think it would be example when you read to the end of source file.
    p->backed = ch;
  }
}

#define SQ ('\'')
#define DQ ('\"')

static int scancode[] = {
    /* 0x20 - 0x2f */
    ' ', '!', DQ, -1, -1, '%', '&', SQ, '(', ')', '*', '+', -1, '-', -1, '/',
    /* 0x30 - 0x3f */
    '0', '9', '9', '9', '9', '9', '9', '9', '9', '9', -1,  -1, '<', '=', '>', -1,
    /* 0x40 - 0x4f */
    '@', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z',
    /* 0x50 - 0x5f */
    'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', '[', -1, ']', -1, 'Z',
    /* 0x60 - 0x6f */
    -1 , 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z',
    /* 0x70 - 0x7f */
    'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', '{', '|', '}', -1, -1
};

#undef SQ
#undef DQ

int codeof(int ch) {
    if (ch < 0x20 || ch > 0x7f) return ch;
    return scancode[ch - 0x20];
}

static kwentry kwtable[] = {
    "if", tIF,
    "else", tELSE,
    "switch", tSWITCH,
    "case", tCASE,
    "default", tDEFAULT,
    "break", tBREAK,
    "continue", tCONTINUE,
    "while", tWHILE,
    "do", tDO,
    "for", tFOR,
    "call", tCALL,
    "return", tRETURN,
    0, 0
};

kwentry *kwlookup(const char *t) {
    kwentry *e;
    for (e=kwtable;e->text;e++) 
        if (strcmp(e->text,t)==0) return e;
    return 0;    // if not found. return 0
}
