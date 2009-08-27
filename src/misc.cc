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
// SCORE TDF compiler:  miscellaneous utility functions
// $Revision: 1.135 $
//
//////////////////////////////////////////////////////////////////////////////

#include "compares.h"
#include "parse.h"
#include "suite.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "linux_meminfo.h"
#include "state.h"
#include "stmt.h"
#include "expr.h"
#include "annotes.h"
#include "misc.h"
#include "operator.h"

using std::cerr;
using std::endl;

////////////////////////////////////////////////////////////////
//  globals

int gErrorCount		= 0;		// number of errors   during compile
int gWarningCount	= 0;		// number of warnings during compile
int gPrintTime		= 0;		// print time at timestamps
int gPrintMemory	= 0;		// print memory usage at timestamps
int gDebugShared	= 0;		// debug mode: find shared AST nodes
int gDeleteType		= 1;		// enable deletion of types
int gEmitVCG		= 0;		// enables emission of .vcg files
int gProfileStates	= 0;		// generate code to profile states

struct timeval	gStartTime;		// for time accounting 
struct timeval	gLastTime;		// for time accounting 
size_t		gLastMem;		// for memory accounting
struct timezone	gTimeZone;		// dummy for gettimeofday call


////////////////////////////////////////////////////////////////
//  init/exit

void tdf_init ()
{
  // - initialize TDF compiler
  // - call once at start of program
  Type::init();
  // Expr::init();	// called from Type::init() for proper ordering
  Symbol::init();
  OperatorBuiltin::init();
  gettimeofday(&gStartTime,&gTimeZone);
  gLastTime.tv_sec=gStartTime.tv_sec;
  gLastTime.tv_usec=gStartTime.tv_usec;
#ifdef __linux__
  gLastMem=memory_in_use();
#else
  gLastMem=0;
#endif
}


void tdf_exit ()
{
  // - cleanup TDF compiler
  // - call once at end of program
  timestamp(string("tdf_exit"));
}


////////////////////////////////////////////////////////////////
//  error handling

void warn (const string &msg, Token *errToken, bool usePerror)
{
  // - print error

  ++gWarningCount;
  extern bool gNoWarnings;
  if (gNoWarnings)
    return;
  if (errToken)
    cerr << *errToken->file << ":"
         <<  errToken->line << ": at \""
         <<  errToken->str  << "\": ";
  if (usePerror)
    perror("warning: "+msg);
  else
    cerr << "warning: " << msg << "\n";
}


void error (int exitCode, const string &msg,
	    Token *errToken, bool usePerror)
{
  // - print error

  ++gErrorCount;
  if (errToken)
    cerr << *errToken->file << ":"
         <<  errToken->line << ": at \""
         <<  errToken->str  << "\": ";
  if (usePerror)
    perror(msg);
  else
    cerr << msg << "\n";
}


void fatal (int exitCode, const string &msg,
	    Token *errToken, bool usePerror)
{
  // - print error + terminate

  error(exitCode,msg,errToken,usePerror);
  if (yydebug)
    abort();
  else
    exit(exitCode);
}


////////////////////////////////////////////////////////////////
//  findShared
//   - debugging service to identify shared (non-unique) AST nodes
//   - maps over AST, aborts if finds multiply-visited nodes

typedef set<Tree*> SharedSet;
#define markShared(setP,t) (setP->insert(t))
#define isShared(setP,t)   (setP->member(t))

static
Tree** findTreeRef (Tree *p, Tree *r)
{
  // - returns first apparent reference to r in p
  // - searches for r's bit-pattern, assumes pointers are alligned
  Tree **pStart = (Tree**)p,
       **pEnd   = (Tree**)p + p->getSizeof()/sizeof(p),
       **pIter;
  for (pIter=pStart; pIter<pEnd; pIter++)
  {
    if (*pIter==r)
      return pIter;
  }
  return NULL;
}

static
bool findSharedMap (Tree *t, void *i)
{
  SharedSet *shared=(SharedSet*)i;
  if (t!=type_none && t!=type_any)
  {
    if (isShared(shared,t))
    {
      cerr << "AST node " << t << " \"" << t->toString()
	   << "\" is shared, apparently referenced by:\n";
      Tree *p;
      forall (p,*shared)
	if (findTreeRef(p,t))
	  cerr << "  " << p << " \"" << p->toString() << "\""
	       << (t==p->getParent()?" (as parent)\n":"\n");
      cerr << "parent is " << t->getParent()
	   << " \"" << t->getParent()->toString() << "\"" << '\n';
      abort();
    }
    markShared(shared,t);
  }
  return true;
}

static
void findShared (Tree *t)
{
  SharedSet shared;
  t->map(findSharedMap,(TreeMap)NULL,&shared);
}


////////////////////////////////////////////////////////////////
//  time and memory accounting

static
bool total_size(Tree *t, void *aux)
{
  size_t *size=(size_t *)aux;
  *size+=t->getSizeof();
  return(1);
}

static
bool total_size_above_types(Tree *t, void *aux)
{
  size_t *size=(size_t *)aux;
  if (t->getKind()==TREE_TYPE)
    return(0);

  *size+=t->getSizeof();
  return(1);
}

  
void timestamp(string note)
{
  timestamp(note,gSuite);
}

void timestamp(string note, Tree *t)
{

  struct timeval cTime;
  gettimeofday(&cTime,&gTimeZone);
  double dtime=((double)(cTime.tv_sec-gLastTime.tv_sec)+
		(double)(cTime.tv_usec-gLastTime.tv_usec)/(1000000.0));
  gLastTime.tv_sec=cTime.tv_sec;
  gLastTime.tv_usec=cTime.tv_usec;
  double ctime=((double)(gLastTime.tv_sec-gStartTime.tv_sec)+
		(double)(gLastTime.tv_usec-gStartTime.tv_usec)/(1000000.0));
  // perhaps condition this (or even send it elsewhere...)
  if (gPrintTime)
    fprintf(stderr,"Time at %s is %7.3f (delta %7.3f)\n",
	    (const char *)note,ctime,dtime);
  if (gPrintMemory)
    {
      #ifdef __linux__
        size_t memory_use=memory_in_use();
	cerr << "Memory"
	     << " delta=" << string("%8ld",(long)(memory_use-gLastMem))
	     << "KB total=" << string("%8ld",(long)memory_use)
	     << "KB" 
	     << " at " << note;
	gLastMem=memory_use;
      #else
	cerr << "Memory at " << note;
      #endif

      if (t)
	{
	  size_t all_nodes=0;
	  size_t all_nodes_no_types=0;
	  t->map(total_size,NULL,&all_nodes);
	  t->map(total_size_above_types,NULL,&all_nodes_no_types);
	  cerr << " total=" << all_nodes 
	       << " above_type=" << all_nodes_no_types << endl;
	}
      else
	cerr << " (no node specified)\n";
    }

  /*
  if (gPrintMemory && t)
    {
      size_t all_nodes=0;
      size_t all_nodes_no_types=0;
      t->map(total_size,NULL,&all_nodes);
      t->map(total_size_above_types,NULL,&all_nodes_no_types);
#ifdef __linux__
      size_t memory_use=memory_in_use();
      cerr << "Memory"
	   << " delta=" << string("%8ld",(long)(memory_use-gLastMem))
	   << "KB total=" << string("%8ld",(long)memory_use)
	   << "KB" 
	   << " at " << note 
	   << " total=" << all_nodes 
	   << " above_type=" << all_nodes_no_types 
	   << endl;
      gLastMem=memory_use;
#else
      cerr << "Memory at " << note << " total=" << all_nodes 
	   << " above_type=" << all_nodes_no_types << endl;
#endif
    }
  */

  if (gDebugShared && t)
  {
    cerr << "Searching for shared nodes at " << note << endl;
    findShared(t);
  }
}


////////////////////////////////////////////////////////////////
//  symbol scope management

static
bool associateScopes_map (Tree *t, void *i)
{
  // - associate local symtabs w/nil-scope to have scope ((SymTab*)i
  SymTab *scope=(SymTab*)i;
  if (t->getKind()==TREE_SYMTAB && t->getScope()==NULL)
  {
    ((SymTab*)t)->setScope(scope);
    return false;
  }
  else
    return true;
}


static
bool dissociateScopes_map (Tree *t, void *i)
{
  // - dissociate local symtabs with scope ((SymTab*)i to have nil scope
  SymTab *scope=(SymTab*)i;
  if (t->getKind()==TREE_SYMTAB && t->getScope()==scope)
  {
    ((SymTab*)t)->setScope(NULL);
    return false;
  }
  else
    return true;
}


void associateScopes (Tree *t, SymTab *scope)
{
  t->map(associateScopes_map,(TreeMap)NULL,scope);
}


void dissociateScopes (Tree *t, SymTab *scope)
{
  t->map(dissociateScopes_map,(TreeMap)NULL,scope);
}


////////////////////////////////////////////////////////////////
//  debugStateFiring

bool debugStateFiring_map (Tree *t, void *i)
{
  // - instrument each state-case with printf at top to show firing info
  // - printf includes op name, state name, and token values/eos

  switch (t->getKind())
  {
    case TREE_SUITE:
    case TREE_OPERATOR:
    case TREE_STATE:
      {
	return true;
      }
    case TREE_STATECASE:
      {
	StateCase *sc=(StateCase*)t;
	Operator *op=sc->getOperator();
	string fmt = string("Fire ")
		   + (op ? (op->getName()+", ") : string())
		   + "state " + ((State*)sc->getParent())->getName()
		   + " (";
	list<Expr*> *printf_args=new list<Expr*>;
	InputSpec *ispec;
	int i=0;
	forall (ispec,*(sc->getInputs())) {
	  Expr *numTokens=ispec->getNumTokens();
	  if (!(numTokens->equals((Expr*)expr_1)))
	    warn("option -ads does not handle multiple consumption "
		 " in state input signature, will only show first token");
	  if (i++>0)
	    fmt += ",";
	  if (ispec->isEosCase()) {
	    fmt += "eos";
	  }
	  else {
	    fmt += "%Ld";
	    printf_args->append(new ExprLValue(NULL,ispec->getStream()));
	  }
	}
	fmt += ")\\n";
	ExprBuiltin *eb = new ExprBuiltin(NULL,printf_args,builtin_printf);
	eb->setAnnote_(ANNOTE_PRINTF_STRING_TOKEN,
		       new Token(STRING,0,NULL,fmt));
	StmtBuiltin *sb = new StmtBuiltin(NULL,eb);
	// - push printf at top of state, or top of first stmt block
	Tree *parent=sc;
	list<Stmt*> *stmts=sc->getStmts();
	if (!stmts->empty() && stmts->head()->getStmtKind()==STMT_BLOCK) {
	  parent=stmts->head();
	  stmts=((StmtBlock*)parent)->getStmts();
	}
	sb->setParent(parent);
	stmts->push(sb);	// - HACK: modifying stmt list directly
	return false;
      }
    default:
      {
	return false;
      }
  }
  return true;	// - dummy
}


void debugStateFiring (Tree *t)
{
  // - instrument each state-case with printf at top to show firing info
  t->map(debugStateFiring_map);
}


////////////////////////////////////////////////////////////////
//  gdb debugging

const char* printstr  (string &s)  { static string str;
				     str=s;
				     return (const char*)str; }

const char* printtree (Tree   *t)  { static string str;
				     str=t->toString();
				     return (const char*)str; }

void *printstr_ =(void*)printstr;     // - reference fns so linker keeps them
void *printtree_=(void*)printtree;


////////////////////////////////////////////////////////////////
//  countBits

int countBits (long long val)
{
  // - return number of significant low-order bits
  // - converse of count-leading-zeros

  for (int numBits=sizeof(val)*8; numBits>0; numBits--)
    if (val & (1ll<<(numBits-1)))
      return numBits;
  return 0;
}


////////////////////////////////////////////////////////////////
//  tdfcComment

string tdfcComment ()
{
  // - return comment:  "Emitted by tdfc version <version>, <date>\n"

  string ret;

  extern char *gProgName;			// - in "tdfc.cc"
  time_t clock=time(NULL);
  ret += "Emitted by " + string(gProgName)
      +  " version "   + TDFC_VERSION
      +  ", "          + ctime(&clock);		// ctime has "\n"

  return ret;
}


////////////////////////////////////////////////////////////////
//  findCalledOps

bool findCalledOps_map (Tree *t, void *i)
{
  // - find all calls in compositional operator *t
  //   and append the called ops to list<Operator*> *i

  list<Operator*> *ops = (list<Operator*>*)i;

  switch (t->getKind()) {
    case TREE_OPERATOR:	return true;
    case TREE_STMT:	return true;
    case TREE_EXPR:	if (((Expr*)t)->getExprKind()==EXPR_CALL) {
			  ops->append(((ExprCall*)t)->getOp());
			  return false;
			}
			else
			  return true;
    default:		return false;
  }
}


list<Operator*> findCalledOps (OperatorCompose *op)
{
  // - find and return ops called in compositional *op
  list<Operator*> calledops;
  op->map(findCalledOps_map, (TreeMap)NULL, (void*)&calledops);
  return calledops;
}
