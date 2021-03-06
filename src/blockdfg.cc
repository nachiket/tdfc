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
// SCORE TDF compiler:  DFG for TDF states / stmt-blocks
// $Revision: 1.5 $
//
//////////////////////////////////////////////////////////////////////////////


#include "blockdfg.h"
#include "state.h"
#include "operator.h"
#include "expr.h"
#include "stmt.h"
#include "canonical.h"
#include "dismantle.h"
#include "symbol.h"
#include <LEDA/core/list.h>
#include <LEDA/core/array.h>
#include <LEDA/core/h_array.h>
#include <LEDA/core/set.h>
#include <LEDA/core/map.h>
#include "ops.h"
#include<set>

using leda::list;
using leda::map;
using leda::node;
using leda::edge;
using leda::array;
using leda::graph;
using leda::node_array;
using leda::edge_array;
using leda::list_item;
using leda::h_array;

//using std::set;

#undef DEBUG

using std::cerr;
//using std::set;

extern bool gPageTiming1;		  // - debug mode, in "tdfc.cc"

extern Expr *EvaluateExpr (Expr *orig);	  // - const fold, in "bindvalues.cc"
Expr            *evalExpr (Expr *orig);	  // - fwd decls
Type            *evalType (Type *orig);


////////////////////////////////////////////////////////////////
//  Create BlockDFG

class BlockDfgInfo {
public:
  BlockDFG                 *dfg;		// dfg being built
  StateCase				   *sc;			// State case.. Added by Nachiket on 12/27/2009
  BlockDfgInfo			   *parent;			// Added by Nachiket on 11/4/2009 to allow searching higher up in the scope... Jeez I hate maps.
  h_array<Expr*,node>          *nodemap;		// map AST expr --> dfg node
  h_array<node, Symbol*>	*symbolmap;		// added by Nachiket on 12/14/2009 to allow recording symbol at a node !!!!!!!!!!!!!!!!! OBSOLETE !!!!!!!!!!!!!!
  h_array<Symbol*,StmtAssign*> *livedefs;		// map sym --> asst
  h_array<Symbol*,StmtAssign*> *initialdefs;		// 1/24/2010
  map<Symbol*,node>	   *extdefs;		// map sym --> PI node
  list<StmtAssign*>        *deaddefs;		// non live defs
  list<Stmt*>              *nondfstmts;		// residual stmts not in dfg
  set<SymbolVar*>	   *locals;		// locals declared in block
  SymTab				*vars;		// Added by Nachiket on 11/3/2009 to support access to state-machine variables..
  SymbolVar				*nextstate;	// Added by Nachiket on 12/27/2009 to support dataflow construction of next-state variables..
  
  BlockDfgInfo (BlockDFG                 *dfg_i,
		StateCase				 *sc_i,
		BlockDfgInfo			 *parent_i,
		h_array<Expr*,node>          *nodemap_i,
		h_array<node,Symbol*>          *symbolmap_i, // 12/14/2009 addition
		h_array<Symbol*,StmtAssign*> *livedefs_i,
		h_array<Symbol*,StmtAssign*> *initialdefs_i,
		map<Symbol*,node>        *extdefs_i,
		list<StmtAssign*>        *deaddefs_i,
		list<Stmt*>              *nondfstmts_i,
		set<SymbolVar*>          *locals_i,
		SymTab					 *vars_i,
		SymbolVar				 *nextstate_i)
    : dfg       (dfg_i        ? dfg_i        : new BlockDFG()),
      sc        (sc_i         ? sc_i        : NULL), // 12/27/2009
      parent	(parent_i), // top level will be NULL
      nodemap   (nodemap_i    ? nodemap_i    : new h_array<Expr*,node>),
      symbolmap   (symbolmap_i    ? symbolmap_i    : new h_array<node,Symbol*>),
      livedefs  (livedefs_i   ? livedefs_i   : new h_array<Symbol*,StmtAssign*>),
      initialdefs  (initialdefs_i   ? initialdefs_i   : new h_array<Symbol*,StmtAssign*>),
      extdefs   (extdefs_i    ? extdefs_i    : new map<Symbol*,node>),
      deaddefs  (deaddefs_i   ? deaddefs_i   : new list<StmtAssign*>),
      nondfstmts(nondfstmts_i ? nondfstmts_i : new list<Stmt*>),
      locals    (locals_i     ? locals_i     : new set<SymbolVar*>)	,
	  vars      (vars_i       ? vars_i       : new SymTab(SYMTAB_OP)),
	  nextstate (nextstate_i   ? nextstate_i   : NULL)	{}
};

using std::cout;
using std::endl;

void match_fanin0_fanout0_nodes(BlockDfgInfo* dfgi, set<node>* n1_fanin0_set, set<node>* n0_fanout0_set, int n1_n2);

node createBlockDfg_for_expr (Expr *e, BlockDfgInfo *dfgi, node uses_e, int edge_index=0)
{
  // - create DFG cone for Expr *e
  // - recurses to create + connect nodes for all subexpressions;
  // - create DFG edge from cone's root node to node uses_e (if uses_e != nil)
  // - Nachiket 14/1/2012: Added edge_index to enable revisiting edge order of creation.. 
  // - if e is symbol ref and symbol has a live def in this block,
  //     then annotate edge with StmtAssign* for symbol's live def,
  //     otherwise annotate edge with NULL
  // - update DFG and maps in *dfgi
  // - return cone's root node
  
  // warn("createBlockDfg_for_expr ("+e->toString()+")");

//  cout << "Nachiket detected node=" << e->toString() << endl;
  switch (e->getExprKind()) {
    case EXPR_VALUE:	{

//    cout << "Nachiket detected EXPR_VALUE node=" << e->toString() << endl;
			  node n=(*dfgi->dfg).new_node(e);
			  (*dfgi->nodemap)[e]=n;			  
			  if (uses_e)
			    (*dfgi->dfg).new_edge(n,uses_e,edge_index);
//			    cout << "VAL:" << n << endl;
			  return n;
			}
    case EXPR_LVALUE:	{
//    cout << "Nachiket detected EXPR_LVALUE node=" << e->toString() << endl;
    			  // - if is stream ref w/possible retime,
    			  //     then there is no live def
			  ExprLValue *lval=(ExprLValue*)e;
			  Symbol *sym=lval->getSymbol();
			  node           n=NULL;
			  StmtAssign *asst=NULL;
			  if ((*dfgi->livedefs).defined(sym) &&
			      (*dfgi->livedefs)[sym]           ) {
			    // - symbol has live def; find it
			    // StmtAssign *asst=...
			    asst=(*dfgi->livedefs)[sym];
			    Expr *rval=asst->getRhs();
			    node nrhs=asst->getRhsnode();
			    // node n=...
			    if(rval!=NULL) {
//			    	cout << "Found rval " << rval << " for " << e->toString() << endl;
			    	n=(*dfgi->nodemap)[rval];
				(*dfgi->symbolmap)[n]=sym; // recording symbols 12/14/2009
//				cout << "------------------Added symbol=" << sym << " for node=" << n << endl;
			    	if(n==NULL) {
			    		cout << "Why is the assignment NULL? lval=" << lval << " assignment=" << asst  << endl;
			    		//exit(-1);
			    	}
			    } else if(nrhs!=NULL){
			    	// Added by Nachiket on 11/3/2009
			    	n=nrhs;
//			    	cout << "Found rhs=" << n  << " " << nrhs << " for " << e->toString() << endl;
			    	if(!(*dfgi->dfg).member(n)) {
			    		cout << n << " is not member of graph dfg=" << *dfgi->dfg << endl;
			    		cout << "" << endl;
			    		//exit(-1);
			    	}
			    }

			    //cout << "Nachiket detected EXPR_LVALUE node=" << e->toString() << " for symbol" << sym << endl;
			    if(n==0u) {
			    	Symbol* symtest;
			    	forall (symtest,*((*dfgi->vars).getSymbolOrder())) {
			    		cout << "Testing.." << symtest << endl;
			    	}
			    }
			    assert(n);
			  }
			  else if ((*dfgi->extdefs).defined(sym)) {
			    // - symbol has PI node (external def)
			    //     and no local live def; reuse PI node
			    // node n=
			    n=(*dfgi->extdefs)[sym];
			    // (*dfgi->nodemap)[e]=n;	// - moved below
			    assert(n);
			  }
			  else {
//			  cout << "No PI node for " << e->toString() << endl;
			    // - symbol has no live/ext def; create PI node
			    // node n=...
			    n=(*dfgi->dfg).new_node(e);
			    // (*dfgi->nodemap)[e]=n;	// - moved below
			    (*dfgi->extdefs)[sym]=n;
			  }			  
			  if (lval->usesAllBits()) {
			    (*dfgi->nodemap)[e]=n;
//			    cout << "NEWLVAL:" << n << endl;
			    if (uses_e)
			      (*dfgi->dfg).new_edge(n,uses_e,edge_index); //asst
			    return n;
			  }
			  else {
			    // - create bit-select node
			    // - edge n-->n_bitsel is annotated with asst
			    // - edge n_bitsel-->uses_e is NOT
			    Expr *posLow =lval->getPosLow();
			    Expr *posHigh=lval->getPosHigh();
			    if ((posLow && posLow->getExprKind()!=EXPR_VALUE)||
				          posHigh->getExprKind()!=EXPR_VALUE)
			      fatal(-1,"createBlockDfg_for_expr: "
				       "bit subscript must be constant "
				       "during page timing", e->getToken());
			    int lo = ((ExprValue*)posLow)->getIntVal();
			    int hi = posHigh ?
				     ((ExprValue*)posHigh)->getIntVal() : -1;
			    ExprBitSel *e_bitsel = new ExprBitSel
						    (lval->getToken(),lo,hi);
			    node n_bitsel=(*dfgi->dfg).new_node(e_bitsel);
                            (*dfgi->nodemap)[e]       =n_bitsel;
                            (*dfgi->nodemap)[e_bitsel]=n_bitsel;
			    // edge ed =
			    (*dfgi->dfg).new_edge(n,n_bitsel,edge_index); //asst
			    if (uses_e)
			      (*dfgi->dfg).new_edge(n_bitsel,uses_e,edge_index); // NULL
			    return n_bitsel;
			  }
			}
    case EXPR_UOP:	{
			  node n=(*dfgi->dfg).new_node(e);
			  (*dfgi->nodemap)[e]=n;
			  Expr *e1=((ExprUop*)e)->getExpr();
			  createBlockDfg_for_expr(e1,dfgi,n,0);
			  if (uses_e)
			    (*dfgi->dfg).new_edge(n,uses_e,edge_index);
			  return n;
			}
    case EXPR_BOP:	{
    			cout << "Found binary operator=" << e->toString() << " of type=" << opToString(((ExprBop*)e)->getOp())  << endl;
    			cout << "Expr1=" << ((ExprBop*)e)->getExpr1()->toString()  << endl;
    			cout << "Expr2=" << ((ExprBop*)e)->getExpr2()->toString()  << endl;
			  node n=(*dfgi->dfg).new_node(e);
			  (*dfgi->nodemap)[e]=n;
			  Expr *e1=((ExprBop*)e)->getExpr1();
			  Expr *e2=((ExprBop*)e)->getExpr2();
			  createBlockDfg_for_expr(e1,dfgi,n,0);
			  createBlockDfg_for_expr(e2,dfgi,n,1);
//			  cout << "BOP: " << n << endl;
			  if (uses_e)
			    (*dfgi->dfg).new_edge(n,uses_e,edge_index);
			  return n;
			}
    case EXPR_COND:	{
//    			cout << "Found EXPR_COND" << endl;
			  node n=(*dfgi->dfg).new_node(e);
			  (*dfgi->nodemap)[e]=n;
			  Expr *ec=((ExprCond*)e)->getCond();
			  Expr *et=((ExprCond*)e)->getThenPart();
			  Expr *ef=((ExprCond*)e)->getElsePart();
			  createBlockDfg_for_expr(ec,dfgi,n,0);
			  createBlockDfg_for_expr(et,dfgi,n,1);
			  createBlockDfg_for_expr(ef,dfgi,n,2);
			  if (uses_e)
			    (*dfgi->dfg).new_edge(n,uses_e,edge_index);
			  return n;
			}
    case EXPR_CAST:	{
			  node n=(*dfgi->dfg).new_node(e);
			  (*dfgi->nodemap)[e]=n;
			  Expr *ev=((ExprCast*)e)->getExpr();
			  createBlockDfg_for_expr(ev,dfgi,n,0);
			  if (uses_e)
			    (*dfgi->dfg).new_edge(n,uses_e,edge_index);
			  return n;
			}
    case EXPR_BUILTIN:  {
			  // - cat(), bitsof()
			  node n=(*dfgi->dfg).new_node(e);
			  (*dfgi->nodemap)[e]=n;
			  Expr *ea;
			  int eid=0;
			  forall (ea,*((ExprBuiltin*)e)->getArgs()) {
			    createBlockDfg_for_expr(ea,dfgi,n,eid);
			    eid++;
			  }
			  if (uses_e)
			    (*dfgi->dfg).new_edge(n,uses_e,edge_index);
			  return n;
			}
    default:		{
			  fatal(1,"unknown expression kind in dfg creation",
				e->getToken());
			  return NULL;
			}
  }
}


void deleteBlockDfgCone (BlockDFG *dfg, node n)
{
  // - delete BlockDFG subtree rooted at n, removing fanout-free ancestors
  // - this does dead-code removal when n is dead
  // - n must have no fanout
  // - visit nodes in reverse topological order from n,
  //     removing each node, and going on to nodes with no remaining fanout

  warn("Before deleting: "+printBlockDFG("",0,0,dfg));

  assert((*dfg).outdeg(n)==0);

  node_array<int> numUnvisitedFanouts(*dfg,-1);
  node u;
  forall_nodes (u,*dfg)
    numUnvisitedFanouts[u]=(*dfg).outdeg(u);

  set<node> frontier;
  frontier.insert(n);
  while (!frontier.empty()) {
    node v=frontier.choose();
    frontier.del(v);
    // - frontier invariant:  numUnvisitedFanouts[v]==0
    list<edge> dfg_in_edges_v = (*dfg).in_edges(v);	// - will delete these
    // warn("deleteBlockDfgCone: removing node "+(*dfg)[v]->toString());
    (*dfg).del_node(v);
    edge e;
    forall (e,dfg_in_edges_v) {
      node src=(*dfg).source(e);
      assert(src!=v);
      if (--numUnvisitedFanouts[src]==0)
	frontier.insert(src);
    }
  }

  // warn("After deleting: "+printBlockDFG(dfg));
}

void recursiveFaninDelete(BlockDfgInfo *dfgi, node dead_po) {
	edge edel;
	list<edge> fanin = (*dfgi->dfg).in_edges(dead_po);
	forall(edel, fanin) {
		node src=(*dfgi->dfg).source(edel);
		//    	cout <<  "--Deleting..." << ((Tree *)((*dfgi->dfg)[src]))->toString()+" which is input to dead_po" << endl;
		(*dfgi->dfg).del_edge(edel);
		if((*dfgi->dfg).outdeg(src)==0) {
			recursiveFaninDelete(dfgi, src);
			(*dfgi->dfg).del_node(src);
		}
	}
}

//-------------------------------------------------------------
// Added by Nachiket on 2/2/2010...
//-------------------------------------------------------------
void recursiveFaninDuplicate(BlockDFG *olddfg, BlockDFG *newdfg, node n, node nnew, bool force) {

	Tree* t=(*olddfg)[n];
	
	// detect condition when the IF statement has both branches... 
	// in this case, there is no need to conditionally write to output. condition=true
	// and duplication is not being forced..
	if(!force && t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND && (olddfg->in_edges(n)).size()==3) {
		//cout << "EXPR_COND has 3 inputs.. output will always be driven!" << endl;
		ExprValue* trueVal=new ExprValue(NULL,new Type(TYPE_BOOL),1,0);
		node srcnew=(*newdfg).new_node(trueVal);

		(*newdfg).new_edge(srcnew, nnew, 0);
		(*newdfg).new_edge(srcnew, nnew, 1);
		(*newdfg).new_edge(srcnew, nnew, 2);
		return;
	}

//	bool duplicate=true;
	bool addfalse=!force && (t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND) && olddfg->indeg(n)==2;

	edge e;
	list<edge> fanin = (*olddfg).in_edges(n);
	int edge_count=0;
	forall(e, fanin) {
		node src=(*olddfg).source(e);
		Tree* tsrc=(*olddfg)[src];

		if(force && t->getKind()==TREE_EXPR) {
			Tree* tsrcnew=tsrc->duplicate();
			node srcnew=(*newdfg).new_node((Expr*)tsrcnew);

			(*newdfg).new_edge(srcnew, nnew, NULL);
			recursiveFaninDuplicate(olddfg, newdfg, src, srcnew, true);
		} else if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND && fanin.size()==3) {
			//cout << "EXPR_COND has 3 inputs.. output will always be driven!" << endl;
			ExprValue* trueVal=new ExprValue(NULL,new Type(TYPE_BOOL),1,0);
			node srcnew=(*newdfg).new_node(trueVal);

			(*newdfg).new_edge(srcnew, nnew, NULL);
			break;
		} else if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND && edge_count==0) {
			Tree* tsrcnew=tsrc->duplicate();
			((Expr*)tsrcnew)->setType(new Type(TYPE_BOOL));
			node srcnew=(*newdfg).new_node((Expr*)tsrcnew);

			(*newdfg).new_edge(srcnew, nnew, NULL);
			recursiveFaninDuplicate(olddfg, newdfg, src, srcnew, true);
		} else if(t->getKind()==TREE_EXPR && ((Expr*)tsrc)->getExprKind()==EXPR_COND) {
			Tree* tsrcnew=tsrc->duplicate();
			((Expr*)tsrcnew)->setType(new Type(TYPE_BOOL));
			node srcnew=(*newdfg).new_node((Expr*)tsrcnew);

			(*newdfg).new_edge(srcnew, nnew, NULL);
			recursiveFaninDuplicate(olddfg, newdfg, src, srcnew, false);	
		} else if(t->getKind()==TREE_EXPR && ((Expr*)tsrc)->getExprKind()!=EXPR_COND) {
			ExprValue* trueVal=new ExprValue(NULL,new Type(TYPE_BOOL),1,0);
			node srcnew=(*newdfg).new_node(trueVal);

			(*newdfg).new_edge(srcnew, nnew, NULL);
		}

		edge_count++;
	}

	if(addfalse) {
		ExprValue* falseVal=new ExprValue(NULL,new Type(TYPE_BOOL),0,0);
		node srcnew=(*newdfg).new_node(falseVal);
		(*newdfg).new_edge(srcnew, nnew, NULL);
	}
}

bool createBlockDfg_map (Tree *t, void *i)
{
  // - build DFG from stmt tree t
  // - i is BlockDfgInfo*, contains DFG and maps which are built during map
  
  // warn("createBlockDfg_map ("+t->toString()+")");
  
  BlockDfgInfo *dfgi=(BlockDfgInfo*)i;

  //cout << "Nachiket is processing statement\n" << t->toString() << endl;
  if (t->getKind()==TREE_STMT) {
    switch (((Stmt*)t)->getStmtKind()) {
      case STMT_ASSIGN:	{
			  Expr       *rhs=((StmtAssign*)t)->getRhs();
			  ExprLValue *lhs=((StmtAssign*)t)->getLValue();
			  Symbol     *sym=lhs->getSymbol();
//			  cout << "Detected string=" << sym->toString() << endl;
			  assert(lhs->usesAllBits());	// - ignoring bit asst
			  // - create RHS tree and connect to a new PO node
			  node po=(*dfgi->dfg).new_node(lhs);
			  (*dfgi->nodemap)[lhs]=po;
			  createBlockDfg_for_expr(rhs,dfgi,po);
			  (*dfgi->symbolmap)[po]=sym; // recording symbols 12/14/2009
//			  cout << "------------------Added STMTASSIGN symbol=" << sym->toString() << "[" << sym  << "]" << " for node=" << po << "->" << (*dfgi->livedefs).defined(sym) << endl;

			  // - mark any old PO node as dead
			  //     (but do not remove it yet!  temporarily,
			  //      there may be multiple PO nodes for same var)
			  if ((*dfgi->livedefs).defined(sym) &&
					  (*dfgi->livedefs)[sym]           ) {
				  StmtAssign *dead_asst=(*dfgi->livedefs)[sym];
				  (*dfgi->deaddefs).append(dead_asst);
				  ExprLValue *dead_lval=dead_asst->getLValue();
				  node        dead_po  =(*dfgi->nodemap)[dead_lval];
				  list<edge> fanout = (*dfgi->dfg).out_edges(dead_po);
				  edge edel;
				  forall(edel, fanout) {
					  (*dfgi->dfg).del_edge(edel);
				  }
				  recursiveFaninDelete(dfgi, dead_po);
				  (*dfgi->dfg).del_node(dead_po);

				  (*dfgi->nodemap)[dead_lval]=NULL; //used by fanout?
				  //(*dfgi->nodemap)[rhs]=
				  (*dfgi->livedefs)[sym]=NULL;
//  			          cout << "------------ Assignment is being overriden.. for " << t->toString() << endl;
				  // WARNING: leaves dangling ptrs in nodemap
			  }

			  // - record live asst
			  (*dfgi->livedefs)[sym]=(StmtAssign*)t;
			  return false;
			}
      case STMT_BLOCK:	{
    	  //cout << "We've already touched the symtab for this stmtblock!" << endl;
			  Symbol *sym;
			  forall (sym,*((StmtBlock*)t)->getSymtab()
						      ->getSymbolOrder()) {
			    assert(sym->getSymKind()==SYMBOL_VAR);
			    (*dfgi->locals).insert((SymbolVar*)sym);
			  }
			  return true;
			}
      case STMT_BUILTIN:{
			  // - close(), done()
//			  cout << "Baaad" << endl;
			
			  // 2/1/2010... Need to support generation of frame-close tokens!!!
			  StmtBuiltin     *bstmt=(StmtBuiltin *)t;
			  ExprBuiltin     *bexpr=bstmt->getBuiltin();
			  list<Expr*>     *args =bexpr->getArgs();
			  Expr            *first=args->empty()?NULL:args->head();
			  Operator        *op   =bexpr->getOp();
			  OperatorBuiltin *bop  =(OperatorBuiltin *)op;

			  if (bop->getBuiltinKind()==BUILTIN_CLOSE || bop->getBuiltinKind()==BUILTIN_FRAMECLOSE) {

/* 2/25/2010: Proper processing of EOFR requires some more thought...			  	
				  ExprLValue *lexpr=(ExprLValue *)first;
				  Symbol     *sym=lexpr->getSymbol();
				  bexpr->setSymbol(sym); // store symbol and propagate..
				  node po=(*dfgi->dfg).new_node(bexpr);
				  (*dfgi->nodemap)[bexpr]=po;
				  (*dfgi->symbolmap)[po]=sym;
*/

/*			  
				  ExprLValue *lexpr=(ExprLValue *)first;
				  cout << "--lexpr=" << lexpr->toString() << endl;

				  //Expr       *rhs=((StmtAssign*)t)->getRhs();
				  Symbol     *sym=lexpr->getSymbol();
				  node po=(*dfgi->dfg).new_node(lexpr);
				  (*dfgi->nodemap)[lexpr]=po;
				  (*dfgi->symbolmap)[po]=sym;

				  if(bop->getBuiltinKind()==BUILTIN_CLOSE) {
					  ExprValue* framecloseVal = new ExprValue(NULL, "frameclose");
					  node frameclosenode = (*dfgi->dfg).new_node(framecloseVal);
					  (*dfgi->nodemap)[framecloseVal]=frameclosenode;

					  (*dfgi->dfg).new_edge(frameclosenode, po);

				  } else if(bop->getBuiltinKind()==BUILTIN_FRAMECLOSE) {
					  ExprValue* closeVal = new ExprValue(NULL, "close");
					  node closenode = (*dfgi->dfg).new_node(closeVal);
					  (*dfgi->nodemap)[closeVal]=closenode;

					  (*dfgi->dfg).new_edge(closenode, po);
				  }
*/

/* 2/25/2010: Proper processing of EOFR requires some more thought...			  	
				  if ((*dfgi->livedefs).defined(sym) && (*dfgi->livedefs)[sym]) {
					  StmtAssign *dead_asst=(*dfgi->livedefs)[sym];
					  (*dfgi->deaddefs).append(dead_asst);
					  ExprLValue *dead_lval=dead_asst->getLValue();
					  node        dead_po  =(*dfgi->nodemap)[dead_lval];
					  list<edge> fanout = (*dfgi->dfg).out_edges(dead_po);
					  edge edel;
					  forall(edel, fanout) {
						  (*dfgi->dfg).del_edge(edel);
					  }
					  recursiveFaninDelete(dfgi, dead_po);
					  (*dfgi->dfg).del_node(dead_po);

					  (*dfgi->nodemap)[dead_lval]=NULL; //used by fanout?
					  (*dfgi->livedefs)[sym]=NULL;
				  }

				  // - record live asst
				  StmtAssign* newAssign = new StmtAssign(NULL, lexpr, po); // lhs=lval, rhs=conditionnode
				  (*dfgi->livedefs)[sym]=newAssign;
*/				  
			  }

			  return false;
			}
      case STMT_GOTO:	{
			  // (*dfgi->nondfstmts).append((Stmt*)t);
			  // Updated on 12/27/2009...

			  //Symbol* sym = (*dfgi->symbolmap)[dfgi->nextstate];
			  SymbolVar* sym = dfgi->nextstate;
			  ExprLValue* stateVal = new ExprLValue(NULL, dfgi->nextstate);
			  node statenode = (*dfgi->dfg).new_node(stateVal);
			  (*dfgi->nodemap)[stateVal] = statenode;
			  (*dfgi->symbolmap)[statenode] = sym;

			  State* gotoState = ((StmtGoto*)t)->getState();
			  ExprValue* gotoStateVal = new ExprValue(NULL, gotoState->getName());
			  node gotonode = (*dfgi->dfg).new_node(gotoStateVal);
			  (*dfgi->nodemap)[gotoStateVal]=gotonode;
			  (*dfgi->dfg).new_edge(gotonode, statenode, NULL);

//			  cout << "GOTO Processing " << sym->toString() << " going to state=" << gotoState->getName() << endl;

			  // override previous definition
			  if ((*dfgi->livedefs).defined(sym) &&
			      (*dfgi->livedefs)[sym]           ) {
			    StmtAssign *dead_asst=(*dfgi->livedefs)[sym];
			    (*dfgi->deaddefs).append(dead_asst);
			    ExprLValue *dead_lval=dead_asst->getLValue();
			    node        dead_po  =(*dfgi->nodemap)[dead_lval];
			    list<edge> fanout = (*dfgi->dfg).out_edges(dead_po);
			    edge edel;
			    forall(edel, fanout) {
			    	(*dfgi->dfg).del_edge(edel);
			    }
			    recursiveFaninDelete(dfgi, dead_po);
			    (*dfgi->dfg).del_node(dead_po);

			    (*dfgi->nodemap)[dead_lval]=NULL; //used by fanout?
			    (*dfgi->livedefs)[sym]=NULL;
#ifdef DEBUG			    
			     cout << "GOTO Processing Overriding=" << t->toString() << endl;
#endif			     
			  }

			  StmtAssign* t1=new StmtAssign(NULL, stateVal, gotonode);
			  (*dfgi->livedefs)[sym]=t1;

			  return false;
			}
      case STMT_IF:	{

//			  cout << "--Dealing with an IF statement " << &t << " in DFG " << dfgi << endl;
			  //cout << ((StmtIf*)t)->toString() << endl;

			  Expr *ec=((StmtIf*)t)->getCond();

			  //createBlockDfg_for_expr(ec,dfgi,n);
			  Stmt *thenPart=((StmtIf*)t)->getThenPart();
			  Stmt *elsePart=((StmtIf*)t)->getElsePart();

			  BlockDFG dfgThen; dfgThen.clear();
			  BlockDFG dfgElse; dfgElse.clear();
			  list<Stmt*> stmtsThen;
			  list<Stmt*> stmtsElse;
			  list<Stmt*> nondfstmtsThen;
			  list<Stmt*> nondfstmtsElse;
			  set<SymbolVar*> localsThen;
			  set<SymbolVar*> localsElse;
			  set<SymbolVar*> new_localsThen;
			  set<SymbolVar*> new_localsElse;

			  h_array<Expr*,node>          nodemapThen;
			  h_array<Symbol*,StmtAssign*> livedefsThen(NULL);
			  h_array<Symbol*,StmtAssign*> initialdefsThen(NULL);
			  map<Symbol*,node>        extdefsThen;
			  h_array<node, Symbol*>	   symbolmapThen;
			  list<StmtAssign*>        deaddefsThen;
			  BlockDfgInfo dfgtheni(&dfgThen,dfgi->sc, dfgi,&nodemapThen,&symbolmapThen,&livedefsThen,&initialdefsThen,&extdefsThen,&deaddefsThen, &nondfstmtsThen,&localsThen, dfgi->vars, dfgi->nextstate);

#ifdef DEBUG						  
			  cout << "======== Generating THEN part of the DFG " << &dfgtheni << endl;
#endif			  
			  initialize_dfginfo(&dfgtheni);
			  thenPart->map(createBlockDfg_map,(TreeMap)NULL,&dfgtheni);
#ifdef DEBUG						  
			  cout << "======== Finishing " << &dfgtheni << endl;
#endif			  
  			  finalize_dfginfo(&dfgtheni);

//			  cout << "Printing THEN part of the DFG " << &dfgtheni << endl;
//			  cout << printBlockDFG(&dfgThen) << endl;

			  if(elsePart!=NULL) {
				  h_array<Expr*,node>          nodemapElse;
				  h_array<Symbol*,StmtAssign*> livedefsElse(NULL);
				  h_array<Symbol*,StmtAssign*> initialdefsElse(NULL);
				  map<Symbol*,node>        extdefsElse;
			  	  h_array<node, Symbol*>	   symbolmapElse;
				  list<StmtAssign*>        deaddefsElse;
				  BlockDfgInfo dfgelsei(&dfgElse,dfgi->sc,dfgi,&nodemapElse,&symbolmapElse,&livedefsElse,&initialdefsElse,&extdefsElse,&deaddefsElse, &nondfstmtsElse,&localsElse, dfgi->vars, dfgi->nextstate);
#ifdef DEBUG						  
				  cout << "====== Generating ELSE part of the DFG " << &dfgelsei << endl;
#endif				  
  				  initialize_dfginfo(&dfgelsei);
				  elsePart->map(createBlockDfg_map,(TreeMap)NULL,&dfgelsei);
#ifdef DEBUG						  
			  	  cout << "======== Finishing " << &dfgelsei << endl;
#endif				  

 				  finalize_dfginfo(&dfgelsei);

//				  cout << "Printing ELSE part of the DFG " << &dfgelsei << endl;
//				  cout << printBlockDFG(&dfgElse) << endl;
			  } else {
//				  cout << "--Skipping ELSE part of the DFG" << endl;
			  }

			  // first identify fanout=0 nodes from all parent dfg..
			  node n0;
			  set<node> n0_set;
			  forall_nodes(n0, (*dfgi->dfg)) {
//			  	cout << "Node=" << n0 << " count=" << (*dfgi->dfg).number_of_nodes() << endl;
				  if((*dfgi->dfg).outdeg(n0)==0 && !n0_set.member(n0)) { // set property should enforce !member anyway.. skip?
					  n0_set.insert(n0);
#ifdef DEBUG					  
					  Tree *t=(*dfgi->dfg)[n0];
					  cout << "--n0=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol()->getName() << " for DFG=" << dfgi << endl;
#endif					  
				  }
			  }

			  // print a list of PO nodes from both then/else parts..
#ifdef DEBUG						  
			  cout << "===N1===" << endl;
#endif			  
			  node n1;
			  set<node> n1_set;
			  set<node> n1_fanin0_set;
			  forall_nodes(n1, dfgThen) {
				  Tree *t=(dfgThen)[n1];
//				  cout << "crappy n1 " << t->toString().replace_all("\n","") << endl; //scope is a PROBLEM in a NESTED if!
				  if(dfgThen.outdeg(n1)==0) {				  	
					  n1_set.insert(n1);
#ifdef DEBUG						  
                                          node input_node = ((*dfgi->dfg).source((*dfgi->dfg).first_in_edge(n1)));
					  cout << "--fanout=0 n1=" << t->toString().replace_all("\n","") << "[" << t << "]"<< " symbol=" << ((ExprLValue*)t)->getSymbol() << " for input=" << input_node << endl;
#endif					  
				  }

				  if(dfgThen.indeg(n1)==0) {
					  n1_fanin0_set.insert(n1);
#ifdef DEBUG						  
                                          node output_node = ((*dfgi->dfg).target((*dfgi->dfg).first_out_edge(n1)));
					  cout << "--fanin=0 n1=" << t->toString().replace_all("\n","") << "[" << t << "]" << " symbol=" << ((ExprLValue*)t)->getSymbol() << " for output=" << output_node << endl;
#endif					  
				  }
			  }
#ifdef DEBUG						  
			  cout << "===N1===" << endl;
#endif

//			  cout << "dfgThen numnodes=" << dfgThen.number_of_nodes() << endl;
//			  cout << "dfgElse numnodes=" << dfgElse.number_of_nodes() << endl;
			  if(elsePart!=NULL && dfgElse.number_of_nodes()!=0) {
				  node n2;
				  set<node> n2_set;
				  set<node> n2_fanin0_set;
				  set<node> n1_n2_set;
				  set<node> only_n2_set;
				  set<node> only_n1_set;
				  forall_nodes(n2, dfgElse) {
					  //cout << "crappy n2 "<< endl;
					  if(dfgElse.outdeg(n2)==0) {
						  n2_set.insert(n2);

						  // if this also belongs to n1, then add to unified list..
						  node n1_check;
						  bool matched=false;
						  forall(n1_check, n1_set) {
							  Symbol* n1_symbol=((ExprLValue*)(dfgThen)[n1_check])->getSymbol();
							  Symbol* n2_symbol=((ExprLValue*)(dfgElse)[n2])->getSymbol();
							  if(n1_symbol==n2_symbol) {
								  matched=true;
								  n1_n2_set.insert(n2);
								  Tree *t=(dfgElse)[n2];
#ifdef DEBUG						  
								  cout << "--n1 and n2=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol()->getName() << "for DFG=" << dfgi << endl;
#endif								  
							  }
						  }

						  if(!matched) {
								  only_n2_set.insert(n2);
								  Tree *t=(dfgElse)[n2];
#ifdef DEBUG						  
								  cout << "--ONLY n2=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol() << "for DFG=" << dfgi << endl;
#endif								  

						  }
#ifdef DEBUG						  
						  Tree *t=(dfgElse)[n2];
						  cout << "--n2=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol() << "for DFG=" << dfgi << endl;
#endif						  
					  }

					  if(dfgElse.indeg(n2)==0) {

						  n2_fanin0_set.insert(n2);
#ifdef DEBUG						  
						  Tree *t=(dfgElse)[n2];
						  cout << "--fanin0 n2=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol() << "for DFG=" << dfgi << endl;
#endif						  

					  }
				  }

				  // check if n1_n2 nodes also lie in n0
				  // TODO: condition node can still use the n0's portion of the graph! yikes!
				  node n0_check;
				  set<node> n0_blacklist;
				  forall(n0_check, n0_set) {
					  node n1_n2;
					  forall(n1_n2, n1_n2_set) {
						  Symbol* n0_symbol=((ExprLValue*)(*dfgi->dfg)[n0_check])->getSymbol();
						  Symbol* n1_n2_symbol=((ExprLValue*)(dfgElse)[n1_n2])->getSymbol();
						  if(n0_symbol==n1_n2_symbol) {
							  // n0 will be overshadowed... get rid of it!
							  n0_blacklist.insert(n0_check);
#ifdef DEBUG						  
							  Tree *t=(dfgElse)[n1_n2];
							  cout << "--blacklisting n0=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol() << "for DFG=" << dfgi << endl;
#endif							  
							  //recursiveFaninDelete(dfgi, n0_check);
						  }
					  }
				  }

				  // find n1-only stragglers..
				  forall_nodes(n1, dfgThen) {
					  if(dfgThen.outdeg(n1)==0) {
						  node n2_check;
						  bool matched=false;
						  forall(n2_check, n2_set) {
							  Symbol* n1_symbol=((ExprLValue*)(dfgThen)[n1])->getSymbol();
							  Symbol* n2_symbol=((ExprLValue*)(dfgElse)[n2_check])->getSymbol();
							  if(n1_symbol==n2_symbol) {
								  matched=true;
							  }
						  }

						  if(!matched) {
							  only_n1_set.insert(n1);
#ifdef DEBUG						  
							  Tree *t=(dfgThen)[n1];
							  cout << "--ONLY n1=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol() << "for DFG=" << dfgi << endl;
#endif							  

						  }
					  }
				  }

				  // Handle all primary inputs of the smaller graph and connect them to the larger graph..
				  node fanout0_node;
				  set<node> n0_fanout0_set;
				  forall_nodes(fanout0_node, (*dfgi->dfg)) {
					  if((*dfgi->dfg).outdeg(fanout0_node)==0) {
						  n0_fanout0_set.insert(fanout0_node);
#ifdef DEBUG						  
						  Tree *t=(*dfgi->dfg)[fanout0_node];
						  cout << "--fanout0 n0=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol() << "for DFG=" << dfgi << endl;
#endif						  
					  }
				  }




				  // Create the condition node prior to merging! FTW?
				  node tempnode = (*dfgi->dfg).new_node(ec);
				  createBlockDfg_for_expr(ec,dfgi,tempnode);

				  edge conditionedge = (*dfgi->dfg).first_in_edge(tempnode); // extract the condition node...
				  node conditionnode = (*dfgi->dfg).source(conditionedge);
				  (*dfgi->dfg).del_edge(conditionedge);
				  (*dfgi->dfg).del_node(tempnode);

				  // 2/12/2010: Need to use the inverted condition node for cases where THEN-part is missing. weirdoo
//				  node invertconditionnode=conditionnode;
				  node invertconditionnode=NULL;
				  if(only_n1_set.size()>0) {
				  	ExprUop* invertexpr = new ExprUop(ec->getToken(), '!', ec);
					Type* type = ec->typeCheck();
					invertexpr->setType(type);
				  	node invertnode = (*dfgi->dfg).new_node(invertexpr);
				  	createBlockDfg_for_expr(invertexpr,dfgi,invertnode);

				  	edge invertconditionedge = (*dfgi->dfg).first_in_edge(invertnode); // extract the inverted condition node...
				  	invertconditionnode = (*dfgi->dfg).source(invertconditionedge);
				  	(*dfgi->dfg).del_edge(invertconditionedge);
				  	(*dfgi->dfg).del_node(invertnode);
				  }

				  // Merge the DFGs and then just do cleanup...
				  (*dfgi->dfg).join(dfgThen);
				  (*dfgi->dfg).join(dfgElse);


				  // Find inputs in merged set that correspond to fanout-0 nodes of n0
				  match_fanin0_fanout0_nodes(dfgi, &n1_fanin0_set, &n0_fanout0_set, 0);
				  match_fanin0_fanout0_nodes(dfgi, &n2_fanin0_set, &n0_fanout0_set, 1);

				  //
				  // eliminate blacklisted nodes now..
				  node n0_blacklist_node;
				  forall(n0_blacklist_node, n0_blacklist) {
					  node input_node = ((*dfgi->dfg).source((*dfgi->dfg).first_in_edge(n0_blacklist_node)));
					  if((*dfgi->dfg).outdeg(input_node)==1) {

#ifdef DEBUG						  
					  	cout << "blacklisted detect: " << n0_blacklist_node << endl;
#endif						
						  recursiveFaninDelete(dfgi, n0_blacklist_node);
					  } else {
					  	(*dfgi->dfg).del_node(n0_blacklist_node);
					  }
				  }


				  // Condition 1: THEN and ELSE halves both contain variable assignments... assume parent does not contain the assignment!
				  node n1_n2_node;
				  forall(n1_n2_node, n1_n2_set) {
					  // create a new node for each element of the set...
					  Symbol* n1_n2_sym = ((ExprLValue*)(dfgThen)[n1_n2_node])->getSymbol();
#ifdef DEBUG						  
					  cout << " n1_n2 Processing" << n1_n2_sym->getName() << endl;
#endif					  
					  // TODO: This should check if node already exists..
//					  if(!(*dfgi->livedefs).defined(n1_n2_sym)) {
					  	ExprLValue* n1_n2_val = new ExprLValue(NULL, n1_n2_sym);
						node n=(*dfgi->dfg).new_node(n1_n2_val);
						(*dfgi->nodemap)[n1_n2_val]=n;
						(*dfgi->symbolmap)[n]=n1_n2_sym; // recording symbols 12/14/2009
					  	importDfg(dfgi->dfg, &dfgThen, n, n1_n2_val, ec, n1_n2_sym, dfgi, (Stmt*)t, &conditionnode);
//					  } else {
//					  	StmtAssign* stmt_defined=(*dfgi->livedefs)[n1_n2_sym];
//					  	ExprLValue* val_defined=stmt_defined->getLValue();
//						node n=stmt_defined->getRhsnode();
//						Expr* val=stmt_defined->getRhs();
//						if(n==NULL) {n=(*dfgi->nodemap)[val]; (*dfgi->symbolmap)[n]=n1_n2_sym;}
//						if(n==NULL) { n=(*dfgi->dfg).new_node(n1_n2_val); }
//					  	importDfg(dfgi->dfg, dfgThen, n, val_defined, ec, n1_n2_sym, dfgi, (Stmt*)t, &conditionnode);
//					  }
				  }

				  // Condition 2: when THEN part is missing!!
				  node only_n2_node;
				  forall(only_n2_node, only_n2_set) {
					  Symbol* only_n2_sym = ((ExprLValue*)(dfgElse)[only_n2_node])->getSymbol();
#ifdef DEBUG						  
					  cout << " only_n2 Processing" << only_n2_sym->getName() << endl;
#endif					  
					  ExprLValue* only_n2_dummyexpr = new ExprLValue(NULL, only_n2_sym);
					  // TODO: this should check if node exists...
					  node n=(*dfgi->dfg).new_node(only_n2_dummyexpr);
					  (*dfgi->nodemap)[only_n2_dummyexpr]=n;
					  (*dfgi->symbolmap)[n]=only_n2_sym; // recording symbols 12/14/2009
					  //importDfg(dfgi->dfg, &dfgElse, n, only_n2_dummyexpr, ec, only_n2_sym, dfgi, (Stmt*)t, &invertconditionnode);
					  importDfg(dfgi->dfg, &dfgElse, n, only_n2_dummyexpr, ec, only_n2_sym, dfgi, (Stmt*)t, &conditionnode);
				  }


				  // Condition 3: when ELSE part is missing!!
				  node only_n1_node;
				  forall(only_n1_node, only_n1_set) {
				  	// 2/12/2010: Remarkably... for THEN-only nodes, since default value if always found first...
				  	if(invertconditionnode==NULL) {
						cout << "inverted condition node missing..." << endl;
						exit(1);
					}
					  Symbol* only_n1_sym = ((ExprLValue*)(dfgThen)[only_n1_node])->getSymbol();
#ifdef DEBUG						  
					  cout << " only_n1 Processing" << only_n1_sym->getName() << endl;
#endif					  
					  ExprLValue* only_n1_dummyexpr = new ExprLValue(NULL, only_n1_sym);
					  // TODO: this should check if node exists...
					  node n=(*dfgi->dfg).new_node(only_n1_dummyexpr);
					  (*dfgi->nodemap)[only_n1_dummyexpr]=n;
					  (*dfgi->symbolmap)[n]=only_n1_sym; // recording symbols 12/14/2009
					  //importDfg(dfgi->dfg, &dfgThen, n, only_n1_dummyexpr, ec, only_n1_sym, dfgi, (Stmt*)t, &conditionnode);
					  importDfg(dfgi->dfg, &dfgThen, n, only_n1_dummyexpr, ec, only_n1_sym, dfgi, (Stmt*)t, &invertconditionnode);

				  }

#ifdef DEBUG
				  cout << "===== Fninishing IFTHEN block\n" << endl;
#endif				  
			  } else if(dfgThen.number_of_nodes()!=0){


				  node fanout0_node;
				  set<node> n0_fanout0_set;
				  forall_nodes(fanout0_node, (*dfgi->dfg)) {
					  if((*dfgi->dfg).outdeg(fanout0_node)==0) {
						  n0_fanout0_set.insert(fanout0_node);
#ifdef DEBUG						  
						  Tree *t=(*dfgi->dfg)[fanout0_node];
						  cout << "--fanout0 n0=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol() << " with inputs==" << (*dfgi->dfg).indeg(fanout0_node) << endl;
#endif						  
					  }
				  }
/*

// 1/24/2010.. search by symbol instead.. 
				  // Handle all primary inputs of the smaller graph and connect them to the larger graph..
				  node fanout0_node;
				  h_array<Symbol*,node> *n0_fanout0_set = new h_array<Symbol*,node>;
				  forall_nodes(fanout0_node, (*dfgi->dfg)) {
					  if((*dfgi->dfg).outdeg(fanout0_node)==0) {
						  node input_node = ((*dfgi->dfg).source((*dfgi->dfg).first_in_edge(fanout0_node)));
						  assert(input_node);
						  (*n0_fanout0_set)[((ExprLValue*)t)->getSymbol()]=input_node;
#ifdef DEBUG						  
						  Tree *t=(*dfgi->dfg)[fanout0_node];
						  Tree *t1=((*dfgi->dfg)[input_node]);
						  cout << "--fanout0 n0=" << t->toString().replace_all("\n","") << " symbol=" << ((ExprLValue*)t)->getSymbol()->getName() << " with input=" << t1->toString()  << "[" << input_node << "]"<< endl;
#endif						  
					  }
				  }

*/

				  // Create the condition node prior to merging! FTW?
				  node tempnode = (*dfgi->dfg).new_node(ec);
				  createBlockDfg_for_expr(ec,dfgi,tempnode);
				  edge conditionedge = (*dfgi->dfg).first_in_edge(tempnode); // extract the condition node...
				  node conditionnode = (*dfgi->dfg).source(conditionedge);
				  (*dfgi->dfg).del_edge(conditionedge);
				  (*dfgi->dfg).del_node(tempnode);

				  // Merge the DFGs and then just do cleanup...
				  (*dfgi->dfg).join(dfgThen);

				  // Now match the fanin0 nodes in the THEN part with the earlier nodes..
				  match_fanin0_fanout0_nodes(dfgi, &n1_fanin0_set, &n0_fanout0_set, 2);

				  // Condition 2: when ELSE part is missing!!
				  node only_n1_node;
				  forall(only_n1_node, n1_set) {
					  Symbol* only_n1_sym = ((ExprLValue*)(dfgThen)[only_n1_node])->getSymbol();
#ifdef DEBUG						  
					  cout << " only_n1 Processing:" << only_n1_sym->getName() << endl;
#endif					  
					  ExprLValue* only_n1_dummyexpr = new ExprLValue(NULL, only_n1_sym);
					  // TODO: this should check if node exists...
					  node n=(*dfgi->dfg).new_node(only_n1_dummyexpr);
					  (*dfgi->nodemap)[only_n1_dummyexpr]=n;
					  (*dfgi->symbolmap)[n]=only_n1_sym; // recording symbols 12/14/2009
					  importDfg(dfgi->dfg, &dfgThen, n, only_n1_dummyexpr, ec, only_n1_sym, dfgi, (Stmt*)t, &conditionnode);
				  }

/*
				  node n1_search;
				  forall(n1_search, n1_fanin0_set) {
					  node n0_search = (*n0_fanout0_set)[((ExprLValue*)(*dfgi->dfg)[n1_search])->getSymbol()];
					  if(n0_search!=NULL) {
						  ExprLValue* t=(ExprLValue*)(*dfgi->dfg)[n0_search];
						  assert(t);
//					cout << "===================== FOUND n1_search=" << n1_search << " n0_search=" << n0_search << endl;
						  
						  edge fanout_edge;
						  list<edge> replace_edges=(*dfgi->dfg).out_edges(n1_search);
						  set<node> new_nodes;

						  forall(fanout_edge,replace_edges) {
							  node sink_node = (*dfgi->dfg).target(fanout_edge);
							  new_nodes.insert(sink_node);
							  (*dfgi->dfg).new_edge(n0_search, sink_node, NULL);
						  }

						  forall(fanout_edge,replace_edges) {
							  (*dfgi->dfg).del_edge(fanout_edge);
						  }

						  (*dfgi->dfg).del_node(n1_search);


					  }
				  }
				  //cout << "Final\n" << printBlockDFG(dfgi->dfg) << endl;
*/				  
			  }

			  return false; //yikes! DOUBLE yikes! maybe can define a merge function here in post?
			
			  /* Commented by Nachiket on Oct 28th 2009
			  Expr *cond=((StmtIf*)t)->getCond();
			  node  dummyBranchCond=(*dfgi->dfg).new_node(NULL);
			  createBlockDfg_for_expr(cond,dfgi,dummyBranchCond);
			  (*dfgi->nondfstmts).append((Stmt*)t);
			  return false;
			  */
			}
      default:		{
    	  cout << "Statement kind=" << stmtkindToString(((Stmt*)t)->getStmtKind()) << endl;
			  fatal(1,"unknown statement kind in dfg creation",
				t->getToken());
			  return false;
			}
    }
  } else {
    return false;
  }
}


void match_fanin0_fanout0_nodes(BlockDfgInfo* dfgi, set<node>* n1_fanin0_set, set<node>* n0_fanout0_set, int n1_n2) {

	// Now match the fanin0 nodes in the THEN part with the earlier nodes..
	node n1_search;
	forall(n1_search, *n1_fanin0_set) {
		Tree *t_n1=(*dfgi->dfg)[n1_search];
		Symbol* n1_sym=((ExprLValue*)t_n1)->getSymbol();
		node n0_search;
		forall(n0_search, *n0_fanout0_set) {
			Tree *t_n0=(*dfgi->dfg)[n0_search];
			Symbol* n0_sym=((ExprLValue*)t_n0)->getSymbol();

			if(n0_sym==n1_sym && n0_sym!=NULL) {
				if((*dfgi->dfg).indeg(n0_search)>=1) {
					node input_node = ((*dfgi->dfg).source((*dfgi->dfg).first_in_edge(n0_search)));
//					cout << "===================== FOUND n1_search=" << n1_sym->getName() << " n0_search=" << n0_sym->getName() << "n1_n2=" << n1_n2 << endl;

					edge fanout_edge;
					list<edge> replace_edges=(*dfgi->dfg).out_edges(n1_search);
					set<node> new_nodes;

					forall(fanout_edge,replace_edges) {
						node sink_node = (*dfgi->dfg).target(fanout_edge);
						new_nodes.insert(sink_node);
						(*dfgi->dfg).new_edge(input_node, sink_node, NULL);
					}

					forall(fanout_edge,replace_edges) {
						(*dfgi->dfg).del_edge(fanout_edge);
					}

//					(*dfgi->dfg).del_node(n1_search);
				} else {
//					cout << "===================== NOT FOUND n1_search=" << n1_sym->getName() << " n0_search=" << n0_sym->getName() << " n1_n2=" << n1_n2 << endl;
				}
			}
		}
	}
}

// copy the DFG from source to dest and attach connections to destnode..
void importDfg(BlockDFG *destdfg, BlockDFG *srcdfg, node destnode, ExprLValue* lval, Expr *ec, Symbol* destsym, BlockDfgInfo *dfgi, Stmt* t, node* conditionnode) {

#ifdef DEBUG
	cout << "importdfg on " << destsym->getName() << endl;
#endif

	// connect all inputs of srcnode to destnode and delete srcnode...
	// primary inputs are fucked aren't they?

	// this become the MUX node! Yeah!
	Expr *ifexpr = new ExprCond(ec->getToken(), ec, (Type*)destsym->getType()->duplicate());
	node ifnode=(*dfgi->dfg).new_node(ifexpr);
	(*dfgi->nodemap)[ifexpr]=ifnode;

	destdfg->new_edge(ifnode, destnode, NULL);
// 2/1/2010.. make condition node the first node?	
	destdfg->new_edge(*conditionnode, ifnode, NULL); // condition node is shared by all IFs..

	StmtAssign* destnodeStmt = new StmtAssign(NULL, lval, ifnode); // lhs=lval, rhs=conditionnode

	bool matched=false;
	set<node> deleting_nodes;
	node srcnode, node, in_node, del_node;


	forall_nodes(node, *destdfg) {
		if(destdfg->outdeg(node)==0 && ((*destdfg)[node])->isLValue()) {
			Symbol* sym = ((ExprLValue*)(*destdfg)[node])->getSymbol();
			if(sym==destsym && node!=destnode) {
				matched=true;
				srcnode=node;

				// collect matched nodes...
				deleting_nodes.insert(node);

#ifdef DEBUG				
				cout << "--Matched srcnode with destnode:" << (*destdfg)[srcnode]->toString() << "," << (*destdfg)[destnode]->toString() << endl;
#endif				
			}
		}
	}

/*
	// attempt to match in parent to differentiate between THEN and ELSE nodes..
	forall(node, deleting_nodes) {
		cout << "======= testing.. destdfg: destnode=" << (*destdfg)[node]->toString() << endl;
		forall_nodes(srcnode, *srcdfg) {
			cout << "======= testing.. srcdfg: srcnode=" << (*srcdfg)[node]->toString() << " to destdfg: destnode=" << (*destdfg)[node]->toString() << endl;
			Symbol* destnodesym = ((ExprLValue*)(*destdfg)[node])->getSymbol();
			Symbol* srcnodesym = ((ExprLValue*)(*srcdfg)[srcnode])->getSymbol();
			if(srcnodesym==destnodesym && node!=destnode) {
//			if(node==srcnode) {
				matched=true;
//#ifdef DEBUG				
				cout << "======= Matched nodes in srcdfg: srcnode=" << (*srcdfg)[node]->toString() << " to destdfg: destnode=" << (*destdfg)[node]->toString() << endl;
//#endif				
			}
		}
	}
*/

	// deferred update
	forall(node, deleting_nodes) {
		// replace srcnode with destnode
		// find all inputs of srcnode and redirect them to destnode
		edge in_edge;
		forall_in_edges(in_edge, node) {
			in_node = destdfg->source(in_edge);
#ifdef DEBUG			
//			cout << "--\tInput=" << (*destdfg)[in_node]->toString() << "(" << in_node << ") -> " << destnode  << "(expecting=" << destdfg->succ_node(in_node) << ")" << endl;
			cout << "--\tInput=" << (*destdfg)[in_node]->toString() << " -> " << (*destdfg)[destnode]->toString()  << endl;
#endif				
			destdfg->new_edge(in_node,ifnode,NULL); // replaced destnode with n
		}
	}

	forall(del_node, deleting_nodes) {
		destdfg->del_node(del_node); // remove this duplicate node
	}

	if(matched) {

		// Don't know if this really needs to be done...
		if((*dfgi->livedefs).defined(destsym)) {
				StmtAssign *dead_asst=(*dfgi->livedefs)[destsym];
				(*dfgi->deaddefs).append(dead_asst);
				//cout << "Removing dead assignment " << endl;
				//if((dead_asst->getRhs())==(Expr*)0) {
				//	deleteBlockDfgCone(dfgi->dfg,*(dead_asst->getRhsnode()));
				//}
		}

		// - record live asst
		(*dfgi->livedefs)[destsym]=destnodeStmt;
//		cout << "-- Defined livedef for " << (*destdfg)[destnode]->toString() << "[" << destnode << "] with lval=" << lval << ",rhs=" << ifnode << " for assignment=" << destnodeStmt << endl;

	}

// 2/1/2010.. make condition node the first node?	destdfg->new_edge(*conditionnode, ifnode, NULL); // condition node is shared by all IFs..

}


h_array<node, Symbol*> createBlockDfg (StateCase* sc, BlockDFG *dfg, list<Stmt*> *stmts,
		                    list<Stmt*> *nondfstmts,
				    set<SymbolVar*> *locals,
				    set<SymbolVar*> *new_locals,
				    SymTab* vars, // Added by Nachiket on 11/3/2009 to allow access to state-machine variables..
				    h_array<Symbol*, node> *valid_map) // 2/3/2010 to support conditional stream writes..
{
  // - create DFG (fill *dfg) for a basic-block stmt list (*stmts)
  // - return in *nondfstmts any stmts not used to build dfg (e.g. goto)
  // - return in *locals any local variables  (should all be in 1 block)
  // - return in *new_locals any new locals created for pseudo-SSA form
  // - modify *stmts into pseudo-SSA form

#ifdef DEBUG
  cout << "Processing state " << sc->getStateName() << endl;
#endif

  // - build DFG from stmts  (chain assignment cones)
  dfg->clear();
  nondfstmts->clear();
  locals->clear();
  new_locals->clear();
  h_array<Expr*,node>          nodemap;
  h_array<Symbol*,StmtAssign*> livedefs(NULL);	// StmtAssign* has no default?
  h_array<Symbol*,StmtAssign*> initialdefs(NULL);	// 1-24/2010 for local variables
  map<Symbol*,node>        extdefs;
  h_array<node, Symbol*>	   symbolmap;
  list<StmtAssign*>        deaddefs;

  BlockDfgInfo dfgi(dfg,sc,NULL,&nodemap,&symbolmap,&livedefs,&initialdefs, &extdefs,&deaddefs,
    		    nondfstmts,locals, vars, NULL);

  initialize_dfginfo(&dfgi, true);

  Stmt *s;
  forall (s,*stmts) {
    s->map(createBlockDfg_map,(TreeMap)NULL,&dfgi);
  }

  finalize_dfginfo(&dfgi, true);

// 2/2/2010.. considering duplication
if(valid_map!=NULL) {
  set<Symbol*> *valid_symbols=new set<Symbol*>();
  h_array<Symbol*, Symbol*> *valid_symbol_map = new h_array<Symbol*, Symbol*>();
  BlockDFG newdfg; newdfg.clear();
  node n;
  forall_nodes (n,*dfg) {
	  Tree* t=(*dfg)[n];
	  if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_LVALUE) {

		  Symbol *asym=((ExprLValue*)t)->getSymbol();
		  if(asym!=NULL && asym->isStream()) {
			  SymbolStream *ssym=(SymbolStream *)asym;
			  if(ssym->getDir()==STREAM_OUT) {
				SymbolVar* valid_sym;
				const string valid_name = string("_valid_"+asym->getName());
				Type* valid_type = new Type(TYPE_BOOL);
				valid_sym = new SymbolVar(NULL, valid_name , valid_type, NULL, NULL);
				valid_sym->setStreamValid();
//				ExprLValue* valid_lval = new ExprLValue(NULL, valid_sym);
//				node valid_node=newdfg.new_node(valid_lval);
//				(nodemap)[valid_lval]=valid_node;
//				(symbolmap)[valid_node]=valid_sym;
				  Tree *tnew = t->duplicate();
				  ((Expr*)tnew)->setType(new Type(TYPE_BOOL));
				  ((ExprLValue*)tnew)->setSymbol(valid_sym);
				  node valid_node = newdfg.new_node((Expr*)tnew);

				  cout << "Duplicating... " << t->toString() << endl;
				  recursiveFaninDuplicate(dfg, &newdfg, n, valid_node, false);
				  (*valid_map)[asym]=valid_node;
				  (symbolmap)[valid_node]=valid_sym;
				  //valid_symbols->insert(valid_lval->getSymbol());
				  //(*valid_symbol_map)[valid_lval->getSymbol()]=asym;
				  //cout << "newsym=" << valid_lval->getSymbol() << ", oldsym=" << asym << ", node=" << valid_node << ", lval=" << valid_lval << ", t's lval=" << ((ExprLValue*)t) << endl;
			  }
		  }
	  }
  }

  dfg->join(newdfg);

/*
  // after join node pointers are all messed up! God awful library!
  Symbol* sym;
  forall(sym, *valid_symbols) {
  	forall_nodes (n,*dfg) {
		  Tree* t=(*dfg)[n];
		  if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_LVALUE) {
			  Symbol *asym=((ExprLValue*)t)->getSymbol();
			  if(asym==sym) {
				  (*valid_map)[(*valid_symbol_map)[asym]]=n;
				  cout << "newsym=" << asym << ", oldsym=" << (*valid_symbol_map)[asym] << ", node=" << n << "=" << ((ExprLValue*)(*dfg)[n])->getSymbol() << ", lval=" << ((ExprLValue*)(*dfg)[n]) << endl;
			  }
		  }
	  }
  }
*/

}

if(0) {
  // - clean up multiple definitions
  // - for all assts to a var other than the last one:
  //     if def is dead, remove PO node & cone (dead-code elimination)
  //     if def is used, replace var by tmp (like SSA)
  map<Symbol*,int> numSymDups;		// - used to rename symbols into tmps
  StmtAssign *asst;
  forall (asst,*dfgi.deaddefs) {
    ExprLValue *lval=asst->getLValue();		// - lval annotates PO and asst
    node po=(*dfgi.nodemap)[lval];
    node def=(*dfgi.dfg).source((*dfgi.dfg).first_in_edge(po));
    if ((*dfgi.dfg).outdeg(def)==1) {
      // - this asst is the var's only use, i.e. def is dead
      // - delete PO node and cone  (deletion leaves dangling ptrs in
      //      nodemap, but that's ok, since we don't need it anymore)
      deleteBlockDfgCone(dfgi.dfg,po);
    }
    else {
      // - this asst is not the var's only use, i.e. def is not dead
      // - create new sym, modify asst to use it
      Symbol *sym=lval->getSymbol();
      if (sym->isStream()) {
	// - WARNING:  not handling multiple stream emission; don't touch PO
      }
      else {
	Symbol *newSym=(Symbol*)sym->duplicate();
	string  newSymName=sym->getName()+string("_%d",++numSymDups[sym]);
	newSym->setName(newSymName);	// - rename in reconstructTDF_onecase()
	lval->setSymbol(newSym);	// - HACK:  overwrite asst lval sym
	new_locals->insert((SymbolVar*)newSym);	// - cast: see WARNING above
      }
    }
  }
}

  return *dfgi.symbolmap;

  // return dfgi.dfg;
  // return dfgi.nondfstmts;
  // return dfgi.locals;
  // return dfgi.new_locals;
}


// need to initialize symbols for nested DFGinfos..

void initialize_dfginfo(BlockDfgInfo* dfgi, bool toplevel) {

	BlockDFG* dfg=dfgi->dfg;
  	h_array<Expr*,node>          *nodemap=(dfgi->nodemap);
	h_array<Symbol*,StmtAssign*> *livedefs = (dfgi->livedefs);
	h_array<Symbol*,StmtAssign*> *initialdefs = (dfgi->initialdefs);
	h_array<node, Symbol*>	     *symbolmap = (dfgi->symbolmap);

	//-------------------------------------------------------------------
	// Initialize with next-state variable 12/27/2009
	//-------------------------------------------------------------------
	SymbolVar* nextstate_sym;
	if(toplevel) {
		const string nextstate_name = string("__nextstate");
		Type* nextstate_type = new Type(TYPE_STATE);
		nextstate_sym = new SymbolVar(NULL, nextstate_name , nextstate_type, NULL, NULL);
	} else {
		nextstate_sym = dfgi->nextstate;
	}
	ExprLValue* nextstate_dummylval = new ExprLValue(NULL, nextstate_sym);
	node nextstate=dfg->new_node(nextstate_dummylval);
	(*nodemap)[nextstate_dummylval]=nextstate;
	(*symbolmap)[nextstate]=nextstate_sym;

	ExprValue* currentStateVal = new ExprValue(NULL, dfgi->sc->getStateName());
	node currentnode = dfg->new_node(currentStateVal);
	(*nodemap)[currentStateVal]=currentnode;
	dfg->new_edge(currentnode, nextstate, NULL);

	StmtAssign* t1=new StmtAssign(NULL, nextstate_dummylval, currentnode);
	(*livedefs)[nextstate_sym]=t1;
	(*initialdefs)[nextstate_sym]=t1;
	dfgi->nextstate=nextstate_sym;

	//-------------------------------------------------------------------
	// Initialize all local variables with dummy nodes
	//-------------------------------------------------------------------

	Symbol* localsym;
	forall (localsym,*(dfgi->vars->getSymbolOrder())) {
		ExprLValue* localvar_dummylval = new ExprLValue(NULL, localsym);
		node localvarnode=dfg->new_node(localvar_dummylval);
		(*nodemap)[localvar_dummylval]=localvarnode;
		(*symbolmap)[localvarnode]=localsym;

		ExprLValue* defaultVal = new ExprLValue(NULL, localsym);
		node defaultnode = dfg->new_node(defaultVal);
		(*nodemap)[defaultVal]=defaultnode;
		dfg->new_edge(defaultnode, localvarnode, NULL);

		// Why didn't we just initialize this as an assignment?
		StmtAssign* t2=new StmtAssign(NULL, localvar_dummylval, defaultnode);
		(*livedefs)[localsym]=t2;
		(*initialdefs)[localsym]=t2;
#ifdef DEBUG
		cout << "-------- Setup:" << defaultnode << "->" << localvarnode << " sym=" << localsym->getName() << " [" << localsym << "]" << endl;
#endif		
	}
}


void finalize_dfginfo(BlockDfgInfo* dfgi, bool toplevel) {

  BlockDFG* dfg=dfgi->dfg;
  h_array<Expr*,node>          *nodemap=(dfgi->nodemap);
  h_array<Symbol*,StmtAssign*> *livedefs = (dfgi->livedefs);
  h_array<Symbol*,StmtAssign*> *initialdefs = (dfgi->initialdefs);
  h_array<node, Symbol*>       *symbolmap = (dfgi->symbolmap);

  // 1/25/2010: Remove initialization of unused nextstate update
  if(!toplevel) {
	  Symbol* nextstate_sym=dfgi->nextstate;
	  if((*livedefs)[nextstate_sym]==(*initialdefs)[nextstate_sym]) {
		  StmtAssign* asst=(*livedefs)[nextstate_sym];
		  if(asst!=NULL) {
			  node n=asst->getRhsnode();
			  if(n!=NULL) {	
				  edge e=dfg->first_out_edge(n);
				  node n_out=dfg->target(e);
#ifdef DEBUG				  
				  cout << "Found unused nextstate variable " << nextstate_sym->getName() << " in " << dfgi->sc->getStateName() << " outs=" << (*dfgi->dfg).outdeg(n) << " ins=" << (*dfgi->dfg).indeg(n_out) << endl;
#endif				  
				  dfg->del_node(n_out);
				  dfg->del_node(n);
			  }
		  }
	  }
  }


  // 1/24/2010: Remove initializations of unused local variables
  Symbol* localsym;
  forall (localsym,*(dfgi->vars->getSymbolOrder())) {

	// check if anyone used it
	StmtAssign* asst=(*livedefs)[localsym];
	if(asst!=NULL) {
		node n=asst->getRhsnode();
		if(n!=NULL) {
			if(dfg->outdeg(n)==1) { 
				edge e=dfg->first_out_edge(n);
				node n_out=dfg->target(e);

				if(dfg->indeg(n)==1) {

					Symbol* targetsym=(*symbolmap)[n_out];
					if(localsym==targetsym) {
#ifdef DEBUG				
					cout << "Found unused variable " << localsym->getName() << " in " << dfgi->sc->getStateName() << endl;
#endif					
						// now delete this unused variable assignment
						dfg->del_node(n);
						dfg->del_node(n_out);
//						dfg->del_edge(e);
						asst->setRhsnode(NULL);
					}
				}
			}
		}
	}

  	// check if anyone updated it
  	if((*livedefs)[localsym]==(*initialdefs)[localsym]) {
		StmtAssign* asst=(*livedefs)[localsym];
		if(asst!=NULL) {
			node n=asst->getRhsnode();
			if(n!=NULL) {	
				edge e=dfg->first_out_edge(n);
				node n_out=dfg->target(e);
#ifdef DEBUG			
				cout << "Found unused output variable " << localsym->getName() << " in " << dfgi->sc->getStateName() << " outs=" << (*dfgi->dfg).outdeg(n) << " ins=" << (*dfgi->dfg).indeg(n_out) << endl;
#endif				
				if(dfg->outdeg(n)==1) {
					dfg->del_node(n);
				}
				dfg->del_node(n_out);
//				dfg->del_edge(e);
			}
		}
	}



  }
}


h_array<node, Symbol*> createBlockDfgSimple (StateCase* sc, BlockDFG *dfg, list<Stmt*> *stmts, SymTab* vars, h_array<Symbol*, node> *valid_map)
{

	list<Stmt*> nondfstmts;
	set<SymbolVar*> locals;
	set<SymbolVar*> new_locals;

	return createBlockDfg(sc, dfg, stmts, &nondfstmts, &locals, &new_locals, vars, valid_map);
}

using std::cout;
using std::endl;

////////////////////////////////////////////////////////////////
//  Print BlockDFG

string printBlockDFG (string statename, int eofr_case, int eos_case, BlockDFG *dfg,
		      node_array<int> *areas,		// "A=..."
		      node_array<int> *latencies,		// "L=..."
		      node_array<int> *depths)		// "D=..."
{
  string ret("StateDfg "+statename+" (%d nodes, %d edges, %d eofr_case, %d eos_case) \n",
	     dfg->number_of_nodes(), dfg->number_of_edges(), eofr_case, eos_case);

//cout << "inside" << endl;

  int nodenum=0;
  node_array<int> nodenums(*dfg);

  int cast_count=0;
  node n;
  forall_nodes (n,*dfg) {

	  //cout << "Node..." << endl;
	  nodenums[n] = nodenum++;
	  ret += string("node %d ",nodenums[n]);
	  //ret += (dfg->indeg (n)==0 ? "PI " :
	  //        dfg->outdeg(n)==0 ? "PO " : "in="+string("%d",dfg->indeg(n))+",out="+string("%d",dfg->outdeg(n)));
	  if (areas)
		  ret += string("A=%d ",(*areas)[n]);
	  if (latencies)
		  ret += string("L=%d ",(*latencies)[n]);
	  if (depths)
		  ret += string("D=%d ",(*depths)[n]);
	  Tree *t=(*dfg)[n];

	  string opType;
	  if(t->getKind()==TREE_EXPR) {
		  if(((Expr*)t)->getExprKind()==EXPR_BOP) {
			  opType= opToString(((ExprBop*)t)->getOp());
			  ret += " " + typekindToString((*((Expr*)t)->getType()).getTypeKind()) + " operator "+opType+" \n";
		  } else if(((Expr*)t)->getExprKind()==EXPR_UOP) {
			  opType=opToString(((ExprUop*)t)->getOp());
			  ret += " " + typekindToString((*((Expr*)t)->getType()).getTypeKind()) + " operator "+opType+" \n";
		  } else if(((Expr*)t)->getExprKind()==EXPR_COND) {
			  ret += " " + typekindToString((*((Expr*)t)->getType()).getTypeKind()) + " operator IF \n";
		  } else {
			  string var="";
			  if(((Expr*)t)->getExprKind()==EXPR_VALUE) {
				  var=" " + typekindToString((*((Expr*)t)->getType()).getTypeKind()) + " constant";
			  }
			  if(((Expr*)t)->getExprKind()==EXPR_LVALUE) {
				  string t_str1 = t ? t->toString().replace_all("\n","") : string("<nil>");
				  var=" " + typekindToString((*((Expr*)t)->getType()).getTypeKind()) + " variable";
			  }
			  string t_str = t ? t->toString().replace_all("\n","") : string("<nil>");
			  
			  // For now I am throwing out the typecasting.. not necessary
			  if(((Expr*)t)->getExprKind()==EXPR_CAST) {
				  var=" " + typekindToString((*((Expr*)t)->getType()).getTypeKind()) + " variable";
				  std::stringstream out; out<<cast_count;
				  t_str = "auto_inserted_cast_node_being_ignored_for_world_peace_"+string(out.str().c_str());
				  cast_count++;
			  }

			  // 2/11/2010: Need to handle BUILTINs during Dataflow Graph generation
			  if(((Expr*)t)->getExprKind()==EXPR_BUILTIN) {
			  	var=" " + typekindToString((*((Expr*)t)->getType()).getTypeKind()) + " variable";
			  }

			  ret += var + " "  + t_str + "\n";
		  }
	  } else {

		  string t_str = t ? t->toString().replace_all("\n","") : string("<nil>");
		  ret += " "+treekindToString(t->getKind())+" "+ t_str + "\n";
	  }
  }

  // attempt to generate graph by iterating over nodes and the input edges of node..
  node n1;
  edge e;
  forall_nodes (n1,*dfg) {
  	forall_in_edges(e, n1) {
      //forall_edges (e,*dfg) {
      // Nachiket: added space around -> for easier tokenization in javacc
      ret += string("edge %d -> %d \n",
		  nodenums[dfg->source(e)], nodenums[dfg->target(e)]);
      //Tree *t=(*dfg)[e];

      Tree *t1=(*dfg)[dfg->source(e)];
      string t1_str = t1 ? t1->toString().replace_all("\n","") : string("<nil>");
      Tree *t2=(*dfg)[dfg->target(e)];
      string t2_str = t2 ? t2->toString().replace_all("\n","") : string("<nil>");

	// Nachiket: avoid trailing crap
      // string t_str = t1_str + " -> " + t2_str;
      // ret += ": " + t_str + "\n";
	
//    string t_str = t ? t->toString().replace_all("\n","") : string("<nil>");
//    ret += ": " + t_str + "\n";

    }
  }

  return ret;
}


////////////////////////////////////////////////////////////////
//  Timing / Area model

void getNodeAreaLatency_1 (BlockDFG *dfg, node n, int *area, int *latency)
{
  // - Compute area + latency for a BlockDFG node (return in *area, *latency)
  // - dummy model:  area, latency = 0 for branch conds, vars, bit-sel, consts
  //                 area, latency = 1 for everything else

  Expr *e=(*dfg)[n];
  if (!e)
    *area = *latency = 0;		// - branch condition
  else if (e->getExprKind()==EXPR_LVALUE
	   && ((ExprLValue*)e)->getSymbol()->getSymKind()==SYMBOL_VAR)
    *area = *latency = 0;		// - var ref
  else if (e->getExprKind()==EXPR_BITSEL)
    *area = *latency = 0;		// - bit selection
  else if (e->getExprKind()==EXPR_VALUE)
    *area = *latency = 0;		// - const value
  else
    *area = *latency = 1;		// - everything else
}


int getTypeWidth (Type *t)
{
  // - return width of *t if it is constant, else fatal error

  switch (t->getTypeKind())
  {
    case TYPE_BOOL:	{
			  return 1;
			}
    case TYPE_INT:	{
			  int w=t->getWidth();
			  if (w)
			    return w;
			  else {
			    Expr *we=t->getWidthExpr();
			    if (we->getExprKind()==EXPR_VALUE)
			      return ((ExprValue*)we)->getIntVal();
			    Expr *eval_we=evalExpr(we);
			    if (eval_we->getExprKind()==EXPR_VALUE)
			      return ((ExprValue*)eval_we)->getIntVal();
			    else
			      fatal(-1,"type has non constant bit width "
				       "in page timing",t->getToken());
			  }
			}
    case TYPE_FIXED:	{
			  fatal(-1,"fixed point type not supported "
				   "in page timing",t->getToken());
			}
    case TYPE_ANY:
    case TYPE_NONE:	{
			  assert(!"getTypeWidth: bad type");
			}
    default:		{
			  assert(!"internal inconsistency");
			}
  }

  assert(!"never get here");
  return 0;
}


void getNodeAreaLatency (BlockDFG *dfg, node n, int *area, int *latency)
{
  // - Compute area + latency for a BlockDFG node (return in *area, *latency)
  // - area    = #PEs
  // - latency = #cycles * 1000
  // - PE is HSRA 5-LUT w/hard-wired 5th input carry chain and output register
  // - PE cascade timing (latency) is:
  //     MILLS_PER_LUT   = 1000/N	N = #LUTs       per cycle
  //     MILLS_PER_CARRY = 1000/M	M = #carry bits per cycle
  // - hidden costs:
  //    * reg/stream write muxes:
  //        ignore area (should add only once per op), but add latency
  //    * stream history read muxes:
  //        ignore (require stream ref's retiming index to be constant)
  //    * reg subscript muxes / shifters:
  //        ignore (require bit subscripts to be constant)
  // - mux implementations:
  //     2:1    A=1      D=1
  //     4:1    A=3      D=2
  //     8:1    A=7      D=3
  //     2^n:1  A=2^n-1  D=n
  //     amortized cost per input:  area=1, depth=(log #inputs)/#inputs

  #define MILLS_PER_LUT   500			// - 2 LUTs per cycle
  #define MILLS_PER_CARRY 250			// - 4 carry bits per cycle

  // - model for n:1 mux
  #define MUXAREA(n)	(((n)<2) ? 0 : ((n)-1))
  #define MUXDEPTH(n)	(((n)<2) ? 0 : (countBits((n)-1)))
  #define MUXLATENCY(n)	(MILLS_PER_LUT*MUXDEPTH(n))

  Expr *e=(*dfg)[n];
  if (!e) {
    *area = *latency = 0;		// - branch condition out-node
  }
  else {
    switch (e->getExprKind()) {
      case EXPR_VALUE:	  {
			    // - const value:  fold into consuming PE
			    *area = *latency = 0;
			    break;
			  }
      case EXPR_LVALUE:	  {
			    // - symbol ref  (read or write, stream or reg)
			    ExprLValue *lval=(ExprLValue*)e;
			    // Symbol  *sym =lval->getSymbol();
			    Expr *retime  = lval->getRetime();
			    if (retime && retime->getExprKind()!=EXPR_VALUE)
			      fatal(-1,"time index of expr "+
				       e->toString()+" must be constant "
				       "during page timing",e->getToken());
			    if ((*dfg).indeg(n)==0) {
			      // - symbol read  (reg or stream)
			      // - const subscr means
			      //     reg read has no bit select/shift mux
			      // - const retime means
			      //     stream read has no input history mux
			      // - hence, reg/stream read is just wires
			      *area = *latency = 0;
			    }
			    else if ((*dfg).outdeg(n)==0) {
			      // - symbol write  (reg or stream)
			      // - assume write is unconditional, i.e. "-xc"
			      // - go thru write mux
			      // int muxwidth=getNumRegWriters(sym);
			      int muxwidth=0;		// - DUMMY ***
			      *area    = MUXAREA(muxwidth);
			      *latency = MUXLATENCY(muxwidth);
			    }
			    else {
			      assert(!"internal inconsistency");
			    }
			    break;
			  }
      case EXPR_BITSEL:	  {
			    // - bit selection:  pure wires
			    // ExprBitSel *bitsel=(ExprBitSel*)e;
			    *area = *latency = 0;
			    break;
			  }
      case EXPR_UOP:	  {
			    // - unary op
			    Expr *e1=((ExprUop*)e)->getExpr();
			    int   w1=getTypeWidth(e1->getType());
			    switch  (((ExprUop*)e)->getOp()) {
			      case '!':
			      case '~':	{
					  // - fold into consuming LUT
					  *area = *latency = 0;
					  break;
					}
			      case '-':	{
					  // - carry-propagate negation
					  *area    = w1;
					  *latency = w1 * MILLS_PER_CARRY;
					  break;
					}
			    }
			    break;
			  }
      case EXPR_BOP:	  { 
			    // - binary op
			    Expr *e1=((ExprBop*)e)->getExpr1();
			    Expr *e2=((ExprBop*)e)->getExpr2();
			    int   w1=getTypeWidth(e1->getType()),
			          w2=getTypeWidth(e2->getType()),
			      //  we=getTypeWidth(e ->getType()),
			          wmin=w1<w2 ? w1 : w2,
			          wmax=w1>w2 ? w1 : w2;
			    switch  (((ExprBop*)e)->getOp()) {
			      case '+':
			      case '-':	{
					  // - carry-propagate adder
					  *area    = wmax;
					  *latency = wmax * MILLS_PER_CARRY;
					  break;
					}
			      case '*':
			      case '/':
			      case '%':	{
					  // - dummy:  w1 x w2 array
					  *area = wmin * wmax;
					  *latency = wmax * MILLS_PER_LUT +
						     wmin * MILLS_PER_CARRY;
					  break;
					}
			      case '&':
			      case '|':
			      case '^':
			      case LOGIC_AND:
			      case LOGIC_OR:	{
						  // - logic in parallel LUTs
						  *area    = wmax;
						  *latency = MILLS_PER_LUT;
						  break;
						}
			      case EQUALS:
			      case NOT_EQUALS:
			      case LTE:
			      case GTE:
			      case '<':
			      case '>':		{
						  // - cmp using carry chain
						  *area    = wmax;
						  *latency = wmax*
							     MILLS_PER_CARRY;
						  break;
						}
			      case '.':		{
						  // - make fixed (wires)
						  *area = *latency = 0;
						  break;
						}
			      case LEFT_SHIFT:
			      case RIGHT_SHIFT:	{
						  if (e->getExprKind()==
								EXPR_VALUE) {
						    // - const shift (wires)
						    *area = *latency = 0;
						  }
						  else {
						    // - log shift
						    *area = w1 * MUXDEPTH(w1);
						    *latency = MUXLATENCY(w1);
						  }
						  break;
						}
			      default:		{
						  assert(!"internal "
							  "inconsistency");
						}
			    }
			    break;
			  }
      case EXPR_COND:	  { 
			    // - conditional:  parallel 2:1 muxes
			    // - conservative, use full we mux width
			    // - could get away with mux width min(w1,w2)
			    //    and fold remaining bits into consuming LUTs,
			    //    but this works even w/o consumer (reg write)
			    int we   = getTypeWidth(e->getType());
			    *area    = we * MUXAREA(2);	// = we
			    *latency = MUXLATENCY(2);	// = MILLS_PER_LUT
			    break;
			  }
      case EXPR_CAST:	  {
			    // - cast:  0-pad or sign-extend using pure wires
			    *area = *latency = 0;
			    break;
			  }
      case EXPR_BUILTIN:  {
			    // - built-in  (bitsof, cat, widthof), all free
			    OperatorBuiltin *op=(OperatorBuiltin*)
						((ExprBuiltin*)e)->getOp();
			    BuiltinKind bkind=op->getBuiltinKind();
			    if (bkind==BUILTIN_BITSOF	||
				bkind==BUILTIN_CAT	||
				bkind==BUILTIN_WIDTHOF    )
			      *area = *latency = 0;
			    else
			      fatal(-1,string("unhandled builtin kind %d "
					      "in page timing",(int)bkind),
				    e->getToken());
			    break;
			  }
      default:		  {
			    fatal(-1,string("unhandled expression kind %d "
					    "in page timing",
					    (int)e->getExprKind()),
				  e->getToken());
			    break;
			  }
    }
  }

}


void getAreasLatencies (BlockDFG *dfg,
			node_array<int> *areas, node_array<int> *latencies)
{
  // - Compute + fill *areas, *latencies for all nodes of a BlockDFG
  //     (the arrays are reinitialized)

  areas->init(*dfg,0);
  latencies->init(*dfg,0);
  node n;
  forall_nodes (n,*dfg) {
    if (gPageTiming1)
      getNodeAreaLatency_1(dfg,n,&(*areas)[n],&(*latencies)[n]); // unit delays
    else
      getNodeAreaLatency  (dfg,n,&(*areas)[n],&(*latencies)[n]); // ~HSRA model
  }
}


////////////////////////////////////////////////////////////////
//  Hazard / precedence edges
//  (needed for static timing analysis and reconstructing TDF code)

set<node> getInputNodes (graph *g)
{
  // - Return set of "primary inputs" (nodes w/no incoming edges) for graph *g
  set<node> inputs;
  node n;
  forall_nodes (n,*g)
    if (g->indeg(n)==0)
      inputs.insert(n);
  return inputs;
}


set<node> getOutputNodes (graph *g)
{
  // - Return set of "primary outputs" (nodes w/no outgoing edges) for graph *g
  set<node> outputs;
  node n;
  forall_nodes (n,*g)
    if (g->outdeg(n)==0)
      outputs.insert(n);
  return outputs;
}


bool isPO (BlockDFG *dfg, node n)
{
  // - a BlockDFG node n is a PO if
  //     (i)  it has no outgoing edges, or
  //     (ii) it is a write-node (annotated by ExprLValue*)
  //            with outgoing edges only to other POs  (precedence edges)

  Expr *n_expr;
  if ((*dfg).outdeg(n)==0) {
    return true;
  }
  else if (   (n_expr=(*dfg)[n])
	   && n_expr->getExprKind()==EXPR_LVALUE
	   && (*dfg).indeg(n)>0                 ) {
    list<edge> n_out_edges=(*dfg).out_edges(n);
    edge e;
    forall (e,n_out_edges) {
      node dst=(*dfg).target(e);
      if (dst==n)			// - ignore self loops
	continue;
      if (!isPO(dfg,dst))
	return false;
    }
    return true;
  }
  else {
    return false;
  }

  assert(!"never get here");
  return false;
}


set<node> getOutputNodes_POtoPO (BlockDFG *dfg)
{
  // - Return set of "primary outputs" (PO) for graph *dfg
  // - recognizes POs that have outgoing edges to other POs (precedence edges)

  set<node> outputs;
  node n;
  forall_nodes (n,*dfg)
    if (isPO(dfg,n))
      outputs.insert(n);
  return outputs;
}


set<edge> addWarEdges (BlockDFG *dfg)
{
  // - augment *dfg with WAR hazard edges, and return set of new edges
  // - WAR edge is to a register's write node (PO)
  //     from each of that register's use nodes (consumer of PI)
  //     (WAR edge establishes precedence:  read first, overwrite after)
  // - these WAR edges are sufficient for timing analysis,
  //     but not for reconstructing TDF code--
  //     for that, use addWarEdges_POtoPO, which add PO-->PO edges
  //     to set precedence between assignment stmts.
  // - WARNING:  must be called before addRawEdges() if using both
  //             (RAW adds outputs to PO nodes, confuses getOutputNodes())
  // - NOTE:  using addRawEdges() and addWarEdges together
  //          on an arbitrary dfg can create feedback cycles;
  //          but using them on a dfg created by createBlockDfg() +
  //          partitionBlockDfg() (i.e. from well-formed TDF) is fine,
  //          since original/pseudo-SSA TDF code has enough temporary
  //          vars to break any feedback.

  set<edge> war_edges;

  set<node> PIs=getInputNodes(dfg);
  set<node> POs=getOutputNodes(dfg);
  map<Symbol*,node> PI_of(NULL);
  node pi;
  forall (pi,PIs) {					// - build PI_of[sym]
    Expr *ex=(*dfg)[pi];
    if (ex && ex->getExprKind()==EXPR_LVALUE) {
      Symbol *sym=((ExprLValue*)ex)->getSymbol();
      PI_of[sym]=pi;
    }
  }
  node po;
  forall (po,POs) {					// - forall write POs
    Expr *ex=(*dfg)[po];
    if (ex && ex->getExprKind()==EXPR_LVALUE) {
      Symbol *sym=((ExprLValue*)ex)->getSymbol();
      node pi=PI_of[sym];
      if (pi) {						// - if sym has a PI
	list<edge> out_edges_pi=(*dfg).out_edges(pi);
	edge e;
	forall (e,out_edges_pi) {			// - forall uses of PI
	  node use=(*dfg).target(e);
	  assert(use!=po);
	  edge e_war=(*dfg).new_edge(use,po);		// - add WAR edge
	  war_edges.insert(e_war);			//     use-->po
	}
      }
    }
  }

  return war_edges;
}


set<edge> addWarEdges_POtoPO (BlockDFG *dfg)
{
  // - augment *dfg with WAR hazard edges, and return set of new edges
  // - WAR edge is to a register's write node (PO)
  //     from each asst that directly the register's read node (PI),
  //     (i.e. for each use (output) of reg's PI,
  //           add edge from earliest PO downstream of that use
  //           with non-annotated pred. edge [direct asst], to reg's PO)
  //     (WAR edge establishes precedence:  read first, overwrite after)
  // - PO-->PO edges set precedence between assignment stmts
  //     for reconstructed TDF code.
  // - WARNING:  must be called before addRawEdges() if using both
  //             (RAW adds outputs to PO nodes, confuses getOutputNodes())
  // - NOTE:  using addRawEdges() and addWarEdges together
  //          on an arbitrary dfg can create feedback cycles;
  //          but using them on a dfg created by createBlockDfg() +
  //          partitionBlockDfg() (i.e. from well-formed TDF) is fine,
  //          since original/pseudo-SSA TDF code has enough temporary
  //          vars to break any feedback.

  set<edge> war_edges;

  set<node> PIs=getInputNodes(dfg);
  set<node> POs=getOutputNodes(dfg);
  map<Symbol*,node> PI_of(NULL);
  node pi;
  forall (pi,PIs) {					// - build PI_of[sym]
    Expr *ex=(*dfg)[pi];
    if (ex && ex->getExprKind()==EXPR_LVALUE) {
      Symbol *sym=((ExprLValue*)ex)->getSymbol();
      PI_of[sym]=pi;
    }
  }

  list<node> war_edges_src, war_edges_dst;
  node po;
  forall (po,POs) {					// - forall write POs
    Expr *ex=(*dfg)[po];
    if (ex && ex->getExprKind()==EXPR_LVALUE) {
      Symbol *sym=((ExprLValue*)ex)->getSymbol();
      node pi=PI_of[sym];
      if (pi) {						// - if sym has a PI
	// warn("detected WAR PO-->PO hazard for PO asst:"+
	//      (*dfg)[po]->getParent()->toString()+", PI sym: "+sym->getName());
	list<edge> out_edges_pi=(*dfg).out_edges(pi);
	edge e_use;
	forall (e_use,out_edges_pi) {			// - check all PI uses
	  node n_use=(*dfg).target(e_use);
	  list<node> frontier;
	  frontier.append(n_use);
	  while (!frontier.empty()) {		// - BFS finds 1st reg write
	    node n=frontier.pop_front();	//   downstream of PI use
	    Expr *n_expr;
	    if (   isPO(dfg,n)
		&& (n_expr=(*dfg)[n])
		&& n_expr->getExprKind()==EXPR_LVALUE
//		&& ((ExprLValue*)n_expr)->getSymbol()
//					->getSymKind()==SYMBOL_VAR
		&& !(*dfg)[(*dfg).first_in_edge(n)]
		   ) {
	      node use_po=n;
	      if (use_po==po) {			// - do not create self loops
	      }
	      else {
		war_edges_src.append(use_po);		// - later will add WAR
		war_edges_dst.append(po);		//   edge  use_po-->po
	      }
	      break;  // while (!frontier.empty())	// - done w/this use;
							//   only need 1st asst
	    }
	    list<edge> out_edges_n=(*dfg).out_edges(n);
	    edge e;
	    forall (e,out_edges_n)
	      frontier.append((*dfg).target(e));    // - add outs to frontier
	  }
	}  // forall (e_use,out_edges_pi)  // i.e. forall uses of PI
      }
    }
  }

  while (!war_edges_src.empty()) {	 	 // - create WAR edges
    node use_po=war_edges_src.pop_front();
    node     po=war_edges_dst.pop_front();
    edge e_war=(*dfg).new_edge(use_po,po);	// - add WAR edge
    war_edges.insert(e_war);			//     use_po-->po
    // warn("adding WAR hazard edge (PO-->PO) from asst: "+
    //      (*dfg)[use_po]->getParent()->toString()+" to asst:"+
    //      (*dfg)[po]    ->getParent()->toString());
  }

  return war_edges;
}


set<edge> addRawEdges (BlockDFG *dfg)
{
  // - augment *dfg with RAW hazard edges, and return set of edges
  // - RAW edge is to a node consuming from an annotated edge (use),
  //     from the register write node (PO) that writes the
  //     annotation's register
  //     (in BlockDFG, each local assignment generates a PO node,
  //      even when writing to local vars)
  //     (RAW edge establishes precedence:  write first, read after)
  // - WARNING:  must be called after addWarEdges() if using both
  //             (RAW adds outputs to PO nodes, confuses getOutputNodes())
  // - NOTE:  using addRawEdges() and addWarEdges together
  //          on an arbitrary dfg can create feedback cycles;
  //          but using them on a dfg created by createBlockDfg() +
  //          partitionBlockDfg() (i.e. from well-formed TDF) is fine,
  //          since original/pseudo-SSA TDF code has enough temporary
  //          vars to break any feedback.

  set<edge> raw_edges;

  map<Symbol*,node> po_of;

  set<node> POs=getOutputNodes_POtoPO(dfg);
  node po;
  forall (po,POs) {				// - build po_of : sym-->PO
    Expr *ex=(*dfg)[po];
    if (ex && ex->getExprKind()==EXPR_LVALUE) {
      Symbol *sym=((ExprLValue*)ex)->getSymbol();
      po_of[sym]=po;
    }
  }

  edge e;
  forall_edges (e,*dfg) {
    // 14/2/2012: Nachiket replaced StmtAssign* with int
    // StmtAssign *asst=(*dfg)[e];
    StmtAssign *asst=NULL;
    if (asst) {					// - visit all annotated edges
      Symbol *sym=asst->getLValue()->getSymbol();
      node po=po_of[sym];
      node use=(*dfg).target(e);
      edge raw_edge=(*dfg).new_edge(po,use);	// - add RAW edge po-->use
      raw_edges.insert(raw_edge);
      // warn("adding RAW hazard edge from asst: "+asst->toString()+
      //      "to use expr: "+(*dfg)[use]->toString());
    }
  }

  return raw_edges;
}


void delEdges (BlockDFG *dfg, const set<edge> *edges)
{
  // - delete all edges in edges from *dfg

  edge e;
  forall (e,*edges)
    (*dfg).del_edge(e);
}


////////////////////////////////////////////////////////////////
//  Static timing analysis

void getTimingDepth (BlockDFG *dfg,
		     node_array<int> *latencies,
		     node_array<int> *depths)
{
  // - compute static time depth for each node of DFG,
  //     return it in *depths[]
  // - depth of node n = max over all paths from any PI to node n
  //                     of the sum of node latencies (including n's) on path
  // - *latencies[] = latency per node, is an input to this routine

  // - Depth marking algorithm:  (static timing analysis)
  //                             (this is ASAP topological sort)
  //     initial frontier = PIs
  //     BFS from frontier
  //       if node has all predecessors visited
  //         compute node's depth, add node to frontier
  // - Implementation:  keep node_array of #unvisited predecessors per node,
  //                    for each node visited in BFS, check if #preds=0
  //                    for each node added to frontier, #preds-- for each succ

  depths->init(*dfg,-1);

  set<edge> war_edges=addWarEdges(dfg);	  // - add WAR hazard edges so topol.
					  //   sort visits reg write nodes
					  //   in correct order; remove later.

  node_array<int> numUnvisitedPreds(*dfg,-1);
  set<node> frontier;
  node n;
  forall_nodes (n,*dfg) {
    // - initialize numUnvisitedPreds and frontier
    int preds = numUnvisitedPreds[n] = dfg->indeg(n);
    if (preds==0) {				// - initial frontier same as
      frontier.insert(n);			//   getInputNodes(*dfg),
      (*depths)[n]=(*latencies)[n];		//   node depth = node latency
    }
  }

  while (!frontier.empty()) {

    /*
    // ***
    cerr << "frontier contains:\n";
    node v;
    forall (v,frontier)
      cerr << "  " << v << ' '
	   << ((*dfg)[v] ? (*dfg)[v]->toString() : string("<branch>"))
	   << '\n';
    // ***
    */

    // - frontier invariant:  \forall n \in frontier, numUnvisitedPreds[n]==0
    //                        \forall n \in frontier, depths[n] is known
    node n=frontier.choose();
    frontier.del(n);

    /*
    // ***
    cerr << "choosing " << n << ' '
	 << ((*dfg)[n] ? (*dfg)[n]->toString() : string("<branch>"))
	 << '\n';
    // ***
    */

    edge outedge;
    list<edge> outedges = dfg->out_edges(n);
    forall (outedge,outedges) {
      node succ=dfg->target(outedge);
      if (succ==n)	// ignore self-loops
	continue;

      /*
      // ***
      cerr << "outedge " << succ << ' '
	   << ((*dfg)[succ] ? (*dfg)[succ]->toString() : string("<branch>"))
	   << " has numUnvisitedPreds= " << numUnvisitedPreds[succ] << '\n';
      // ***
      */

      // - update successor depths (incremental max)
      int depth = (*depths)[n] + (*latencies)[succ];
      if ((*depths)[succ]<depth)
	  (*depths)[succ]=depth;

      if (--numUnvisitedPreds[succ]==0)
	// - add successor to frontier
	frontier.insert(succ);
    }
  }

  // - sanity check:  \forall n \in *dfg, numUnvisitedPreds[n]==0
  forall_nodes (n,*dfg) {
    assert(numUnvisitedPreds[n]==0);	// - assert BFS visited node
    assert((*depths)[n]!=-1);		// - assert valid depth
  }

  delEdges(dfg,&war_edges);
}


////////////////////////////////////////////////////////////////
//  Partition BlockDFG

// - Function to map over nodes of a graph:
// - return true to continue map, false to abort
typedef bool (*NodeMap)(graph *g, node n, void *i);


void topologicalSort (graph *g, NodeMap f, void *i, int reverse=false)
{
  // - visit nodes of *g in topological order
  // - if (reverse==true) then use reverse edge order

  node_array<int> numUnvisitedPreds(*g,-1);
  set<node> frontier;
  node n;
  forall_nodes (n,*g) {
    // - initialize numUnvisitedPreds and frontier
    int preds = numUnvisitedPreds[n] = (!reverse ? (*g).indeg(n)
						 : (*g).outdeg(n));
    if (preds==0) {				// - initial frontier same as
      frontier.insert(n);			//   getInputNodes(*g)
    }
  }

  while (!frontier.empty()) {
    // - frontier invariant:  \forall n \in frontier, numUnvisitedPreds[n]==0
    node n=frontier.choose();
    frontier.del(n);
    // - do work
    if (!f(g,n,i))
      return;
    // - update frontier
    list<edge> outedges = (!reverse ? (*g).out_edges(n)
				    : (*g).in_edges(n) );
    edge outedge;
    forall (outedge,outedges) {
      node dst = (!reverse ? (*g).target(outedge)
			   : (*g).source(outedge));
      if (dst==n)				// - ignore self loops
	assert(!"internal inconsistency");	// continue;
      if (--numUnvisitedPreds[dst]==0)
	frontier.insert(dst);
    }  // forall (outedge,outedges)
  }  // while (!frontier.empty())
}


static
int predsBefore (BlockDFG *dfg, node n, int stage, node_array<int> *stage_of)
{
  // - return true iff all predecessors of node n happen in a previous stage,
  //     i.e.      iff all predecessors of node n have  stage_of[pred]<stage

  list<edge> in_edges = (*dfg).in_edges(n);
  edge e;
  forall (e,in_edges) {
    node v=(*dfg).source(e);
    if ((*stage_of)[v]>=stage)
      return false;
  }
  return true;
}


static
int isMovable (BlockDFG *dfg, node n,
	       node_array<int> *latencies,
	       node_array<int> *start_depths,
	       int maxLatency, int maxStage,
	       node_array<int> *stage_of)
{
  // - a 0-latency node at the begining of a partition
  //   should be moved to the end of the previous partition if:
  //     (1) it has latency 0
  //     (2) it is at the begining of the partition
  //     (3) it is not in 1st partition  (could not move back any further)
  //     (4) it has fanout 0             (should consider total I/O gain!)
  //     (5) its predecessor is in previous partition
  // - also, move any 0-latency node (1,2,3,5) past the last real stage
  // - this is a modification to ASAP scheduling
  // - useful for register write nodes
  // - useful for bit-selection nodes   [NOT HANDLED]
  //     (only if source has fanout 1; if source fans out more,
  //      then will not move bit-sel back, so will only need 1 pipe reg
  //      from previous partition, and will fan it out in this partition)
  // - isMovable must called on nodes in topological order,
  //     so we know the moveable predecessors of a node
  //     have already been moved back (as reflected in stage_of[pred])
  // - assume stage_of[n] is not yet defined  (depends on this fn)
  // - maxStage is number of highest "real" stage,
  //     i.e. stage that contains non-0-latency computations;
  //     any 0-latency node scheduled past this stage is movable
  //     by definition (since we don't want anything past the last stage)

  int  start_depth = (*start_depths)[n];
  int  stage       = start_depth / maxLatency;
  return (    stage>0				// - not in 1st partn
	   && (*latencies)[n]==0		// - has 0 latency
	   && stage*maxLatency==start_depth	// - is at start of partn
	   && (   (*dfg).outdeg(n)==0		// - is reg write, branch cond
	       || stage>maxStage		// - or is past last real stage
	       || false              )		// - or is other?  (not yet)
	   && predsBefore(dfg,n,stage,stage_of)
	  );
}


class PartitionInfo {		// - info for partitioning BlockDFG into stages
public:
  BlockDFG        *dfg;
  node_array<int> *latencies;
  node_array<int> *start_depths;
  int              maxLatency;
  int              maxStage;
  node_array<int> *stage_of;	// - output

  PartitionInfo (BlockDFG        *dfg_i,
		 node_array<int> *latencies_i,
		 node_array<int> *start_depths_i,
		 int              maxLatency_i,
		 int              maxStage_i,
		 node_array<int> *stage_of_i)
    : dfg(dfg_i), latencies(latencies_i),   start_depths(start_depths_i),
                  maxLatency(maxLatency_i), maxStage(maxStage_i),
					    stage_of(stage_of_i) {}
};


bool partnNodes_topomap (graph *g, node n, void *i)
{
  // - determine partition (stage) of each node of BlockDFG *g

  BlockDFG     *dfg=(BlockDFG*)g;
  PartitionInfo *pi=(PartitionInfo*)i;
  int stage = (*pi->start_depths)[n] / pi->maxLatency;
  if (isMovable(dfg,n,pi->latencies, pi->start_depths,
		      pi->maxLatency,pi->maxStage,pi->stage_of)) {
    // - special case:  0-latency node at start of partn might move
    //                  to end of prev partn  (e.g. reg write, bit-select)
    stage=stage-1;
  }
  else if ((*dfg).indeg(n)==0 && (*dfg)[n]->getExprKind()==EXPR_VALUE) {
    // - special case: consts are ALAP w.r.t. user, not ASAP & piped
    node user=(*dfg).target((*dfg).first_adj_edge(n));
    stage = ((*pi->start_depths)[user] -
	     (*pi->latencies)[n]        ) / pi->maxLatency;
  }
  (*pi->stage_of)[n] = stage;
  return true;
}


array<set<node> > partitionBlockDFG_ASAP(BlockDFG *dfg,
					 node_array<int> *latencies,
					 node_array<int> *depths,
					 int maxLatency)
{
  // - Partition a blockDFG into a temporally-ordered sequence of node sets
  //      s.t. each partition has:  latency < maxLatency
  // - "node set" == "partition" = "stage"
  // - Each node starts during its own stage
  //     but may complete several stages later if it has long latency
  // - The final stages may be empty, to allow long nodes to complete

  if ((*dfg).number_of_nodes()==0)	// - empty DFG --> single, empty partn
    return array<set<node> >(1);

  // - depths[]       is time at which each node finishes
  // - start_depths[] is time at which each node starts -- compute it
  node_array<int> start_depths(*dfg);
  node n;
  forall_nodes (n,*dfg)
    start_depths[n] = (*depths)[n] - (*latencies)[n];

  // cerr << "Start depth " << printBlockDFG(dfg,NULL,latencies,&start_depths);

  // - partition p will contain nodes n s.t.
  //     p*maxLatency <= start_depth[n] < (p+1)*maxLatency
  // - may have additional, empty partitions to complete ops

  // - determine number of partitions (stages)
  int max_depth=0;
  forall_nodes (n,*dfg) {
    int depth = (*depths)[n];
    if (max_depth<depth)
        max_depth=depth;
  }
  int max_stage = (max_depth==0) ? 0 : ((max_depth-1) / maxLatency);

  // - determine partition (stage) of each node
  // - do this in topological order to properly handle backward moves
  //     of 0 latency nodes (guarantee that preds were already moved)
  node_array<int> stage_of(*dfg);
  PartitionInfo info(dfg,latencies,&start_depths,
		     maxLatency,max_stage,&stage_of);
  topologicalSort(dfg,partnNodes_topomap,&info);

  // - build output partitions
  array<set<node> > partitions(max_stage+1);
  // node n;
  forall_nodes (n,*dfg)
    partitions[stage_of[n]].insert(n);

  return partitions;
}


bool evalExpr_postmap2 (Tree **h, void *i)
{
  // - post-map to recursively constant-fold an expression tree (in place)
  if ((*h)->getKind()==TREE_EXPR)
    (*h)=EvaluateExpr((Expr*)(*h));
  return false;
}


Expr *evalExpr (Expr *e)
{
  // - recursively constant-fold an expression tree
  // - returns e if no folding necessary, else returns newly allocated expr
  // - may modify / reuse pieces of original expr tree
  // - Note, Andre's "EvaluateExpr()" is not properly recursive
  e->map2((Tree**)&e,(TreeMap2)NULL,evalExpr_postmap2,NULL);
  return e;
}


Type* evalType (Type *t)
{
  // - evaluate (constant fold) width expr of a type
  // - return a newly allocated Type, with no pred expr
  // - may modify / reuse pieces of original type tree (width expr tree)
  // - only valid for TYPE_BOOL and TYPE_INT
  // - will type-check to fix TYPE_ANY

  TypeKind tkind=t->getTypeKind();
  if (tkind==TYPE_ANY)			// - fix bad types by type-checking
    return evalType(t->getParent()->typeCheck());
  assert(tkind==TYPE_BOOL || tkind==TYPE_INT || tkind==TYPE_DOUBLE || tkind==TYPE_FLOAT);  
  int width=t->getWidth();
  if (tkind==TYPE_BOOL) {
    // - bool
    return new Type(TYPE_BOOL);
  }
  else if (width>=0) {
    // - int, width known
    return new Type(tkind,width,t->isSigned());
  }
  else {
    // - int, width unknown
    Expr       *widthExpr = t->getWidthExpr();
	  
    Expr *evaledWidthExpr = evalExpr(widthExpr);
    if (evaledWidthExpr==widthExpr)
      evaledWidthExpr = (Expr*)widthExpr->duplicate();
    return new Type(tkind,evaledWidthExpr,t->isSigned());
  }
}


Type* makeBitSelType (BlockDFG *dfg, node n)
{
  // - create (allocate) and return a Type for an ExprBitSel DFG node n
  // - need this because ExprBitSel implementation is not complete
  //     (has no type; in fact, has no arg being bit-subscripted!)
  // - note, ExprBitSel node n has either:
  //     (i)  fanin edge with asst, for live def of sym being subscripted
  //     (ii) fanin edge from PI,   for externally defined sym
  //            (ExprLValue of PI has the bit subscript, but we ignore it)

  ExprBitSel* bitsel=(ExprBitSel*)(*dfg)[n];
  assert(bitsel->getExprKind()==EXPR_BITSEL);
  assert((*dfg).indeg(n)==1);
  edge  ein=(*dfg).first_in_edge(n);
  Type* tin;				// - type of sym being subscripted
  //StmtAssign *asst=(*dfg)[ein];		// - live def of sym being subscripted
  StmtAssign *asst=NULL;		// - live def of sym being subscripted
  if (asst)
    tin=asst->getLValue()->getSymbol()->getType();  // - sym type from live def
  else {
    node n_pred=(*dfg).source(ein);
    ExprLValue *lval=(ExprLValue*)(*dfg)[n_pred];
    assert(lval->getExprKind()==EXPR_LVALUE);
    tin=lval->getSymbol()->getType();		    // - sym type from PI
  }
  int lo=bitsel->getPosLow();
  int hi=bitsel->getPosHigh();
  if (hi>=0) {
    // - bit field
    return new Type(TYPE_INT,hi-lo+1,tin->isSigned());
  }
  else {
    // - single bit
    return new Type(TYPE_INT,1,tin->isSigned());
  }
}


void reformBlockDFGs (BlockDFG *dfg, array<set<node> > *partitions,
		      array<BlockDFG> *new_dfgs, set<SymbolVar*> *new_vars)
{
  // - Given *dfg and sequence of partitions,
  //     form a sequence of partition dfgs     (fill *new_dfgs)
  //     and create pipeline vars between them (fill *new_vars)
  // - Note, original TDF code is unmodified
  //     (new_vars are not inserted into any symbol table_
  //     (pipeline regs are not visible; retain refs to orig regs)

  // for each dfg
  //   assume input vars exist
  //   form input nodes
  //   scan outputs, create new output vars
  //   form output nodes

  int pipe_var_num=0;	// - count to generate unique local names for pipe vars

  int num_partitions=(*partitions).size();	// - initialize output arrays
  (*new_dfgs)=array<BlockDFG>(num_partitions);
  (*new_vars).empty();

  // for (int i=0; i<num_partitions; i++)	// - manually init dfgs in
  //   (*new_dfgs)[i]=BlockDFG();		//   new_dfgs for fakeLEDA's
						//   broken GRAPH copy cnstrctr

  node_array<SymbolVar*>  new_var_map (*dfg,NULL);  // - remember new pipe var
  node_array<ExprLValue*> new_lval_map(*dfg,NULL);  //   for a node's outputs
  node_array<StmtAssign*> new_asst_map(*dfg,NULL);

  node_array<node> po_of(*dfg,NULL);		// - map node i -> PO node j
  node n;					//     if edge (i,j) exists
						//     and j is reg write
						// - need this later to reuse
						//     a reg var as a pipe reg
  forall_nodes (n,*dfg) {
    Expr *n_expr;
    if (   (*dfg).outdeg(n)==0
	&& (n_expr=(*dfg)[n])
	&& n_expr->getExprKind()==EXPR_LVALUE
	&& ((ExprLValue*)n_expr)->getSymbol()->getSymKind()==SYMBOL_VAR)
    {
      assert((*dfg).indeg(n)==1);
      node src=(*dfg).source((*dfg).first_in_edge(n));
      po_of[src]=n;
    }
  }

  for (int partition_num=0; partition_num<num_partitions; partition_num++)
  {
    // warn(string("reformBlockDFGs:  partition %d",partition_num));

    set<node> &partition = (*partitions)[partition_num];
    if (partition.empty())	// - empty partn (pure delay) -> empty DFG
      continue;

    // - create induced new DFG for partition
    BlockDFG &new_dfg = (*new_dfgs)[partition_num];
    node_array<node> new_nodes(*dfg,NULL);  // map dfg node -> new_dfg node
    edge_array<edge> new_edges(*dfg,NULL);  // map dfg edge -> new_dfg edge
    node n;
    // - (reconstruct nodes, in arbitrary order)
    forall (n,partition)
      new_nodes[n] = new_dfg.new_node((*dfg)[n]);
    // Nachiket removed this: edge e;
    /*
    // - (reconstruct edges, retaining order of main edge list - WRONG)
    forall_edges (e,*dfg) {
      node src=(*dfg).source(e);
      node dst=(*dfg).target(e);
      if (partition.member(src) && partition.member(dst))
	new_edges[e] = new_dfg.new_edge(new_nodes[src],
					new_nodes[dst],(*dfg)[e]);
    }
    */
    /*
    // - (reconstruct edges, retaining order of output adjacency lists - WRONG)
    forall (n,partition) {
      list<edge> outs=(*dfg).out_edges(n);
      edge e;
      forall (e,outs) {
	node src=(*dfg).source(e);	// = n
	node dst=(*dfg).target(e);
	if (partition.member(src) && partition.member(dst))
	  new_edges[e] = new_dfg.new_edge(new_nodes[src],
					  new_nodes[dst],(*dfg)[e]);
      }
    }
    */
    // - (reconstruct edges, retaining order of output & input adjacency lists)
    forall (n,partition) {
      list<edge> outs=(*dfg).out_edges(n);
      edge e;
      forall (e,outs) {
	node src=(*dfg).source(e);	// = n
	node dst=(*dfg).target(e);
	if (partition.member(src) && partition.member(dst)) {
	  // - add edge new_nodes[src] -> new_nodes[dst]
	  //   by appending to out_nodes(new_nodes[src]),
	  //     inserting into in_nodes(new_nodes[dst])
	  node new_src=new_nodes[src];
	  node new_dst=new_nodes[dst];
	  if (new_dfg.indeg(new_dst)==0)	// be only in new_dst ins
	    new_edges[e] = new_dfg.new_edge(new_src,new_dst,(*dfg)[e]);
	  else {
	    list<edge> dst_ins=(*dfg).in_edges(dst);
	    edge dst_in, new_dst_in_before_e=NULL;
	    forall (dst_in,dst_ins) {
	      if (dst_in==e)
		break;
	      else {
		edge new_dst_in = new_edges[dst_in];
		if (new_dst_in)
		  new_dst_in_before_e = new_dst_in;
	      }
	    }
	    if (new_dst_in_before_e) {		// be not first in new_dst ins
	      new_edges[e] = new_dfg.new_edge(new_src,new_dst_in_before_e,
					      (*dfg)[e],LEDA::after);
	    }
	    else {				// be first in new_dst ins
	      edge new_dst_in_after_e = new_dfg.in_edges(new_dst).head();
	      new_edges[e] = new_dfg.new_edge(new_src,new_dst_in_after_e,
					      (*dfg)[e],LEDA::before);
	    }
	  }
	}
      }
    }

    // - map dfg node --> new_dfg node,  to remember new PI nodes in this partn
    //     (domain node "posrc" is piped thru reg in new partitions,
    //      though may not be piped in orig DFG)
    node_array<node> posrc_to_new_pi(*dfg,NULL);

    // - create primary input/output nodes (PI, PO) for pipe regs in new DFG
    forall (n,partition) {
      // - visit all nodes n of partition
      // - if n has unnamed output to another partition, then
      //     create a var/asst for it, remember new var, create PO node
      // - should never find unnamed input from another partition
      // warn("reformBlockDFGs:  node "+
      //      ((*dfg)[n] ? (*dfg)[n]->toString() : string("<branch>")));

      // - outputs from n to next partition --> create PO
      list<edge> dfg_out_edges_n=(*dfg).out_edges(n);
      edge e;
      forall (e,dfg_out_edges_n) {
	// - examine outputs of n
	node dst=(*dfg).target(e);
	// warn("reformBlockDFGs:  has dst "+
	//      ((*dfg)[dst] ? (*dfg)[dst]->toString() : string("<branch>")));
	if (!partition.member(dst)) {
	  // - e is output to another partition
	  // - find or create var, lval, asst
	  // - will create pipe reg & PO unless:
	  //     (1) e is asst to reg var whose PO is in this partition,
	  //           then reuse that reg var instead of new pipe reg
	  //     (2) already created pipe_reg for n, then do nothing
	  // warn("reformBlockDFGs:  non-local dst "+(*dfg)[dst]->toString());
	  StmtAssign *asst=NULL;	// - asst for e
	  ExprLValue *lval=NULL;	// - lval for e
	  Symbol     *var =NULL;	// - var  for e (may be stream)
	  /*
	  // - old asst does not really matter... this code is wrong
	  if (   (asst=(*dfg)[e])
	      && (lval=asst->getLValue())
	      && (var =lval->getSymbol())
	      &&  var->getSymKind()==SYMBOL_VAR
	      &&  partition.member(po_of[n])   ) {
	    // - if assigning to a reg var, use var as a pipe reg
	    new_var_map [n]=(SymbolVar*)var;
	    new_lval_map[n]=lval;
	    new_asst_map[n]=asst;
	  }
	  */
	  node po=po_of[n];
	  if (po && partition.member(po)) {
	    // - if assigning to a reg var, use var as a pipe reg
	    lval = new_lval_map[n] = (ExprLValue*)(*dfg)[po];
	    asst = new_asst_map[n] = (StmtAssign*)lval->getParent();
	    var  = new_var_map [n] = (SymbolVar*) lval->getSymbol();
	  }
	  else if ((var=new_var_map[n])) {
	    // - if already created pipe reg from fanout of n, then do nothing
	    lval = new_lval_map[n];
	    asst = new_asst_map[n];
	  }
	  else {
	    // - need new pipe reg:  create var, lval, asst
	    string varName("_pipe_%d",pipe_var_num++);
	    Expr *n_expr=(*dfg)[n];
	    Type *varType = (n_expr->getExprKind()==EXPR_BITSEL)  // make type
		          ? makeBitSelType(dfg,n)
			  : (n_expr->getExprKind()==EXPR_LVALUE)  // ignorebits
			  ? evalType(((ExprLValue*)n_expr)->getSymbol()
							  ->getType()  )
		          : evalType(n_expr->getType());	  // otherwise
				// - evalType() dups + const-folds the type
	    // Type *varType=(Type*)( n_expr->getType()->duplicate() );  // dup
	    // - create pipe reg with same type as n_expr
	    // - n_expr may have TYPE_ANY from overwrite
	    // - if we are just copying the type, then we can fix a bad
	    //     type later in reconstructTDF() by n_expr->typeCheck()
	    // - if we are constant-folding the type
	    //     (namely to avoid wierd width expressions like 8<8?8:8),
	    //     then must fix a bad type now (typeCheck in evalType())
	    SymbolVar  *new_var =new SymbolVar (NULL,varName,varType);
	    ExprLValue *new_lval=new ExprLValue(NULL,new_var);
	    StmtAssign *new_asst=new StmtAssign(NULL,new_lval,n_expr);
	    // warn("created pipe reg "+varType->toString()+" "+varName+
	    //      " for assignment "+new_asst->toString());
	    var  = new_var_map [n] = new_var;
	    lval = new_lval_map[n] = new_lval;
	    asst = new_asst_map[n] = new_asst;
	    (*new_vars).insert(new_var);
	    // - create primary output node
	    // - note, edge def->PO should NOT be annotated
	    //     with assignment to the PO var!  [was: asst]
	    node po =new_dfg.new_node(lval);
	    edge poe=new_dfg.new_edge(new_nodes[n],po,NULL);
	    assert(poe);
	  }
	}
      }


      // - inputs to n from previous partition --> create PI 
      list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
      int edgenum=0;
      forall (e,dfg_in_edges_n) {
	// - examine inputs of n
	node src=(*dfg).source(e);
	// warn("reformBlockDFGs:  has src "+(*dfg)[src]->toString());
	if (!partition.member(src)) {
	  // - e is input from another partition
	  // - find var (or stream), lval, asst  (either orig or new pipe var)
	  // warn("reformBlockDFGs:  non-local src "+(*dfg)[src]->toString());
	  StmtAssign *asst = new_asst_map[src];	// - not orig asst (*dfg)[e]
	  ExprLValue *lval = asst->getLValue();	//     (is asst to pipe,
	  Symbol     *var  = lval->getSymbol(); //      if used pipe, I think)
	  assert(asst);
	  assert(lval);
	  assert(var);
	  /*
	  if ((*dfg).outdeg(n)==0 && (*dfg)[n]) {
	    // - special case:  n is PO for var/stream write,
	    //			but value is in previous partition;
	    //                  create new assignment from pipe reg
	    // - WHY DO WE DO THIS?  (EC 4/1/02)   ***
	    // - DEFUNCT CODE        (EC 4/19/02)  ***
	    ExprLValue *dst_lval=(ExprLValue*)(*dfg)[n];
	    assert(dst_lval->getExprKind()==EXPR_LVALUE);
	    asst = new StmtAssign(NULL,dst_lval,
				  (ExprLValue*)lval->duplicate());
	  }
	  */
	  // - create primary input node
	  // node pi = new_dfg.new_node((ExprLValue*)lval->duplicate());
	  // - reuse or create primary input node
	  node pi = posrc_to_new_pi[src];
	  if (!pi) {
	    // warn("SQUAK - NEW PI");			// ***
	    pi = posrc_to_new_pi[src]
	       = new_dfg.new_node((ExprLValue*)lval->duplicate());
	  }
	  else {
	    // Nachiket removed this: Expr *e=new_dfg[pi];
	    // warn("SQUAK - REUSE PI "+e->toString());	// ***
	  }
	  // - connect primary input node, maintain order of target edge list
	  // - if do not care about order of target edge list, use:
	  //     edge pie=new_dfg.new_edge(pi,new_nodes[n],NULL);
	  // - note, edge PI->use should NOT be annotated
	  //     with (new) assignment to the PI var!  (was: asst)
	  edge pie;
	  list<edge> new_dfg_in_edges_n=new_dfg.in_edges(new_nodes[n]);
	  if (edgenum==0) {
	    if (new_dfg_in_edges_n.empty())
	      pie=new_dfg.new_edge(pi,new_nodes[n],NULL);
	    else {
	      edge new_nextedge=new_dfg_in_edges_n.head();
	      pie=new_dfg.new_edge(pi,new_nextedge,0, LEDA::before);
	    }
	  }
	  else {
	    edge new_prevedge=new_dfg_in_edges_n.inf(
			      new_dfg_in_edges_n.get_item(edgenum-1));
	    pie=new_dfg.new_edge(pi,new_prevedge, 0, LEDA::after);
	  } 
	  assert(pie);
	}
	edgenum++;
      }
    }	// forall (n,partition)
  }	// for (int partition_num=0...)
}


void partitionBlockDFG (BlockDFG *dfg,
			node_array<int> *areas,
			node_array<int> *latencies,
			int maxArea,
			int maxLatency,
			array<BlockDFG> *new_dfgs,
			set<SymbolVar*> *new_vars)
{
  // - Convert state's *dfg into a sequence of state dfgs
  //     s.t. each new_dfg has area<maxArea, latency<maxLatency
  // - Return new dfgs in *new_dfgs
  // - Return new variables in *new_vars (for inter-state comm)
  //     (new vars are not inserted into any symbol table)
  // - NOTE: areas / maxArea are ignored for now

  // - compute node static timing depths
  node_array<int> depths;
  getTimingDepth(dfg,latencies,&depths);
  // cerr << "Depth known " << printBlockDFG(dfg,areas,latencies,&depths);

  // - partition dfg into node sets
  array<set<node> > nodesets = partitionBlockDFG_ASAP(dfg,latencies,&depths,
						      maxLatency);
  /*
  string nodeset_msg="Partition into node-sets:\n";
  int nodeset_num=0;
  set<node> dummy, &s=dummy;
  forall (s,nodesets) {
    nodeset_msg += string("%d:\n",nodeset_num++);
    node n;
    forall (n,s) {
      Tree *t=(*dfg)[n];
      nodeset_msg += "  " + (t ? t->toString() : string("<nil>")) + "\n";
    }
  }
  cerr << nodeset_msg;
  */

  // - form stand-alone dfgs from partitions
  reformBlockDFGs(dfg,&nodesets,new_dfgs,new_vars);
  /*
  for (int pnum=0; pnum<(*new_dfgs).size(); pnum++)
    cerr << "Partition " << pnum << " " << printBlockDFG(&(*new_dfgs)[pnum]);
  */
}


////////////////////////////////////////////////////////////////
//  Reconstruct TDF

Expr* reconstructTDF_node (BlockDFG *new_dfg, node n);
Expr* reconstructTDF_edge (BlockDFG *new_dfg, edge e);


Expr* reconstructTDF_edge (BlockDFG *new_dfg, edge e, Expr *ee)
{
  // - wrapper for reconstructTDF_node(BlockDFG*,node)
  //     with node n = source(e)
  // - Expr *ee is arg of (*new_dfg)[n] corresponding to input edge e of n
  // - if e is assignment to var then return var (lvalue w/o bit subscript)
  //     else return reconstructed TDF for n subtree

  /*
  warn("reconstructTDF_edge(" +
       ((*new_dfg)[e] ? (*new_dfg)[e]->toString() : string("<nil>")) + ")");
  */

  //StmtAssign *asst=(*new_dfg)[e];
  StmtAssign *asst=NULL;
  if (asst) {
    // - return sym ref (dup) without bit-sel
    ExprLValue *lval=asst->getLValue();
    ExprLValue *new_lval=new ExprLValue(lval->getToken(),lval->getSymbol());

    // - rebuild type, or else getType() crashes later
    //     due to inconsistent parent/child pointers
    // - no longer necessary (fix parent pointers in reconstructTDF_node())
    // assert(new_lval->typeCheck());	// ***

    return new_lval;
  }
  else
    return reconstructTDF_node (new_dfg,(*new_dfg).source(e));
}


Expr* reconstructTDF_node (BlockDFG *new_dfg, node n)
{
  // - given a node n of *new_dfg,
  //     reconstruct & return a TDF expr for the subtree rooted at n
  // - will attempt to reuse the existing expr:  (*new_dfg)[n],
  //     will modify it if it is inconsistent (esp. w.r.t. bit range).
  // - HACK:  overwrite tree nodes in place,
  //          and fix parent pointers in subtrees
  //          (do not thread here for efficiency-- would cost O(n^2);
  //           instead, will thread later when overwriting stmts)
  //          (note, must thread if want to print an overwritten node,
  //           or else t->toString() will barf on bad parent pointers;
  //           actually, that's no longer true, since we fix parent ptrs)
  // - assume that node's input edges are properly ordered
  //     as created by createBlockDfg()
  // - assume no bit subscripts in reg write PO

  /*
  warn("reconstructTDF_node(" +
       ((*new_dfg)[n] ? (*new_dfg)[n]->toString() : string("<nil>")) + ")");
  warn("  with parent " +
       ((*new_dfg)[n] ?
	(*new_dfg)[n]->getParent() ? (*new_dfg)[n]->getParent()->toString()
				   : string("<nil>") : string("<n/a>")));
  */

  list<edge> in_edges=(*new_dfg).in_edges(n);
  Expr *ne = (*new_dfg)[n];
  Tree *p  = ne->getParent();	if(p);
  switch (ne->getExprKind())
  {

    case EXPR_VALUE:	{
			  return ne;
			}
    case EXPR_LVALUE:	{
			  assert((*new_dfg).indeg(n)==0);	// PI
			  // - return sym ref w/o bit-sel
			  ExprLValue *lval=(ExprLValue*)ne;
			  ExprLValue *new_lval=
			    new ExprLValue(lval->getToken(),lval->getSymbol());

			  // - rebuild type, or else getType() crashes later
			  //     due to inconsistent parent/child pointers
			  // - no longer necessary
			  //     (fix parent pointers in reconstructTDF_node())
			  // assert(new_lval->typeCheck());	// ***

			  // warn("*** reconstructed "+new_lval->toString()+
			  //      " with reconstructed type "+
			  //      new_lval->getType()->toString());
			  return new_lval;
			}
    case EXPR_BITSEL:	{
			  assert((*new_dfg).indeg(n)==1);
			  edge in=in_edges.inf(in_edges.get_item(0));
			  Expr *lval=reconstructTDF_edge(new_dfg,in,NULL);
			  assert(lval->getExprKind()==EXPR_LVALUE);
			  int lo=((ExprBitSel*)ne)->getPosLow();	
			  int hi=((ExprBitSel*)ne)->getPosHigh();
			  Expr *elo = lo>=0 ? constIntExpr(lo) : NULL;
			  Expr *ehi = hi>=0 ? constIntExpr(hi) : NULL;
			  *(ExprLValue*)lval =		// - HACK: overwrite
			    ExprLValue(((ExprLValue*)lval)->getToken(),
				       ((ExprLValue*)lval)->getSymbol(),
				       elo,ehi);

			  // - rebuild type, or else getType() crashes later
			  //     due to inconsistent parent/child pointers
			  // - no longer necessary
			  //     (fix parent pointers in reconstructTDF_node())
			  // assert(lval->typeCheck());	// ***

			  if (elo) elo->setParent(ne);
			  if (ehi) ehi->setParent(ne);
			  // lval->thread(p);	// - DEBUG: only for warning
			  return lval;
			}
    case EXPR_UOP:	{
			  assert((*new_dfg).indeg(n)==1);
			  edge in=in_edges.inf(in_edges.get_item(0));
			  Expr *ein=((ExprUop*)ne)->getExpr();
			  *(ExprUop*)ne =		// - HACK: overwrite
			    ExprUop(ne->getToken(),
				    ((ExprUop*)ne)->getOp(),
				    reconstructTDF_edge(new_dfg,in,ein));
			  ein->setParent(ne);
			  // ne->thread(p);	// - DEBUG: only for warning
			  return ne;
			}
    case EXPR_BOP:	{
			  assert((*new_dfg).indeg(n)==2);
			  edge in1=in_edges.inf(in_edges.get_item(0));
			  edge in2=in_edges.inf(in_edges.get_item(1));
			  Expr *ein1=((ExprBop*)ne)->getExpr1();
			  Expr *ein2=((ExprBop*)ne)->getExpr2();
			  *(ExprBop*)ne =		// - HACK: overwrite
			    ExprBop(ne->getToken(),
				    ((ExprBop*)ne)->getOp(),
				    reconstructTDF_edge(new_dfg,in1,ein1),
				    reconstructTDF_edge(new_dfg,in2,ein2));
			  ein1->setParent(ne);
			  ein2->setParent(ne);
			  // ne->thread(p);	// - DEBUG: only for warning
			  // warn("reconstructTDF_node: created "+ne->toString());
			  return ne;
			}
    case EXPR_COND:	{
			  assert((*new_dfg).indeg(n)==3);
			  edge inC=in_edges.inf(in_edges.get_item(0));
			  edge inT=in_edges.inf(in_edges.get_item(1));
			  edge inF=in_edges.inf(in_edges.get_item(2));
			  Expr *einC=((ExprCond*)ne)->getCond();
			  Expr *einT=((ExprCond*)ne)->getThenPart();
			  Expr *einF=((ExprCond*)ne)->getElsePart();
			  *(ExprCond*)ne =		// - HACK: overwrite
			    ExprCond(ne->getToken(),
				     reconstructTDF_edge(new_dfg,inC,einC),
				     reconstructTDF_edge(new_dfg,inT,einT),
				     reconstructTDF_edge(new_dfg,inF,einF));
			  einC->setParent(ne);
			  einT->setParent(ne);
			  einF->setParent(ne);
			  // ne->thread(p);	// - DEBUG: only for warning
			  return ne;
			}
    case EXPR_CAST:	{
			  assert((*new_dfg).indeg(n)==1);
			  edge in=in_edges.inf(in_edges.get_item(0));
			  Expr *ein=((ExprCast*)ne)->getExpr();
			  *(ExprCast*)ne =		// - HACK: overwrite
			    ExprCast(ne->getToken(),
				     ((ExprCast*)ne)->getType(),
				     reconstructTDF_edge(new_dfg,in,ein));
			  ein->setParent(ne);
			  // ne->thread(p);	// - DEBUG: only for warning
			  return ne;
			}
    case EXPR_BUILTIN:  {
			  // - cat(), bitsof()
			  list<Expr*> *args=((ExprBuiltin*)ne)->getArgs();
			  list_item it_in, it_arg;
			  for (it_in  = in_edges.first(),	    // - init
			       it_arg =  (*args).first();
			       it_in;				    // - test
			       it_in  = in_edges.succ(it_in),
			       it_arg =  (*args).succ(it_arg)) {    // - inc
			    // - simultaneously walk over in edges & args
			    edge  in  = in_edges.inf(it_in);
			    Expr *arg =  (*args).inf(it_arg);
			    Expr *new_arg=reconstructTDF_edge(new_dfg,in,arg);
			    // - HACK: reconstructTDF_edge() may overwrite arg
			    //         (and return new_arg==arg),
			    //         otherwise it allocs+returns a new expr
			    (*args).assign(it_arg,new_arg);
			    arg->setParent(ne);
			    // arg->thread(ne);	// - DEBUG: only for warning
			  }
			  return ne;
			}
    default:		{
			  fatal(1,"unknown expression kind in DFG "
				  "while reconstructing TDF", ne->getToken());
			  return NULL;
			}
  }
}


bool setIfCond_map (Tree *t, void *i)
{
  // - find "if" stmt in stmt tree, set its condition to (Expr*)i
  // - HACK: overwrite StmtIf in place

  Expr *cond=(Expr*)i;

  if (t->getKind()==TREE_STMT) {
    if (((Stmt*)t)->getStmtKind()==STMT_IF) {
      StmtIf *sif=(StmtIf*)t;
      Tree *p=sif->getParent();
      *sif = StmtIf(sif->getToken(),cond,
		    sif->getThenPart(),sif->getElsePart());	// - HACK
      // sif->getCond()    ->setParent(sif);
      // sif->getThenPart()->setParent(sif);
      // if (sif->getElsePart())
      //     sif->getElsePart()->setParent(sif);
      // - setting these parent pointers does not suffice;
      //     partition + reconstruct may leave an ExprLValue leaf
      //     with dangling (local) parent ptr (why?)
      //     (e.g. jpeg_encode_nopar), so we thread anyway.
      sif->thread(p);
      return false;
    }
    else if (((Stmt*)t)->getStmtKind()==STMT_BLOCK)
      return true;
    else
      return false;
  }
  else
    return false;
}


bool buildTopolOrder_topomap (graph *g, node n, void *i)
{
  // - build list of nodes in topological order in *i:list<node>
  // - specifically, just append n to *i:list<node>

  list<node> *nodes=(list<node>*)i;
  nodes->append(n);
  return true;
}


Expr* reconstructTDF (BlockDFG *new_dfg, list<Stmt*> *stmts,
					 list<Stmt*> *nondfstmts)
{
  // - given *new_dfg, reconstruct corresponding stmt list in *stmts
  // - will attempt to reuse stmts/exprs in *new_dfg,
  //     will modify/fix them if they are inconsistent (after time partn)
  // - HACK:  overwrite stmts/exprs in place
  // - will append *nondfstmts to end of reconstructed *stmts
  // - return reconstructed expr for "if" branch condition,
  //     if find one in *new_dfg (node w/edge to NULL-info sink node),
  //     else return NULL

  // visit *new_dfg nodes in topological order
  //   for each reg/stream write PO
  //     reconstruct subtree (cone) of def and create asst to PO var
  //   for branch-condition output successor
  //     reconstruct subtree (cone) of def and remember for return value

  stmts->clear();
  Expr *branchcond=NULL;

  // - prepare topological sort
  // - add WAR/RAW hazard edges so topol. sort visits POs in proper order
//set<edge> war_edges=addWarEdges(new_dfg);	    // - add WAR hazard edges
  set<edge> war_edges=addWarEdges_POtoPO(new_dfg);  // - add WAR hazard edges
  set<edge> raw_edges=addRawEdges(new_dfg);	    // - add RAW hazard edges
  list<node> nodesInTopolOrder;
  topologicalSort(new_dfg,buildTopolOrder_topomap,&nodesInTopolOrder);
  delEdges(new_dfg,&raw_edges);
  delEdges(new_dfg,&war_edges);

  // - create assignment stmts from DFG

  while (!nodesInTopolOrder.empty()) {
    node n=nodesInTopolOrder.pop_front();

    /*
    // ***
    cerr << "reconstructTFD() choosing " << n << ' '
	 << ((*new_dfg)[n] ? (*new_dfg)[n]->toString() : string("<branch>"))
	 << '\n';
    // ***
    */

    if (isPO(new_dfg,n)) {
      Expr *n_expr=(*new_dfg)[n];
      if (!n_expr) {
	// - n is branch condition output  (reconstruct expr & put it aside)
	node src=(*new_dfg).source((*new_dfg).first_in_edge(n));
	branchcond=reconstructTDF_node(new_dfg,src);
      }
      else {
	// - n is reg/stream write PO  (create asst for it)
	ExprLValue *lval=(ExprLValue*)n_expr;		// - LHS of asst to PO
	assert(lval && lval->getExprKind()==EXPR_LVALUE);
	StmtAssign *asst=(StmtAssign*)lval->getParent();  //        asst to PO
	assert(asst && asst->getStmtKind()==STMT_ASSIGN);
	assert(asst->getLValue()==lval);
	Tree *p=asst->getParent();
	edge e=(*new_dfg).first_in_edge(n);
	// warn(string(" - about to reconstruct asst %p ",asst)+
	//      asst->toString());					// ***
	Expr *n_expr=reconstructTDF_edge(new_dfg,e,NULL); // RHS of asst to PO
	// - reconstruct n subtree expr from edge n-->po, not from n itself,
	//     so can see edge annotation and use DF thru annotation reg
	n_expr->typeCheck();	// - fix reconstructed exprs that have TYPE_ANY
	// asst->thread(p);	// - DEBUG: needed only for warn on next line
				//          (actually, no longer necessary)
	// warn(string(" - about to overwrite asst   %p ",asst)+
	//      asst->toString());				    // ***
	*asst=StmtAssign(asst->getToken(),lval,n_expr);	    // HACK: overwrite
	// lval->setParent(asst);
	// n_expr->setParent(asst);
	// - setting these parent pointers does not suffice;
	//     partition + reconstruct may leave an ExprLValue leaf
	//     with dangling (local) parent ptr (why?)
	//     (e.g. jpeg_encode_nopar), so we thread anyway.
	asst->thread(p);
	// warn(string(" - done with asst            %p ",asst)+
	//      asst->toString());					// ***
	stmts->append(asst);
      }
    }  // if (isPO(n))
  }  // while (!nodesInTopolOrder.empty())

  if (nondfstmts) {
    // - patch branch condition
    Stmt *s;
    forall (s,*nondfstmts)
      s->map(setIfCond_map,(TreeMap)NULL,branchcond);
    
    // - append *nondfstmts to *stmts
    forall (s,*nondfstmts)
      stmts->append(s);
  }

  return branchcond;
}


void reconstructTDF_onecase (OperatorBehavioral *op,
			     StateCase          *statecase,
			     BlockDFG           *dfg,
			     list<Stmt*>        *nondfstmts,
			     set<SymbolVar*>    *locals,
			     array<BlockDFG>    *new_dfgs,
			     set<SymbolVar*>    *new_vars,
			     list<State*>       *new_states)
{
  // - reconstruct TDF for a single, partitioned state-case
  // - given a state-case's DFG *dfg with *nondfstmts and *locals,
  //     partitioned into *new_dfgs with *new_vars,
  //     reconstruct a corresponding sequence of states in *new_states,
  //     and add them to parent operator *op
  // - modifies the original *statecase, and adds regs to *op
  // - locals     are added to 1st  partition
  // - nondfstmts are added to last partition

  int num_new_states=(*new_dfgs).size()-1;
  // if (num_new_states<=0)	// skip reconstruction if no new states: BAD
  //   return;
  // - NO, reconstruct TDF even if number of states did not change,
  //       since TDF->DFG->TDF does dead-code elimination

  // - prepare list of resulting states *new_states (with no state-cases)
  // - for convenience, we put the original state at the front of the list
  //     and treat it like any other state (we replace its state-case),
  //     but we will remove it from the list before exiting this fn
  State *orig_state=(State*)statecase->getParent();
  /*
  if (op!=orig_state->getParent())
    warn("big doodoo:  op="+op->getName()+" but state parent="+		// ***
	 (orig_state->getParent() ?
	  ((Operator*)orig_state->getParent())->getName() : string("<nil>"))+
	 "; state-case is:\n"+statecase->toString());
  */
  assert(op==orig_state->getParent());
  orig_state->removeCase(statecase);
  new_states->clear();
  new_states->append(orig_state);
  for (int i=0; i<num_new_states; i++) {
    State *new_state=new State(NULL,orig_state->getName()); // - unique rename
    new_states->append(new_state);			    //     later
  }

  Expr *branchcond=NULL;	// - condition for exit "if", in unknown partn
  
  int partition_num=0, num_partitions=(*new_dfgs).size();
  for (partition_num=0; partition_num<num_partitions; partition_num++) {
    BlockDFG &new_dfg=(*new_dfgs)[partition_num];
    State *new_state=(*new_states).inf((*new_states).get_item(partition_num));
    list<Stmt*> new_stmts;
    Expr *branchcond_or_null = reconstructTDF(&new_dfg,&new_stmts,NULL);
    if (branchcond_or_null)
      branchcond=branchcond_or_null;

    /*
    cerr << "Reconstructed partition " << partition_num << ":\n";
    Stmt *s;
    forall (s,new_stmts)
      cerr << s->toString();
    */

    if (partition_num<num_partitions-1) {
      // - intermediate partition; add "goto" next state
      State *next_state=(*new_states).inf(
			(*new_states).get_item(partition_num+1));
      StmtGoto *new_goto=new StmtGoto(NULL,next_state);
      new_stmts.append(new_goto);
    }
    else {
      // - final partition; add *nondfstmts (with fixed branch cond)
      if (nondfstmts) {
	Stmt *s;
	forall (s,*nondfstmts) {
	  if (branchcond)
	    s->map(setIfCond_map,(TreeMap)NULL,branchcond);
	  new_stmts.append(s);
	}
      }
    }

    list<InputSpec*> *new_ispecs = (partition_num==0)
					? statecase->getInputs()
					: new list<InputSpec*>;
    StmtBlock *new_block=new StmtBlock(NULL,new SymTab(SYMTAB_BLOCK),
				       new list<Stmt*>(new_stmts));
    if (partition_num==0) {
      // - rename and add locals to 1st partition
      SymTab    *symtab=new_block->getSymtab();
      SymbolVar *sym;
      forall (sym,*locals) {
	uniqueRenameSym_inc(sym,symtab);
	symtab->addSymbol(sym);
      }
    }
    list<Stmt*> *new_stmtlist=new list<Stmt*>;
    new_stmtlist->append(new_block);
    StateCase *new_case=new StateCase(NULL,new_ispecs,new_stmtlist);
    new_state->addCase(new_case);
    // warn("reconstructed state-case "+new_case->toString());

    // warn(string("  state now has %d cases",new_state->getCases()->size()));
  }

  // - rename pipe regs (and SSA tmp regs), add them to op
  SymTab *symtab=op->getVars();
  Symbol *sym;
  forall (sym,*new_vars) {
    uniqueRenameSym_inc(sym,symtab);
    symtab->addSymbol(sym);
  }

  // - remove orig state from *new_states
  new_states->pop_front();

  // - clean and add *new_states to *op
  // - if there are multiple partitions, each will be renamed:  <origname>_n
  string new_name = (num_partitions==1) ? (orig_state->getName())
					: (orig_state->getName()+"_0");
  orig_state->setName(new_name);
  State *new_state;
  forall (new_state,*new_states) {
    // warn("touching up state "+new_state->getName()+
    //      string(" with %d cases",new_state->getCases()->size()));

    new_state->setName(new_name);
    uniqueRenameState_inc(new_state,op->getStates());	    // - fix state name

    StateCase *sc=new_state->getCases()->choose();
    // warn(string("lifting in state-case %p: ",sc)+sc->toString());
    liftOrphanedLocals(sc,op);
    // warn("done touching up state\n"+new_state->toString());

    liftOrphanedLocals(new_state->getCases()->choose(),op); // - lift vars
    op->addState(new_state);				    // - add state
  }
}


////////////////////////////////////////////////////////////////
//  Everything

void timing_stateCase (OperatorBehavioral *op, StateCase *stateCase,
		       int cycleTime)
{
  // - apply timing constraints to a TDF state, return list of new states

  warn("Timing state-case "+printStateCaseHead(stateCase)+
       " of op "+op->getName());

  // - create dfg + residual stmts
  BlockDFG dfg;
  list<Stmt*> nondfstmts;
  set<SymbolVar*> locals;
  set<SymbolVar*> new_locals;
  createBlockDfg(stateCase,&dfg,stateCase->getStmts(),&nondfstmts,&locals,&new_locals, NULL, NULL); // stupid
  /*
  cerr << printBlockDFG(&dfg);
  cerr << "Residual statements:\n";
  Stmt *s;
  forall (s,nondfstmts)
    cerr << s->toString();
  cerr << "Locals:\n";
  SymbolVar *sym;
  forall (sym,locals)
    cerr << sym->toString() << '\n';
  cerr << "New locals:\n";
  forall (sym,new_locals)
    cerr << sym->toString() << '\n';
  */

  // - compute node areas, latencies
  node_array<int> areas, latencies;
  getAreasLatencies(&dfg,&areas,&latencies);
  // cerr << "Area + Latency known " << printBlockDFG(&dfg,&areas,&latencies);

  int area=0;
  node n;
  forall_nodes (n,dfg)
    area += areas[n];
  // stateCase->setAnnote_(ANNOTE_STATE_AREA,area);
  cerr << "Total area for state-case " << printStateCaseHead(stateCase)
       << " = " << area << '\n';

  /*
  list<Stmt*> reconstructed_stmts;
  reconstructTDF(&dfg,&reconstructed_stmts,&nondfstmts,locals+new_locals);
  cerr << "Reconstructed statements:\n";
  forall (s,reconstructed_stmts)
    cerr << s << "  " << s->toString();
  */

  // - partition (ASAP by time depths) into new DFGs
  array<BlockDFG> new_dfgs;
  set<SymbolVar*> new_pipes;
  partitionBlockDFG(&dfg,&areas,&latencies,-1,cycleTime,&new_dfgs,&new_pipes);

  // - reconstruct TDF states
  set<SymbolVar*> all_locals=locals+new_locals;
  list<State*> new_states;
  reconstructTDF_onecase(op,stateCase,&dfg,&nondfstmts,&all_locals,
			 &new_dfgs,&new_pipes,&new_states);
  // warn("Partially reconstructed op:\n"+op->toString());

  // - record state areas for page partitioning
}


Operator* timing_instance (Operator *iop,
			   list<OperatorBehavioral*> *page_ops,
			   int cycleTime)
{
  // - apply timing constraints to a compose-op instance (modify in place)

  warn("Applying timing constraints to instance "+iop->getName());

  // - list of pages *page_ops_p is either *page_ops or single behavioral iop
  list<OperatorBehavioral*> page_ops_iop;
  page_ops_iop.append((OperatorBehavioral*)iop);
  list<OperatorBehavioral*> *page_ops_p = 
    (iop->getOpKind()==OP_COMPOSE) ? page_ops : &page_ops_iop;

  OperatorBehavioral *op;
  forall (op,*page_ops_p) {
    set<StateCase*> statecases;
    State     *s;
    StateCase *sc;
    forall (s,*op->getStates())
      forall (sc,*s->getCases())
	statecases.insert(sc);
    forall (sc,statecases)
      timing_stateCase(op,sc,cycleTime);
    // warn("Post-timing page "+op->toString());
  }

  return iop;
}
