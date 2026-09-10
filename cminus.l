/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

%{
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "y.tab.h"

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

%x COMMENT

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}({letter}|{digit})*
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"int"           {return INT;}
"void"          {return VOID;}
"while"         {return WHILE;}
"return"        {return RETURN;}
"="             {return ASSIGN;}
"<"             {return LT;}
">"             {return GT;}
"=="            {return EQ;}
"!="            {return NE;}
"<="            {return LE;}
">="            {return GE;}
","             {return COMMA;}
"["             {return LBRACKET;}
"]"             {return RBRACKET;}
"{"             {return LCURLY;}
"}"             {return RCURLY;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"/"             {return OVER;}
"("             {return LPAREN;}
")"             {return RPAREN;}
";"             {return SEMI;}
"/*"            { BEGIN(COMMENT); }
<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>\n     { lineno++; }
<COMMENT>.      { }

{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
.               {return ERROR;}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }

  if (currentToken == ID) {
      TreeNode node = newExpNode(IdK); 
      node->attr.name = copyString(tokenString);
      node->lineno = lineno;
      yylval = node;
  } else if (currentToken == NUM) {
      TreeNode node = newExpNode(ConstK);
      node->attr.val = atoi(tokenString);
      node->lineno = lineno;
      yylval = node;
  }
  
  return currentToken;
}
