/////////////////////////////////////////////////////////////////////////////
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
// SCORE TDF compiler:  generate C++ output for Dataflow graphs (Nachiket)
// $Revision: 1.115 $
//
//////////////////////////////////////////////////////////////////////////////
#include "compares.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <LEDA/core/sortseq.h>
#include <LEDA/core/string.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/node_set.h>
#include <LEDA/graph/node_list.h>
#include "operator.h"
#include "expr.h"
#include "misc.h"
#include "ops.h"
#include "symbol.h"
#include "stmt.h"
#include "state.h"
#include "annotes.h"
#include "version.h"
#include "feedback.h"
#include "bindvalues.h"
#include "cctype.h"
#include "ccannote.h"
#include "cccase.h"
#include "ccheader.h"
#include "ccbody.h"
#include "cceval.h"
#include "ccstmt.h"
#include "ccprep.h"
#include "ccmem.h"
#include "instance.h" // blah, just for unique_name()
#include "cccopy.h"
#include "blockdfg.h" // Added by Nachiket on 11/29/2009

/***********************************************************************
Note:
  This file mirrors ccbody.cc but avoids unnecessary complexities
  * not handle retiming
  * not handle IPC with HSRA simulator
***********************************************************************/    

using leda::sortseq;
using leda::list_item;
using leda::dic_item;
using std::ofstream;
using leda::node_array;
using leda::node;
using leda::edge;
using leda::node_set;
using leda::node_list;
using leda::array;

void computeASAPOrdering(BlockDFG* dfg, node_list* arranged_list, node_array<int>* depths);
//string nodetostring(node n, Tree* t, int nodenum, bool gappa = false);
string nodetovarstring(node n, Tree* t);
string nodetofnstring(node n, Tree* t);
string nodetofout(BlockDFG* dfg, node src, node_array<int> nodenums); // simplify node name generation for all types of operations

void ccDfgComposeEvalExpr(ofstream *fout, Expr *expr, Symbol *rsym)
{
  if (expr->getExprKind()==EXPR_VALUE)
    *fout << expr->toString();
  else if (expr->getExprKind()==EXPR_LVALUE)
    {
	ExprLValue *lexpr=(ExprLValue *)expr;
	if (lexpr->usesAllBits())
	  if (lexpr->getSymbol()==rsym)
	    *fout << "result" ;
	  else
	    *fout << lexpr->getSymbol()->getName();
	else
	  {
	    warn("not handling bit selects in compose exprs...should be decomposed earlier",
		 lexpr->getToken());
	  }
    }
  else if ((expr->getExprKind()==EXPR_CALL)
	   || ((expr->getExprKind()==EXPR_BUILTIN) &&
	       (((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		==BUILTIN_SEGMENT)
	       ||((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		==BUILTIN_COPY)
	       ||((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		==BUILTIN_TERMINATE))))
    {
      ExprCall *ecall=(ExprCall *)expr;
      Operator *cop=ecall->getOp();
      list<Expr*>* args=ecall->getArgs();
      if (expr->getExprKind()==EXPR_BUILTIN)
	{

	  if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		==BUILTIN_COPY)
	    {
	      *fout << "new " << SCORE_COPY_NAME << endl;
	    }
	  else if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		   ==BUILTIN_TERMINATE)
	    {
	      *fout << "STREAM_FREE" <<  endl;
	    }
	  else
	    {
	      *fout << "new " << ccMemoryOperatorName(cop);
	    }
	}
      else
	{
	  if (noReturnValue(cop->getRetSym()))
	    *fout << "new " ;
	  *fout << cop->getName();
	}
      *fout << "(" ;
      Expr *earg;
      int first=1;
      forall(earg,*args)
	{
	  if (!first)
	      *fout << ",";
	  else
	      first=0;
	  ccDfgComposeEvalExpr(fout,earg,rsym);
	}
      *fout << ")";
    }
  // NOTE: these should only be viable if they evaluate to a SCALAR!
  // assuming type checking already caught that...
  else if (expr->getExprKind()==EXPR_COND)
      {
	ExprCond * cexpr=(ExprCond *)expr;
	*fout << "(";
	ccDfgComposeEvalExpr(fout,cexpr->getCond(),rsym);
	*fout << ")?(";
	ccDfgComposeEvalExpr(fout,cexpr->getThenPart(),rsym);
	*fout << "):(";
	ccDfgComposeEvalExpr(fout,cexpr->getElsePart(),rsym);
	*fout << ")";
      }
  else if (expr->getExprKind()==EXPR_BOP)
      {
	ExprBop *bexpr=(ExprBop *)expr;

	// wrong thing for "." operator...
        // TODO: deal properly with fixed point construction/representation
	*fout << "(";
	ccDfgComposeEvalExpr(fout,bexpr->getExpr1(),rsym);
	*fout <<  opToString(bexpr->getOp());
	ccDfgComposeEvalExpr(fout,bexpr->getExpr2(),rsym);
	*fout << ")";
      }
  else if (expr->getExprKind()==EXPR_UOP)
      {
	ExprUop *uexpr=(ExprUop *)expr;
	string ops=opToString(uexpr->getOp());
	Expr *iexpr=uexpr->getExpr();
	*fout << "(" << ops << "(";
	ccDfgComposeEvalExpr(fout,iexpr,rsym);
	*fout << "))";
      }
  else if (expr->getExprKind()==EXPR_CAST)
      {

	ExprCast *cexpr=(ExprCast *)expr;
	Expr *real_expr=cexpr->getExpr();

	// only thing I need to do is deal with unsigned->signed upgrade

	if (cexpr->getType()->isSigned() &&
	    (!real_expr->getType()->isSigned()))
	  {
	    *fout << "((long)";
	    ccDfgComposeEvalExpr(fout,real_expr,rsym);
	    *fout << ")";
	  }
	else
	  ccDfgComposeEvalExpr(fout,real_expr,rsym);
      }
  // ... to here only viable for SCALARS
  else if (expr->getExprKind()==EXPR_BUILTIN)
    {
      ExprBuiltin *bexpr=(ExprBuiltin *)expr;
      OperatorBuiltin *bop=(OperatorBuiltin *)(bexpr->getOp());
      if (bop->getBuiltinKind()==BUILTIN_BITSOF)
	{
	  Expr *first=bexpr->getArgs()->head();

	  *fout << "(" << getBits(first) << ")" ;
	}
      else if (bop->getBuiltinKind()==BUILTIN_WIDTHOF)
	{
	  Expr *first=bexpr->getArgs()->head();
	  *fout << "(" << getWidth(first) << ")" ;
	}
      else if (bop->getBuiltinKind()==BUILTIN_CAT)
	{
	  // N.B. using cceval here because
	  //    the only cases using this cat at this point
	  // should be those operating on scalar parameters
	  *fout << ccEvalExpr(expr);
	}
      else
	{
	  warn(string("not handling builtin kind %d in compose",
		      (int)bop->getBuiltinKind()), expr->getToken());
	}
    }
  else
    {
      fatal(-1,string("unknown 'acceptable' expression type [%d] in compose",
		      (int)expr->getExprKind()),
	    expr->getToken());
    }
}

//

void ccDfgCompose (ofstream *fout, string name, OperatorCompose *op)
{

  // How do streams get created in the behavioral world?
  //  in only one of two ways:
  //  (1) as a return value from a functional operator
  //  (2) as an intermediate (local) in a compose operator
  //      which is *not* the result of an assignment
  //
  //  So, whenever an operator is called, all of its input and
  //   output streams are defined, though it's result stsream
  //   may not be.
  //
  //  If new streams are necessary to link up child operators,
  //   it will create them.
  //
  //  When we see assignments to an intermediate stream
  //    (a) from the return value of an operator
  //    (b) from another stream
  //  That's an indication we don't need to create a new stream,
  //  but can use the other stream or return value stream.
  //
  //         -- amd 8/24/99


  // walk over symbols
  //  a. declare
  //  b. add to set

  Symbol *returnValue=op->getRetSym();
  SymTab *symtab=op->getVars(); // get internal streams
  // *not* the formal parameters
  list<Symbol*>* lsyms=symtab->getSymbolOrder();
  set<Symbol*>* syms=new set<Symbol*>();
  set<Symbol*>* partial=new set<Symbol*>();
  list_item item;

  if (!noReturnValue(returnValue))
    syms->insert(returnValue);

  // n.b. result exists in header definition of operator
  // doesn't need to be declared here

  forall_items(item,*lsyms)
    {
      Symbol *asum=lsyms->inf(item);
      syms->insert(asum);
      *fout << "    " << getCCtype(asum,1) << " " << asum->getName()
	    << ";" << endl;

    }

  // walk over stmts
  //   subtracting LHS from set

  list<Stmt *>* statements=op->getStmts();
  Stmt *statement;
  forall(statement,*statements)
    {
      if (statement->okInComposeOp())
	{
	  if (statement->getStmtKind()==STMT_ASSIGN)
	    {
	      ExprLValue *lval=((StmtAssign *)statement)->getLValue();
	      Symbol *lsym=lval->getSymbol();
	      // n.b. assume for now that split busses/composition cannonicalized elsewere
	      if (lval->usesAllBits())
		syms->del(lsym);
	      else
		{
		  partial->insert(lsym);
		  warn("ccDfgCompose: not expecting/handling partial stream assignment",
		       lsym->getToken());
		}
	    }
	}
      else
	{
	  // maybe a noop if other code complains on first pass
	  error(-1,"bad statement in compose",statement->getToken());
	}
    }

  // walk over set
  //   creating streams AND segments
  Symbol *sym;
  forall(sym,*syms)
    {
      if (sym==returnValue)
	*fout << "    result="
	      << getCCtypeConstructor(sym,1) << ";" << endl;
      else
	*fout << "    " << sym->getName() << "="
	      << getCCtypeConstructor(sym,1) << ";" << endl;

      if (sym->isArray())
	{
	  // write initial values into memory
	  // NOTE: could change this to some kind of DMA...
	  //   if/when we have such instructions
	  SymbolVar *svar=(SymbolVar *)sym;
	  Expr      *aval=svar->getValue();
	  if (aval)		// uninitialized array is scratch (EC 12/4/99)
	    {
	      if (aval->getExprKind()==EXPR_ARRAY)
		{
		  Type  *aeType   =((TypeArray*)sym->getType())->getElemType();
		  string aeTypeStr=getCCvarType(aeType);
		  ExprArray *earr =(ExprArray *)aval;
		  list<Expr *>*elist=earr->getExprs();
		  Expr *e;
		  int i=0;
		  forall (e,*elist)
		    {
		      // *fout << "    ((" << getCCvarType(e->getType())
		      //  - cast to array element's type,
		      //    not value element's type (EC 1/4/99)
		      // *fout << "    ((" << aeTypeStr
		      //  - cast to (long long) to match
		      //    C implementation of segments (EC 8/13/04)
		      *fout << "    ((" << "long long"
			    <<" *)" << sym->getName() << "->data())["
			    << i << "]=" << ccEvalExpr(e) << ";" << endl;
		      i++;
		    }
		}
	      else
		{
		  error(-1,
			string("symbol [%s] of array type, but has non-array value [%s]",
			       (const char *)sym->getName(),
			       (const char *)aval->toString()),
			sym->getToken());
		}
	    }
	}
    }

  // walk over stms
  //   creating instances
  forall(statement,*statements)
    {
      if (statement->okInComposeOp())
	{
	  if ((statement->getStmtKind()==STMT_CALL)
	      ||(statement->getStmtKind()==STMT_BUILTIN))
	    {
	      *fout << "    " ;
	      ccDfgComposeEvalExpr(fout,((StmtCall *)statement)->getCall(),
				returnValue);
	      *fout << ";" << endl;
	    }
	  else if (statement->getStmtKind()==STMT_ASSIGN)
	    {
	      ExprLValue *lval=((StmtAssign *)statement)->getLValue();
	      Symbol *lsym=lval->getSymbol();

	      if (lsym==returnValue)
		{
		  *fout << "    result=" ;
		  ccDfgComposeEvalExpr(fout,((StmtAssign *)statement)->getRhs(),
				    returnValue);

		}
	      else
		{
		  *fout << "    " << lsym->getName() << "=" ;
		  ccDfgComposeEvalExpr(fout,((StmtAssign *)statement)->getRhs(),
				    returnValue);
		}

	      *fout << ";" << endl;


	    }
	  else
	    {
	      fatal(-1,string("unknown 'acceptable' statement type [%d] in compose",
			      (int)statement->getStmtKind()),
		    statement->getToken());

	    }
	}
      else
	{
	  //noop, already complained
	}
    }


}


//
////////////////////////////////////////////////////////////////////////
// constructor for master operator
void ccdfgconstruct(ofstream *fout,string name, Operator *op)
{
  Symbol *rsym=op->getRetSym();
  list<Symbol*> *argtypes=op->getArgs();
  string prefix="n_";

  // dump signature and count ins, outs, params
  *fout << name << "::" << name << "(";

  int ins=0;
  int outs=0;
  int params=0;
  int i=0;
  if (!noReturnValue(rsym))
    outs++;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (i>0) *fout << ",";

      *fout << getCCtype(sym) << " " << prefix << sym->getName();
      if (sym->isStream())
	{
	  SymbolStream *ssym=(SymbolStream *)sym;
	  if (ssym->getDir()==STREAM_OUT)
	    outs++;
	  else if (ssym->getDir()==STREAM_IN)
	    ins++;
	  else
	    {
	      error(-1,"Don't believe in streams w/ ANY direction",
		    sym->getToken());
	    }
	}
      else
	if (ccParamCausesInstance(sym))
	  params++;

      i++;
    }
  *fout << ")" << endl;

  *fout << "{" << endl;




  // parameter assignments and registration
  *fout << "  int *params=(int *)malloc(" << params << "*sizeof(int));" << endl;
  int pi=0;
  forall(sym,*argtypes)
    {
      if (ccParamCausesInstance(sym))
	{
	  *fout << "  " << sym->getName() << "=" 
		<< prefix << sym->getName() << ";" << endl ;
	  *fout << "  " << "params[" << pi << "]=" 
		<< prefix << sym->getName() << ";" << endl ;
	  pi++;
	}
    }
  *fout << "  addInstance(instances,params);" << endl;

  // check type expressions for all arguments
  forall(sym,*argtypes)
    {
      // TODO:
      //    should I only be checking streams (as am)
      //    or also checking all exprs?
      //    or maybe only should happen in proc_run or instance?
      //

      if (sym->isStream())
	{
	  Expr *pexpr=sym->getType()->getPredExpr();
	  if (pexpr!=(Expr *)NULL)
	    {
	      string pstr=ccEvalExpr(EvaluateExpr(pexpr));
	      *fout << " if (" << pstr << " == 0 ) " << endl;
	      *fout << "    throw (\"runtime type error constructing argument " 
		    << sym->getName() << " to constructor "
		    << name << "--failed type expression.\n";
	      *fout << "\texpr: " << pstr << "\");" << endl;
	    }
	}
    }


  // create instance name and resolve
  *fout << "  char * name=mangle(" << op->getName() << "_name," << params 
	<< ",params);" << endl;
  *fout << "  char * instance_fn=resolve(name);" << endl;

  // one way or another, launch instance
  *fout << "  assert (instance_fn==(char *)NULL);" << endl;

  if (!noReturnValue(rsym))
    {
      if (rsym->isStream())
	{
	  *fout << "  result=" << getCCtypeConstructor(rsym); 
	  *fout << ";" << endl;
	}
    }
  // ScoreGraphNode linkage

  int icnt=0;
  int ocnt=0;

  if (op->getOpKind()!=OP_COMPOSE)
    {
      *fout << "    declareIO(" << ins << "," << outs << ", "<<op->getName()<<"_name);" << endl;

      if (!noReturnValue(rsym))
	{
	  if (rsym->isStream())
	    {
	      // had better be an output stream
	      SymbolStream *ssym=(SymbolStream *)rsym;
	      if (ssym->getDir()!=STREAM_OUT)
		{
		  // error already issues
		}
	      else
		{
		  *fout << "    bindOutput(" << ocnt<< "," << "result," 
			<< getCCStreamType(rsym) << ");" << endl;
		  ocnt++;
		}
	    }
	  else
	    {
	      // presumably we've already issued this complaint above
	    }
	}
      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
		  *fout << "    bindOutput(" << ocnt << "," << prefix 
			<< sym->getName() << ","
			<< getCCStreamType(sym) << ");" << endl;
                  *fout << "    SCORE_MARKWRITESTREAM(" << prefix
                        << sym->getName() << ",globalCounter->threadCounter);"
                        << endl;
		  ocnt++;
		}
	      else if (ssym->getDir()==STREAM_IN)
		{
		  *fout << "    bindInput(" << icnt << ","<< prefix 
			<< sym->getName() << ","
			<< getCCStreamType(sym) << ");" << endl;
                  *fout << "    SCORE_MARKREADSTREAM(" << prefix
                        << sym->getName() << ",globalCounter->threadCounter);"
                        << endl;
		  icnt++;
		}
	      
	      else
		{
		  // already complained
		}
	    }
	}
    }
  if (op->getOpKind()==OP_BEHAVIORAL)
    {
      // TODO:  deal with running attached and keeping id for
      //         later join...
      // launch pthread
      *fout << "    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));\n"
	    << "    pthread_attr_init(a_thread_attribute);\n"
	    << "    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);\n"
	    << "    pthread_create(&rpt,a_thread_attribute,&" 
	    << op->getName() << "_proc_run, this);"
	    << endl;
    }
  else if (op->getOpKind()==OP_COMPOSE)
    {

      // bind up / rename inputs
      forall(sym,*argtypes)
	{
	  if (sym->isStream() || sym->isArray())
	    {
	      *fout << "    " << getCCtype(sym) << " " 
		    << sym->getName() << "=" << prefix << sym->getName() 
		    << ";" << endl;
	    }
	  else
	    {
	      // already complained
	    }
	}

      ccDfgCompose(fout,name,(OperatorCompose *)op);
    }
  else 
    {
      // maybe print getOpKind for offending...
      error(-1,"FUBAR: don't know how to handle op kind omitting ",
	    op->getToken());
    }

  *fout << "}" << endl; // end of constructor
  
}

//
////////////////////////////////////////////////////////////////////////
// procrun for master instance

void ccdfgprocrun(ofstream *fout, string name, Operator *op,
	       int debug_logic)
{
  *fout << "void *" << name << "::proc_run() {"  << endl;

  if (op->getOpKind()==OP_COMPOSE)
    {
      *fout << "  cerr << \"proc_run should never be called for a compose operator!\" << endl;" << endl;
    }
  else if (op->getOpKind()==OP_BEHAVIORAL)
    {
      OperatorBehavioral *bop=(OperatorBehavioral *)op;
      dictionary<string,State*>* states=bop->getStates();
      dic_item item;

      *fout << "  enum state_syms {" ;
	
      
      State* start_state=bop->getStartState(); 
      int first=1;
      forall_items(item,*states)
	{
	  if (!first)
	    *fout << "," ;
	  else
	    first=0;
	  
	  *fout << STATE_PREFIX << states->inf(item)->getName(); 
	}
      *fout << "};" << endl;

      *fout << "  state_syms state=" << STATE_PREFIX 
	    << start_state->getName() << ";" << endl; 
      // declare top level vars
      SymTab *symtab=bop->getVars();
      list<Symbol*>* lsyms=symtab->getSymbolOrder();
      list_item item2;
      forall_items(item2,*lsyms)
	{
	  Symbol *sum=lsyms->inf(item2);
	  SymbolVar *asum=(SymbolVar *)sum;
	  *fout << "  " << getCCvarType(asum) << " " << asum->getName() ;
	  Expr* val=asum->getValue();
	  if (val!=(Expr *)NULL)
	      *fout << "=" << ccEvalExpr(EvaluateExpr(val)) ;
	  *fout << ";" << endl;
	}

      // declare/initialize retiming slots for input streams
      // (1) map over and create annotations with all retiming exprs
      //op->map(collect_retime_exprs,ccbody_treemap_false,(void *)NULL);
      // n.b. This "works" by ripping an expression out of context
      //   If we allow the user to define a local variable in 
      //    a state then use that variable (or a chain of such variables)
      //    as a retiming offset, then this will fail because
      //    it won't have picked up the local variables.
      //   If we support that...we should come back and FIX this.
      //   (...but I'm going to fix other things first -- amd 7/14/99)
      // (2) walk over all inputs (below) and declare/initialize

      // declare/initialize stream open / free state
      //  (a) compile time version
      //  (b) run time version
      // declare variable/type for holding input stream tokens
      int icnt=0;
      int ocnt=0;
      Symbol *rsym=op->getRetSym();
      list<Symbol*> *argtypes=op->getArgs();
      if (!noReturnValue(rsym))
	{
	  if (rsym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)rsym;
	      if (ssym->getDir()!=STREAM_OUT)
		{
		  // error already issues
		}
	      else
		{
		  rsym->setAnnote(CC_STREAM_ID,(void *)ocnt);
		  ocnt++;
		}
	    }
	  else
	    {
	      // presumably we've already issued this complaint above
	    }
	}
      Symbol *sym;
      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
		  sym->setAnnote(CC_STREAM_ID,(void *)ocnt);
		  ocnt++;
		}
	      else if (ssym->getDir()==STREAM_IN)
		{
		  *fout << "  " << getCCvarType(sym) << " " 
			<< sym->getName() << ";" << endl;
		  sym->setAnnote(CC_STREAM_ID,(void *)icnt);
		  // Declare and initialize retime chain here
		  set<Expr *> *rset=
		    (set <Expr *> *)sym->getAnnote(MAX_RETIME_DEPTH);
		  set<Expr *> *ncset=new set<Expr *>();
		  int constmax=0;
		  Expr *rexp;
		  if (rset!=(set <Expr *> *)NULL)
		    {
		      forall(rexp,*rset)
			{
			  if (rexp->getExprKind()==EXPR_VALUE)
			    {
			      int eval=((ExprValue *)rexp)->getIntVal();
			      if (eval>constmax) constmax=eval;
			    }
			  else
			    ncset->insert(rexp);
			}
		    }
		  *fout << "    "
			<< sym->getName()
		    // TODO: assign initial values instead of 0
			<< "=0;" << endl;

		  icnt++;
		}
	      else
		{
		  // already complained
		}
	    }
	}
      int *early_free=new int[icnt];
      for (int i=0;i<icnt;i++)
	early_free[i]=0;
      int *early_close=new int[ocnt];
      for (int i=0;i<ocnt;i++)
	early_close[i]=0;
      // These are for when need to dynamicly track input/output closing
      *fout << "  int *input_free=new int[" << icnt << "];" << endl;
      *fout << "  for (int i=0;i<" << icnt << ";i++)" << endl;
      *fout << "    input_free[i]=0;" << endl;
      *fout << "  int *output_close=new int[" << ocnt << "];" << endl;
      *fout << "  for (int i=0;i<" << ocnt << ";i++)" << endl;
      *fout << "    output_close[i]=0;" << endl;
      *fout << "  int done=0;" << endl;

      // operative loop
      *fout << "  while (!done) {" << endl;

      int num_states=states->size();
      if (num_states>1)
	*fout << "    switch(state) {" << endl;
      int snum=0;
      forall_items(item,*states)
	{
	  State* cstate=states->inf(item);
	  string sname=cstate->getName();

//	  StateCase* case_inst;
//	  forall(case_inst, *cstate->getCases()) {
//		  cout << "Before processing=" << case_inst << endl;
//	  }

	  array<StateCase*>* cases=caseSort(cstate->getCases());
	  array<Symbol*>* caseIns=allCaseIns(cases);
	  array<int>* firstUsed=inFirstUsed(caseIns,cases);
          int numNestings=0;

	  if (num_states>1)
	    *fout << "      case " << STATE_PREFIX << sname
		  << ": { " << endl;

	string atomiceofrcase="0";

	  for (int i=cases->low();i<=cases->high();i++)
	    {
	      // walk over inputs to case
	      InputSpec *ispec;
	      StateCase *acase=(*cases)[i];

              // increment the nesting count and also output a beginning
              // nesting bracket.
              numNestings++;
              *fout << "        {" << endl;

	      forall (ispec,*(acase->getInputs()))
		{
		  //  find input in caseIns
		  int loc=caseIns->binary_search(ispec->getStream());
		  if (loc<caseIns->low())
		    fatal(-1,"inconsistency in ccproc_run search",
			  ispec->getStream()->getToken());
		  //  if firstUsed, set var by asking EOS
		  if ((*firstUsed)[loc]==i)
		    {
		      *fout << "        int eos_" 
			    << loc
			    << "=STREAM_EOS(" 
			    << "in[" 
			    << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			    << "]" 
			    << ");" << endl;

		      // EOF support added by Nachiket for C++ code generation on October 5th 2009
		      *fout << "        int eofr_"
			    << loc
			    << "=STREAM_EOFR("
			    << "in["
			    << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID))
			    << "]"
			    << ");" << endl;

		    }
		  
		}
	      *fout << "       ";
	      *fout << " if (1" ;
              // walk over and list vars/polarity
	      forall (ispec,*(acase->getInputs()))
		{
		  int loc=caseIns->binary_search(ispec->getStream());
		  *fout << "&&";
		  if (!ispec->isEosCase() && !ispec->isEofrCase()) {
		     *fout << "!eos_" << loc << "&&!eofr_" << loc;
		  } else if(ispec->isEosCase()) {
			  *fout << "eos_" << loc; // single out eos
		  } else if(ispec->isEofrCase()) {
			  *fout << "eofr_" << loc; // add eofr support

			  std::stringstream out; out<<loc;
			  atomiceofrcase+="|| eofr_" + string(out.str().c_str()); 

		  } // both eofr and eos cannot be set simultaneously!
		  //*fout << "eos_" << loc;
		}
	      *fout << ") {" << endl;

	      
	      forall (ispec,*(acase->getInputs()))
		{
		  if (!ispec->isEosCase() && !ispec->isEofrCase())
		    {
			// Added by Nachiket on Sep 29th to support floating-point operations
			TypeKind intyp = ispec->getStream()->typeCheck()->getTypeKind();
			bool floattyp=(intyp==TYPE_FLOAT);
			bool doubletyp=(intyp==TYPE_DOUBLE);

			// cout << "Type=" << typekindToString(intyp) << " for stream " << ispec->getStream()->getName() << endl;

		      *fout << "          " 
			    << ispec->getStream()->getName() 
			    << (floattyp? "=STREAM_READ_FLOAT(" : (doubletyp)? "=STREAM_READ_DOUBLE(" : "=STREAM_READ_NOACC(") 
			    << "in[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			    << "]"
			    << ");" << endl;
		    }
		  else if(ispec->isEosCase())
		    {
		    *fout << "          "
			  << "if (!input_free[" 
			  << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			  << "])" << endl;
		      *fout << "          " 
			    << "STREAM_FREE(" 
			    << "in[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			    << "]"
			    << ");" << endl;
		       early_free[(long)(ispec->getStream()->getAnnote(CC_STREAM_ID))]=1;
		       *fout << "          " 
			     << "input_free[" 
			     << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID))
			     << "]=1;" << endl;
		    }
			  // Special case of End-of-Frame
			  // consume token and simply move to a different state...
			  // XXX: No retiming being done on the EOFR token.. is this correct?
			  else if(ispec->isEofrCase())
			  {
					TypeKind intyp = ispec->getStream()->typeCheck()->getTypeKind();
					bool floattyp=(intyp==TYPE_FLOAT);
					bool doubletyp=(intyp==TYPE_DOUBLE);

					// dummy read of the token..
				    *fout << "          "
					    << (floattyp? "STREAM_READ_FLOAT(" : (doubletyp)? "STREAM_READ_DOUBLE(" : "STREAM_READ_NOACC(")
					    << "in[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID))
					    << "]"
					    << ");" << endl;

			  }
		}

	  // Nachiketism: Not adding eofr to debug logic yet...
	      if (debug_logic)
		{
		  *fout << "          " 
			<< "cerr << \"" << name << " entering state " 
			<< sname << "(\"";
		  int first=1;
		  forall (ispec,*(acase->getInputs()))
		    {
		      if (!ispec->isEosCase())
			{
			  if (first==1)
			    first=0;
			  else
			    *fout << "<< \",\"";
			  *fout << "<< \""
				<< ispec->getStream()->getName() 
			        << "=\" <<"
				<< ispec->getStream()->getName() ;
			    }
		    }
                  *fout << "<< \")\" << endl;" << endl;
		}
	      BlockDFG dfgVal=(acase->getDataflowGraph());
	      h_array<node, Symbol*> symbolmap=(acase->getSymbolMap());
	      h_array<Symbol*,node> *valid_map=(acase->getValidMap());
	      /*
	      // 12/14/2009 The following debugging code shows that symbolmap is not being faithfully populated fully..
	      node n1;
		forall_defined(n1, symbolmap) {
			cout << "Symbolmap testing.. Node=" << n1 <<  " Symbol=" << symbolmap[n1] << endl;
		}
		// What a horrible waste of time? ExprLVAl is obtained from merely typecasting Tree*
		*/

	      BlockDFG* dfg=&dfgVal;
	      if(dfg==NULL) {
	    	  cout << "No dataflow graph found in state=" << sname << "["<< cstate << "] acase="<< acase <<"!" << endl;
	      } else {
	    	  cout << "Dataflow graph found in state=" << sname << "["<< cstate << "] acase="<< acase <<" DFG="<< &dfgVal << "!" << endl;
	    	  // dump out the dataflow graph..
	    	  int nodenum=0;

	    	  //cout << "Graph" << endl;
	    	  printf("dfg statistics->  %d nodes, %d edges\n",dfg->number_of_nodes(), dfg->number_of_edges());
	    	  fflush(stdout);
	    	  node n;

		  // compute latency ordered list of nodes based on ASAP scheduling
		  node_list arranged_list;
		  node_array<int> depths;

		  computeASAPOrdering(dfg, &arranged_list, &depths);

		  //cout << "Started" << endl;
		  node_array<int> nodenums(*dfg);		    
		  forall(n,arranged_list) {
		  	nodenums[n]=nodenum++;
		  }

		  // TODO: From the ASAP ordering, start printing out nodes... How about just building the dataflow expression?
		  forall(n,arranged_list) {
			Tree* t=(dfgVal)[n];
			cout << "Processing " << nodetostring(n,(dfgVal)[n],nodenums[n]);
			TypeKind type = ((Expr*)t)->typeCheck()->getTypeKind();
		  	if(!dfg->indeg(n)==0) {
				if(dfg->indeg(n)==3) {
					// if operator
					*fout << "          // IF Node: Type=" << typekindToCplusplus(type) << endl;
					*fout << "          " << typekindToCplusplus(type) << " ";
					*fout << nodetofout(dfg, n, nodenums) << " = ";
					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					int edgenum=0;
					edge e;
					string ifstr=" ";
					forall (e,dfg_in_edges_n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						/*if(edgenum==0) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") : ";
						} else if(edgenum==1) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") ";
						} else if(edgenum==2) {
							ifstr = "(" + nodetofout(dfg, src, nodenums) + ") ? " + ifstr;
						}*/
						if(edgenum==0) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") ? " ;
						} else if(edgenum==1) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") : ";
						} else if(edgenum==2) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") ";
						}
						edgenum++;
					}
					*fout << ifstr << ";" << endl;
				} else if(dfg->indeg(n)==2 && t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND) {
					// if operator
					*fout << "          // IF ? Node: Type=" << typekindToCplusplus(type) << endl;
					*fout << "          " << typekindToCplusplus(type) << " ";
					*fout << nodetofout(dfg, n, nodenums) << " = ";
					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					int edgenum=0;
					edge e;
					string ifstr=" ";
					forall (e,dfg_in_edges_n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						/*if(edgenum==0) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") : (" + nodetofout(dfg,n,nodenums) + ")";
						} else if(edgenum==1) {
							ifstr = "(" + nodetofout(dfg, src, nodenums) + ") ? " + ifstr;
						}*/
						if(edgenum==0) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") ? ";
						} else if(edgenum==1) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") : (" + nodetofout(dfg,n,nodenums) + ")";
						}
						edgenum++;
					}
					*fout << ifstr << ";" << endl;
				} else if(dfg->indeg(n)==2) {
					// binary operator
					*fout << "          // Binary Node: Type=" << typekindToCplusplus(type) << endl;
					*fout << "          " << typekindToCplusplus(type) << " ";
					*fout << nodetofout(dfg,n,nodenums) << " = ( ";
					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					int edgenum=0;
					edge e;
					forall (e,dfg_in_edges_n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						*fout << nodetofout(dfg, src, nodenums);
						*fout << " ";
						if(edgenum==0) {
							*fout << nodetofnstring(n,(dfgVal)[n]) + " ";
						}
						edgenum++;
					}
					*fout << ");" << endl;
				} else if(dfg->indeg(n)==1 && type!=TYPE_STATE) {
					// unary operator or function?
					*fout << "          // Unary Node: Type=" << typekindToCplusplus(type) << endl;
					*fout << "          " << typekindToCplusplus(type) << " ";
					*fout << nodetofout(dfg,n,nodenums) << " = ";
					*fout << nodetofnstring(n,(dfgVal)[n]) + " ( ";

//					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					edge e=dfg->first_in_edge(n);
//					forall (e,dfg_in_edges_n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						*fout << nodetofout(dfg, src, nodenums) << " ";
//					}

					*fout << ");" << endl;
				} else if(dfg->indeg(n)==1 && type==TYPE_STATE && num_states>1) {
					// unary operator or function?
					*fout << "          // Unary State Node: Type=" << typekindToCplusplus(type) << endl;
					*fout << "          state = ( ";

					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					edge e;
					forall (e,dfg_in_edges_n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						// @2/12/2010 apply prefix at right code point... *fout << "STATE_" << nodetofout(dfg, src, nodenums) << " ";
						*fout << nodetofout(dfg, src, nodenums) << " ";
					}

					*fout << ");" << endl;
				}
			}
		  	cout << " finished" << endl;
		  }

		  // Assign output nodes
		  forall(n,arranged_list) {
			  // fanout-0 nodes
			  if(dfg->outdeg(n)==0) {
				  Tree* t=(dfgVal)[n];
				  if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_LVALUE) {
					  Symbol *asym=((ExprLValue*)t)->getSymbol();
					  if (asym!=NULL && asym->isStream())
					  {
						  SymbolStream *ssym=(SymbolStream *)asym;
						  if (ssym->getDir()==STREAM_OUT)
						  {
						  	TypeKind outType = ((ExprLValue*)t)->typeCheck()->getTypeKind();
							bool floattype = (outType==TYPE_FLOAT);
						        bool doubletype = (outType==TYPE_DOUBLE);

							node valid_node = (*valid_map)[asym];
//							cout << "sym=" << asym ; asym=NULL;
//							cout << ", node=" << valid_node << " " << n << "=" << ((ExprLValue*)(*dfg)[valid_node])->getSymbol() << ", lval=" << ((ExprLValue*)(*dfg)[valid_node]) << " symbolmapped=" << symbolmap[valid_node] << endl;
							if(valid_node!=NULL) {
							        *fout << "          " ;
								*fout << "if(" << symbolmap[valid_node]->toString() << ") {" << endl;
							        *fout << "  " ;
							}

							int id=(long)(ssym->getAnnote(CC_STREAM_ID));
							*fout << "          " ;
							*fout  << (floattype?"STREAM_WRITE_FLOAT":doubletype?"STREAM_WRITE_DOUBLE":"STREAM_WRITE_NOACC")
								<< "(out[" << id << "]," << nodetofout(dfg,n,nodenums) << ");" << endl; // asym->toString();
							if(valid_node!=NULL) {
							        *fout << "          }" << endl;
							}
						  }
					  }
				  } else if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_BUILTIN) {
					  ExprBuiltin *bexpr = (ExprBuiltin*)t;
					  Symbol *asym=bexpr->getSymbol();
					  if (asym!=NULL && asym->isStream())
					  {
						  SymbolStream *ssym=(SymbolStream *)asym;
						  if (ssym->getDir()==STREAM_OUT)
						  {
							int id=(long)(ssym->getAnnote(CC_STREAM_ID));
							*fout << "          " ;
							if(((OperatorBuiltin*)bexpr->getOp())->getBuiltinKind()==BUILTIN_CLOSE) {
								*fout  << "STREAM_CLOSE(out[" << id << "]);" << endl;
							} else if(((OperatorBuiltin*)bexpr->getOp())->getBuiltinKind()==BUILTIN_FRAMECLOSE) {
								*fout  << "FRAME_CLOSE(out[" << id << "]);" << endl;
							}
						  }
					  }
				  }
			  }
		  }
		  //cout << "Ended.." << endl;
		  //exit(-1);


	      }
		{
		  //ccStmt(fout,string("          "),stmt,early_close,
			// STATE_PREFIX,0, false); // 0 was default for ccStmt.h
		}
	      *fout << "        }" << endl;
	      *fout << "        else" << endl;
	    }
	    // default case will be to punt out of loop (exit/done)
	    // For now. split eofr cases into separate statecases.. wtf!?
	  *fout << "        {" << endl ;
	  *fout << "        if (" << atomiceofrcase.cstring() << ") {}" << endl ;
	  *fout << "        else" << endl;
	  *fout << "         done=1;" << endl;
	  *fout << "        }" << endl;
	    
	  // close the nesting brackets.
	  *fout << "        ";
	  for (; numNestings>0; numNestings--) {
	    *fout << "}";
	  }

	  if (num_states>1)
	    {
	      *fout << "        break; " << endl;
	      *fout << "      } // end case " << STATE_PREFIX << 
		sname << endl;
	    }
	  snum++;
	}
      if (num_states>1)
	{
	  *fout << "      default: cerr << \"ERROR unknown state [\" << (int)state << \"] encountered in " << name << "::proc_run\" << endl;" << endl;
	  *fout << "        abort();" << endl;
	  *fout << "    }" << endl;
	}

      *fout << "  }" << endl;
      // any final stuff
      if (!noReturnValue(rsym))
	if (early_close[(long)(rsym->getAnnote(CC_STREAM_ID))])
	  {
	    *fout <<"  if (!output_close[" 
		  <<  (long)(rsym->getAnnote(CC_STREAM_ID))
		  << "])" << endl;
	    *fout << "  STREAM_CLOSE(" 
		  << "out[" << (long)(rsym->getAnnote(CC_STREAM_ID))
		  << "]" 
		  << ");" << endl;
	  }
	else
	  *fout << "  STREAM_CLOSE(" 
		<< "out[" << (long)(rsym->getAnnote(CC_STREAM_ID))
		<< "]"
		<< ");" << endl;
      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
		  if (early_close[(long)(ssym->getAnnote(CC_STREAM_ID))])
		    {
		      *fout <<"  if (!output_close[" 
			    <<  (long)(ssym->getAnnote(CC_STREAM_ID))
			    << "])" << endl;
		      *fout << "  STREAM_CLOSE(" 
			    << "out[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			    << "]" 
			    << ");" << endl;
		    }
		  else
		    *fout << "  STREAM_CLOSE(" 
			  << "out[" << (long)(ssym->getAnnote(CC_STREAM_ID)) 
			  << "]" 
			  << ");" << endl;
		}
	      else
		if (early_free[(long)(ssym->getAnnote(CC_STREAM_ID))])
		  {
		    cerr << "DEBUG early free[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			 << "]=" << early_free[(long)(ssym->getAnnote(CC_STREAM_ID))]
			 << endl;
		    
		    *fout <<"  if (!input_free[" 
			  << (long)(ssym->getAnnote(CC_STREAM_ID)) 
			  << "])" << endl;
		    *fout << "    STREAM_FREE(" 
			  << "in[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			  << "]" 
			  << ");" << endl;
		  }
		else
		  *fout << "  STREAM_FREE(" 
			<< "in[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			<< "]" 
			<< ");" << endl;
	    }
	}
    }
  else
    {
      fatal(-1,string("Don't know how to handle opKind=%d",
		      (int)op->getOpKind()),
		      op->getToken());
	    
    }

  *fout << "  return((void*)NULL); }" << endl;
  
}

//
////////////////////////////////////////////////////////////////////////
// Top level routine to create master C++ code
void ccdfgbody (Operator *op, int debug_logic)
{
  //N.B. assumes renaming of variables to avoid name conflicts
  //  w/ keywords, locally declared, etc. has already been done

  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name;
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+".cc";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "// cctdfc autocompiled header file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;

  // some includes
  *fout << "#include \"Score.h\"" << endl;
  *fout << "#include <errno.h>" << endl;
  *fout << "#include <stdio.h>" << endl;
  *fout << "#include <stdlib.h>" << endl;
  *fout << "#include <iostream>" << endl;
  *fout << "#include <string.h>" << endl;
  *fout << "#include <sys/types.h>" << endl;
  *fout << "#include <sys/ipc.h>" << endl;
  *fout << "#include <sys/msg.h>" << endl;
  *fout << "#include \"" << name << ".h\"" << endl;

  // include anythying I depend upon
  ccprep(op); // generic for things need to be done on pre pass
  set<Operator *>* called_ops=(set<Operator *>*)op->getAnnote(CC_CALL_SET);

  if (called_ops!=(set<Operator *>*)NULL)
    {
      Operator *cop;
      forall(cop,*called_ops)
	{
	  if (cop->getOpKind()!=OP_BUILTIN)
	    *fout << "#include \"" << cop->getName() << ".h\"" << endl;
	}
    }

  // broiler name
  *fout << "char * " << name << "_name=\"" << name << "\";" << endl;

  // proc_run wrapper
  *fout << "void * " << name << "_proc_run(void *obj) {" << endl;
  *fout << "  return(((" << classname << " *)obj)->proc_run()); }" << endl;

  // instance initiation
  *fout << "ScoreOperatorElement *" << classname << "init_instances() {" << endl;
  *fout << "  return(ScoreOperator::addOperator(" << name << "_name," 
	<< ccCountParameters(argtypes) << "," 
	<< count_args(argtypes) << ","
	<< ccParameterLocs(argtypes) << "));  }" << endl;
  *fout << "ScoreOperatorElement *" << classname 
	<< "::instances="<< classname <<"init_instances();" << endl;

  *fout << endl;

  // constructor
  ccdfgconstruct(fout,classname,op);

  *fout << endl;

  // proc_run
  ccdfgprocrun(fout,classname,op,debug_logic);

  *fout << endl;

  // if necessary, functional version
  if (!noReturnValue(rsym))
    functional_constructor(fout,name,classname,rsym,argtypes);

  // output the C linkage pseudo constructor.
  *fout << "#undef NEW_" << classname << endl;
  *fout << "extern \"C\" void *NEW_" << classname << "(" ;
  constructor_signatures(fout,rsym,argtypes);
  *fout << ") {" << endl;
  *fout << "  return((void *) (new " << classname << "(" ;
  constructor_signatures_notypes(fout,rsym,argtypes);
  *fout << ")));" << endl;
  *fout << "}" << endl;

  // close up
  fout->close();

}

/*
 * Compute a sorted sequence based on ASAP ordering
 * Borrowed code from blockdfg.cc's getTimingDepth
 */
void computeASAPOrdering(BlockDFG* dfg, node_list* arranged_list, node_array<int>* depths) {
	//cout << "Started.." << endl;
	node_array<int> touched;

	(*depths).init(*dfg,-1); // initialize depths
	touched.init(*dfg, -1); // initialized touch counts [This was crucial to access the array later! Whoa? 12/2/2009 9.34pm Philly time @US447]
	node_set frontier=node_set(*dfg);
	node n;
	forall_nodes (n,*dfg) {
		//cout << "Initialized Node=" << n << " Name=" << ((dfg)[n])->toString() << " Depth=" << depths[n] << endl;
		// - initialize numUnvisitedPreds and frontier
		int preds = touched[n] = dfg->indeg(n);
		if (preds==0) {                         // - initial frontier same as
			frontier.insert(n);             //   getInputNodes(*dfg),
			(*depths)[n]=1;             	//   node latency=1
		}
	}
	//cout << "Seeded/initialized.." << endl;

	while (!frontier.empty()) {
	    // Get node
	    node n=frontier.choose();
	    frontier.del(n);

	    // update depth
	    edge outedge;
	    list<edge> outedges = dfg->out_edges(n);
	    forall (outedge,outedges) {
		    node succ=dfg->target(outedge);
		    if (succ==n)      // ignore self-loops
			    continue;
		    int depth = (*depths)[n] + 1;
		    if ((*depths)[succ]<depth)
			   (*depths)[succ]=depth;

		    if (--touched[succ]==0)
			    // - add successor to frontier
			    frontier.insert(succ);
	    }

	}
	//cout << "Depth annotated.." << endl;

	// check if we screwed up..
	int max_depth=0;
	forall_nodes (n,*dfg) {
		assert(touched[n]==0);    // - assert BFS visited node
		assert((*depths)[n]!=-1);           // - assert valid depth
		if((*depths)[n]>max_depth)
			max_depth=(*depths)[n];
	}

	int current_depth=0;
	for(current_depth=0;current_depth<=max_depth;current_depth++) {
		forall_nodes (n,*dfg) {
			if((*depths)[n]==current_depth) {
				arranged_list->append(n);
			}
		}
	}

}

/*
 * Convert a node into it's string representation
		Tree *t=(*dfg)[n];
 *
 */
string nodetostring(node n, Tree* t, int nodenum, list<string>* list_input, bool LHS) {
//	std::stringstream out;
//	out << nodenum;
//	std::string str;
//	str = out.str();
//	char *buf = new char[std::strlen(str.c_str())];
//	std::strcpy(buf,str.c_str());

	string ret="";
	if(t->getKind()==TREE_EXPR) {
		if(((Expr*)t)->getExprKind()==EXPR_BOP || ((Expr*)t)->getExprKind()==EXPR_UOP) {
			ret = opToNodename(((ExprBop*)t)->getOp());
		} else if (((Expr*)t)->getExprKind()==EXPR_COND) {
			ret = "if";
		} else {
			ret = t ? t->toString().replace_all("\n","") : string("<nil>");
		}
	} else {
		string t_str = t ? t->toString().replace_all("\n","") : string("<nil>");
		ret = " "+treekindToString(t->getKind())+" "+ t_str;
	}
	bool found = false;
	if (list_input != NULL)
	{
		string temp;
		forall (temp, *list_input)
		{
			if ( ret == temp)
				found = true;
		}
	}
	
	/*if ((t->getKind()==TREE_EXPR))
		cout << "tree is TREE_EXPR" << endl;
	
	if (((Expr*)t)->getExprKind()==EXPR_VALUE)
		cout << "expr kind is EXPR_VALUE" << endl;
		*/				
	
	if(!((t->getKind()==TREE_EXPR) && (((Expr*)t)->getExprKind()==EXPR_VALUE)) && !found)  
	{
		ret += "_";
		ret += string("%d",nodenum);
	
	}
	else if (found && LHS)
	{

		list_input->remove(ret);			
		ret += "_";
		ret += string("%d",nodenum);
		
	}
		
	return ret;
}

/**
 * Convert a node to its operation string
 */
string nodetofnstring(node n, Tree* t) {
	string ret;
	if(t->getKind()==TREE_EXPR) {
		if(((Expr*)t)->getExprKind()==EXPR_BOP || 
			((Expr*)t)->getExprKind()==EXPR_UOP ||
			((Expr*)t)->getExprKind()==EXPR_COND) {
			ret = opToString(((ExprBop*)t)->getOp());
		} else {
			// unary is a simple assignment
			ret = "";
			//cerr << "Wrong optype for string conversion? ExprKind=" << exprkindToString(((Expr*)t)->getExprKind()) << endl;
		}
	} else {
		cerr << "Wrong kind for string conversion" << endl;
	}

	return ret;
}

/**
 * Simply return name of input
 */
string nodetovarstring(node n, Tree* t) {
	return t ? t->toString().replace_all("\n","") : string("<nil>");
}

/**
 * Share code that generates the name to use for an input node
 */
string nodetofout(BlockDFG* dfg, node src, node_array<int> nodenums) {

//	cout << " enter node to fout fn " << endl;

	if(dfg->indeg(src)==0 || dfg->outdeg(src)==0) { // shouldn't we process outputs similarly as well?
		
		//cout << " dfg->indeg(src)==0 || dfg->outdeg(src)==0" << endl;
		
		Tree* t=(*dfg)[src];
		TypeKind type = ((Expr*)t)->typeCheck()->getTypeKind();
		if(dfg->indeg(src)==0 && type==TYPE_STATE) 
		{
			
			//cout << " dfg->indeg(src)==0 && type==TYPE_STATE" << endl;
			
			return string("STATE_"+nodetovarstring(src, (*dfg)[src]));
		} 
		else if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_LVALUE) 
		{
			//cout << "t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_LVALUE"  << endl;
			
			Symbol *asym=((ExprLValue*)t)->getSymbol();
			if (asym!=NULL && asym->isStream() && !asym->isStreamValid())
			{
				//cout << "asym!=NULL && asym->isStream() && !asym->isStreamValid()" << endl;
				
				SymbolStream *ssym=(SymbolStream *)asym;
				if (ssym->getDir()==STREAM_IN)
				{
					//cout << " ssym->getDir()==STREAM_IN " << endl;
					return nodetovarstring(src, (*dfg)[src]);
				}
				else if(ssym->getDir()==STREAM_OUT)
				{
					//cout << "ssym->getDir()==STREAM_OUT" << endl;
					return nodetovarstring(src, (*dfg)[src]);

				}
				else 
				{
					cerr << "STREAM directions fail during name generation" << endl;
					exit(-1);
				}
			} 
			else if(asym!=NULL && asym->isReg())
			{ // local variables are "registers" you idiot! nomenclature!!
				//cout << "Found register! " << nodetovarstring(src, (*dfg)[src]) << endl;
				//cout << " asym!=NULL && asym->isReg()" << endl;
				return nodetovarstring(src, (*dfg)[src]);
			} 
			else if (asym!=NULL && asym->isStreamValid()) 
			{
				//cout << " asym!=NULL && asym->isStreamValid() " << endl;
//			cout << "what?>" << endl;
				//return string("what? asym->toString()");
				return asym->toString();
			} 
			else {
				//cout << "node is not a stream.. What kind of a variable is this? neither local nor stream!?!" << endl;
				return nodetostring(src, (*dfg)[src],nodenums[src]);
		//		exit(-1);
			}
		} 
		else {
			// this is a constant? what can we check?
			return nodetovarstring(src, (*dfg)[src]);
		}

	} 
	else {
		return nodetostring(src, (*dfg)[src],nodenums[src]);
	}

}
