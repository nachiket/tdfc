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
// SCORE TDF compiler:  generate C++ output
// $Revision: 1.115 $
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
Note:
  According to our discussion of 7/20/99, this should 
  * not handle direct memory references
  * not handle inlined calls/inline memory blocks
  * not handle bit-level composition operators
  * not handle nested calls
  * not have to deal with scoping heirarchy for operators
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

using leda::list_item;
using leda::dic_item;
using leda::string;
using std::ofstream;

void ccComposeEvalExpr(ofstream *fout, Expr *expr, Symbol *rsym)
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
	  ccComposeEvalExpr(fout,earg,rsym);
	}
      *fout << ")";
    }
  // NOTE: these should only be viable if they evaluate to a SCALAR!
  // assuming type checking already caught that...
  else if (expr->getExprKind()==EXPR_COND)
      {
	ExprCond * cexpr=(ExprCond *)expr;
	*fout << "(";
	ccComposeEvalExpr(fout,cexpr->getCond(),rsym);
	*fout << ")?(";
	ccComposeEvalExpr(fout,cexpr->getThenPart(),rsym);
	*fout << "):(";
	ccComposeEvalExpr(fout,cexpr->getElsePart(),rsym);
	*fout << ")";
      }
  else if (expr->getExprKind()==EXPR_BOP)
      {
	ExprBop *bexpr=(ExprBop *)expr;

	// wrong thing for "." operator...
        // TODO: deal properly with fixed point construction/representation
	*fout << "(mickey";
	ccComposeEvalExpr(fout,bexpr->getExpr1(),rsym);
// 1-2-2010: Don't understand the deal with processing %..
//	if(bexpr->getOp()=='%') {
//		cout << "WTF!!" << endl;
//		exit(0);
//	}
	*fout <<  opToString(bexpr->getOp());
	ccComposeEvalExpr(fout,bexpr->getExpr2(),rsym);
	*fout << ")";
      }
  else if (expr->getExprKind()==EXPR_UOP)
      {
	ExprUop *uexpr=(ExprUop *)expr;
	string ops=opToString(uexpr->getOp());
	Expr *iexpr=uexpr->getExpr();
	*fout << "(" << ops << "(";
	ccComposeEvalExpr(fout,iexpr,rsym);
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
	    ccComposeEvalExpr(fout,real_expr,rsym);
	    *fout << ")";
	  }
	else
	  ccComposeEvalExpr(fout,real_expr,rsym);
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

void ccCompose (ofstream *fout, string name, OperatorCompose *op)
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
  //         -- amd 8/24/99
  //  This observationa above is valid.. but doesn't seem to work
  //  for _copy_ operators...
  //         -- ngk 8/Feb/2011


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
	      {
		syms->del(lsym);
		//cout << "deleting internal symbol " << lsym->toString() << endl;
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
	      ccComposeEvalExpr(fout,((StmtCall *)statement)->getCall(),
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
		  ccComposeEvalExpr(fout,((StmtAssign *)statement)->getRhs(),
				    returnValue);

		}
	      // add exception for assignments to external output signals...
	      // a stmtassign in this case will overwrite the stream pointer by
	      // mistake
	      else
		{
		  *fout << "    ";
		  ccComposeEvalExpr(fout,((StmtAssign *)statement)->getRhs(),
				    returnValue);
		  *fout << "=" << lsym->getName();
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
void ccconstruct(ofstream *fout,string name, Operator *op)
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
  *fout << "  if (instance_fn!=(char *)NULL) {" << endl;

  *fout << "    long slen;" << endl;
  *fout << "    long alen;" << endl;
  *fout << "    long blen;" << endl;
  *fout << "    " << op->getName() << "_arg" << " *data;" << endl;
  *fout << "    struct msgbuf *msgp;" << endl;
  *fout << "    data=(" << op->getName() << "_arg *)malloc(sizeof("
	<< op->getName() << "_arg));" << endl;

  i=0;
  if (!noReturnValue(rsym))
    {
      if (rsym->isStream())
	{
	  *fout << "  result=" << getCCtypeConstructor(rsym); 
	  *fout << ";" << endl;
	  *fout << "    data->i" << i << "=" 
		<< getCCidConvert(rsym,string("result"))  << ";" << endl;
	  i++;
	}
    }

  forall(sym,*argtypes)
    {
      if (!ccParamCausesInstance(sym))
	{
	  *fout << "    data->i" << i << "=" 
		<< getCCidConvert(sym,(prefix+sym->getName())) << ";" << endl;
	  i++;
	}
    }
  *fout << "    alen=sizeof(" << op->getName() << "_arg);" << endl;
  *fout << "    slen=strlen(instance_fn);" << endl;
  *fout << "    blen=sizeof(long)+sizeof(long)+slen+alen;" << endl;
  *fout << "    msgp=(struct msgbuf *)malloc(sizeof(msgbuf)+sizeof(char)*(blen-1));" << endl;
  *fout << "    int sid=schedulerId();\n" 
	<< "    memcpy(msgp->mtext,&alen,sizeof(long));\n"
        << "    memcpy(msgp->mtext+sizeof(long),&slen,sizeof(long));\n"
	<< "    memcpy(msgp->mtext+sizeof(long)*2,instance_fn,slen);\n"
	<< "    memcpy(msgp->mtext+sizeof(long)*2+slen,data,alen);\n"
	<< "    msgp->mtype=SCORE_INSTANTIATE_MESSAGE_TYPE;\n"
	<< "    int res=msgsnd(sid, msgp, blen, 0) ;  \n"
	<< "    if (res==-1) {\n"
        << "       cerr <<\"" << name << " msgsnd call failed with errno=\" << errno << endl;" << "\n"
	<< "       exit(2);    }" << endl;
  

  *fout << "  }" << endl; // if there is instance, launch on scheduler
  *fout << "  else {" << endl; // else no instance, run proc_run


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
//      *fout << "  cout << \"TDF: Setting operator=\" << this->getName() << \"in=\" << this->getInputs() << \" out=\" << this->getOutputs() << endl; " << endl;
	      
  *fout << "  addInstance(instances,this,params);" << endl;

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
		  *fout << "    out["<<ocnt<<"]->setName(\""<<sym->getName()<<"\");" << endl;
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
		  *fout << "    in["<<icnt<<"]->setName(\""<<sym->getName()<<"\");" << endl;
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
	    << op->getName() << "_proc_run, this);\n"
	    << "    pthread_attr_destroy(a_thread_attribute);" 
	    << "    free(a_thread_attribute);" 
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

      ccCompose(fout,name,(OperatorCompose *)op);
    }
  else 
    {
      // maybe print getOpKind for offending...
      error(-1,"FUBAR: don't know how to handle op kind omitting ",
	    op->getToken());
    }


  *fout << "  }" << endl; // else no instance, run proc_run
  *fout << "  free(params);" << endl;
  *fout << "  free(name);" << endl;
  *fout << "  free(instance_fn);" << endl;
  *fout << "}" << endl; // end of constructor
  
}

//
////////////////////////////////////////////////////////////////////////
// collect up retiming exprs per symbol
bool ccbody_treemap_false(Tree *t, void *aux)
{ return (0); }

bool collect_retime_exprs(Tree *t, void *aux)
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
      if (!value_zero)
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

  return(0);
}

//
////////////////////////////////////////////////////////////////////////
// procrun for master instance

void ccprocrun(ofstream *fout, string name, Operator *op,
	       int debug_logic)
{
  *fout << "void *" << name << "::proc_run() {"  << endl;

	*fout << "\tsleep(1);" << endl;
  	*fout << "\tif(ScoreOperator::fout!=NULL) {" << endl;
	*fout << "\t\tget_graphviz_strings(); return NULL;" << endl;
	*fout << "\t}" << endl;

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
      op->map(collect_retime_exprs,ccbody_treemap_false,(void *)NULL);
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
		  *fout << "  int retime_length_"
			<< (long)sym->getAnnote(CC_STREAM_ID)
			<< "=" << constmax << ";" << endl;
		  // blah, this to avoid using non-existent max operator...
		  forall(rexp,*ncset)
		    *fout << "if ((" 
			  << ccEvalExpr(rexp)
			  << ")>"
			  << "retime_length_" 
			  << (long)sym->getAnnote(CC_STREAM_ID)
			  <<")"
			  << "retime_length_" 
			  << (long)sym->getAnnote(CC_STREAM_ID)
			  << "=" << ccEvalExpr(rexp) << ";" << endl;
		  *fout << "  " << getCCvarType(sym) << " " 
			<< "*"
			<< sym->getName()
			<< "_retime="
			<< "new " 
			<< getCCvarType(sym) << " " 
			<< "[retime_length_"
			<< (long)sym->getAnnote(CC_STREAM_ID)
			<< "+1]" // 0->max retiming depth seen
			<< ";" << endl;
		  *fout << "  " 
			<< "for (int j=retime_length_"
			<< (long)sym->getAnnote(CC_STREAM_ID)
			<< ";j>=0;j--)" << endl;
		  *fout << "    " 
			<< sym->getName() 
		    // TODO: assign initial values instead of 0
			<< "_retime[j]=0;" << endl;
		  
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

		      // move around data in retime (if necessary)

		      // not most efficient thing in world, 
		      //   but most straightforward
		      *fout << "          " 
			    << "for (int j=retime_length_"
			    << (long)ispec->getStream()->getAnnote(CC_STREAM_ID)
			    << ";j>0;j--)" << endl;
		      *fout << "            " 
			    << ispec->getStream()->getName() 
			    << "_retime[j]="
			    << ispec->getStream()->getName() 
			    << "_retime[j-1];" << endl;
		      *fout << "          " 
			    << ispec->getStream()->getName() 
			    << "_retime[0]="
			    << ispec->getStream()->getName()
			    << ";" << endl;
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
	      Stmt* stmt;
	      forall(stmt,*(acase->getStmts()))
		{
		  ccStmt(fout,string("          "),stmt,early_close,
			 STATE_PREFIX,0); // 0 was default for ccStmt.h
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
	{
	  *fout << "  STREAM_CLOSE(" 
		<< "out[" << (long)(rsym->getAnnote(CC_STREAM_ID))
		<< "]"
		<< ");" << endl;
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


// Added by Nachiket on 1/20/2010 @ 1.29pm
void ccgraphviz(ofstream *fout, string classname, Operator *op) {
	*fout << "void " << classname << "::get_graphviz_strings() {"  << endl;
	*fout << "\tconst char* stupid=\"null\";" << endl;
	*fout << "\tflockfile(stdout);" << endl;

	if(op->getOpKind()!=OP_COMPOSE) {
	      int ocnt=0, icnt=0;
	      list<Symbol*> *argtypes=op->getArgs();
	      Symbol *sym;
		forall(sym,*argtypes)
		{
			if (sym->isStream())
			{
				SymbolStream *ssym=(SymbolStream *)sym;
				if (ssym->getDir()==STREAM_OUT)
				{
					*fout << "\tif(out["<<ocnt<<"]->src!=NULL && out["<<ocnt<<"]->sink!=NULL) {" << endl;
					*fout << "\t\t*(ScoreOperator::fout) << \"\\t\" << out["<<ocnt<<"]->src->getName() << \"->\" << out["<<ocnt<<"]->sink->getName() << \"[ label= \\\" " << sym->getName() << "\\\" ]\" << endl;" << endl;
					*fout << "\t\tcout << \"\\t\" << out["<<ocnt<<"]->src->getName() << \"->\" << out["<<ocnt<<"]->sink->getName() << \"[ label= \\\" " << sym->getName() << "\\\" ]\" << endl;" << endl;
					//*fout << "\t}" << endl; 
					*fout << "\t} else {cout<<\"cannot connect "<< sym->getName() <<" \"<<endl;}" << endl;
//					*fout << "*fout << \"\\t\" << (out["<<ocnt<<"]->src!=NULL)? out["<<ocnt<<"]->src->getName():stupid << \"->\" << (out["<<ocnt<<"]->sink!=NULL)? out["<<ocnt<<"]->sink->getName():stupid << \"[ label= \\\" " << sym->getName() << "\\\" ]\" << endl;" << endl;
					ocnt++;
				}
				else if (ssym->getDir()==STREAM_IN)
				{
// only one-side is adequate					*fout << "cout << \"\\t\" << in["<<icnt<<"]->src->getName() << \"->\" << in["<<icnt<<"]->sink->getName() << \"[ label= \\\" \"" << sym->getName() << "\\\" \" << \"]\" << endl;" << endl;
//					*fout << "//wtf? in" << endl;
					icnt++;
				}

				else
				{
//					*fout << "//wtf?" << endl;
					// already complained
				}
			} else {
//				*fout << "//wtf? not stream" << endl;
			}
		}
	}
  
	*fout << "\tfunlockfile(stdout);" << endl;
	*fout << "}" << endl;
}

//
////////////////////////////////////////////////////////////////////////
// Top level routine to create master C++ code
void ccbody (Operator *op, int debug_logic)
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
  *fout << "using namespace std;" << endl;

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
//  *fout << "ofstream *ScoreOperator::fout=NULL;" << endl;

  *fout << endl;

  // constructor
  ccconstruct(fout,classname,op);

  *fout << endl;

  // proc_run
  ccprocrun(fout,classname,op,debug_logic);
  *fout << endl;

  // graphviz structure
  ccgraphviz(fout, classname, op);
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




