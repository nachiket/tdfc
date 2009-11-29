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
// SCORE TDF compiler:  generate C++ output
// $Revision: 1.132 $
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include "operator.h"
#include "stmt.h"
#include "state.h"
#include "annotes.h"
#include "bindvalues.h"
#include "cctype.h"
#include "ccannote.h"
#include "cceval.h"
#include "ccstmt.h"

using std::cout;
using std::endl;
using leda::list_item;

/***********************************************************************
Note: builtins expecting to handle here 
   done
   close
  (only two left at stmt level)
***********************************************************************/

void ccStmt(ofstream *fout, string indent, Stmt *stmt, int *early_close,
	    string state_prefix, bool in_pagestep=0, bool retime)
{

//	if(retime) {
//		cout << "Such bullshit" << endl; exit(1);
//	}

  switch (stmt->getStmtKind())
    {

    case STMT_GOTO:
      {
	StmtGoto *gstmt=(StmtGoto *)stmt;
	*fout << indent << "state=" << state_prefix 
	  // was:
	  //	      << gstmt->getStateName() << ";" 
	  // changed to:
	      << (gstmt->getState())->getName() << ";" 
	  // DEBUG:    << "//" << gstmt->toString() 
	      << endl;
	if (gProfileStates && in_pagestep) {
	  Tree* t;	// t = present state
	  for (t=gstmt; t && t->getKind()!=TREE_STATE; t=t->getParent());
	  if (t==NULL)
	    assert(!"internal inconsistency");
	  *fout << indent << "state_xfer_freqs["
	        << state_prefix << ((State*)t)->getName() << "]["
	        << state_prefix << gstmt->getState()->getName() << "]++;\n";
	}
	return;
      }
    case STMT_IF:
      {
	StmtIf *ifstmt=(StmtIf *)stmt;
	*fout << indent << "if (" 
	      << ccEvalExpr(EvaluateExpr(ifstmt->getCond()), retime) 
	      << ") {" << endl;
	ccStmt(fout,string("%s  ",indent),ifstmt->getThenPart(),
	       early_close,state_prefix,in_pagestep);
	*fout << indent << "}" << endl;
	Stmt *epart=ifstmt->getElsePart();
	if (epart!=(Stmt *)NULL)
	  {
	    *fout << indent << "else {" << endl;
	    ccStmt(fout,string("%s  ",indent),epart,early_close,
		   state_prefix,in_pagestep);
	    *fout << indent << "}" << endl;
	  }
	return;
      }
    case STMT_CALL:
      {
	warn("ccStmt: expecting calls to be transformed out before calling this",
	     stmt->getToken());
	return;
      }
    case STMT_BUILTIN:
      {
	StmtBuiltin     *bstmt=(StmtBuiltin *)stmt;
	ExprBuiltin     *bexpr=bstmt->getBuiltin();
	list<Expr*>     *args =bexpr->getArgs();
	Expr            *first=args->empty()?NULL:args->head();
	Operator        *op   =bexpr->getOp();
	OperatorBuiltin *bop  =(OperatorBuiltin *)op;
	if (bop->getBuiltinKind()==BUILTIN_CLOSE)
	  {

	    if (first->getExprKind()!=EXPR_LVALUE)
	      warn(string("close given invalid argument %s",
			  first->toString()),first->getToken());
	    else
	      {
	      	ExprLValue *lexpr=(ExprLValue *)first;
		int id=(int)(lexpr->getSymbol()->getAnnote(CC_STREAM_ID));
		*fout << indent << "STREAM_CLOSE(" 
		      << "out[" << id << "]" 
		      << ");" << endl;

		early_close[id]=1;
		*fout << indent 
		      << "output_close[" << id << "]=1;" 
		      << endl;
	      }
	  }
	// Added by Nachiket on 10/6/2009 to support frameclose operation
	else if (bop->getBuiltinKind()==BUILTIN_FRAMECLOSE)
		  {

		    if (first->getExprKind()!=EXPR_LVALUE)
		      warn(string("frameclose given invalid argument %s",
				  first->toString()),first->getToken());
		    else
		      {
		      	ExprLValue *lexpr=(ExprLValue *)first;
			int id=(int)(lexpr->getSymbol()->getAnnote(CC_STREAM_ID));
			*fout << indent << "FRAME_CLOSE("
			      << "out[" << id << "]"
			      << ");" << endl;

			/* Nachiket: Don't think any of these are valid.. are they?
			early_close[id]=1;
			*fout << indent
			      << "output_close[" << id << "]=1;"
			      << endl;
			*/
		      }
		  }
	else if (bop->getBuiltinKind()==BUILTIN_DONE)
	  {
	    *fout << indent 
		  << "done=1;" << endl;
	  }
	else if (bop->getBuiltinKind()==BUILTIN_PRINTF)
	  {
	    // *fout << indent << "printf(\""
	    *fout << indent << "fprintf(stderr,\""
	          << ((Token*)bexpr->getAnnote(ANNOTE_PRINTF_STRING_TOKEN))->str
		  << "\"";
	    for (list_item i=args->first(); i; i=args->succ(i)) {
		    // Nachiket's modifications to support floating-point casting
		    Expr* orig=args->inf(i);
		    if (orig->getType()->getTypeKind()!=TYPE_FLOAT &&
				    orig->getType()->getTypeKind()!=TYPE_DOUBLE) {
			    //*fout << ", (long long)" --> Not sure if this is such a good idea in any case
			    *fout << ", "
				    << ccEvalExpr(EvaluateExpr(args->inf(i)), retime) << "";
		    } else {
			    *fout << ", " << ccEvalExpr(EvaluateExpr(args->inf(i)), retime) << "";
		    }
	    }
	    *fout << ");" << endl;
	  }
	else
	  {
	    fatal(-1,string("Unexpected builtin Operator [%d] in Statement",
			    (int)bop->getBuiltinKind()),bop->getToken());
	  }
	return;
      }
    case STMT_ASSIGN:
      {
	StmtAssign *astmt=(StmtAssign *)stmt;
	ExprLValue *lval=astmt->getLValue();
	// added by Nachiket on Sep29th to use fancy floating-point stream access functions in the Score runtime
	TypeKind lvalTypeKind = lval->typeCheck()->getTypeKind();
	bool floattyp = (lvalTypeKind==TYPE_FLOAT);
	bool doubletyp = (lvalTypeKind==TYPE_DOUBLE);
	Expr *rexp=astmt->getRhs();
	Symbol *asym=lval->getSymbol();
	if (asym->isStream())
	  {
	    SymbolStream *ssym=(SymbolStream *)asym;
	    if (ssym->getDir()==STREAM_OUT)
	      {
		int id=(int)(ssym->getAnnote(CC_STREAM_ID));
		*fout<<indent
		     <<(in_pagestep?"STREAM_WRITE_ARRAY("
				   : (floattyp)? "STREAM_WRITE_FLOAT(": (doubletyp)? "STREAM_WRITE_DOUBLE(":"STREAM_WRITE_NOACC(")
		     << "out[" << id << "]" 
		     << ","
		     << ccEvalExpr(EvaluateExpr(rexp), retime) << ");" << endl;
	      }
	    else
	      {
		warn("writing to input stream!",lval->getToken());
		// someone else complained?
	      }
	  }
	else
	  { 
	    /* don't have to handle retime here since it shouldn't appear */

	    /* MAYBE: add mask here to get rid of any bits out of type range */
	    if (lval->usesAllBits())
	      *fout<<indent<<asym->getName()<<"="
		   <<ccEvalExpr(EvaluateExpr(rexp), retime)<<";"<<endl;
	    else
	      {
		Expr *low_expr=lval->getPosLow();
		Expr *high_expr=lval->getPosHigh();
		string lstr=ccEvalExpr(EvaluateExpr(low_expr), retime);
		string hstr=ccEvalExpr(EvaluateExpr(high_expr), retime);
		string rstr=ccEvalExpr(EvaluateExpr(rexp), retime);
		string one =getCCvarType(asym).pos("long long")>=0 ? "1ll":"1";
		*fout << indent
		      << asym->getName() << "="
		      << "(" << asym->getName() << " & " 
		      << "~(((" << one << "<<("      << hstr
		      << "+1))-1)-((" << one << "<<" << lstr
		      << ")-1))"
		      << ")"
		      << " | (" << rstr << "<<" << lstr << ")"
		      << ";" << endl;
		  
	      }

	    // TODO: somewhere here handles memories, or something like that
		       
	  }

	return;
      }
    case STMT_BLOCK:
      {
	StmtBlock *bstmt=(StmtBlock *)stmt;
	
	SymTab *symtab=bstmt->getSymtab();
	list<Symbol*>* lsyms=symtab->getSymbolOrder();
	list_item item;
	*fout << indent << "{" << endl;

	forall_items(item,*lsyms)
	  {
	    // TODO: deal with memories separately...maybe somewhere earlier
	    Symbol *sum=lsyms->inf(item);
	    SymbolVar *asum=(SymbolVar *)sum;
	    *fout << indent <<  "  " << getCCvarType(asum) 
		  << " " << asum->getName() ;
	    Expr* val=asum->getValue();
	    if (val!=(Expr *)NULL)
	      *fout << "=" << ccEvalExpr(EvaluateExpr(val), retime) ;
	    *fout << ";" << endl;
	  }

	Stmt* astmt;
	forall(astmt,*(bstmt->getStmts()))
	  {
	    ccStmt(fout,string("%s  ",indent),astmt,early_close,state_prefix,
								in_pagestep);
	  }

	*fout << indent << "}" << endl;
	
	return;
      }
    default:
      {
	fatal(-1,string("unknown statement kind [%d] in ccStmt",
			(int)stmt->getStmtKind()),
	      stmt->getToken());
      }

    }

}


