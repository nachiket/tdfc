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
// SCORE TDF compiler:  generate C output for Xilinx Microblaze processor
//			generate code should run standalone
//			it should be possible to cross-compile on Intel
//
//////////////////////////////////////////////////////////////////////////////
#include "compares.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include <LEDA/core/array.h>
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

/***********************************************************************
Note: This will use a simple round-robin scheduling technique..
***********************************************************************/    

using leda::list_item;
using leda::dic_item;
using std::ofstream;

void ccmicroblaze_state_definition(ofstream *fout, string classname,
		Operator *op)
{

	// define shared variables for this class..
//	if (op->getOpKind()==OP_BEHAVIORAL) {
//		*fout << "int "<<classname<<"_id=0;" << endl;
//		*fout << "pthread_t "<<classname<<"_rpt;" << endl;
//		*fout << "struct Operator* " << op->getName() << "_ptr;" << endl;
//	} else if(op->getOpKind()==OP_COMPOSE) {
//		*fout << "// skipping id, pthread and local structure of stream pointers.." << endl;
//	} else {
//		cerr << "Unsupported opKind in ccmicroblazebody.h" << endl;
//		exit(1);
//	}
//	*fout << endl;


//	list<Symbol*> *argtypes=op->getArgs();
//	Symbol *sym;
//	forall(sym, *argtypes) {
//		*fout << "ScoreStream* " << classname << "_" << sym->getName() << ";" << endl;
//	}
	*fout << endl;
}

void ccMicroblazeComposeEvalExpr(ofstream *fout, Expr *expr, Symbol *rsym)
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
//	  if (noReturnValue(cop->getRetSym()))
//	    *fout << "new " ;
	  *fout << cop->getName() << "_create";
	}
      *fout << "(-1," ; // -1 for start_state.. April 10th 2010
      Expr *earg;
      int first=1;
      forall(earg,*args)
	{
	  if (!first)
	      *fout << ",";
	  else
	      first=0;
	  ccMicroblazeComposeEvalExpr(fout,earg,rsym);
	}
      *fout << ")";
    }
  // NOTE: these should only be viable if they evaluate to a SCALAR!
  // assuming type checking already caught that...
  else if (expr->getExprKind()==EXPR_COND)
      {
	ExprCond * cexpr=(ExprCond *)expr;
	*fout << "(";
	ccMicroblazeComposeEvalExpr(fout,cexpr->getCond(),rsym);
	*fout << ")?(";
	ccMicroblazeComposeEvalExpr(fout,cexpr->getThenPart(),rsym);
	*fout << "):(";
	ccMicroblazeComposeEvalExpr(fout,cexpr->getElsePart(),rsym);
	*fout << ")";
      }
  else if (expr->getExprKind()==EXPR_BOP)
      {
	ExprBop *bexpr=(ExprBop *)expr;

	// wrong thing for "." operator...
        // TODO: deal properly with fixed point construction/representation
	*fout << "(";
	ccMicroblazeComposeEvalExpr(fout,bexpr->getExpr1(),rsym);
	*fout <<  opToString(bexpr->getOp());
	ccMicroblazeComposeEvalExpr(fout,bexpr->getExpr2(),rsym);
	*fout << ")";
      }
  else if (expr->getExprKind()==EXPR_UOP)
      {
	ExprUop *uexpr=(ExprUop *)expr;
	string ops=opToString(uexpr->getOp());
	Expr *iexpr=uexpr->getExpr();
	*fout << "(" << ops << "(";
	ccMicroblazeComposeEvalExpr(fout,iexpr,rsym);
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
	    ccMicroblazeComposeEvalExpr(fout,real_expr,rsym);
	    *fout << ")";
	  }
	else
	  ccMicroblazeComposeEvalExpr(fout,real_expr,rsym);
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
	  *fout << ccEvalExpr(expr, false);
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
void ccMicroblazeComposePerfTest(ofstream *fout, Expr *expr, Symbol *rsym) 
{

  if ((expr->getExprKind()==EXPR_CALL)) 
  {
      ExprCall *ecall=(ExprCall *)expr;
      Operator *cop=ecall->getOp();
      list<Expr*>* args=ecall->getArgs();
      if(expr->getExprKind()!=EXPR_BUILTIN) 
      {
	      *fout << cop->getName() << "_perftest(";
	      Expr *earg;
	      int first=1;
	      forall(earg,*args)
	      {
		      if (!first)
			      *fout << ", ";
		      else
			      first=0;

		      *fout << "n_" ;		 
		      ccMicroblazeComposeEvalExpr(fout,earg,rsym);
	      }
	      *fout << ");" << endl;
      } else {
          cerr << "Wrong exprtype\n" << endl;
      }
  }
  
}


void ccMicroblazeCompose (ofstream *fout, string classname, OperatorCompose *op)
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
  *fout << endl;


  forall_items(item,*lsyms)
    {
      Symbol *asum=lsyms->inf(item);
      syms->insert(asum);
//      *fout << "    ScoreStream* " << asum->getName()  << ";" << endl;
      
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
		  warn("ccMicroblazeCompose: not expecting/handling partial stream assignment",
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
  // TEMPLATE
  // === ScoreStream *cc_temp_ptr=(ScoreStream*)bufmalloc(pool, sizeof(ScoreStream));
  // === ScoreStream cc_temp_val = new_stream_basic(cc_temp_ptr,16,0,16,SCORE_STREAM_UNSIGNED_TYPE,16,pool);
  // === ScoreStream *cc_temp=&cc_temp_val;	
  Symbol *sym;
  forall(sym,*syms)
    {
      if (sym==returnValue) {
	*fout << "    ScoreStream* result= (ScoreStream*)bufmalloc(pool, sizeof(ScoreStream));"  << endl;
	*fout << "    new_stream(result, 16, 0, 16, SCORE_STREAM_UNSIGNED_TYPE, 16, pool);"  << endl;
      } else {
	*fout << "    ScoreStream* " << sym->getName() << " = (ScoreStream*)bufmalloc(pool, sizeof(ScoreStream));"  << endl;
	*fout << "    new_stream("<<sym->getName()<<", 16, 0, 16, SCORE_STREAM_UNSIGNED_TYPE, 16, pool);"  << endl;
      }

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
			    << i << "]=" << ccEvalExpr(e, false) << ";" << endl;
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

    *fout << endl;
  
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
	      ccMicroblazeComposeEvalExpr(fout,((StmtCall *)statement)->getCall(),
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
		  ccMicroblazeComposeEvalExpr(fout,((StmtAssign *)statement)->getRhs(),
				    returnValue);

		}
	      else
		{
		  *fout << "    " << lsym->getName() << "=" ;
		  ccMicroblazeComposeEvalExpr(fout,((StmtAssign *)statement)->getRhs(),
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

// simple test script
void ccmicroblazeperftest(ofstream *fout, string classname, Operator* op) {
  list<Symbol*> *argtypes=op->getArgs();
  string prefix="n_";
  int i=0;
  
  *fout << "void* " << classname << "_perftest(";
  
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (i>0) *fout << ",";
      *fout << " ScoreStream* " << prefix << sym->getName();
      i++;
    }

  *fout << ") {" << endl;

  *fout << "  Operator* "<<classname<<"_ptr = "<<classname<<"_create(-1,";
  i=0;
  forall(sym,*argtypes)
  {
	  if (i>0) *fout << ",";
	  *fout << prefix << sym->getName();
	  i++;
  }
  *fout<<");"<< endl;

  int num_states=0;
  if (op->getOpKind()==OP_BEHAVIORAL)
    {
      OperatorBehavioral *bop=(OperatorBehavioral *)op;
      dictionary<string,State*>* states=bop->getStates();
      num_states=states->size();
    
      *fout << "  for(int i=0; i<"<<num_states<<"; i++) {" << endl;
      *fout << "    finished=0;" << endl;
      *fout << "    "<<classname<<"_launch_pthread(i,"<<classname<<"_ptr);" << endl;
      *fout << "    while(!finished){};" << endl;
      *fout << "  }" << endl;
      *fout << "}" << endl;
    } 
   else if(op->getOpKind()==OP_COMPOSE) 
   {
				    Symbol *returnValue=op->getRetSym();
	   SymTab *symtab=((OperatorCompose* )op)->getVars(); // get internal streams
	   list<Symbol*>* lsyms=symtab->getSymbolOrder();
	   set<Symbol*>* syms=new set<Symbol*>();
	   list_item item;
	   if (!noReturnValue(returnValue))
		   syms->insert(returnValue);
	   forall_items(item,*lsyms)
	   {
		   Symbol *asum=lsyms->inf(item);
		   syms->insert(asum);
	   }
	   Symbol *sym;
	   forall(sym,*syms)
	   {
		   if (sym==returnValue) {
			   *fout << "    ScoreStream* result= (ScoreStream*)bufmalloc(pool, sizeof(ScoreStream));"  << endl;
			   *fout << "    new_stream(result, 16, 0, 16, SCORE_STREAM_UNSIGNED_TYPE, 16, pool);"  << endl;
		   } else {
			   *fout << "    ScoreStream* n_" << sym->getName() << " = (ScoreStream*)bufmalloc(pool, sizeof(ScoreStream));"  << endl;
			   *fout << "    new_stream(n_"<<sym->getName()<<", 16, 0, 16, SCORE_STREAM_UNSIGNED_TYPE, 16, pool);"  << endl;
			//   *fout << "    #ifdef PERF" << endl;
			//   *fout << "      STREAM_WRITE_UNSIGNED(n_"<<sym->getName()<<",0);" << endl;
			//   *fout << "    #endif" << endl;
		   }
	   }
	   *fout << endl;

	    // Need to run perftest on the leaves.
	    list<Stmt *>* statements=((OperatorCompose* )op)->getStmts();
	    Stmt* statement;
	    forall(statement,*statements)
	    {
		    if (statement->okInComposeOp())
		    {
			    if ((statement->getStmtKind()==STMT_CALL)
					    ||(statement->getStmtKind()==STMT_BUILTIN))
			    {
				    *fout << "    " ;
				    ccMicroblazeComposePerfTest(fout,((StmtCall *)statement)->getCall(),
						    returnValue);
			    }
		    }
	    }
	    *fout << "}" << endl;
    }
}

////////////////////////////////////////////////////////////////////////
// constructor for master operator
void ccmicroblazeconstruct(ofstream *fout,string classname, Operator *op)
{
  Symbol *rsym=op->getRetSym();
  list<Symbol*> *argtypes=op->getArgs();
  string prefix="n_";

  // dump signature and count ins, outs, params
  *fout << "Operator* " << classname << "_create(int n_start_state ";

  int ins=0;
  int outs=0;
  int params=0;
  int i=1;
  if (!noReturnValue(rsym))
    outs++;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (i>0) *fout << ",";

      *fout << " ScoreStream* " << prefix << sym->getName();
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
//  *fout << "  int *params=(int *)malloc(" << params << "*sizeof(int));" << endl;
//  int pi=0;
//  forall(sym,*argtypes)
//    {
//      if (ccParamCausesInstance(sym))
//	{
//	  *fout << "  " << sym->getName() << "=" 
//		<< prefix << sym->getName() << ";" << endl ;
//	  *fout << "  " << "params[" << pi << "]=" 
//		<< prefix << sym->getName() << ";" << endl ;
//	  pi++;
//	}
//    }
//
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
	      string pstr=ccEvalExpr(EvaluateExpr(pexpr),false);
	      *fout << " if (" << pstr << " == 0 ) " << endl;
	      *fout << "    throw (\"runtime type error constructing argument " 
		    << sym->getName() << " to constructor "
		    << classname << "--failed type expression.\n";
	      *fout << "\texpr: " << pstr << "\");" << endl;
	    }
	}
    }


  // create instance name and resolve

  if (!noReturnValue(rsym))
    {
      if (rsym->isStream())
	{
	  *fout << "  "<<classname<<"_result=" << "=&(new_stream(16, 0, 16, SCORE_STREAM_UNSIGNED_TYPE, 16, pool))"; 
	  *fout << ";" << endl;
	}
    }

  int icnt=0;
  int ocnt=0;

  if (op->getOpKind()!=OP_COMPOSE)
    {
      *fout << endl;
      *fout << "  struct Operator* " << classname << "_ptr=(Operator*)bufmalloc(pool,sizeof(Operator));" << endl;
      *fout << "  if(" << classname << "_ptr==NULL) {" << endl;
      *fout << "    xil_printf(\"Could not create operator:" << classname << " error=%d\\n\",errno);" << endl;
      *fout << "  }" << endl;
      *fout << endl;
      *fout << "  new_operator(" << classname << "_ptr" << "," << ins << "," << outs << ", pool, printf_mutex);" << endl;
      *fout << endl;
      *fout << "  " << classname << "_ptr->state_id = n_start_state;" << endl;

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
		  *fout << "  "<< classname <<"_ptr->outputs["<<ocnt<<"]=result);" << endl;
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
		  *fout << "  "<<classname<<"_ptr->outputs[" << ocnt << "]=" << prefix << sym->getName() << ";" << endl;
		  ocnt++;
		}
	      else if (ssym->getDir()==STREAM_IN)
		{
		  *fout << "  "<<classname<<"_ptr->inputs[" << icnt << "]=" << prefix << sym->getName() << ";" << endl;
		  *fout << "#ifdef PERF" << endl;
		  *fout << "  STREAM_WRITE_UNSIGNED(" << prefix << sym->getName() << ",0);" << endl;
		  *fout << "#endif" << endl;
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
	  *fout << endl;
	  *fout << endl;
  	  *fout << " return " << classname << "_ptr;" << endl;
	  *fout << endl;
          *fout << "}" << endl;

	  // launch pthread on xilkernel... this is going to be tricky
	  *fout << "void "<<classname<<"_launch_pthread(int state_id, Operator* "<<classname<<"_ptr) {";
	  *fout << endl;
	  *fout << "  "<<classname<<"_ptr->state_id = state_id;" << endl;
	  *fout << "  // setup pthread for this leaf-level operator" << endl;
	  *fout   << "  pthread_attr_t thread_attribute;\n"
		  << "  pthread_attr_init(&thread_attribute);\n"
		  << "  pthread_attr_setdetachstate(&thread_attribute,PTHREAD_CREATE_DETACHED);\n"
		  << "  pthread_create(&"<<classname<<"_ptr->thread,&thread_attribute,&"
		  << op->getName() << "_proc_run, "<<classname<<"_ptr);"
		  << endl;
//          *fout << "}" << endl;
  }
  else if (op->getOpKind()==OP_COMPOSE)
  {
	  // TODO: 1/11/2010.. I need to think about this part..
	  // bind up / rename inputs
	  forall(sym,*argtypes)
	  {
		  if (sym->isStream() || sym->isArray())
		  {
			  *fout << "    ScoreStream* "
				  << sym->getName() << "=" << prefix << sym->getName()
				  << ";" << endl;
		  }
		  else
		  {
			  // already complained
		  }
	  }

	  ccMicroblazeCompose(fout,classname,(OperatorCompose *)op);
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
// collect up retiming exprs per symbol
bool ccmicroblaze_body_treemap_false(Tree *t, void *aux)
{ return (0); }

bool microblaze_collect_retime_exprs(Tree *t, void *aux)
{
  if (t->getKind()!=TREE_EXPR)
    return(1);
  Expr *expr = (Expr *)t;
  if (expr->getExprKind()!=EXPR_LVALUE)
    return(1);
  ExprLValue *lval=(ExprLValue *)expr;
  Expr *rexpr=lval->getRetime();
  Symbol *sym=lval->getSymbol();
  set<Expr *> *rset=(set<Expr *> *)sym->getAnnote(MAX_RETIME_DEPTH);
  if (rexpr!=(Expr *)NULL)
    {
      int value_zero=0;
      if (rexpr->getExprKind()==EXPR_VALUE)
	{
	  ExprValue *val=(ExprValue *)rexpr;
	  if ((val->getIntVal())==0)
	    value_zero=1;
	}
      if (!value_zero) {
	if (rset!=(set<Expr *> *)NULL) 
	  {
	    rset->insert(rexpr);
	  }
	else
	  {
	    rset=new set<Expr *>();
	    rset->insert(rexpr);
	    sym->setAnnote(MAX_RETIME_DEPTH,(void *)rset);
	  }
      }
    }

  return(0);
}

//
////////////////////////////////////////////////////////////////////////
// procrun for master instance

void ccmicroblazeprocrun(ofstream *fout, string classname, Operator *op)
{
  *fout << "void *" << classname << "_proc_run(void* dummy) {"  << endl;

  if (op->getOpKind()==OP_COMPOSE)
    {
      *fout << "  xil_printf(\"proc_run should never be called for a compose operator!\\n\");" << endl;
    }
  else if (op->getOpKind()==OP_BEHAVIORAL)
    {
      OperatorBehavioral *bop=(OperatorBehavioral *)op;
      dictionary<string,State*>* states=bop->getStates();
      dic_item item;

      *fout << endl;
      *fout << "  struct Operator* " << classname << "_ptr = (struct Operator*)dummy;"<< endl;
      *fout << endl;

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

      // Added April 10th 2010: specify starting state.. according to initialized state if
      *fout << endl;
      *fout << "  switch("<<classname<<"_ptr->state_id) {" << endl;
      int index=0;
      forall_items(item, *states) {
        *fout << "    case "<<index<<": state = " << STATE_PREFIX << states->inf(item)->getName() << "; break;" << endl;
	index++;
      }
        *fout << "    default : state = " << STATE_PREFIX << start_state->getName() << "; break;" << endl;
      *fout << "  }" << endl;

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
	      *fout << "=" << ccEvalExpr(EvaluateExpr(val), false) ;
	  *fout << ";" << endl;
	}

      // declare/initialize retiming slots for input streams
      // (1) map over and create annotations with all retiming exprs
      op->map(microblaze_collect_retime_exprs,ccmicroblaze_body_treemap_false,(void *)NULL);
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
			    << classname<<"_ptr->inputs[" 
			    << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			    << "]);" << endl;

		      // EOF support added by Nachiket for C++ code generation on October 5th 2009
		      *fout << "        int eofr_"
			    << loc
			    << "=STREAM_EOFR("
			    << classname<<"_ptr->inputs["
			    << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID))
			    << "]);" << endl;

		    }
		  
		}


		// April 10th 2010: Added support for timers
	      *fout << endl;
	      *fout << "#ifdef PERF" << endl;
	      *fout << "        start_timer();" << endl;
	      *fout << "#endif" << endl;
	      
	      *fout << endl;
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
			bool unsignedtyp=(intyp==TYPE_INT);

			// cout << "Type=" << typekindToString(intyp) << " for stream " << ispec->getStream()->getName() << endl;

		      *fout << "          " 
			    << ispec->getStream()->getName() 
			    << (floattyp? "=STREAM_READ_FLOAT(" : (doubletyp)? "=STREAM_READ_DOUBLE(" : (unsignedtyp)? "=STREAM_READ_UNSIGNED(" : "=STREAM_READ_UNSIGNED(") 
			    << classname<<"_ptr->inputs[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			    << "]);" << endl;

		    }
		  else if(ispec->isEosCase())
		    {
		    *fout << "          "
			  << "if (!"<<classname<<"_ptr->input_free[" 
			  << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			  << "])" << endl;
		      *fout << "          " 
			    << "STREAM_FREE(" 
			    << classname << "_ptr->inputs[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			    << "]);" << endl;
		       early_free[(long)(ispec->getStream()->getAnnote(CC_STREAM_ID))]=1;
		       *fout << "          " 
			     << classname << "_ptr->input_free[" 
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
					bool unsignedtyp=(intyp==TYPE_INT);

					// dummy read of the token..
				    *fout << "          "
			    		    << (floattyp? "STREAM_READ_FLOAT(" : (doubletyp)? "STREAM_READ_DOUBLE(" : (unsignedtyp)? "STREAM_READ_UNSIGNED(" : "STREAM_READ_UNSIGNED(") 
					    << classname << "_ptr->inputs[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID))
					    << "]);" << endl;

			  }
		}

	      Stmt* stmt;
	      forall(stmt,*(acase->getStmts()))
		{
		  ccStmt(fout,string("          "),stmt,early_close,
			 STATE_PREFIX,0, false, true, classname); // 0 was default for ccStmt.h
			 // added false to remove retiming
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
	  *fout << "      ";
	  for (; numNestings>0; numNestings--) {
	    *fout << "}" << endl;
	  }

	  // April 10th 2010: Added support for timers
	  *fout << endl;
	  *fout << "#ifdef PERF" << endl;
	  *fout << "      int timer = stop_timer();" << endl;
//	  *fout << "      free_operator(" << classname << "_ptr, pool, printf_mutex);" << endl;
	  *fout << "      pthread_mutex_lock(&printf_mutex);" << endl;
	  *fout << "      xil_printf(\"operator="<<classname<<" state="<<sname<<" cycles=\%d\\n\",timer);" << endl;
	  *fout << "      pthread_mutex_unlock(&printf_mutex);" << endl;
	  *fout << "      finished=1;" << endl;
	  *fout << "      pthread_exit((void*)NULL);" << endl;
//	  *fout << "      return((void*)NULL);" << endl;
	  *fout << "#endif" << endl;

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
	  *fout << "      default: xil_printf(\"ERROR unknown state encountered in " << classname << "_proc_run\\n\");" << endl;
	  *fout << "        return((void*)NULL);" << endl;
	  *fout << "    }" << endl;
	}

      *fout << "  }" << endl;
      // any final stuff
      if (!noReturnValue(rsym)) {
	if (early_close[(long)(rsym->getAnnote(CC_STREAM_ID))])
	  {
	    *fout <<"  if (!"<<classname<<"_ptr->output_close[" 
		  <<  (long)(rsym->getAnnote(CC_STREAM_ID))
		  << "])" << endl;
	    *fout << "  STREAM_CLOSE(" 
		  << classname << "_ptr->outputs[" << (long)(rsym->getAnnote(CC_STREAM_ID))
		  << "]);" << endl;
	  }
	else {
	  *fout << "  STREAM_CLOSE(" 
		<< classname << "_ptr->outputs[" << (long)(rsym->getAnnote(CC_STREAM_ID))
		<< "]);" << endl;
        }
      }

      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
		  if (early_close[(long)(ssym->getAnnote(CC_STREAM_ID))])
		    {
		      *fout <<"  if (!"<<classname<<"_ptr->output_close[" 
			    <<  (long)(ssym->getAnnote(CC_STREAM_ID))
			    << "])" << endl;
		      *fout << "  STREAM_CLOSE(" 
			    << classname << "_ptr->outputs[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			    << "]);" << endl;
		    }
		  else
		    *fout << "  STREAM_CLOSE(" 
			  << classname << "_ptr->outputs[" << (long)(ssym->getAnnote(CC_STREAM_ID)) 
			  << "]);" << endl;
		}
	      else
		if (early_free[(long)(ssym->getAnnote(CC_STREAM_ID))])
		  {
		    *fout <<"  if (!"<<classname<<"_ptr->input_free[" 
			  << (long)(ssym->getAnnote(CC_STREAM_ID)) 
			  << "])" << endl;
		    *fout << "    STREAM_FREE(" 
			  << classname << "_ptr->inputs[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			  << "]);" << endl;
		  }
		else
		  *fout << "  STREAM_FREE(" 
			<< classname << "_ptr->inputs[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			<< "]);" << endl;
	    }
	}
    }
  else
    {
      fatal(-1,string("Don't know how to handle opKind=%d",
		      (int)op->getOpKind()),
		      op->getToken());
	    
    }

  *fout << "  return((void*)NULL);" << endl;
  *fout << "}" << endl;
  
}

//
////////////////////////////////////////////////////////////////////////
// Top level routine to create master C++ code
void ccmicroblazebody (Operator *op)
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
  *fout << "// tdfc-microblaze backend autocompiled body file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;

  // some includes
  *fout << "#include \"xilscore.h\"" << endl;
  *fout << "#include <stdio.h>" << endl;
  *fout << "#include \"" << name << ".h\"" << endl;
  *fout << "#include \"xparameters.h\"" << endl;
  *fout << "#include \"shared_pool.h\"" << endl;
  *fout << "#include \"perfctr.h\"" << endl; // Added April 10th 2010 to suppoert perf-measurement of microblaze code
  *fout << "#include \"math.h\"" << endl; // Added April 10th 2010 to suppoert perf-measurement of microblaze code
  *fout << "#include \"errno.h\"" << endl; // Added April 11th 2010
  *fout << endl;

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
  *fout << endl;

  // define the operator-local variables
  ccmicroblaze_state_definition(fout,classname,op);

  // constructor
  ccmicroblazeconstruct(fout,classname,op);

  *fout << endl;

  // perf test
  ccmicroblazeperftest(fout, classname, op);

  *fout << endl;

  // proc_run
  ccmicroblazeprocrun(fout,classname,op);

  *fout << endl;

  // if necessary, functional version
  if (!noReturnValue(rsym))
    microblaze_functional_constructor(fout,name,classname,rsym,argtypes);

  // close up
  fout->close();

}




