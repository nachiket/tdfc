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
// SCORE TDF compiler:  generate C++ output for instances
// $Revision: 1.147 $
//
//////////////////////////////////////////////////////////////////////////////

#include "compares.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include <LEDA/core/array.h>
#include "version.h"
#include "misc.h"
#include "operator.h"
#include "expr.h"
#include "stmt.h"
#include "state.h"
#include "feedback.h"
#include "instance.h"
#include "bindvalues.h"
#include "cctype.h"
#include "cceval.h"
#include "ccstmt.h"
#include "annotes.h"
#include "ccprep.h"
#include "ccheader.h"
#include "cccase.h"
#include "ccmem.h"
#include "ccinstance.h"
#include "cccopy.h"

using leda::array;
using leda::list_item;
using leda::dic_item;

/***********************************************************************
Note:
  According to our discussion of 7/20/99, this should 
  * not handle direct memory references
  * not handle inlined calls/inline memory blocks
  * not handle bit-level composition operators
  * not handle nested calls
  * not have to deal with scoping heirarchy for operators
  * all parameters values should be bound
  Does have to handle the following builtins:
  + compose:
    - memory builtins (still not supported in frontend, so still TODO)
    - bitsof
    - widthof
    - cat (for exprs only)
    - copy   (9/99)
    = terminate (9/6/99)
  + behavioral:
    - done
    - cat 
    - bitsof
    - widthof
    - close
***********************************************************************/    

// REQUIREMENT: this needs to be consistent w/ ScoreOperator::mangle
char *mangle(const char *base, int nparam, int *params)
{
  char *res;
  if (nparam>0)
    {
      res=(char *)malloc(strlen(base)+1+9*nparam);
      char *tptr;
      memcpy(res,base,strlen(base));
      tptr=res+strlen(base);
      for (int i=0;i<nparam;i++)
	{
	  sprintf(tptr,"_%x",params[i]);
	  tptr+=strlen(tptr);
	}
    }
  else
    {
      res=(char *)malloc(strlen(base)+2+strlen(SINGLE_INSTANCE_EXTENSION));
      sprintf(res,"%s_%s",base,SINGLE_INSTANCE_EXTENSION);
    }
  return(res);
}

// blah, could unify
//  note: at least used by the maps in this file...
bool ccinstance_treemap_false(Tree *t, void *aux)
{ return (0); }

//
////////////////////////////////////////////////////////////////////////
// produce and consume computation

ARGMASK_TYPE allConsumers(State *astate)
{
  ARGMASK_TYPE res=0;
  set<StateCase*> *cases=astate->getCases();
  StateCase *acase;
  forall(acase,*cases)
    {
      list<InputSpec *>*ispec=acase->getInputs();
      InputSpec *in;
      forall(in,*ispec)
	{
	  Symbol *sym=in->getStream();
	  long loc=(long)sym->getAnnote(CC_STREAM_ID);
	  res=res|(((ARGMASK_TYPE)1)<<loc);
	}
    }
  return(res);
}

bool mark_set_syms(Tree *t, void *aux)
{ 
  if (t->getKind()==TREE_STATECASE)
    {
      ARGMASK_TYPE res=0;
      Stmt *s;
      forall (s,*((StateCase*)t)->getStmts())
	s->map(mark_set_syms,NULL,&res);
      t->setAnnote_(CC_ALLPRODUCERS, new ARGMASK_TYPE(res));	// use later in
								// CC emission
      ARGMASK_TYPE *locs=(ARGMASK_TYPE*)aux;
      *locs=*locs|res;
      return false;
    }
  else if (t->getKind()==TREE_STMT)
    {
      Stmt *stmt=(Stmt *)t;
      if (stmt->getStmtKind()==STMT_ASSIGN)
	{
	  StmtAssign *assign=(StmtAssign *)stmt;
	  ExprLValue *lval=assign->getLValue();
	  Symbol *sym=lval->getSymbol();
	  if (sym->isStream())
	    {
	      long loc=(long)sym->getAnnote(CC_STREAM_ID);
	      ARGMASK_TYPE *locs=(ARGMASK_TYPE*)aux;
	      *locs=*locs|(((ARGMASK_TYPE)1)<<loc);
	    }
	}
      return(1); // could be side effect in call expr...
    }
  else if (t->getKind()==TREE_EXPR)
    {
      Expr *expr=(Expr *)t;
      if ((expr->getExprKind()==EXPR_CALL)
	  || ((expr->getExprKind()==EXPR_BUILTIN) &&
	      ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
	       ==BUILTIN_SEGMENT)))
	// need to check not assigned through the call
	{
	  ExprCall *ecall=(ExprCall *)expr;
	  list <Expr*> *actuals=ecall->getArgs();
	  list <Symbol *> *formals=(ecall->getOp())->getArgs();
	  Symbol *sym;
	  list_item act=actuals->first();
	  forall (sym,*formals)
	    {
	      if (actuals !=(list<Expr *> *)NULL)
		{
		  Expr *actual=actuals->inf(act); // blah is this the right thing?
		  act=actuals->succ(act);    //  is it efficient?
		  if (sym->isStream())
		    {
		      SymbolStream *sstream=(SymbolStream *)sym;
		      if (sstream->getDir()!=STREAM_IN)
			if (actual->getExprKind()==EXPR_LVALUE)
			  {
			    Symbol *asym=((ExprLValue *)actual)->getSymbol();
			    int loc=(long)asym->getAnnote(CC_STREAM_ID);
			    ARGMASK_TYPE *locs=(ARGMASK_TYPE*)aux;
			    *locs=*locs|(((ARGMASK_TYPE)1)<<loc);
			  }
		    }
		}
	    }
	}
      return(1);
    }
  else if ((t->getKind()==TREE_SYMBOL)
	   || (t->getKind()==TREE_SYMTAB)
	    )
    {
      return(0);
    }
  else
    return(1);

}

ARGMASK_TYPE allProducers(State *astate)
{
  ARGMASK_TYPE res;
  res=0;
  astate->map(mark_set_syms,ccinstance_treemap_false,&res);
  return(res);
}


//
////////////////////////////////////////////////////////////////////////
// collect up retiming exprs per symbol

bool instance_collect_retime_exprs(Tree *t, void *aux)
{
  if (t->getKind()!=TREE_EXPR)
    return(1);
  Expr *expr = (Expr *)t;
  if (expr->getExprKind()!=EXPR_LVALUE)
    return(1);
  ExprLValue *lval=(ExprLValue *)expr;
  Expr *rexpr=EvaluateExpr(lval->getRetime());
  Symbol *sym=lval->getSymbol();
  Expr * oldexpr=(Expr *)sym->getAnnote(MAX_RETIME_DEPTH);
  if (rexpr!=(Expr *)NULL)
    {
      if (rexpr->getExprKind()==EXPR_VALUE)
	{
	  ExprValue *val=(ExprValue *)rexpr;

	if (oldexpr!=(Expr *)NULL)
	  {
	    int oval=((ExprValue *)oldexpr)->getIntVal();
	    int nval=val->getIntVal();
	    if (nval>oval)
	      sym->setAnnote(MAX_RETIME_DEPTH,(void *)val);
	  }
	else
	  {
	    sym->setAnnote(MAX_RETIME_DEPTH,(void *)val);
	  }
	}
      else
	{
	  error(-1,string("Retiming value [")+rexpr->toString()+
		string("] in lvalue [") + expr->toString() +
		string("] does not resolve to constant after parameter binding"),
		expr->getToken());
	}


    }
  return(0);
}

//
////////////////////////////////////////////////////////////////////////
// for ScoreOperatorInstance assignments

void instance_assign_expr(ofstream *fout, Expr *orig_expr, string aname,
			  int *pid, int *sid)
{

  Expr *expr=EvaluateExpr(orig_expr);
  if (expr->getExprKind()==EXPR_VALUE)
    *fout << expr->toString(); 
  else if (expr->getExprKind()==EXPR_LVALUE)
    {
	assert(aname);
	ExprLValue *lexpr=(ExprLValue *)expr;
	if (lexpr->usesAllBits())
	  *fout << "    " << aname << "=" 
		<< lexpr->getSymbol()->getName();
	else
	  {
	    warn("not handling bit selects in compose exprs...should be decomposed earlier",
		 lexpr->getToken());
	  }
    }
  else if ((expr->getExprKind()==EXPR_CALL)
	   || ((expr->getExprKind()==EXPR_BUILTIN) &&
	       ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		==BUILTIN_SEGMENT)))
    {
      ExprCall *ecall=(ExprCall *)expr;
      Operator *cop=ecall->getOp();
      list<Expr*>* args=ecall->getArgs();
      if (expr->getExprKind()==EXPR_CALL)
	*fout << "    ScorePage * " << cop->getName() 
	      << "_" << *pid << "_inst=new" 
	      << " Page_" << cop->getName() << "(";
      else
	if (expr->getExprKind()==EXPR_BUILTIN)
	  if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		==BUILTIN_SEGMENT)
	    {
	      *fout << "    ScoreSegment * " 
		    << cop->getName() << "_" << *sid 
		    << "_inst=new " 
		    << ccMemoryName(cop)
		    << "(";
	    }
	  else if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		   ==BUILTIN_TERMINATE)
	    {
	      *fout << "    STREAM_FREE(" ;
	    }
	  else if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		   ==BUILTIN_COPY)
	    {
	      OperatorBuiltin *cop=((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp());
	      *fout << "    ScorePage * " 
		    << ccCopyPageName((OperatorBuiltin *)cop) 
		    << "_" << *pid
		    << "_inst=new" 
		    << " Page_" << ccCopyPageName(cop) << "(";
	    }


      int first=1;
      if (!noReturnValue(cop->getRetSym()))
	{
	  assert(aname);
	  *fout << aname;
	  first=0;
	}
      Expr *earg;
      int plocs=ccParameterLocs(cop->getArgs());
      forall(earg,*args)
	{
	  if ((plocs & 0x01)==0) // not a parameter slot
	    {
	      if (!first)
		*fout << ",";
	      else
		first=0;
	      if (earg->getExprKind()!=EXPR_LVALUE)
		{
		  fatal(-1,"expecting only lvalues after flattenning",
			earg->getToken());
		}
	      else
		*fout << ((ExprLValue *)earg)->getSymbol()->getName();
	    }
	  else
	    {
	      if (expr->getExprKind()==EXPR_BUILTIN)
		{
		  if (!first)
		    *fout << ",";
		  else
		    first=0;
		  if (earg->getExprKind()==EXPR_LVALUE)
		    *fout << ((ExprLValue *)earg)->getSymbol()->getName();
		  else if (earg->getExprKind()==EXPR_VALUE)
		    {
		      *fout << earg->toString(); 
		    }
		  else
		    {
		      fatal(-1,"expecting only values or lvalues in segment args after flattenning",
			    earg->getToken());
		    }
		}
	      else
		{
		  // is a parameter, skip it in this context
		}
	    }
	  plocs=plocs>>1; // shift down for next parameter
	}
      if (cop->getToken()!=(Token *)NULL)
	*fout << "); /* " << cop->getToken()->str << " */";
      else
	if (ecall->getToken()!=(Token *)NULL)
	  *fout << "); /* " << ecall->getToken()->str << " */";
	else
	  *fout << ");" ;

      *fout << endl;

      // now for page/group 
      if (expr->getExprKind()==EXPR_CALL)
	{
	  *fout << "    page[" << *pid << "]=" << cop->getName() 
		<< "_" << *pid << "_inst;" << endl;
	  *fout << "    page_group[" << *pid << "]="
		<< "NO_GROUP;"  << endl;
	  (*pid)++;
	}
      else
	if (expr->getExprKind()==EXPR_BUILTIN)
	  if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		==BUILTIN_SEGMENT)
	    {
	      *fout << "    segment[" << *sid << "]="
		    << cop->getName() << "_" << *sid 
		    << "_inst;" << endl;
	      *fout << "    segment_group[" << *sid << "]="
		    << "NO_GROUP;" << endl;
	      (*sid)++;
	    }
	  else if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		   ==BUILTIN_TERMINATE)
	    {
	      // nothing
	    }
	  else if ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->getBuiltinKind())
		   ==BUILTIN_COPY)
	    {
	      OperatorBuiltin *cop=((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp());
	      *fout << "    page["  << *pid << "]="
		    << ccCopyPageName((OperatorBuiltin *)cop) 
		    << "_" << *pid
		    << "_inst;"  << endl;
	      *fout << "    page_group[" << *pid << "]="
		    << "NO_GROUP;"  << endl;
	      (*pid)++;
	    }
    }
  // note all the constants should be folded by now, so 
  //   shouldn't have to do math on constants here...
  else if (expr->getExprKind()==EXPR_BUILTIN)
    {
      warn("not handling compose builtin (maybe should happen earlier?)",
	   expr->getToken());
    }
  else
    {
      fatal(-1,string("unknown 'acceptable' expression type [%d] in operator instance compose ",
		      (int)expr->getExprKind()),
	    expr->getToken());
    }
  
}

//
////////////////////////////////////////////////////////////////////////
// for behavioral operators

void compose_instance_wrapper(ofstream *fout, Operator *op)
{
  // should just be a call
  *fout << "    ScorePage *" << op->getName() << "_inst" 
	<< "=new Page_" << op->getName() << "(" ;
  Symbol *returnValue=op->getRetSym();
  list<Symbol*> *args=op->getArgs();
  int i=0;
  if (!noReturnValue(returnValue))
    {
      *fout << returnValue->getName();
      i++;
    }
  Symbol *sym;
  forall(sym,*args)
    {
      if (!ccParamCausesInstance(sym))
	{
	  if (i>0) 
	    *fout << ",";
	  *fout << sym->getName(); 
	  i++;
	}
    }
  *fout << ");" << endl;

}

//
////////////////////////////////////////////////////////////////////////
// generate instance body ... 

void compose_instance_body(ofstream *fout, OperatorCompose *op)
{
  // walk over symbols
  //  a. declare
  //  b. add to set

  // Symbol *returnValue=op->getRetSym(); ... unused 9/18/99... probably orphaned
  SymTab *symtab=op->getVars();
  list<Symbol*>* lsyms=symtab->getSymbolOrder();
  set<Symbol*>* syms=new set<Symbol*>();
  set<Symbol*>* partial=new set<Symbol*>();
  list_item item;

  *fout << "    // local declarations " << endl;
  forall_items(item,*lsyms)
    {
      Symbol *asum=lsyms->inf(item);
      syms->insert(asum);
      *fout << "    " << getCCtype(asum,1) << " " << asum->getName()
	    << "; // "
	    << (asum->getToken() ? asum->getToken()->str
				 : (string("internal ")+asum->toString()))
	    << endl;
      
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
		{
		  if ((((StmtAssign *)statement)->getRhs())->getExprKind()!=
		      EXPR_CALL)
		    syms->del(lsym);
		  // Note: if it is an expr call, then this is the return value
		  //   in this instance, we're going to have to collapse
		  //   return values into arguments for the page steps, so
		  //   we need to have the stream created before we 
		  //   pass it to the page step from which it is a return
		  //   value.
		}
	      else
		{
		  partial->insert(lsym);
		  warn("ccCompose: not expecting/handling partial stream assignment",
		       lsym->getToken());
		}

	    }
	}
      else
	{
	  cerr << "non compose op " << statement->toString() << endl;
	  // complaint should have been issued earlier
	}
    }

  *fout << "    // create local streams and segments " << endl;


  // walk over set
  //   creating streams AND segments
  Symbol *sym;
  forall(sym,*syms)
    {
      //if (sym==returnValue)
      //	*fout << "    result=" 
      //	      << getCCtypeConstructor(sym,1,1) << ";" << endl;
      //else
      *fout << "    " << sym->getName() << "=" 
	    << getCCtypeConstructor(sym,1,1) << ";" << endl;

      // SEGMENT initial values 
      if (sym->isArray())
	{
	  // write initial values into memory
	  // NOTE: could change this to some kind of DMA...
	  //   if/when we have such instructions
	  SymbolVar *svar=(SymbolVar *)sym;
	  Expr *aval=svar->getValue();
	  if (aval)
	    {
	      if (aval->getExprKind()==EXPR_ARRAY)
		{
		  Type  *aeType   =((TypeArray*)sym->getType())->getElemType();
		  string aeTypeStr=getCCvarType(aeType);
		  ExprArray *earr=(ExprArray *)aval;
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

  
  *fout << "    // compose body statements " << endl;

  int pid=0;
  int sid=0;

  // walk over stms
  //   creating instances
  forall(statement,*statements)
    {
      if (statement->okInComposeOp())
	{
	  if (statement->getStmtKind()==STMT_CALL)
	    {
	      Symbol *crsym=(((StmtCall *)statement)->getCall())->getOp()->getRetSym();
	      // TODO: need to be dealing with unused outputs
	      //  either I need to do a free on this streams output value,
	      //  *or* be passing null in...or maybe
	      //  earlier this should already have been dealt with...

	      // create dummy return stream when necessary
	      if (noReturnValue(crsym))
		instance_assign_expr(fout,((StmtCall *)statement)->getCall(),
				     string("bogus name"),&pid,&sid);
	      else
		{
		  string rname=unique_name();
		  // odd case where we are discarding the return value stream
		  *fout << "    " << getCCtype(crsym) 
			<< " " << rname
			<< "=" << getCCtypeConstructor(crsym,1,1) 
			<< "; // n.b. no consumer for this stream" << endl;
		  instance_assign_expr(fout,
				       ((StmtCall *)statement)->getCall(),
				       rname,&pid,&sid);
		}
	    }
	  else if (statement->getStmtKind()==STMT_ASSIGN)

	    {
	      ExprLValue *lval=((StmtAssign *)statement)->getLValue();
	      Symbol *lsym=lval->getSymbol();

	      // check if lsym properly in current dictionary
	      Symbol *lusym=symtab->lookup(lsym->getName());
	      if (lusym==(Symbol *)NULL)
		cerr << " PROBLEM " << lsym->getName() << " not found in " 
		     << op->getName() << " parent is " 
		     << lsym->getOperator()->getName()
		     << endl;
	      else if (lusym!=lsym)
		{
		  cerr << " PROBLEM " << lsym->getName() 
		       << " found differnt symbol in " 
		       <<	  op->getName() << endl;
		  cerr << "   lsym parent " << lsym->getOperator()->getName()
		       << "; sym parent " << lusym->getOperator()->getName()
		       << endl;
		  Symbol *rsym=op->getRetSym();
		  if (!noReturnValue(rsym))
		    cerr << "   rsym is " << rsym->getName() << " parent is "
			 << rsym->getOperator()->getName() << endl; 
		}

	      instance_assign_expr(fout,
				   ((StmtAssign *)statement)->getRhs(),
				   lsym->getName(),&pid,&sid);

	      
	    }
	  else if (statement->getStmtKind()==STMT_BUILTIN)
	    {
	      instance_assign_expr(fout,
				   ((StmtBuiltin *)statement)->getBuiltin(),
				   string("FUBAR"),&pid,&sid);

	    }
	  else
	    {
	      fatal(-1,"unknown 'acceptable' statement type in compose",
		    statement->getToken());
	    }
	}
      else
	{
	  //noop, already complained
	}
    }
  *fout << "    // end body statements " << endl;

}

//
////////////////////////////////////////////////////////////////////////
// generate page contents  (very long)

void ccpage(ofstream *fout, Operator *op, int debug_logic)
{
  string prefix="n_";

  // maybe should guard must be Operator Behavioral
  OperatorBehavioral *bop=(OperatorBehavioral *)op;
  dictionary<string,State*>* states=bop->getStates();
  dic_item item;
  // define states in use
  string state_prefix=op->getName()+string("_state_");
  *fout << "enum " + op->getName() + "_state_syms {" ;
  State* start_state=bop->getStartState(); 
  int first=1;
  int num_states=states->size();
  // ARGMASK_TYPE *produces=new ARGMASK_TYPE[num_states];
  // ARGMASK_TYPE *consumes=new ARGMASK_TYPE[num_states];
  ARGMASK_TYPE produces[num_states], consumes[num_states];
  int scnt=0;
  forall_items(item,*states)
    {
      if (!first)
	*fout << "," ;
      *fout << state_prefix << states->inf(item)->getName();
      if (first) {
	*fout << "=0" ;
	first=0;
      }
      /* moved to the just before pagestep() code is output.
	 -- mmchu, 11/9/99.
      //compute produces and consumes here while passing over
      produces[scnt]=allProducers(states->inf(item));
      consumes[scnt]=allConsumers(states->inf(item));
      scnt++;
      */
    }
  // output the "fictional" done state.
  if (!first)
    *fout << ",";
  else
    first=0;
  *fout << state_prefix << "this_page_is_done";
  *fout << "};" << endl;

  string state_name_array (op->getName()+"_state_names");
  if (gProfileStates) {
    *fout << "static char* " << state_name_array
	  << "[" << num_states << "] = {" ;
    first=1;
    forall_items(item,*states) {
      if (first)
	first=0;
      else
	*fout << ",";
      *fout << "\"" << states->inf(item)->getName() << "\"";
    }
    *fout << "};" << endl;
  }

  *fout << "extern " ARGMASK_TYPE_STR " " << op->getName() << "_consumes[];"
	<< endl;
  *fout << "extern " ARGMASK_TYPE_STR " " << op->getName() << "_produces[];"
	<< endl;

  /* moved to after the class declaration. -- mmchu, 11/9/99.
  *fout << ARGMASK_TYPE_STR " " << op->getName() << "_consumes[]={";
  first=1;
  for(int i=0;i<num_states;i++)
    {
      if (!first)
	*fout << "," ;
      else
	first=0;
      *fout << consumes[i];
    }
  *fout << "};" << endl;
  *fout << ARGMASK_TYPE_STR " " << op->getName() << "_produces[]={";
  first=1;
  for(int i=0;i<num_states;i++)
    {
      if (!first)
	*fout << "," ;
      else
	first=0;
      *fout << produces[i];
    }
  *fout << "};" << endl;
  */

  // collect up retiming depth information
  //   bindvalues should have occurred, so this time the collected
  //   values should all be constants... (unlike ccbody)
  op->map(instance_collect_retime_exprs,ccinstance_treemap_false,(void *)NULL);

  //
  ////////////////////////////////////////////////////////////////////////
  // constructor (inside ccpage)

  *fout << "class Page_" << op->getName() << ": public ScorePage {" << endl;
  *fout << "public:" << endl;
  if (gProfileStates) {
    *fout << "  long long state_freqs["      << num_states << "];" << endl;
    *fout << "  long long state_xfer_freqs[" << num_states << "]["
					     << num_states << "];" << endl;
  }
  *fout << "  Page_" << op->getName() << "(" ;
  //  arguments
  int i=0;
  Symbol *rsym=op->getRetSym(); 
  if (!noReturnValue(rsym))
    {
      *fout << getCCtype(rsym) << " " << prefix << "result";
      i++;
    }
  Symbol *sym;
  list<Symbol*> *argtypes=op->getArgs(); 
  forall(sym,*argtypes)
    {
      if (!ccParamCausesInstance(sym))
	{
	  if (i>0) *fout << ",";
	  *fout << getCCtype(sym) << " " << prefix << sym->getName() ;
	  i++;
	}
    }
  *fout << ") {" << endl;
  // linkage
  //   first count ins and outs, annote/number streams
  int ins=0;
  int outs=0;
  if (!noReturnValue(rsym))
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
		  rsym->setAnnote(CC_STREAM_ID,(void *)outs);
		  outs++;
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
	      sym->setAnnote(CC_STREAM_ID,(void *)outs);
	      outs++;
	    }
	  else if (ssym->getDir()==STREAM_IN)
	    {
	      sym->setAnnote(CC_STREAM_ID,(void *)ins);
	      ins++;

	      // allocate retiming values and initialize
	      Expr * rexpr=(Expr *)sym->getAnnote(MAX_RETIME_DEPTH);
	      int rlen;
	      if (rexpr!=(Expr *)NULL)
		  rlen=((ExprValue *)rexpr)->getIntVal();
	      else 
		rlen=0;
	      *fout << "    retime_length_"
		    << (long)sym->getAnnote(CC_STREAM_ID)
		    << "=" << rlen
		    << ";" << endl;
	      *fout << "    " 
		    << sym->getName()
		    << "_retime="
		    << "new "
		    << getCCvarType(sym) << " " 
		    << "[retime_length_"
		    << (long)sym->getAnnote(CC_STREAM_ID)
		    << "+1]" // 0->max retiming depth seen
		    << ";" << endl;
	      *fout << "    " 
		    << "for (int j=retime_length_"
		    << (long)sym->getAnnote(CC_STREAM_ID)
		    << ";j>=0;j--)" << endl;
	      *fout << "      " 
		    << sym->getName() 
		// TODO: assign initial values instead of 0
		    << "_retime[j]=0;" << endl;

	    }
	      
	  else
	    {
	      error(-1,"Don't believe in streams w/ ANY direction",
		    sym->getToken());
	    }
	}
    }

  *fout << "    declareIO(" << ins << "," << outs << ");" << endl;
  // now declare each
  int icnt=0;
  int ocnt=0;
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
	      *fout << "    bindOutput(" << ocnt<< "," << prefix << "result," 
		    << getCCStreamType(rsym) << ");" << endl;
	      ocnt++;
	    }
	}
      else
	{
	  // presumably we've already issued this complaint much earlier
	}
    }
  forall(sym,*argtypes)
    {
      if (sym->isStream())
	{
	  SymbolStream *ssym=(SymbolStream *)sym;
	  if (ssym->getDir()==STREAM_OUT)
	    {
	      *fout << "    bindOutput(" << ocnt << "," 
		    << prefix << sym->getName() << ","
		    << getCCStreamType(sym) << ");" << endl;
	      ocnt++;
	    }
	  else if (ssym->getDir()==STREAM_IN)
	    {
	      *fout << "    bindInput(" << icnt << ","
		    << prefix << sym->getName() << ","
		    << getCCStreamType(sym) << ");" << endl;
	      icnt++;
	    }
	  
	  else
	    {
	      // already complained
	    }
	}
    }

  // initial state
  if (num_states > 0) {
    *fout << "    state=" << state_prefix
  	  << start_state->getName() << ";" << endl; 
  } else {
    *fout << "    state=" << state_prefix
          << "this_page_is_done;" << endl;
  }

  // we output that we have 1 more state than we think we do to include
  // the "fictional" done state.
  *fout << "    states="   << (num_states+1)    << ";"          << endl;
  //*fout << "    states="   << num_states    << ";"          << endl;
  *fout << "    produces=" << op->getName() << "_produces;" << endl;
  *fout << "    consumes=" << op->getName() << "_consumes;" << endl;
  *fout << "    source=\"" << (op->getToken() ? (op->getToken()->str+" in "+
						 *op->getToken()->file)
					      : (string("internal ")+
						 op->getName()))
			   << "\";" << endl; 
  if (gProfileStates) {
    *fout << "    for (int s1=0; s1<" << num_states << "; s1++)" << endl;
    *fout << "      state_freqs[s1]=0;" << endl;
    *fout << "    for (int s1=0; s1<" << num_states << "; s1++)" << endl;
    *fout << "      for (int s2=0; s2<" << num_states << "; s2++)" << endl;
    *fout << "        state_xfer_freqs[s1][s2]=0;" << endl;
  }
  forall(sym,*argtypes)
    {
      if (ccParamCausesInstance(sym))
	{
	  SymbolVar *svar=(SymbolVar *)sym;
	  Expr *sval=svar->getValue();
	  if (sval!=(Expr *)NULL)
	    if (sval->getExprKind()==EXPR_VALUE)
	      {
		ExprValue *eval=(ExprValue *)sval;
		long long ival=eval->getIntVal();
		long long fval=eval->getFracVal();
		if ((sval->getType())->getTypeKind()==TYPE_FIXED)
		  *fout << "    // " << sym->getName() << " = " <<
		    ival << "." << fval << endl;
		else if ((sval->getType())->getTypeKind()==TYPE_INT ||
			 (sval->getType())->getTypeKind()==TYPE_BOOL  )
		  *fout << "    // " << sym->getName() << " = " << ival << endl;
		else
		  {
		    fatal(-1,
			  string("Not expecting to handle initial value for type [%d]",
				 ((int)((sval->getType())->getTypeKind()))),
			  sval->getToken()); 
		  }
	      }
	    else
	      {
		*fout << "    // " << sym->getName() << " = " 
		      << sval->toString() << "(NOT a value!)" << endl;
		error(-1,string("%s [%s] is bound with non-value %s",
				(const char *)op->getName(),
				(const char *)
				  (op->getToken() ? op->getToken()->str
						  : (string("internal ")+
						     op->getName())),
				(const char *)sval->toString()),
		      sval->getToken());

	      }
	  else
	    {
	      *fout << "    // " << sym->getName() << " WAS GIVEN NO VALUE!"
		    << endl;
	      error(-1,string("%s [%s] has no value bound",
			      (const char *)op->getName(),
			      (const char *)
			        (op->getToken() ? op->getToken()->str
						: (string("internal ")+
						   op->getName()))),
		    svar->getToken());
	    }
	}
    }

  // This should change whenever we need to construct superpages....
  *fout << "    loc=NO_LOCATION;" << endl;
  // TODO properly define input_rates, output_rates
  *fout << "    input_rates=new int[" << ins << "];" << endl;
  *fout << "    output_rates=new int[" << outs << "];" << endl;
  for(int i=0;i<ins;i++)
    // NOTE: eventually these should be set based on rate determination
    *fout << "    input_rates[" << i << "]=" << "-1" << ";" << endl;
  for(int i=0;i<outs;i++)
    // NOTE: eventually these should be set based on rate determination
    *fout << "    output_rates[" << i << "]=" << "-1" << ";" << endl;


  // initialize input/output free record keeping
  *fout << "    input_free=new int[" << ins << "];" << endl;
  *fout << "    for (int i=0;i<" << ins << ";i++)" << endl;
  *fout << "      input_free[i]=0;" << endl;
  *fout << "    output_close=new int[" << outs << "];" << endl;
  *fout << "    for (int i=0;i<" << outs << ";i++)" << endl;
  *fout << "      output_close[i]=0;" << endl;

  // declare initial values for variables
  // This really better be a behavioral operator...
  SymTab *symtab=((OperatorBehavioral *)op)->getVars();
  list<Symbol*>* lsyms=symtab->getSymbolOrder();
  list_item item2;
  forall_items(item2,*lsyms)
    {
      Symbol *sum=lsyms->inf(item2);
      SymbolVar *asum=(SymbolVar *)sum;
      Expr* val=EvaluateExpr(asum->getValue());
      if (val!=(Expr *)NULL)
	*fout << "    " << asum->getName() << "=" 
	      << ccEvalExpr(EvaluateExpr(val)) << ";" << endl;
    }

  *fout << "  } // constructor " << endl;
  // END CONSTRUCTOR


  // moved from top of ccpage. -- mmchu, 11/9/99.
  forall_items(item,*states)
    {
      //compute produces and consumes here while passing over
      produces[scnt]=allProducers(states->inf(item));
      consumes[scnt]=allConsumers(states->inf(item));
      scnt++;
    }
  
  
  //
  ////////////////////////////////////////////////////////////////////////
  // define pagestep (still in ccpage)

  *fout << "  int pagestep() { " << endl;  
  // pagestep --> real work here
  //  declare symbols to hold stream read values
  forall(sym,*argtypes)
    {
      if (sym->isStream())
	{
	  SymbolStream *ssym=(SymbolStream *)sym;
	  if (ssym->getDir()==STREAM_IN)
	    {
	       *fout << "    " << getCCvarType(sym) << " " 
		    << sym->getName() << ";" << endl;
	    }
	}
    }
  int *early_free=new int[ins];
  for (int i=0;i<ins;i++)
    early_free[i]=0;
  int *early_close=new int[outs];
  for (int i=0;i<outs;i++)
    early_close[i]=0;
  // PAGESTEP CODE HERE 
  *fout << "    int done=0;" << endl; 
  *fout << "    int canfire=1;" << endl;
  if ((num_states+1)>1)
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
      if ((num_states+1)>1)
	*fout << "      case " << state_prefix << sname
	      << ": { " << endl;
      for (int i=cases->low();i<=cases->high();i++)
	{
	  // walk over inputs to case
	  InputSpec *ispec;
	  StateCase *acase=(*cases)[i];
	  ARGMASK_TYPE caseproduces=*(ARGMASK_TYPE*)
				    (acase->getAnnote(CC_ALLPRODUCERS));
					// NOTE: this is assuming 
					// allProducers() was called 
					// before this point! -- mmchu
					// 11/9/99
	  int j;

          // increment the nesting count and also output a beginning nesting
          // bracket.
          numNestings++;
          *fout << "        {" << endl;

	  forall (ispec,*(acase->getInputs()))
	    {
	      //  find input in caseIns
	      int loc=caseIns->binary_search(ispec->getStream());
	      if (loc<caseIns->low())
		fatal(-1,"inconsistency in ccpage search",
		      ispec->getStream()->getToken());
	      //  if firstUsed, set var by asking EOS
	      if ((*firstUsed)[loc]==i)
		{
		  *fout << "        int data_" 
			<< (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			<< "=STREAM_DATA_ARRAY(" 
			<< "in[" 
			<< (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			<< "]" 
			<< ");" << endl;

		  *fout << "        int eos_" 
			<< (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			<< "=0;" << endl;
		  *fout << "        if (data_" 
			<< (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			<< ") " 
			<< "eos_" 
			<< (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			<< "=STREAM_EOS_ARRAY(" 
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
	      *fout << " && data_" 
		    << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) ; 
	      *fout << " && ";
	      if (!ispec->isEosCase()) 	// Nachiket: Not supporting EOFR in _instance.cc file since it runs on HSRA.. and who ares about such crazy acronyms!? :)
		*fout << "!";
	      *fout << "eos_" 
		    << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) ;
	    }
	  *fout << ") {" << endl;
	  // check on the FULLness of all of the output streams of this
	  // state case! NOTE: if -xc was not run and the outputs are
	  // conditional on the inputs, then this will be overly conservative
	  // since it will wait for all possible outputs of this state case
	  // to become non-FULL before proceeding! -- mmchu 11/9/99
	  *fout << "         ";
	  *fout << " if (1";
	  j = 0;
	  while (caseproduces != 0) {
	    bool producingToStream = ((caseproduces&1)!=0);

	    if (producingToStream) {
	      *fout << " && !STREAM_FULL_ARRAY(out[" << j << "])";
	    }

	    // increment index to next bit.
	    caseproduces = caseproduces>>1;
	    j++;
	  }
	  *fout << ") {" << endl;
	  // at this point we've decided to fire
	  if (gProfileStates) {
	    *fout << "            state_freqs[" << state_prefix << sname
					        << "]++;"       << endl;
	  }
	  forall (ispec,*(acase->getInputs()))
	    {
	      if (!ispec->isEosCase())
		{
		  *fout << "            " 
			<< ispec->getStream()->getName() 
			<< "=STREAM_READ_ARRAY(" 
			<< "in[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			<< "]"
			<< ");" << endl;
		  // move around data in retime (if necessary)

		  // not most efficient thing in world, 
		  //   but most straightforward
		  *fout << "            " 
			<< "for (int j=retime_length_"
			<< (long)ispec->getStream()->getAnnote(CC_STREAM_ID)
			<< ";j>0;j--)" << endl;
		  *fout << "              " 
			<< ispec->getStream()->getName() 
			<< "_retime[j]="
			<< ispec->getStream()->getName() 
			<< "_retime[j-1];" << endl;
		  *fout << "            " 
			<< ispec->getStream()->getName() 
			<< "_retime[0]="
			<< ispec->getStream()->getName()
			<< ";" << endl;
		}
	      else
		{
		  *fout << "            " 
			<< "STREAM_FREE(" 
			<< "in[" << (long)(ispec->getStream()->getAnnote(CC_STREAM_ID)) 
			<< "]"
			<< ");" << endl;
		  early_free[(long)(ispec->getStream()->getAnnote(CC_STREAM_ID))]=1;
		  *fout << "            " 
			<< "input_free[" 
			<< (long)(ispec->getStream()->getAnnote(CC_STREAM_ID))
			<< "]=1;" << endl;
		}
	    }

	  if (debug_logic)
	    {
	      *fout << "            " 
		    << "cerr << \"" << op->getName() << " entering state " 
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
	  
	  Stmt* stmt;
	  forall(stmt,*(acase->getStmts()))
	    {
	      ccStmt(fout,string("            "),stmt,early_close,state_prefix,
		     1/*in_pagestep=1*/);
	    }
	  *fout << "          }" << endl;
	  *fout << "        }" << endl;
	  *fout << "        else" << endl;
	}
      // check if there is an uncaught eos which implies a default exit
      *fout << "         if (0";
      for (int i=caseIns->low();i<=caseIns->high();i++)
	{
	  long iloc=(long)((*caseIns)[i])->getAnnote(CC_STREAM_ID); 
	  *fout << " || (data_" << iloc << " && eos_" << iloc <<")" ;
	}
      *fout << ") done=1; else canfire=0;" << endl;
      // otherwise (else case), no data...don't fire this time

      // close the nesting brackets.
      *fout << "        ";
      for (; numNestings>0; numNestings--) {
        *fout << "}";
      }
      *fout << endl;
	    
      if ((num_states+1)>1)
	{
	  *fout << "        break; " << endl;
	  *fout << "      } // end case " << state_prefix << 
	    sname << endl;
	}
      snum++;
    }
  // output the "fictional" done state.
  *fout << "      case " << state_prefix << "this_page_is_done: {" << endl;
  *fout << "        doneCount++;" << endl;
  *fout << "        return(0);" << endl;
  *fout << "      } // end case " << state_prefix << 
    "this_page_is_done" << endl;
  if ((num_states+1)>1)
    {
      *fout << "      default: cerr << \"ERROR unknown state [\" << state << \"] encountered in " << op->getName() << "::pagestep\" << endl;" << endl;
      *fout << "        abort();" << endl;
      *fout << "    }" << endl;
    }

  *fout << "    if (canfire) fire++; else stall++;" << endl;

  *fout << "    if (done) {" << endl;
  // any final stuff
  if (!noReturnValue(rsym))
    if (early_close[(long)(rsym->getAnnote(CC_STREAM_ID))])
      {
	*fout <<"      if (!output_close[" 
	      <<  (long)(rsym->getAnnote(CC_STREAM_ID))
	      << "])" << endl;
	*fout << "        STREAM_CLOSE(" 
	      << "out[" << (long)(rsym->getAnnote(CC_STREAM_ID))
	      << "]" 
	      << ");" << endl;
      }
    else
      *fout << "      STREAM_CLOSE(" 
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
		  *fout <<"      if (!output_close[" 
			<<  (long)(ssym->getAnnote(CC_STREAM_ID))
			<< "])" << endl;
		  *fout << "        STREAM_CLOSE(" 
			<< "out[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			<< "]" 
			<< ");" << endl;
		}
	      else
		*fout << "      STREAM_CLOSE(" 
		      << "out[" << (long)(ssym->getAnnote(CC_STREAM_ID)) 
		      << "]" 
		      << ");" << endl;
	    }
	  else
	    if (early_free[(long)(ssym->getAnnote(CC_STREAM_ID))])
	      {
		/*
		  cerr << "DEBUG early free[" << (int)(ssym->getAnnote(CC_STREAM_ID))
		  << "]=" << early_free[(int)(ssym->getAnnote(CC_STREAM_ID))]
		  << endl;
		  */
		
		*fout <<"      if (!input_free[" 
		      << (long)(ssym->getAnnote(CC_STREAM_ID)) 
		      << "])" << endl;
		*fout << "        STREAM_FREE(" 
		      << "in[" << (long)(ssym->getAnnote(CC_STREAM_ID))
		      << "]" 
		      << ");" << endl;
	      }
	    else
	      *fout << "      STREAM_FREE(" 
		    << "in[" << (long)(ssym->getAnnote(CC_STREAM_ID))
		    << "]" 
		    << ");" << endl;
	}
    }
  *fout << "      state=" << state_prefix << "this_page_is_done;" << endl;

  if (gProfileStates) {
    // - add code to output FSM feedback file
    *fout << "      // Generate FSM feedback file" << endl;
    *fout << "      //  - omit all self loops, "
			"since implicit stay is not captured correctly" <<endl;
    *fout << "      //  - omit dummy done state" << endl;
    *fout << "      char *fpath=getenv(\"" << SCORE_FEEDBACK_PATH_ENV << "\");"
								      << endl;
    *fout << "      char  fname[1024];" << endl;
    *fout << "      sprintf(fname, \"%s/" << op->getName()
					  << ".ffsm\", (fpath?fpath:\""
					  << DEFAULT_FEEDBACK_PATH << "\"));"
					  << endl;
    *fout << "      FILE *fp=fopen(fname,\"a\");" << endl;
    *fout << "      if (fp) {" << endl;
    *fout << "        int s1,s2;" << endl;
    *fout << "        long long freq;" << endl;
    *fout << "        for (s1=0; s1<" << num_states << "; s1++)" << endl;
    *fout << "          fprintf(fp, \"count(%s)=%Ld\\n\", "
			  << state_name_array << "[s1], "
			  << "state_freqs[s1]);" << endl;
    *fout << "        for (s1=0; s1<" << num_states << "; s1++)" << endl;
    *fout << "          for (s2=0; s2<" << num_states << "; s2++)" << endl;
    *fout << "            if ((freq=state_xfer_freqs[s1][s2])>0)" << endl;
    *fout << "              fprintf(fp, \"count(%s,%s)=%Ld\\n\", "
				<< state_name_array << "[s1], "
				<< state_name_array << "[s2], "
				<< "freq);" << endl;
    *fout << "        fclose(fp);" << endl;
    *fout << "      }" << endl;
    *fout << "      else" << endl;
    *fout << "        fprintf(stderr, \"Could not open FSM feedback file "
					"\\\"%s\\\"\\n\", fname);" << endl;
  }

  *fout << "      return(0);" << endl; 
  *fout << "    }" << endl;
  *fout << "    else return(1);" << endl;
  *fout << "  } // pagestep" << endl;

  // END PAGESTEP

  //
  ////////////////////////////////////////////////////////////////////////
  // define class variables (mostly things that persist across pagesteps)
  //  (still in ccpage)

  *fout << "private:" << endl;
  // classvars == everything in operator symbol table
  //  persist across states

  /* this was removed since having a local class state variable causes the
     ScorePage.class variable to always be set to 0. -- mmchu, 11/11/99.
  *fout << "  " + op->getName() + "_state_syms state;" << endl;
  */

  // declare retiming lengths and values
  forall(sym,*argtypes)
    {
      if (sym->isStream())
	{
	  SymbolStream *ssym=(SymbolStream *)sym;
	  if (ssym->getDir()==STREAM_IN)
	    {
	      *fout << "  int retime_length_"
		    << (long)sym->getAnnote(CC_STREAM_ID)
		    << ";" << endl;
	      *fout << "  " << getCCvarType(sym) << " " 
		    << "*"
		    << sym->getName()
		    << "_retime;" << endl;
	    }
	}
    }
		      



  // This really better be a behavioral operator...

  // These were set way above when we wrote initial values
  //    SymTab *symtab=((OperatorBehavioral *)op)->getVars();
  //    list<Symbol*>* lsyms=symtab->getSymbolOrder();
  forall_items(item2,*lsyms)
    {
      Symbol *sum=lsyms->inf(item2);
      SymbolVar *asum=(SymbolVar *)sum;
      *fout << "  " << getCCvarType(asum) << " " << asum->getName() 
	    << ";" << endl;
    }
  // TODO: declare/initialize retiming slots for input streams
  // These are for when need to dynamicly track input/output closing
  *fout << "  int *input_free;" << endl;
  *fout << "  int *output_close;" << endl;
  *fout << "};" << endl;


  // moved from top of ccpage. -- mmchu, 11/9/99.
  *fout << ARGMASK_TYPE_STR " " << op->getName() << "_consumes[]={";
  first=1;
  for(int i=0;i<num_states;i++)
    {
      if (!first)
	*fout << "," ;
      else
	first=0;
      *fout << consumes[i];
    }
  // output the consumes for the "fictional" done state.
  if (!first)
    *fout << ",";
  else
    first=0;
  *fout << "0";
  *fout << "};" << endl;
  *fout << ARGMASK_TYPE_STR " " << op->getName() << "_produces[]={";
  first=1;
  for(int i=0;i<num_states;i++)
    {
      if (!first)
	*fout << "," ;
      else
	first=0;
      *fout << produces[i];
    }
  // output the produces for the "fictional" done state.
  if (!first)
    *fout << ",";
  else
    first=0;
  *fout << "0";
  *fout << "};" << endl;
}

//
////////////////////////////////////////////////////////////////////////
// top level routine to create instance (ScoreOperatorInstance)


string ccinstance(Operator *op, const char *basename, FeedbackRecord *rec,
		  int debug_logic)
{
  // note: depends on ccrename (or something) being called to 
  //   rename all variables so won't conflict w/ variables
  //   added here or C++ keywords

  timestamp(string("ccinstances called on ")+ op->getName());

  ccCopyPageInit(); // initialize set of known pages 

  // construct name of output file
  // should not have trashed top level function name....
  int pcnt=ccCountParameters(op->getArgs());
  int *params=new int[pcnt];
  int pi=0;
  int i=0;
  Symbol *sym;
  forall(sym,*(op->getArgs()))
    {
      if (rec->isParam(i))
	if (ccParamCausesInstance(sym))
	  {
	    params[pi]=rec->getParam(i);
	    pi++;
	  }
      i++;
    }

  char *iname=mangle(basename,pcnt,params);
  // cerr << " renaming " << op->getName() << " to instance " << iname << endl;
  op->setName(iname);
  
  //  open output file

  string fname=string("%s.cc",iname);
  ofstream *fout=new ofstream(fname);
  *fout << "// cctdfc autocompiled instance file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;

  // header files  / includes
  *fout << "#include \"Score.h\"" << endl;
  *fout << "#include <stdlib.h>" << endl;
  *fout << "#include <iostream>" << endl;
  *fout << "#include <string.h>" << endl;
  *fout << "#include \"" << basename << ".h\"" << endl;

  // for each operator==page
  // ?? will I need to clear this annotation and redo this?
  long pages=0;
  long segments=0;
  if (op->getOpKind()==OP_COMPOSE)
    {

      ccprep(op); // generic for things need to be done on pre pass
      set<Operator *>* called_ops=(set<Operator *>*)op->getAnnote(CC_CALL_SET);
      pages=(long)(op->getAnnote(CC_PAGES)); 
      segments=(long)(op->getAnnote(CC_SEGMENTS)); 
      if (called_ops!=(set<Operator *>*)NULL)
	{
	  Operator *cop;
	  forall(cop,*called_ops)
	    {
	      if (cop->getOpKind()==OP_BUILTIN)
		{
		  if (((OperatorBuiltin *)cop)->getBuiltinKind()==BUILTIN_SEGMENT)
		    {
		      // nothing
		    }
		  else if (((OperatorBuiltin *)cop)->getBuiltinKind()==BUILTIN_COPY)
		    {
		      ccCopyPage(fout,(OperatorBuiltin *)cop); // TODO
		    }
		  else if (((OperatorBuiltin *)cop)->getBuiltinKind()==BUILTIN_TERMINATE)
		    {
		      // skip, nothing to do here
		    }
		  // no other cases at present
		}
	      else // not builtin
		{
		  ccpage(fout,cop,debug_logic);
		}
	    }

	}
    }
  else
    {
      ccpage(fout,op,debug_logic);
      pages=1;
    }

  // DEBUG
  // cout << "***AFTER COUNTING GOT p=" << pages << " s=" << segments << endl;

  // composite (operatorinstance) code
  // constructor
  *fout << "class " << iname << ": public ScoreOperatorInstance {" << endl;
  *fout << "public:" << endl;
  *fout << "  " << iname << "(" ;
  // args sans params
  i=0;
  Symbol *rsym=op->getRetSym();
  list<Symbol*> *argtypes=op->getArgs(); 
  if (!noReturnValue(rsym))
    {
      /*
	cerr << " renaming " << rsym->getName();
	rsym->setName(unique_name()); // ?? working?
	cerr << " to " <<  rsym->getName() << endl;
      */
      *fout << getCCtype(rsym) << " " << rsym->getName() ;
      
      i++;
    }
  forall(sym,*argtypes)
    {
      if (!ccParamCausesInstance(sym))
	{
	  if (i>0) *fout << ",";
	  *fout << getCCtype(sym) << " " << sym->getName() ;
	  i++;
	}
    }
  *fout << ") {" << endl;
  *fout << "    pages=" << pages <<";"<< endl;
  *fout << "    segments=" << segments <<";"<< endl; 
	// ?? this getting computed wrong 10/14/99 -- amd?
        // 10/21 -- note the problem here is that called_ops
        //    was giving unique instances so 'segments' is
        //  the number of unique segements, not the number of instances!
  *fout << "    page=new (ScorePage *)[" << pages << "];" << endl;
  *fout << "    segment=new (ScoreSegment *)[" << segments << "];" << endl;
  *fout << "    page_group=new int[" << pages << "];" << endl;
  *fout << "    segment_group=new int[" << segments << "];" << endl;

  if (op->getOpKind()==OP_COMPOSE)
    compose_instance_body(fout,(OperatorCompose *)op);
  else
    {
      compose_instance_wrapper(fout,op);
      *fout << "    page[0]=" << op->getName() << "_inst;" << endl;
      *fout << "    page_group[0]=NO_GROUP;" << endl;
    }

  *fout << "  }" << endl;
  *fout << "};" << " // " << op->getName() << endl;

  // external 'construct' 
  *fout << "extern \"C\" ScoreOperatorInstance *" << SCORE_CONSTRUCT_NAME 
	<< "(char *argbuf) {" << endl;
  *fout << "  " << basename << "_arg *data;" << endl;
  *fout << "  data=(" << basename << "_arg *)malloc(sizeof(" 
	<< basename << "_arg));" << endl;
  *fout << "  memcpy(data,argbuf,sizeof(" << basename << "_arg));" << endl;
  *fout << "  return(new " << iname << "(" ;
  i=0;
  if (!noReturnValue(rsym))
    {
      *fout << getCCobjConvert(rsym,string("data->i%d",i));
      i++;
    }
  forall(sym,*argtypes)
    {
      // ?? TODO --> check how return sym works in this case...
      if (!ccParamCausesInstance(sym))
	{
	  if (i>0)
	    *fout << ",";
	  *fout << getCCobjConvert(sym,string("data->i%d",i));
	  i++;
	}
    }
  *fout << "));" << endl;
  *fout << "}" << endl;

  // close up
  fout->close();
  return(op->getName());

}

string ccinstance(Operator *op, const char *basename, FeedbackRecord *rec)
{
  return(ccinstance(op,basename,rec,0)); 
}
