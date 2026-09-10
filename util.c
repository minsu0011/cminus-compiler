/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF:
    case ELSE:
    case WHILE:
    case RETURN:
    case INT:
    case VOID:
      fprintf(listing,
          "\t%d: reserved word: %s\n", lineno, tokenString);
      break;
    case ASSIGN:
    case EQ:
    case NE:
    case LT:
    case LE:
    case GT:
    case GE:
    case PLUS:
    case MINUS:
    case TIMES:
    case OVER:
    case LPAREN:
    case RPAREN:
    case LBRACKET:
    case RBRACKET:
    case LCURLY:
    case RCURLY:
    case SEMI:
    case COMMA:
      fprintf(listing,"\t%d: %s\n",lineno, tokenString);
      break;
    
    case ENDFILE: fprintf(listing,"\t%d: EOF\n", lineno); break;
    case NUM:
      fprintf(listing,
          "\t%d: NUM, val= %s\n", lineno, tokenString);
      break;
    case ID:
      fprintf(listing,
          "\t%d: ID, name= %s\n", lineno, tokenString);
      break;
    case ERROR:
      fprintf(listing,
          "\t%d: ERROR: %s\n",lineno, tokenString);
      break;
    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
      break;
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode newStmtNode(StmtKind kind)
{ TreeNode t = (TreeNode) malloc(sizeof(struct treeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode newExpNode(ExpKind kind)
{ TreeNode t = (TreeNode) malloc(sizeof(struct treeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

/* Function newDeclNode creates a new declaration
 * node for syntax tree construction
 */
TreeNode newDeclNode(DeclKind kind)
{ TreeNode t = (TreeNode) malloc(sizeof(struct treeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DeclK;
    t->kind.decl = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"If Statement:\n");
          break;
        case IfElseK:
          fprintf(listing,"If-Else Statement:\n");
          break;
        case WhileK:
          fprintf(listing,"While Statement:\n");
          break;
        case ReturnK:
          fprintf(listing,"Return Statement:\n");
          break;
        case CompoundK:
          fprintf(listing,"Compound Statement:\n");
          break;
        default:
          fprintf(listing,"Unknown StmtNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: ");
          switch (tree->attr.op) {
            case PLUS: fprintf(listing,"+\n"); break;
            case MINUS: fprintf(listing,"-\n"); break;
            case TIMES: fprintf(listing,"*\n"); break;
            case OVER: fprintf(listing,"/\n"); break;
            case LT: fprintf(listing,"<\n"); break;
            case LE: fprintf(listing,"<=\n"); break;
            case GT: fprintf(listing,">\n"); break;
            case GE: fprintf(listing,">=\n"); break;
            case EQ: fprintf(listing,"==\n"); break;
            case NE: fprintf(listing,"!=\n"); break;
            case ASSIGN: fprintf(listing,"=\n"); break;
            default: fprintf(listing,"unknown op\n"); break;
          }
          break;
        case ConstK:
          fprintf(listing,"Const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"Variable: name = %s\n",tree->attr.name);
          break;
        case AssignK:
          fprintf(listing,"Assign:\n");
          break;
        case CallK:
          fprintf(listing,"Call: function name = %s\n",tree->attr.name);
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==DeclK)
    { switch (tree->kind.decl) {
        case VarK:
          fprintf(listing,"Variable Declaration: name = %s, type = ",tree->attr.name);
          switch (tree->type) {
            case Integer: fprintf(listing,"int\n"); break;
            case Void: fprintf(listing,"void\n"); break;
            case IntArr: fprintf(listing,"int[]\n"); break;
            case VoidArr: fprintf(listing,"void[]\n"); break;
            default: fprintf(listing,"unknown\n"); break;
          }
          break;
        case FunK:
          fprintf(listing,"Function Declaration: name = %s, return type = ",tree->attr.name);
          switch (tree->type) {
            case Integer: fprintf(listing,"int\n"); break;
            case Void: fprintf(listing,"void\n"); break;
            default: fprintf(listing,"unknown\n"); break;
          }
          break;
        case ParamK:
          if (tree->type == Void)
            fprintf(listing,"Void Parameter\n");
          else {
            fprintf(listing,"Parameter: name = %s, type = ",tree->attr.name);
            switch (tree->type) {
              case Integer: fprintf(listing,"int\n"); break;
              case IntArr: fprintf(listing,"int[]\n"); break;
              default: fprintf(listing,"unknown\n"); break;
            }
          }
          break;
        default:
          fprintf(listing,"Unknown DeclNode kind\n");
          break;
      }
    }
    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}