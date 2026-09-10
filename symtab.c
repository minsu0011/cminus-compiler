/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
   { char * name;
     LineList lines;
     int memloc ; /* memory location for variable */
     struct BucketListRec * next;
     TreeNode treeNode;
     char * type;
     char * kind;
     char * scopeName;
   } * BucketList;

typedef struct ScopeListRec
   { char * name;
     BucketList hashTable[SIZE];
     struct ScopeListRec * parent;
     int nestedLevel;
   } * ScopeList;

static ScopeList globalScope = NULL;
static ScopeList currentScope = NULL;
static BucketList allSymbols[1000];
static int symIndex = 0;

void scope_push(TreeNode funcNode)
{ ScopeList newScope = (ScopeList) malloc(sizeof(struct ScopeListRec));
  int i;
  newScope->name = (funcNode == NULL) ? "global" : funcNode->attr.name;
  newScope->parent = currentScope;
  newScope->nestedLevel = (currentScope == NULL) ? 0 : currentScope->nestedLevel + 1;
  for (i=0;i<SIZE;++i) newScope->hashTable[i] = NULL;
  
  currentScope = newScope;
  if (globalScope == NULL) globalScope = newScope;
}

void scope_pop(void)
{ if (currentScope != NULL)
    currentScope = currentScope->parent;
}

void st_insert( char * name, int lineno, int loc, TreeNode treeNode )
{ int h = hash(name);
  ScopeList scope = currentScope;
  BucketList l =  scope->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = scope->hashTable[h];
    l->treeNode = treeNode;
    l->scopeName = scope->name;
    
    if (treeNode->nodekind == DeclK) {
      if (treeNode->kind.decl == VarK) l->kind = "Variable";
      else if (treeNode->kind.decl == FunK) l->kind = "Function";
      else if (treeNode->kind.decl == ParamK) l->kind = "Variable";
      else l->kind = "Unknown";

      if (treeNode->type == Integer) l->type = "int";
      else if (treeNode->type == Void) l->type = "void";
      else if (treeNode->type == IntArr) l->type = "int[]";
      else if (treeNode->type == VoidArr) l->type = "void[]";
      else l->type = "error";
    } else {
      l->kind = "Variable"; 
      l->type = "int"; 
    }

    scope->hashTable[h] = l; 
    
    /* Add to linear list for ordered printing */
    allSymbols[symIndex++] = l;
  }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
}

int st_lookup ( char * name )
{ ScopeList scope = currentScope;
  while (scope != NULL)
  { int h = hash(name);
    BucketList l =  scope->hashTable[h];
    while ((l != NULL) && (strcmp(name,l->name) != 0))
      l = l->next;
    if (l != NULL) return l->memloc;
    scope = scope->parent;
  }
  return -1;
}

int st_lookup_top ( char * name )
{ int h = hash(name);
  if (currentScope == NULL) return -1;
  BucketList l =  currentScope->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l != NULL) return l->memloc;
  return -1;
}

void printSymTab(FILE * listing)
{ int i;
  
  fprintf(listing,"< Symbol Table >\n");
  fprintf(listing," Symbol Name   Symbol Kind   Symbol Type    Scope Name   Location  Line Numbers\n");
  fprintf(listing,"-------------  -----------  -------------  ------------  --------  ------------\n");
  
  for (i=0; i<symIndex; ++i) {
      BucketList l = allSymbols[i];
      fprintf(listing,"%-14s %-12s %-14s %-13s %-8d ", l->name, l->kind, l->type, l->scopeName, l->memloc);
      LineList t = l->lines;
      while (t != NULL) {
          fprintf(listing,"%3d ", t->lineno);
          t = t->next;
      }
      fprintf(listing,"\n");
  }

  fprintf(listing,"\n\n< Functions >\n");
  fprintf(listing,"Function Name   Return Type   Parameter Name  Parameter Type\n");
  fprintf(listing,"-------------  -------------  --------------  --------------\n");
  
  for (i=0; i<symIndex; ++i) {
      BucketList l = allSymbols[i];
      if (strcmp(l->kind, "Function") == 0) {
          fprintf(listing,"%-14s %-14s ", l->name, l->type);
          
          TreeNode funcNode = l->treeNode;
          TreeNode params = funcNode->child[0];
          
          if (params == NULL || params->kind.decl != ParamK) {
               fprintf(listing,"%-15s %-14s\n", "void", "");
          } else {
               int first = 1;
               while (params != NULL) {
                   char * pName = params->attr.name; 
                   if (pName == NULL) pName = "void";

                   char * pType = "unknown";
                   if (params->type == Integer) pType = "int";
                   else if (params->type == IntArr) pType = "int[]";
                   else if (params->type == Void) pType = "void";
                   if (first) {
                       fprintf(listing,"%-15s %-14s\n", params->attr.name, pType);
                       first = 0;
                   } else {
                       fprintf(listing,"%-14s %-14s %-15s %-14s\n", "-", "-", params->attr.name, pType);
                   }
                   params = params->sibling;
               }
          }
      }
  }

  fprintf(listing,"\n\n< Global Symbols >\n");
  fprintf(listing," Symbol Name   Symbol Kind   Symbol Type\n");
  fprintf(listing,"-------------  -----------  -------------\n");
  
  for (i=0; i<symIndex; ++i) {
      BucketList l = allSymbols[i];
      if (strcmp(l->scopeName, "global") == 0) {
          fprintf(listing,"%-14s %-12s %-13s\n", l->name, l->kind, l->type);
      }
  }

  fprintf(listing,"\n\n< Scopes >\n");
  fprintf(listing," Scope Name   Nested Level   Symbol Name   Symbol Type\n");
  fprintf(listing,"------------  ------------  -------------  -----------\n");
  
  /* Naive scope printing based on allSymbols list logic */
  for (i=0; i<symIndex; ++i) {
      BucketList l = allSymbols[i];
      if (strcmp(l->scopeName, "global") != 0) {
          int level = 1; /* Assume function scope is level 1 for this project output format */
          fprintf(listing,"%-13s %-13d %-14s %-11s\n", l->scopeName, level, l->name, l->type);
      }
  }
  fprintf(listing,"\n");
}