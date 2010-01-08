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
// SCORE TDF compiler:  operator preprocessing for C++ backend
// $Revision: 1.88 $
//
//////////////////////////////////////////////////////////////////////////////

#include "operator.h"
#include "expr.h"
#include "stmt.h"
#include "annotes.h"
#include <LEDA/core/set.h>
#include <LEDA/core/list.h>

void ccPrepComposeExpr(Operator *op, Expr* expr)
{
  if ((expr->getExprKind()==EXPR_CALL)
      || (expr->getExprKind()==EXPR_BUILTIN))
    {
      ExprCall *ecall=(ExprCall *)expr;
      Operator *cop=ecall->getOp();

      // update set
      set<Operator *>* called_ops=(set <Operator *>*)op->getAnnote(CC_CALL_SET);
      called_ops->insert(cop);
      
      // count segments / pages
      if (expr->getExprKind()==EXPR_BUILTIN)
	{
	  if (((OperatorBuiltin *)cop)->getBuiltinKind()==BUILTIN_SEGMENT)
	    {
	      op->setAnnote(CC_SEGMENTS,
			    (void *)((long)(op->getAnnote(CC_SEGMENTS))+1));
	    }
	  else if (((OperatorBuiltin *)cop)->getBuiltinKind()==BUILTIN_COPY)
	    {
	      op->setAnnote(CC_PAGES,
			    (void *)((long)(op->getAnnote(CC_PAGES))+1));
	    }
	  else if (((OperatorBuiltin *)cop)->getBuiltinKind()==BUILTIN_TERMINATE)
	    {
	      // don't count
	    }
	}
      else // not builtin == page
	{
	  op->setAnnote(CC_PAGES,
			(void *)((long)(op->getAnnote(CC_PAGES))+1));
	}

      // recurse
      list<Expr*>* args=ecall->getArgs();
      Expr *earg;
      forall(earg,*args)
	{
	  ccPrepComposeExpr(op,earg);
	}
    }
  else 
    {
      // complaints elsewhere
    }

}

void ccprep(Operator *op)
{

  set<Operator*>* called_ops=new set<Operator*>(); 
  op->setAnnote(CC_CALL_SET,(void *)(called_ops));
  op->setAnnote(CC_PAGES,0);
  op->setAnnote(CC_SEGMENTS,0);

  switch (op->getOpKind())
    {
    case OP_COMPOSE:
      {
	OperatorCompose *cop=(OperatorCompose *)op;
	
	// walk over stms

	Stmt* statement;
	forall(statement,*(cop->getStmts()))
	  {
	    if (statement->okInComposeOp())
	      {
		if (statement->getStmtKind()==STMT_CALL)
		    ccPrepComposeExpr(op,((StmtCall *)statement)->getCall());
		else if (statement->getStmtKind()==STMT_BUILTIN)
		    ccPrepComposeExpr(op,((StmtBuiltin *)statement)->getBuiltin());
		else if (statement->getStmtKind()==STMT_ASSIGN)
		  {
		    ccPrepComposeExpr(op,((StmtAssign *)statement)->getRhs());
		  }
	      
	      }
	    else 
	      {
		// don't do anything
	      }
	  }
      }
    case OP_BEHAVIORAL:
      {
      }
    default:
      {
	// someone else should complain
      }

    }

}
