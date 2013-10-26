#ifndef _AST_H_
#define _AST_H_

#define MAX_AST_NODE 1000

typedef int AST;
typedef enum { false=0, true=1 } bool;

typedef enum {
  nID=1024, nLIT, nOP, 
  nSTMTS, nSTMT, nVAR, nCON, nEXPR, nNAME, 
  nASN, nCALL, nARGS, nARG, nARGLIST,
  nEND, nERROR = -1
} node_type;

typedef struct Node {
  node_type type;
  char      *text;
  int       ival;
  AST       father;
  AST       son[4];
} Node ;

int make_AST(int,AST,AST,AST,AST); /* type, son[0..3] */
int make_AST_id(char*);
int make_AST_lit(char*,int);
int make_AST_op(int);

AST new_list(int);
AST append_list(AST,AST);

void print_AST(AST);
bool isleaf(AST);

#endif /* AST__H_ */
