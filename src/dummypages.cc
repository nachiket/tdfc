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
// SCORE TDF compiler:  Dummy page partitioning with 1 op per pge
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#include "symbol.h"
#include "expr.h"
#include "stmt.h"
#include "operator.h"
#include "suite.h"
#include "dismantle.h"

extern Suite *gSuite;


OperatorCompose *makeDummyPage (OperatorBehavioral *op, Token *token)
{
  // - Create and return a compositional op that simply calls *op
  // - New op has all tokens set to to *token
  // - New op is named "_page_<opname>"

  // - compositional op's args:
  list<Symbol*> *args = new list<Symbol*>;
  Symbol *arg;
  forall (arg, *op->getArgs())
    args->append((Symbol*)arg->duplicate());

  Symbol *retsym = op->getRetSym() ? (Symbol*)op->getRetSym()->duplicate()
				   : NULL;

  // - compositional op's local streams:  (vars)
  SymTab *vars = new SymTab(SYMTAB_BLOCK);

  // - call to behav op:
  list<Expr*> *callargs = new list<Expr*>;
  forall (arg, *args)
    callargs->append(new ExprLValue(token,arg));
  ExprCall *call = new ExprCall(token,callargs,op);
  Stmt *stmt;
  if (retsym && !retsym->equals(sym_none))
    stmt = new StmtAssign(token, new ExprLValue(token,retsym), call);
  else
    stmt = new StmtCall(token,call);
  list<Stmt*> *stmts = new list<Stmt*>;
  stmts->append(stmt);

  // - compositional op's name:  <op>_page
  string newName = "_page_" + op->getName();
  if (retsym)
    retsym->setName(newName);

  // - create compositional op
  OperatorCompose *cop = new OperatorCompose(token,newName,
					     retsym,args,vars,stmts);

  return cop;
}


bool dummyPages_map (Tree *t, void *i)
{
  // - map for dummyPages_instance
  // - Convert flattened compositional op into a paged composition
  //     by wrapping every called behavioral operator in a "page"
  //     compositional operator that calls only that behavioral op
  // - Compositional op is modified to call the new pages
  //     instead of behavioral ops
  // - New pages (compositional ops) are added to gSuite

  switch (t->getKind())
  {
    case TREE_OPERATOR:	return true;

    case TREE_STMT:	return true;

    case TREE_EXPR:	if (((Expr*)t)->getExprKind()==EXPR_CALL &&
			    ((ExprCall*)t)->getOp()->getOpKind()
							    ==OP_BEHAVIORAL) {
			  OperatorBehavioral *op = (OperatorBehavioral*)
						   ((ExprCall*)t)->getOp();
			  OperatorCompose *page = makeDummyPage(op,
								t->getToken());
			  // - unique rename op and add to gSuite
			  SymbolOp dummySym(NULL,page);
			  uniqueRenameSym_inc(&dummySym,gSuite->getSymtab());
			  page->setName(dummySym.getName());
			  gSuite->addOperator(page);
			  // page->link();	// - called ops not in suite!
			  // - fix original call to behavioral op
			  //     so it calls the new page instead
			  ((ExprCall*)t)->setOp(page);	// - HACK: modify call
			  return false;
			}
			else
			  return true;

    default:		return false;
  }
}


Operator* dummyPages_instance (OperatorCompose *iop)
{
  // - Convert flattened compositional *iop into a paged composition
  //     by wrapping every called behavioral operator in a "page"
  //     compositional operator that calls only that behavioral op
  // - *iop is modified to call the new pages instead of behavioral ops
  // - New pages (compositional ops) are added to gSuite

  iop->map(dummyPages_map);
  return iop;
}
