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
// SCORE TDF compiler:  Garbage collector
// $Revision: 1.80 $
//
//////////////////////////////////////////////////////////////////////////////


#include "compares.h"
#include "gc.h"
#include "misc.h"
#include "type.h"
#include "expr.h"
#include "operator.h"


set<Tree*> TreeGC::rootSet;			// - known Tree roots
set<Tree*> TreeGC::allocSet;			// - all allocated Tree nodes

int gEnableGC = 0;				// - enable GC (-gc option)


////////////////////////////////////////////////////////////////
//  isReachable

typedef struct { const Tree *target; bool found; } IsReachableInfo;

static
bool isReachableMap (Tree *t, void *i)
{
  // - i is IsReachableInfo*
  // - maps until find node i->target, then sets i->found

  IsReachableInfo *info=(IsReachableInfo*)i;
  if (info->found)
    return false;
  else if (info->target==t)
  {
    info->found=true;
    return false;
  }
  else
    return true;
}


bool TreeGC::isReachable (const Tree *t)
{
  // - return true iff t is reachable from rootSet
  Tree *root;
  forall (root,rootSet)
  {
    IsReachableInfo info={t,false};
    root->map(isReachableMap,(TreeMap)NULL,&info);
    if (info.found)
      return true;
  }
  return false;
}


////////////////////////////////////////////////////////////////
//  gc

static
bool markMap (Tree *t, void *i)
{
  // - i is set<Tree*>*
  // - adds t to i  (i.e. adds all reachable nodes to i set)
  // - special case, if t is inlined call, add called-op (entire tree) to i

  set<Tree*> *markedSet=(set<Tree*>*)i;
  if (gDebugShared)
  {
    // - identify previously-marked nodes (i.e. multiply-reachable)
    if (markedSet->member(t))
      if (t!=type_any && t!=type_none)
	// - HACK: type_any, type_none are shared, multiple reachability ok
	fatal(1, string("garbage collector: AST node %p multiply reachable",t),
	      t->getToken());
  }
  markedSet->insert(t);

  // - HACK: for inlined calls, also mark called operator
  //         (prevents gc-ing unregistered called operators,
  //          e.g. during instance generation)
  Operator *op;
  if (   t->getKind()==TREE_EXPR
      && ((Expr*)t)->getExprKind()==EXPR_CALL
      && (op=((ExprCall*)t)->getOp()) )
    op->map(markMap,(TreeMap)NULL,i);

  return true;
}


void TreeGC::gc ()
{
  if (gEnableGC)
  {
    timestamp("TreeGC::gc() - before gc");
    set<Tree*> markedSet;
    Tree *root;
    forall(root,rootSet)
      root->map(markMap,(TreeMap)NULL,&markedSet);
    set<Tree*> unmarkedSet=allocSet-markedSet;
    Tree *unmarked;
    forall(unmarked,unmarkedSet)
    {
      // cerr << "- deleting node " << unmarked << '\n';
      delete unmarked;			// - removes unmarked from allocSet
    }
    timestamp("TreeGC::gc() - after gc");
  }
}
