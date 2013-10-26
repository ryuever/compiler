#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "ast.h"

static Node *ast_buf;
static int   ast_cnt;

static int xml = 0;
static void print_Node_begin(Node*);
static void print_Node_end(Node*);

void init_AST() {
    int sz = (MAX_AST_NODE+1) * sizeof(Node);
    ast_buf = (Node *)malloc(sz);
    bzero(ast_buf, sz);
    ast_cnt = 0;
}

AST new_AST() {
   if (ast_cnt < MAX_AST_NODE)
      return ++ast_cnt;
   return 0;
}

AST make_AST(int type, AST s0, AST s1, AST s2, AST s3) {
    AST a = new_AST();
    Node *np = &ast_buf[a];

    np->type = type;
    np->son[0] = s0; if (s0) ast_buf[s0].father = a;
    np->son[1] = s1; if (s1) ast_buf[s1].father = a;
    np->son[2] = s2; if (s2) ast_buf[s2].father = a;
    np->son[3] = s3; if (s3) ast_buf[s3].father = a;

    return a;
}

AST make_AST_id(char *text) {
    AST a = new_AST();
    Node *np = &ast_buf[a];

    np->type = nID;
    np->text = text;
    np->ival = 0;

    return a;
}

bool isleaf(AST a) {
    int i;
    Node *np = &ast_buf[a];

    for (i=0;i<4;i++) {
       if (np->son[i]) return false;
    } 
    return true;
}

AST make_AST_lit(char *text, int ival) {
    AST a = new_AST();
    Node *np = &ast_buf[a];

    np->type = nLIT;
    np->text = text;
    np->ival = ival;

    return a;
}

AST make_AST_op(int op) {
    AST a = new_AST();
    Node *np = &ast_buf[a];

    np->type = nOP;
    np->ival = op;
    return a;
}

AST new_list(int type) {
    AST a = new_AST();
    Node *np = &ast_buf[a];

    np->type = type;
    np->son[0] = np->son[1] = 0;
    return a;
}

/* caution: very slow ! */
AST append_list(AST l, AST a1) {
    Node *np;
    int type;
    AST a2;

    np = &ast_buf[l];
    type = np->type;
    while (np->son[1]) {
       np = &ast_buf[np->son[1]];
    }
    a2 = make_AST(type, 0, 0, 0, 0);
    np->son[0] = a1;
    np->son[1] = a2;

    return l;
}

static char *name_str[] = {
  "ID", "LIT", "OP",
  "@stmts", "stmt", "var", "con", "expr", "name",
  "asn", "call", "@args", "arg", "arglist",
  0
} ;

static char *nameof(int n) {
   if (n < nID || n >= nEND)
      return "";
   return name_str[n-nID];
}

static void print_Node_begin(Node *np)  {
    char *s = nameof(np->type);

    if (xml) {
       if (*s == '@') s++;
       printf("<%s", s);
    } else {
       if (*s == '@') { printf("\n"); s++ ;}
       printf("%s<", s);
    }

    switch (np->type) {
       case nID  : 
		if (xml) printf(">");
		printf("%s", np->text); 
		break;
       case nLIT : 
		if (xml) printf(">");
		printf("%d", np->ival); 
		break;
       case nOP  : 
		if (xml) printf(">");
		printf("%d", np->ival); 
		break;
       default: 
		if (xml) printf(">");
		break;
    }
}

static void print_Node_end(Node *np) {

    char *s = nameof(np->type);

    if (xml) {
       if (*s == '@') s++;
       printf("</%s>", s);
    } else {
       printf(">");
    }

}

void print_AST(AST a) {
    Node *np = &ast_buf[a];
    int i;

    print_Node_begin(np); 
    for (i=0;i<4;i++) {
       if (np->son[i]) {
          if (!xml && i) printf(",");
          print_AST(np->son[i]);
       }
    }
    print_Node_end(np);
}
