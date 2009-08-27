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
// SCORE TDF compiler:  map operators to rebind symbols 
// $Revision: 1.106 $
//
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "expr.h"
#include "operator.h"
#include "symbol.h"

bool treemap_false(Tree *t, void *aux)
{ return (0); }

bool rebind_symbols(Tree *t, void *aux)
{
  if (t->getKind()!=TREE_EXPR)
    return(1);
  Expr *expr = (Expr *)t;
  if (expr->getExprKind()!=EXPR_LVALUE)
    return(1);
  ExprLValue *lval=(ExprLValue *)expr;
  SymTab *stab=(SymTab *)aux;
  Symbol *lsym=lval->getSymbol();
  Symbol *new_sym=stab->lookup(lsym->getName());
  if (new_sym==(Symbol *)NULL)
    {
      cerr << "could not resolve a new symbol for " 
	   << lsym->getName() << "["
	   << (lsym->getToken() ? lsym->getToken()->str
				: (string("internal ")+lsym->toString()))
	   << "]" << endl;
      Operator *op=stab->getOperator();
      cerr << "  in " << op->getName() << " [" 
	   << (op->getToken() ? op->getToken()->str
			      : (string("internal ")+op->getName()))
	   << "]" << endl;

      list<Symbol*>* lsyms=stab->getSymbolOrder();
      list_item item;

      cerr << "  existing symbols " << endl;
      forall_items(item,*lsyms)
	{
	  Symbol *asum=lsyms->inf(item);
	  cerr << "    "  << asum->getName()
	       << " ["
	       << (asum->getToken() ? asum->getToken()->str
				    : (string("internal ")+asum->toString()))
	       << "]" << endl;
	}
      abort();
    }
  else
    lval->setSymbol(new_sym);
  return(1);
}
