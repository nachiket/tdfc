%{
//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999 The Regents of the University of California 
// Permission to use, copy, modify, and distribute this software and
// its documentation for any purpose, without fee, and without a
// written agreement is hereby granted, provided that the above copyright 
// notice and this paragraph and the following two paragraphs appear in
// all copies. 
//
// IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
// DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
// LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE. 
//
// THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON
// AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
// PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
//
//////////////////////////////////////////////////////////////////////////////
//
// BRASS source file
//
// SCORE TDF compiler:  TDF parser using bison
// $Revision: 1.143 $
//
//////////////////////////////////////////////////////////////////////////////


#include <assert.h>

#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/dictionary.h>

#include "parse.h"
#include "type.h"
#include "symbol.h"
#include "suite.h"
#include "operator.h"
#include "state.h"
#include "stmt.h"
#include "expr.h"
#include "misc.h"
#include "annotes.h"
#include "gc.h"

#ifdef __APPLE__
#define strtoll strtoq
#endif

using std::cout;
using std::endl;

////////////////////////////////////////////////////////////////
//  classes (local)

// - auxiliary info for parsing type decls:
class TypeDeclElem
{
public:
  Token *id; Expr *val; Expr *depth;
  TypeDeclElem (Token *id_i, Expr *val_i, Expr *depth_i)
    : id(id_i), val(val_i), depth(depth_i) {}
};


////////////////////////////////////////////////////////////////
//  globals

string	*gFileName = new string("<null>");  // file being parsed
int	 gLineNum  = 0;			    // line being parsed (by lex)
bool	 gParsing  = false;		    // true during parse (by parseFile)
Suite	*gSuite	   = NULL;		    // filled with operators by parser

// - auxiliary info for objects being parsed:
static	SymTab				*gSymtab = NULL;
static	dictionary<string,State*>	*gStates = NULL;


////////////////////////////////////////////////////////////////
//  yyerror, yywarn

void yyerror (const char *msg)
{
  extern char *yytext;
  fatal(1, string(msg), new Token(0,gLineNum,gFileName,yytext));
}

void yywarn (const char *msg)
{
  extern char *yytext;
  warn(string(msg), new Token(0,gLineNum,gFileName,yytext));
}


////////////////////////////////////////////////////////////////
//  symbol table management

SymTab* symtabPush (SymTab *s)
{
  // - push new child symtab into gSymtab
  assert(s);
  if (gSymtab)
    gSymtab->addChild(s);
  gSymtab=s;
  return gSymtab;
}

SymTab* symtabPop ()
{
  // - pop a symtab from gSymtab
  assert(gSymtab);
  gSymtab=gSymtab->getScope();
  return gSymtab;
}

Symbol* lookupDuplicateDecl (Token *t)
{
  // - return duplicate or overriden type-decl for given name
  // - yyerror on duplicate, warn on overriden type-decl
  Symbol *s=NULL;
  if (gSymtab && (s=gSymtab->lookup(t->str)))
  {
    if (s->getScope()==gSymtab)
      yyerror((const char*)(string("redefinition of \"") + t->str + "\""));
    else
      {
	Token *st=s->getToken();
	yywarn ((const char*)(string("definition of \"") + t->str +
			      string("\" overrides previous definition") +
			      (st ? (string(" from ")+*st->file+":"+
				     string("%d",st->line)) : string()) ));
      }
  }
  return s;
}

Symbol* lookupTry (Token *t)
{
  // - return visible symbol of given name (NULL if none visible)
  Symbol *s=NULL;
  if (gSymtab && (s=gSymtab->lookup(t->str)))
    return s;
  else
    return NULL;
}

Symbol* lookup (Token *t)
{
  // - return visible symbol of given name (yyerror if none visible)
  Symbol *s=NULL;
  if (gSymtab && (s=gSymtab->lookup(t->str)))
    return s;
  else
    if (gParsing)
      yyerror((const char*)(string("symbol \"") + t->str +
			    string("\" not found")));
    else
      fatal(1, string("symbol \"")+t->str+"\" not found", t);
      // - may be called from ExprLValue::link() after parse,
      //   when gFileName/gLineNum are not valid (so use token)
  return NULL;
}

////////////////////////////////////////////////////////////////
%}

%union {
  Token 	*token;
  Expr		*expr;
  Stmt		*stmt;
  StateCase	*stateCase;
//State		*state;
  State		*states;	// $$=start state
  Operator	*operatr;
  Symbol	*sym;
  SymTab	*symtab;
  Type		*type;
  InputSpec	*inputSpec;
  OpBody	*opBody;
  OpSignature	*opSignature;
  TypeDeclElem	*typeDeclElem;
  list<Expr*>			*exprs, *args;
  list<Stmt*>			*stmts;
  list<InputSpec*>		*inputSpecs;
  list<Symbol*>			*symList;
  list<TypeDeclElem*>		*typeDeclElems;
//dictionary<string,Symbol*>	*symDict;
//dictionary<string,State*>	*states;
  Suite				*suite;
};

%token <token>	ATTENTION BITSOF BOOLEAN FLOAT DOUBLE CAT CLOSE FRAMECLOSE COPY DONE ELSE EOS EOFR EQUALS FALSE GOTO GTE ID_ IF INPUT LEFT_SHIFT LOGIC_AND LOGIC_OR LTE NOT_EQUALS NUM NUMDBL OUTPUT PARAM PASS_THROUGH_EXCEPTION PRINTF RIGHT_SHIFT SEGMENT_R_ SEGMENT_RW_ SEGMENT_SEQ_R_ SEGMENT_SEQ_CYCLIC_R_ SEGMENT_SEQ_RW_ SEGMENT_SEQ_W_ SEGMENT_W_ SIGNED STATE STAY STRING TRUE UNSIGNED WIDTHOF '(' ')' '{' '}' '[' ']' '<' '>' '-' '+' '~' '!' '@' '#' '%' '^' '&' '*' '/' '=' '|' ';' ':' ',' '.' '?' EXP LOG SQRT FLOOR ABS

%type <token>		sizedType ioKind exception_opt exception equalOp inequalOp shiftOp addOp prodOp unaryOp exprOp logOp sqrtOp floorOp absOp
%type <expr>		arraySize call lvalue expr condExpr logOrExpr logAndExpr bitOrExpr bitXorExpr bitAndExpr equalExpr inequalExpr shiftExpr addExpr prodExpr unaryExpr fixedExpr atomExpr builtinExpr floatExpr
%type <exprs>		condExprs_opt condExprs
%type <stmt>		stmt stmt_nonEmpty matchedStmt unmatchedStmt stmtBlock builtinStmtBehav builtinStmtCompose assign callOrAssign callOrAssign_nonEmpty copyStmt segmentStmt
%type <stmts>		callsOrAssigns stmts_opt stmts
%type <type>		type_opt type
%type <args>		args_opt more_args_opt args
%type <inputSpec>	stateVar
%type <inputSpecs>	stateVars stateVars_opt
%type <stateCase>	stateCase
%type <states>		states
%type <suite>		start operators_opt operators
%type <operatr>		operator
%type <opBody>		operatorBody
%type <opSignature>	opSignature
%type <sym>		ioDecl
%type <symList>		ioDecls_opt ioDecls
%type <symtab>		typeDecls_opt typeDecls typeDecl
%type <typeDeclElem>	typeDeclElem
%type <typeDeclElems>	typeDeclElems
%%

start
: operators_opt
			{ $$=$1; }
;

operators_opt
: operators
			{ $$=$1; }
|
			{
			  if (!gSuite)
			  {
			    gSuite=new Suite();
			    if (gEnableGC)
			      TreeGC::addRoot(gSuite);
			  }
			  $$=gSuite;
			}
;

operators
: operators operator
			{
			  $$=$1;
			  $$->addOperator($2);
			}
| operator
			{
			  if (!gSuite)
			  {
			    gSuite=new Suite();
			    if (gEnableGC)
			      TreeGC::addRoot(gSuite);
			  }
			  $$=gSuite;
			  $$->addOperator($1);
			}
;

operator
: type_opt opSignature
			{
			  // opSignature leaves iodecls in pushed symtab
			  if ($1)
			  {
			    // add operator return to iodecls symtab:
			    $2->retSym = new SymbolStream($2->token,$2->name,
							  $1,STREAM_OUT);
			    gSymtab->addSymbol($2->retSym);
			  }
			  else
			  {
			    // no operator return, use TYPE_NONE symbol:
			    $2->retSym=(Symbol*)sym_none->duplicate();
			  }
			}
  operatorBody exception_opt
			{
			  if ($4->opKind==OP_BEHAVIORAL)
			  {
			    BehavOpBody *body=(BehavOpBody*)$4;
			    $$=new OperatorBehavioral($2->token, $2->name,
						      $2->retSym, $2->args,
						      body->vars, body->states,
						      body->startState, $5);
			  }
			  else if ($4->opKind==OP_COMPOSE)
			  {
			    ComposeOpBody *body=(ComposeOpBody*)$4;
			    $$=new OperatorCompose($2->token, $2->name,
						   $2->retSym, $2->args,
						   body->vars, body->stmts,$5);
			  }
			  else  // ($4->opKind==OP_BUILTIN)
			    assert(!"internal inconsistency");
			  // $$->thread(NULL);	// defunct
			  delete symtabPop();	// kill opSignature's symtab
			}
;

type_opt
: type
			{ $$=$1; }
|
			{ $$=NULL; }
;

type
: BOOLEAN
			{ $$=new Type(TYPE_BOOL); }
| FLOAT
			{ $$=new Type(TYPE_FLOAT); }
| DOUBLE
			{ $$=new Type(TYPE_DOUBLE); }
| DOUBLE arraySize
			{ $$=new TypeArray(new Type(TYPE_DOUBLE),$2); }
| BOOLEAN arraySize
			{ $$=new TypeArray(new Type(TYPE_BOOL),$2); }
| sizedType '[' expr ']'
			{
			  bool signd  = ($1->code==SIGNED) ? true : false;
			  Type *typeE = $3->getType();
			  if      (typeE->getTypeKind()==TYPE_ANY)
			    // possible unlinked width expr of type TYPE_ANY
			    $$=new Type(TYPE_INT,$3,signd);	// assume int
			  else if (typeE->getTypeKind()==TYPE_INT)
			  {
			    $$=new Type(TYPE_INT,$3,signd);
			    if ($3->getExprKind()==EXPR_VALUE &&
				((ExprValue*)$3)->getIntVal()<0)
			      yyerror("bit-width must be non-negative");
			  }
			  else if (typeE->getTypeKind()==TYPE_FIXED)
			  {
			    yyerror("fixed types not supported");
			    // requires const expr or '.' operator: "x.y"
			  }
			  else
			    yyerror("invalid type for bit-width expression");
			}
| sizedType arraySize '[' expr ']'
			{
			  bool signd  = ($1->code==SIGNED) ? true : false;
			  Type *typeE = $4->getType();
			  if      (typeE->getTypeKind()==TYPE_ANY)
			  {
			    // possible unlinked width expr of type TYPE_ANY
			    Type *elemType=new Type(TYPE_INT,$4,signd);
			    $$=new TypeArray(elemType,$2);
			  }
			  else if (typeE->getTypeKind()==TYPE_INT)
			  {
			    Type *elemType=new Type(TYPE_INT,$4,signd);
			    if ($4->getExprKind()==EXPR_VALUE &&
				((ExprValue*)$4)->getIntVal()<0)
			      yyerror("bit-width must be non-negative");
			    $$=new TypeArray(elemType,$2);
			  }
			  else if (typeE->getTypeKind()==TYPE_FIXED)
			  {
			    yyerror("fixed types not supported");
			    // requires const expr or '.' operator: "x.y"
			  }
			  else
			    yyerror("invalid type for bit-width expression");
			}
;

sizedType
: SIGNED
			{ $$=$1; }
| UNSIGNED
			{ $$=$1; }
;

arraySize
: '[' expr ']'
			{
			  Type *typeE = $2->getType();
			  if (typeE->getTypeKind()==TYPE_ANY)
			    // possible unlinked nelems expr of type TYPE_ANY
			    $$=$2;
			  else if (typeE->getTypeKind()==TYPE_INT)
			  {
			    $$=$2;
			    if ($2->getExprKind()==EXPR_VALUE &&
				((ExprValue*)$2)->getIntVal()<0)
			      yyerror("array size must be non-negative");
			  }
			  else
			    yyerror("invalid type for array size expression");
			}
;

opSignature
: ID_ '(' ioDecls_opt ')'
			{ $$=new OpSignature($1,$1->str,$3); }
;

ioDecls_opt
: ioDecls
			{ $$=$1; }
|
			{
			  $$=new list<Symbol*>;
			  symtabPush(new SymTab(SYMTAB_OP));
			}
;

ioDecls
: ioDecls ',' ioDecl
			{
			  $$=$1;
			  $$->append($3);
			  gSymtab->addSymbol($3);
			}
| ioDecl
			{
			  $$=new list<Symbol*>;
			  $$->append($1);
			  symtabPush(new SymTab(SYMTAB_OP));
			  gSymtab->addSymbol($1);
			}
;

ioDecl
: ioKind type ID_
			{
			  if ($1->code==INPUT)
			    $$=new SymbolStream($3,$3->str,$2,STREAM_IN);
			  else if ($1->code==OUTPUT)
			    $$=new SymbolStream($3,$3->str,$2,STREAM_OUT);
			  else  // ($1->code==PARAM)
			    $$=new SymbolVar($3,$3->str,$2);
			}
| ioKind type ID_ '[' expr ',' expr ']'
			{
			  
			  if ($1->code==INPUT)
			  {
			    Type *typeE = $5->getType();
			    Type *typeE2 = $7->getType();
			    
/*			      
			      if (typeE->getTypeKind() == TYPE_BOOL)
						  yywarn ((const char*)(string("type of expr 1 : TYPE_BOOL ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_INT)
						yywarn ((const char*)(string("type of expr 1 : TYPE_INT ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_FIXED)
						yywarn ((const char*)(string("type of expr 1 : TYPE_FIXED ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_ARRAY)
						yywarn ((const char*)(string("type of expr 1 : TYPE_ARRAY ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_ANY)
						yywarn ((const char*)(string("type of expr 1 : TYPE_ANY ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_FLOAT)
						yywarn ((const char*)(string("type of expr 1 : TYPE_FLOAT ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_DOUBLE)
						yywarn ((const char*)(string("type of expr 1 : TYPE_DOUBLE ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_NONE)
						yywarn ((const char*)(string("type of expr 1 : TYPE_NONE ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_STATE)
						yywarn ((const char*)(string("type of expr 1 : TYPE_STATE ") + 
									+string("\n value of expr 1 : ") + $5->toString()));
			      
			      
			      if (typeE2->getTypeKind() == TYPE_BOOL)
						  yywarn ((const char*)(string("type of expr 2 : TYPE_BOOL ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_INT)
						yywarn ((const char*)(string("type of expr 2 : TYPE_INT ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_FIXED)
						yywarn ((const char*)(string("type of expr 2 : TYPE_FIXED ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_ARRAY)
						yywarn ((const char*)(string("type of expr 2 : TYPE_ARRAY ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_ANY)
						yywarn ((const char*)(string("type of expr 2 : TYPE_ANY ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_FLOAT)
						yywarn ((const char*)(string("type of expr 2 : TYPE_FLOAT ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_DOUBLE)
						yywarn ((const char*)(string("type of expr 2 : TYPE_DOUBLE ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_NONE)
						yywarn ((const char*)(string("type of expr 2 : TYPE_NONE ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
			      else if (typeE2->getTypeKind() == TYPE_STATE)
						yywarn ((const char*)(string("type of expr 2 : TYPE_STATE ") + 
									+string("\n value of expr 2 : ") + $7->toString()));
*/
	
			    
			    if ((typeE->getTypeKind()==TYPE_ANY || typeE->getTypeKind()==TYPE_DOUBLE ||typeE->getTypeKind() == TYPE_INT 
						|| typeE->getTypeKind()==TYPE_FLOAT || typeE->getTypeKind()==TYPE_FIXED)
						
						&& (typeE2->getTypeKind()==TYPE_ANY || typeE2->getTypeKind()==TYPE_DOUBLE ||typeE2->getTypeKind() == TYPE_INT 
						|| typeE2->getTypeKind()==TYPE_FLOAT|| typeE2->getTypeKind()==TYPE_FIXED))
			    {
					$$=new SymbolStream($3,$3->str,$2,STREAM_IN, $5->toString(), $7->toString());
//					cout << $5->toString() << endl;
				}
				else
					yyerror("invalid type for RANGE expression"+typekindToString(typeE->getTypeKind())+" and "+typekindToString(typeE2->getTypeKind()));
					
			  }
			  else if ($1->code==OUTPUT)
			    $$=new SymbolStream($3,$3->str,$2,STREAM_OUT, $5->toString(), $7->toString());
			  else  // ($1->code==PARAM)
			    $$=new SymbolVar($3,$3->str,$2);
			}
			
| ioKind type ID_ '=' expr 
			{
			  
			  if ($1->code==PARAM)
			  {
			    Type *typeE = $5->getType();
/*			    
			      
			      if (typeE->getTypeKind() == TYPE_BOOL)
						  yywarn ((const char*)(string("type of param : TYPE_BOOL ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_INT)
						yywarn ((const char*)(string("type of param : TYPE_INT ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_FIXED)
						yywarn ((const char*)(string("type of param : TYPE_FIXED ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_ARRAY)
						yywarn ((const char*)(string("type of param : TYPE_ARRAY ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_ANY)
						yywarn ((const char*)(string("type of param : TYPE_ANY ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_FLOAT)
						yywarn ((const char*)(string("type of param : TYPE_FLOAT ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_DOUBLE)
						yywarn ((const char*)(string("type of param : TYPE_DOUBLE ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_NONE)
						yywarn ((const char*)(string("type of param : TYPE_NONE ") + 
									+string("\n value of param : ") + $5->toString()));
			      else if (typeE->getTypeKind() == TYPE_STATE)
						yywarn ((const char*)(string("type of param : TYPE_STATE ") + 
									+string("\n value of param : ") + $5->toString()));
*/			      		    
			    
			    if ((typeE->getTypeKind()==TYPE_DOUBLE ||typeE->getTypeKind() == TYPE_INT 
						|| typeE->getTypeKind()==TYPE_FLOAT || typeE->getTypeKind()==TYPE_FIXED))
			    {
					$$=new SymbolVar($3,$3->str,$2, $5->toString());
				}
				else
					yyerror("invalid type for RANGE expression");
				
				 
					
			  }
			  else if ($1->code==OUTPUT)
			    $$=new SymbolStream($3,$3->str,$2,STREAM_OUT);
			  else  // ($1->code==INPUT)
			    $$=new SymbolStream($3,$3->str,$2,STREAM_OUT);
			}
;

ioKind
: INPUT
			{ $$=$1; }
| OUTPUT
			{ $$=$1; }
| PARAM
			{ $$=$1; }
;

exception_opt
: exception
			{ $$=$1; }
|
			{ $$=NULL; }
;

exception
: PASS_THROUGH_EXCEPTION
			{ $$=$1; }
;

operatorBody
: '{' typeDecls_opt
			{
			  // typeDecls_opt leaves pushed symtab
			  gStates=new dictionary<string,State*>;
			}
  states '}'
			{
			  $$=(OpBody*)new BehavOpBody(gSymtab,gStates,$4);
			  symtabPop();
			  gStates=NULL;
			}
| '{' typeDecls_opt
			{
			  // typeDecls_opt leaves pushed symtab
			}
   callsOrAssigns  '}'
			{
			  $$=(OpBody*)new ComposeOpBody(gSymtab,$4);
			  symtabPop();
			}
| '{' typeDecls_opt '}'
			{
			  // typeDecls_opt leaves pushed symtab
			  delete symtabPop();
			  yyerror("empty operator body");
			}
;

typeDecls_opt
: typeDecls
			{ $$=$1; }
|
			{
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			  $$=gSymtab;
			}
;

typeDecls
: typeDecls typeDecl
			{ $$=gSymtab; }
|
			{
			  // push new empty symtab before processing first decl
			  symtabPush(new SymTab(SYMTAB_BLOCK));
			}			  
  typeDecl
			{ $$=gSymtab; }
;

typeDecl
: type typeDeclElems ';'
			{
			  $$=gSymtab;
			  bool first=true;	// - mark 1st loop iter
			  TypeDeclElem *tde;
			  forall (tde,*$2) {
			    Type   *typ=first ? $1 : (Type*)$1->duplicate();
			    Symbol *sym=new SymbolVar(tde->id,tde->id->str,
						      typ,tde->val,tde->depth);
			    gSymtab->addSymbol(sym);
			    first=false;
			  }
			  // delete $2;
			}
| ';'
			{ $$=gSymtab; }
;

typeDeclElems
: typeDeclElem		{ $$=new list<TypeDeclElem*>; $$->append($1); }
| typeDeclElems ',' typeDeclElem
			{ $$=$1;                      $$->append($3); }
;

typeDeclElem
: ID_			{
			  lookupDuplicateDecl($1);
			  $$=new TypeDeclElem($1,NULL,NULL);
			}
| ID_
			{ lookupDuplicateDecl($1); }
  '=' expr
			{ $$=new TypeDeclElem($1,$4,NULL); }

| ID_
			{ lookupDuplicateDecl($1); }
  '(' expr ')'
			{ $$=new TypeDeclElem($1,NULL,$4); }
;

callsOrAssigns
: callsOrAssigns callOrAssign
			{ $$=$1;		if ($2) $$->append($2); }
| callOrAssign_nonEmpty
			{ $$=new list<Stmt*>;	if ($1) $$->append($1); }
;

callOrAssign
: callOrAssign_nonEmpty
			{ $$=$1; }
| ';'
			{ $$=NULL; }
;

callOrAssign_nonEmpty
: call ';'
			{ $$=new StmtCall($1->getToken(),(ExprCall*)$1); }
| lvalue '=' call ';'
			{ $$=new StmtAssign($2,(ExprLValue*)$1,$3); }
| lvalue '=' lvalue ';'
			{ $$=new StmtAssign($2,(ExprLValue*)$1,$3); }
| builtinStmtCompose ';'
			{ $$=$1; }
/*
| segmentStmt ';'
			{ $$=$1; }
*/
;

states
: states stateCase
			{
			  $$=$1;			// $$ = start state
			  // states are stored in gStates by stateCase action
			}
| stateCase
			{
			  $$=(State*)($1->getParent());	// $$ = start state
			  // states are stored in gStates by stateCase action
			}
;
/* no restriction on ordering/grouping of stateCases */

stateCase
: STATE ID_ '(' stateVars_opt ')' ':' stmts_opt
			{
			  // lookup/create state and add stateCase to it
			  dic_item state_item=gStates->lookup($2->str);
			  State *state;
			  if (state_item)
			    state=gStates->inf(state_item);
			  else
			  {
			    state=new State($2,$2->str,new set<StateCase*>);
			    gStates->insert($2->str,state);
			  }
			  $$=new StateCase($2,$4,$7);
			  state->addCase($$);
			}
;

stateVars_opt
: stateVars
			{ $$=$1; }
|
			{ $$=new list<InputSpec*>; }
;

stateVars
: stateVars ',' stateVar
			{ $$=$1;			$$->append($3); }
| stateVar
			{ $$=new list<InputSpec*>;	$$->append($1); }
;

stateVar
: ID_
			{ $$=new InputSpec($1,lookup($1),NULL,false,false); }
| ID_ '#' expr
			{ $$=new InputSpec($1,lookup($1),$3,false,false); }
| EOS '(' ID_ ')'
			{ $$=new InputSpec($3,lookup($3),NULL,true,false); }
| EOFR '(' ID_ ')'
			{ $$=new InputSpec($3,lookup($3),NULL,false,true); }			
;

stmts_opt
: stmts
			{ $$=$1; }
|
			{ $$=new list<Stmt*>; }
;

stmts
: stmts stmt
			{ $$=$1;		if ($2) $$->append($2); }
| stmt_nonEmpty
			{ $$=new list<Stmt*>;	if ($1) $$->append($1); }
;

stmt
: stmt_nonEmpty
			{ $$=$1; }
| ';'
			{ $$=NULL; }
;

stmt_nonEmpty
: matchedStmt
			{ $$=$1; }
| unmatchedStmt
			{ $$=$1; }
;

matchedStmt
: IF '(' expr ')' matchedStmt ELSE matchedStmt
			{ $$=new StmtIf($1,$3,$5,$7); }
| GOTO ID_ ';'
			{ $$=new StmtGoto($1,$2->str); }
| STAY ';'
			{ $$=new StmtGoto($1,NULL); }
| assign ';'
			{ $$=$1; }
| builtinStmtBehav ';'
			{ $$=$1; }
| call ';'
			{ $$=new StmtCall($1->getToken(),(ExprCall*)$1); }
| stmtBlock
			{ $$=$1; }
;

unmatchedStmt
: IF '(' expr ')' stmt
			{ $$=new StmtIf($1,$3,$5); }
| IF '(' expr ')' matchedStmt ELSE unmatchedStmt
			{ $$=new StmtIf($1,$3,$5,$7); }
;

stmtBlock
: '{' typeDecls_opt stmts_opt '}'
			{
			  // typeDecls_opt leaves pushed symtab
			  $$=new StmtBlock($1,gSymtab,$3);
			  symtabPop();
			}
;

builtinStmtBehav
: ATTENTION '(' expr ')'
			{
			  list<Expr*> *args=new list<Expr*>;
			  args->append($3);
			  ExprBuiltin *eb=new ExprBuiltin($1,args,
							  builtin_attn);
			  $$=new StmtBuiltin($1,eb);
			}
| CLOSE '(' ID_ ')'
			{
			  ExprLValue *el=new ExprLValue($3,lookup($3));
			  list<Expr*> *args=new list<Expr*>;
			  args->append(el);
			  ExprBuiltin *eb=new ExprBuiltin($1,args,
							  builtin_close);
			  $$=new StmtBuiltin($1,eb);
			}
| FRAMECLOSE '(' ID_ ')'
			{
			  ExprLValue *el=new ExprLValue($3,lookup($3));
			  list<Expr*> *args=new list<Expr*>;
			  args->append(el);
			  ExprBuiltin *eb=new ExprBuiltin($1,args,
							  builtin_frameclose);
			  $$=new StmtBuiltin($1,eb);
			}			
| DONE '(' ')'
			{
			  list<Expr*> *args=new list<Expr*>;
			  args->append((Expr*)expr_0->duplicate());   // sync 0
			  ExprBuiltin *eb=new ExprBuiltin($1,args,
							  builtin_done);
			  $$=new StmtBuiltin($1,eb);
			}
| PRINTF '(' STRING more_args_opt ')'
			{
			  list<Expr*> *args=$4;
			  ExprBuiltin *eb=new ExprBuiltin($1,args,
							  builtin_printf);
			  eb->setAnnote_(ANNOTE_PRINTF_STRING_TOKEN,$3);
			  $$=new StmtBuiltin($1,eb);
			}
| copyStmt
			{ $$=$1; }
| segmentStmt
			{ $$=$1; }
;

builtinStmtCompose
: copyStmt
			{ $$=$1; }
| segmentStmt
			{ $$=$1; }
;

copyStmt
: COPY '(' args ')'
			{
			  ExprBuiltin *eb=new ExprBuiltin($1,$3,builtin_copy);
			  $$=new StmtBuiltin($1,eb);
			}
;

segmentStmt
: SEGMENT_R_ '(' args ')'
			{
			  OperatorSegment *o
			    = new OperatorSegment($1,string("_dummy_seg_op"),
						  SEGMENT_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin($1,$3,o);
			  $$=new StmtBuiltin($1,e);
			}
| SEGMENT_W_ '(' args ')'
			{
			  OperatorSegment *o
			    = new OperatorSegment($1,string("_dummy_seg_op"),
						  SEGMENT_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin($1,$3,o);
			  $$=new StmtBuiltin($1,e);
			}
| SEGMENT_RW_ '(' args ')'
			{
			  OperatorSegment *o
			    = new OperatorSegment($1,string("_dummy_seg_op"),
						  SEGMENT_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin($1,$3,o);
			  $$=new StmtBuiltin($1,e);
			}
| SEGMENT_SEQ_R_ '(' args ')'
			{
			  OperatorSegment *o
			    = new OperatorSegment($1,string("_dummy_seg_op"),
						  SEGMENT_SEQ_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin($1,$3,o);
			  $$=new StmtBuiltin($1,e);
			}
| SEGMENT_SEQ_CYCLIC_R_ '(' args ')'
			{
			  OperatorSegment *o
			    = new OperatorSegment($1,string("_dummy_seg_op"),
						  SEGMENT_SEQ_CYCLIC_R,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin($1,$3,o);
			  $$=new StmtBuiltin($1,e);
			}
| SEGMENT_SEQ_W_ '(' args ')'
			{
			  OperatorSegment *o
			    = new OperatorSegment($1,string("_dummy_seg_op"),
						  SEGMENT_SEQ_W,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin($1,$3,o);
			  $$=new StmtBuiltin($1,e);
			}
| SEGMENT_SEQ_RW_ '(' args ')'
			{
			  OperatorSegment *o
			    = new OperatorSegment($1,string("_dummy_seg_op"),
						  SEGMENT_SEQ_RW,
						  type_none,new list<Symbol*>);
			  ExprBuiltin *e=new ExprBuiltin($1,$3,o);
			  $$=new StmtBuiltin($1,e);
			}
;

call
: ID_ '(' args_opt ')'
			{ $$=new ExprCall($1,$3); }
;

args_opt
: args
			{ $$=$1; }
|
			{
			  // empty args list implies 0-width sync stream
			  $$=new list<Expr*>;
			  $$->append((Expr*)expr_0->duplicate());
			}
;

more_args_opt
: ',' args
			{ $$=$2; }
|
			{ $$=new list<Expr*>; }
;

args
: args ',' expr
			{ $$=$1;		$$->append($3); }
| expr
			{ $$=new list<Expr*>;	$$->append($1); }
;

assign
: lvalue '=' expr
			{ $$=new StmtAssign($2,(ExprLValue*)$1,$3); }
;

lvalue
: ID_
			{
			  Symbol *s  = lookupTry($1),
				 *sym= s?s:new SymbolVar($1,$1->str,type_any);
			  $$=new ExprLValue($1,sym);
			  // warning - a param appearing in the width
			  //           expression of an operator's return type,
			  //           where the param formal has not yet been
			  //           parsed, is left as an unlinked symbol.
			  //           the symbol ref is linked by link().
			  //           e.g.:
			  // "unsigned[n] doit(param unsigned[8] n, ...)"
			}
| ID_ '[' expr ']'
			{
			  Symbol *s=lookup($1);
			  if (s->getType()->getTypeKind()==TYPE_ARRAY)
			    $$=new ExprLValue($1,lookup($1),NULL,NULL,NULL,$3);
			  else
			    $$=new ExprLValue($1,lookup($1),$3);
			}
| ID_ '[' expr ':' expr ']'
			{ $$=new ExprLValue($1,lookup($1),$5,$3); }
| ID_ '[' expr ']' '[' expr ']'
			{ $$=new ExprLValue($1,lookup($1),$6,NULL,NULL,$3); }
| ID_ '[' expr ']' '[' expr ':' expr ']'
			{ $$=new ExprLValue($1,lookup($1),$8,$6,NULL,$3); }
;

expr
: condExpr
			{ $$=$1; }
| '{' condExprs_opt '}'
			{
			  TypeArray *type=new TypeArray(type_any,$2->size());
			  $$=new ExprArray($1,type,$2);
			}
;

condExprs_opt
: condExprs
			{ $$=$1; }
|
			{ $$=new list<Expr*>; }
;

condExprs
: condExprs ',' condExpr
			{ $$=$1; $$->append($3); }
| condExpr
			{ $$=new list<Expr*>; $$->append($1); }
;

condExpr
: logOrExpr '?' condExpr ':' condExpr
			{ $$=new ExprCond($2,$1,$3,$5); }
| logOrExpr
			{ $$=$1; }
;

logOrExpr
: logOrExpr LOGIC_OR logAndExpr
			{ $$=new ExprBop($2,LOGIC_OR,$1,$3); }
| logAndExpr
			{ $$=$1; }
;

logAndExpr
: logAndExpr LOGIC_AND bitOrExpr
			{ $$=new ExprBop($2,LOGIC_AND,$1,$3); }
| bitOrExpr
			{ $$=$1; }
;

bitOrExpr
: bitOrExpr '|' bitXorExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| bitXorExpr
			{ $$=$1; }
;

bitXorExpr
: bitXorExpr '^' bitAndExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| bitAndExpr
			{ $$=$1; }
;

bitAndExpr
: bitAndExpr '&' equalExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| equalExpr
			{ $$=$1; }
;

equalExpr
: equalExpr equalOp inequalExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| inequalExpr
			{ $$=$1; }
;

inequalExpr
: inequalExpr inequalOp shiftExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| shiftExpr
			{ $$=$1; }
;

shiftExpr
: shiftExpr shiftOp addExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| addExpr
			{ $$=$1; }
;

addExpr
: addExpr addOp prodExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| prodExpr
			{ $$=$1; }
;

prodExpr
: prodExpr prodOp unaryExpr
			{ $$=new ExprBop($2,$2->code,$1,$3); }
| unaryExpr
			{ $$=$1; }
;

unaryExpr
: unaryOp fixedExpr
			{ $$=new ExprUop($1,$1->code,$2); }
| exprOp '(' expr ')'
			{ $$=new ExprUop($1,$1->code,$3); }
| logOp '(' expr ')'
			{ $$=new ExprUop($1,$1->code,$3); }
| sqrtOp '(' expr ')'
			{ $$=new ExprUop($1,$1->code,$3); }
| absOp '(' expr ')'
			{ $$=new ExprUop($1,$1->code,$3); }
| floorOp '(' expr ')'
			{ $$=new ExprUop($1,$1->code,$3); }
| '(' type ')' fixedExpr
			{ $$=new ExprCast($1,$2,$4); }
| '(' sizedType ')' fixedExpr
			{ $$=new ExprCast($1,$2->code==SIGNED?true:false,$4); }
| floatExpr
			{ $$=$1; }
;

floatExpr
: fixedExpr
			{ $$=$1; }
;

/*: fixedExpr 'e' atomExpr
			{ 
				
				Type *t=$1->getType();
				double fixedVal=0;
				if(t->getTypeKind()==TYPE_FIXED) {
					TypeFixed *tf=(TypeFixed*)t;
					fixedVal=((ExprValue*)$1)->getIntVal()+(((ExprValue*)$1)->getFracVal()/pow(10,tf->getFracWidth()));
				} else {
					fixedVal=((ExprValue*)$1)->getIntVal();
				}
				double doubleValue=fixedVal*pow(10,((ExprValue*)$3)->getIntVal());
				$$ = (Expr*) new ExprValue(NULL,new Type(TYPE_DOUBLE),doubleValue);
			}
| fixedExpr
			{ $$=$1; }
;
*/

fixedExpr
: atomExpr '.' atomExpr
			{
			  Type *ti=$1->getType(),
			       *tf=$3->getType();
			  // possible unlinked param types (ti,tf=NULL)
			  if (ti && tf)
			  {
			    if (ti->isSigned() || tf->isSigned())
			      yyerror("fixed number requires unsigned parts");
			    // detect constant parts:
			    if ($1->getExprKind()==EXPR_VALUE &&
			        $3->getExprKind()==EXPR_VALUE &&
				ti->getTypeKind()==TYPE_INT   &&
				tf->getTypeKind()==TYPE_INT)
			    {
			      int	wi=ti->getWidth(),
					wf=tf->getWidth();
			      long long vi=((ExprValue*)$1)->getIntVal(),
					vf=((ExprValue*)$3)->getIntVal();
			      Type *t = new TypeFixed(wi,wf,false);
			      $$=(Expr*)new ExprValue($2,t,vi,vf);
			    }
			    else // non-const parts
			    {
			    	printf("WTFBBQ!");
			      $$=(Expr*)new ExprBop($2,'.',$1,$3);
			    }
			  }
			  else
			    $$=(Expr*)new ExprBop($2,'.',$1,$3);
			}
| atomExpr
			{ $$=$1; }
;

atomExpr
: lvalue
			{ $$=$1; }
| lvalue '@' atomExpr
			{
			  ExprLValue *lvalue=(ExprLValue*)$1;
			  $$=new ExprLValue(lvalue->getToken(),
					    lvalue->getSymbol(),
					    lvalue->usesAllBits() ? NULL :
					      lvalue->getPosLow(),
					    lvalue->usesAllBits() ? NULL :
					      lvalue->getPosHigh(),
					    $3,
					    lvalue->getArrayLoc());
			  // should just update token & retime directly in $1
			}
| NUM
			{ 
			  const char *tokenChars=$1->str;
			  if (  tokenChars[0]=='0' &&
			       (tokenChars[1]=='b' || tokenChars[1]=='B'))
			    // binary "0b..."
			    $$=constIntExpr(strtoll($1->str.del(0,1),NULL,2),
					    $1);
			  else
			    // decimal, octal, or hex
			    $$=constIntExpr(strtoll($1->str,NULL,0),$1);
			}

| NUMDBL
			{
			  $$=(Expr*)new ExprValue($1, new Type(TYPE_DOUBLE),strtod($1->str,NULL));
			}
| TRUE
			{ $$=(Expr*)new ExprValue($1,new Type(TYPE_BOOL),1,0); }
| FALSE
			{ $$=(Expr*)new ExprValue($1,new Type(TYPE_BOOL),0,0); }
| call
			{ $$=$1; }
| builtinExpr
			{ $$=$1; }
| '(' expr ')'
			{ $$=$2; }
;

builtinExpr
: CAT     '(' args ')'
			{ $$=new ExprBuiltin($1,$3,builtin_cat); }
/*
| EOS     '(' ID_ ')'
			{
			  list<Expr*> *args=new list<Expr*>;
			  ExprLValue *e=new ExprLValue($3,lookup($3));
			  args->append(e);
			  $$=new ExprBuiltin($1,args,builtin_eos);
			}
*/
| WIDTHOF '(' expr ')'
			{
			  list<Expr*> *args=new list<Expr*>;
			  args->append($3);
			  $$=new ExprBuiltin($1,args,builtin_widthof);
			}
| BITSOF  '(' expr ')'
			{
			  list<Expr*> *args=new list<Expr*>;
			  args->append($3);
			  $$=new ExprBuiltin($1,args,builtin_bitsof);
			}
;

equalOp
: EQUALS
| NOT_EQUALS
;

inequalOp
: '<'
| '>'
| LTE
| GTE
;

shiftOp
: LEFT_SHIFT
| RIGHT_SHIFT
;

addOp
: '+'
| '-'
;

exprOp
: EXP
;

logOp
: LOG
;

sqrtOp
: SQRT
;

absOp
: ABS
;

floorOp
: FLOOR
;

prodOp
: '*'
| '/'
| '%'
;

unaryOp
: '+'
| '-'
| '!'
| '~'
;

%%
