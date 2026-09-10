/****************************************************/
/* File: cminus.y                                   */
/* The C-Minus Yacc/Bison specification file        */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TreeNode savedTree; /* stores syntax tree for later return */
static int yylex(void);
int yyerror(char * message);

%}


%token IF ELSE WHILE RETURN INT VOID
%token ID NUM 
%token ASSIGN EQ NE LT LE GT GE PLUS MINUS TIMES OVER LPAREN RPAREN LBRACKET RBRACKET LCURLY RCURLY SEMI COMMA
%token ERROR 

/* Precedence and Associativity */
%nonassoc ELSE 
%left LT LE GT GE EQ NE
%left PLUS MINUS
%left TIMES OVER

%% /* Grammar for C-Minus */

program     : declaration_list
                 { savedTree = $1;} 
            ;

declaration_list : declaration_list declaration
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | declaration  { $$ = $1; }
            ;

declaration : var_declaration { $$ = $1; }
            | fun_declaration { $$ = $1; }
            ;

var_declaration : type_specifier ID SEMI
                 { $$ = newDeclNode(VarK);
                   $$->type = $1->type;
                   $$->attr.name = $2->attr.name;
                   $$->lineno = $2->lineno;
                 }
            | type_specifier ID LBRACKET NUM RBRACKET SEMI
                 { $$ = newDeclNode(VarK);
                   $$->type = ($1->type == Integer) ? IntArr : VoidArr;
                   $$->attr.name = $2->attr.name;
                   $$->lineno = $2->lineno;
                   $$->child[0] = $4;
                 }
            ;

type_specifier : INT 
                 { $$ = newDeclNode(VarK); /* Dummy node */
                   $$->type = Integer;
                 }
            | VOID 
                 { $$ = newDeclNode(VarK); 
                   $$->type = Void;
                 }
            ;

fun_declaration : type_specifier ID LPAREN params RPAREN compound_stmt
                 { $$ = newDeclNode(FunK);
                   $$->type = $1->type;
                   $$->attr.name = $2->attr.name;
                   $$->lineno = $2->lineno;
                   $$->child[0] = $4;
                   $$->child[1] = $6;
                 }
            ;

params      : param_list { $$ = $1; }
            | VOID 
                 { $$ = newDeclNode(ParamK);
                   $$->type = Void; 
                 }
            ;

param_list  : param_list COMMA param
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; }
                     else $$ = $3;
                 }
            | param { $$ = $1; }
            ;

param       : type_specifier ID
                 { $$ = newDeclNode(ParamK);
                   $$->type = $1->type;
                   $$->attr.name = $2->attr.name;
                 }
            | type_specifier ID LBRACKET RBRACKET
                 { $$ = newDeclNode(ParamK);
                   $$->type = ($1->type == Integer) ? IntArr : VoidArr;
                   $$->attr.name = $2->attr.name;
                 }
            ;

compound_stmt : LCURLY local_declarations statement_list RCURLY
                 { $$ = newStmtNode(CompoundK);
                   $$->child[0] = $2; 
                   $$->child[1] = $3; 
                 }
            ;

local_declarations : local_declarations var_declaration
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | /* empty */ { $$ = NULL; }
            ;

statement_list : statement_list statement
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | /* empty */ { $$ = NULL; }
            ;

statement   : expression_stmt { $$ = $1; }
            | compound_stmt { $$ = $1; }
            | selection_stmt { $$ = $1; }
            | iteration_stmt { $$ = $1; }
            | return_stmt { $$ = $1; }
            ;

expression_stmt : expression SEMI { $$ = $1; }
            | SEMI { $$ = NULL; } 
            ;

selection_stmt : IF LPAREN expression RPAREN statement
                 { $$ = newStmtNode(IfK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            | IF LPAREN expression RPAREN statement ELSE statement
                 { $$ = newStmtNode(IfElseK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                   $$->child[2] = $7;
                 }
            ;

iteration_stmt : WHILE LPAREN expression RPAREN statement
                 { $$ = newStmtNode(WhileK);
                   $$->child[0] = $3;
                   $$->child[1] = $5;
                 }
            ;

return_stmt : RETURN SEMI
                 { $$ = newStmtNode(ReturnK); }
            | RETURN expression SEMI
                 { $$ = newStmtNode(ReturnK);
                   $$->child[0] = $2;
                 }
            ;

expression  : var ASSIGN expression
                 { $$ = newExpNode(AssignK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                 }
            | simple_expression { $$ = $1; }
            ;

var         : ID
                 { $$ = $1; }
            | ID LBRACKET expression RBRACKET
                 { $$ = $1; 
                   $$->child[0] = $3; /* Array indexing */
                 }
            ;

simple_expression : additive_expression relop additive_expression
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = $2->attr.op;
                 }
            | additive_expression { $$ = $1; }
            ;

relop       : LE { $$ = newExpNode(OpK); $$->attr.op = LE; }
            | LT { $$ = newExpNode(OpK); $$->attr.op = LT; }
            | GT { $$ = newExpNode(OpK); $$->attr.op = GT; }
            | GE { $$ = newExpNode(OpK); $$->attr.op = GE; }
            | EQ { $$ = newExpNode(OpK); $$->attr.op = EQ; }
            | NE { $$ = newExpNode(OpK); $$->attr.op = NE; }
            ;

additive_expression : additive_expression addop term
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = $2->attr.op;
                 }
            | term { $$ = $1; }
            ;

addop       : PLUS { $$ = newExpNode(OpK); $$->attr.op = PLUS; }
            | MINUS { $$ = newExpNode(OpK); $$->attr.op = MINUS; }
            ;

term        : term mulop factor
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = $2->attr.op;
                 }
            | factor { $$ = $1; }
            ;

mulop       : TIMES { $$ = newExpNode(OpK); $$->attr.op = TIMES; }
            | OVER { $$ = newExpNode(OpK); $$->attr.op = OVER; }
            ;

factor      : LPAREN expression RPAREN
                 { $$ = $2; }
            | var { $$ = $1; }
            | call { $$ = $1; }
            | NUM { $$ = $1; }
            ;

call        : ID LPAREN args RPAREN
                 { $$ = newExpNode(CallK);
                   $$->attr.name = $1->attr.name;
                   $$->child[0] = $3;
                 }
            ;

args        : arg_list { $$ = $1; }
            | /* empty */ { $$ = NULL; }
            ;

arg_list    : arg_list COMMA expression
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; }
                     else $$ = $3;
                 }
            | expression { $$ = $1; }
            ;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

static int yylex(void)
{ return getToken(); }

TreeNode parse(void)
{ yyparse();
  return savedTree;
}