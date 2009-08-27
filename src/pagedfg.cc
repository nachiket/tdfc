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
// SCORE TDF compiler:  Page-level data-flow graph
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#include <fstream>
#include <LEDA/core/list.h>
#include <LEDA/core/map.h>
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "pagedfg.h"

using leda::map;
using leda::node;
using leda::edge;

////////////////////////////////////////////////////////////////
//  buildPageDFG

class PageDFGInfo {	// - info for building a PageDFG
public:
  PageDFG *dfg;
  map<Tree*,node>   nodemap;	// - nodes of dfg
  set<Symbol*>      edgesyms;	// - syms for edges of dfg
  map<Symbol*,node> srcnodemap;	// - src node for symbol's edge
  map<Symbol*,node> dstnodemap;	// - dst node for symbol's edge

  PageDFGInfo (PageDFG *dfg_i) : dfg(dfg_i) {}
};


bool buildPageDFG_map (Tree *t, void *i)
{
  // - this is a pass for building a PageDFG
  // - specifically, it visits all stmts/exprs of a compose-op instance
  //     to  (1) create nodes for called operators,
  //     and (2) record endpoints for streams, as seen in calls/assignments
  // - all info is written into PageDFGInfo* i

  PageDFGInfo *info = (PageDFGInfo*)i;

  switch (t->getKind())
  {
    case TREE_OPERATOR:	{ return true; }
    case TREE_STMT:	{
			  if (((Stmt*)t)->getStmtKind()==STMT_ASSIGN) {
			    Symbol *sym=((StmtAssign*)t)->getLValue()
							->getSymbol();
			    Expr *rhs=((StmtAssign*)t)->getRhs();
			    if (rhs->getExprKind()!=EXPR_CALL &&
				rhs->getExprKind()!=EXPR_BUILTIN) {
			      fatal(1,"buildPageDFG_map: do not know "
				      "what to do with right side of asst "+
				      t->toString()+", expecting call",
				      rhs->getToken());
			    }
			    rhs->map(buildPageDFG_map,NULL,i);
			    Operator *op=((ExprCall*)rhs)->getOp();
			    node opnode=info->nodemap[op];
			    info->srcnodemap[sym]=opnode;
			    // if (sym->isStream()) {  // - primary IO
			    //   node pionode=info->nodemap[sym];
			    //   info->dstnodemap[sym]=pionode;
			    // }
			    info->edgesyms.insert(sym);
			    return false;
			  }
			  else  // not STMT_ASSIGN
			    return true;
			}
    case TREE_EXPR:	{
			  if (((Expr*)t)->getExprKind()==EXPR_CALL ||
			      ((Expr*)t)->getExprKind()==EXPR_BUILTIN) {
			    // - create node
			    Operator *op=((ExprCall*)t)->getOp();
			    node opnode=(*info->dfg).new_node(op);
			    info->nodemap[op]=opnode;
			    // - walk over args to record edge endpoints
			    list<Expr*>   *args=((ExprCall*)t)->getArgs();
			    list<Symbol*> *formals=op->getArgs();
			    int pos=0;
			    Expr* e;
			    forall (e,*args) {
			      // formal:
			      Symbol *formalsym=formals->inf(
						formals->get_item(pos++));
			      if (!formalsym->isStream())
				continue;  // forall (e,*args)
			      StreamDir dir=((SymbolStream*)formalsym)->
								    getDir();
			      if (e->getExprKind()==EXPR_LVALUE) {
				// actual:
				Symbol *sym=((ExprLValue*)e)->getSymbol();
				if (dir==STREAM_IN)
				  info->dstnodemap[sym]=opnode;
				else  // (dir==STREAM_OUT)
				  info->srcnodemap[sym]=opnode;
				info->edgesyms.insert(sym);
			      }
			      else {
				fatal(1,"buildPageDFG_map: "
				        "do not know what to do with arg "+
				        e->toString()+" of call to op "+
				        op->getName()+", expecting stream ref",
				      e->getToken());
			      }
			    }
			  }
			  else  // not EXPR_CALL, EXPR_BUILTIN
			    assert(!"internal inconsistency");
			  return false;
			}
    default:		{
			  return false;
			}
  }
  assert(!"never get here");
}


void buildPageDFG (OperatorCompose *iop, PageDFG *dfg)
{
  // - return in *dfg a page-level data-flow-graph for a compose-op instance
  // - node = Operator* (page or built-in, e.g. segment op)
  //          or Symbol* for a primary I/O (formal arg) of compose-op
  // - arc  = Symbol* for a local or primary I/O stream of compose-op

  dfg->clear();
  PageDFGInfo info(dfg);
  Symbol *sym;

  // - create primary IO nodes for formal args of compose-op
  // - also record them as end-points of corresponding stream edge
  forall (sym,*iop->getArgs()) {
    if (sym->isStream()) {
      node symnode=(*info.dfg).new_node(sym);
      info.nodemap[sym]=symnode;
      if (((SymbolStream*)sym)->getDir()==STREAM_IN)
	info.srcnodemap[sym]=symnode;
      else  // (sym->getDir()==STREAM_OUT)
	info.dstnodemap[sym]=symnode;
      info.edgesyms.insert(sym);
    }
  }

  // - create rest of nodes + record endpoints:
  iop->map(buildPageDFG_map,NULL,&info);

  // - create edges
  forall (sym,info.edgesyms) {
    node src=info.srcnodemap[sym];
    node dst=info.dstnodemap[sym];
    if (src==NULL)
      fatal(1,"buildPageDFG: no src node for edge sym "+sym->getName()+
	      " in compos op "+iop->getName());
    if (dst==NULL)
      fatal(1,"buildPageDFG: no dst node for edge sym "+sym->getName()+
	      " in compos op "+iop->getName());
    // edge symedge =
    (*info.dfg).new_edge(src,dst,sym);
  }
}


////////////////////////////////////////////////////////////////
//  printPageDFG

string printPageDFG (PageDFG *dfg)
{
  // - print a page-level *dfg

  string res("PageDFG [%d nodes, %d edges] {\n",
	     (*dfg).number_of_nodes(),(*dfg).number_of_edges());
  node n;
  forall_nodes (n,*dfg) {
    Tree *t=(*dfg)[n];
    if (t->getKind()==TREE_OPERATOR)
      res += string("  node ") + "op  "
	  +  ((Operator*)t)->getName() + "\n";
    else if (t->getKind()==TREE_SYMBOL)
      res += string("  node ") + (((*dfg).indeg(n)==0) ? "in  " : "out ")
	  +  ((Symbol*)t)->getName() + "\n";
    else
      assert(!"internal inconsistency");
  }
  edge e;
  forall_edges (e,*dfg) {
    Symbol *sym=(*dfg)[e];
    res += "  edge " + sym->getName() + " : ";
    node  ns=(*dfg).source(e);
    node  nd=(*dfg).target(e);
    Tree *ts=(*dfg)[ns];
    Tree *td=(*dfg)[nd];
    res += ((ts->getKind()==TREE_OPERATOR) ? ((Operator*)ts)->getName() :
	    (ts->getKind()==TREE_SYMBOL  ) ? ((Symbol*)  ts)->getName() :
	    (assert(!"internal inconsistency"),string()));
    res += " --> ";
    res += ((td->getKind()==TREE_OPERATOR) ? ((Operator*)td)->getName() :
	    (td->getKind()==TREE_SYMBOL  ) ? ((Symbol*)  td)->getName() :
	    (assert(!"internal inconsistency"),string()));
    res += "\n";
  }
  res += "}\n";
  return res;
}
