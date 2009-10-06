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
// SCORE TDF compiler:  
// $Revision: 1.3 $
//
//////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <iostream>

#include <LEDA/graph/graph.h>
#include "symbol.h"
#include <LEDA/core/d_array.h>
#include "state.h"
#include "operator.h"
#include "stmt.h"
#include "pagedfg.h"
#include "streamdepth.h"
#include <LEDA/core/map.h>
#include <LEDA/core/tuple.h>

using leda::node;
using leda::node_array;
using leda::edge;
using leda::edge_array;
using leda::map;
using leda::two_tuple;
using leda::list_item;
using leda::d_array;

using std::istream;
using std::ostream;
using std::cerr;
using std::endl;

// - map<I,E> using trees instead of hash tables (implemented by d_array)
#define dmap leda::d_array

extern int gStreamDepthMax;


////////////////////////////////////////////////////////////////
//  StreamDepthIA interface automaton class

enum ActionKind {ACTION_IN, ACTION_OUT, ACTION_INT};

class StreamDepthIANode {
public:
  State		*state;

  StreamDepthIANode (State *state_i=NULL) : state(state_i) {}
};

class StreamDepthIAEdge {
public:
  ActionKind	actionKind;		// - action kind
  bool		eosKind;		// - true for EOS i/o action
  Symbol	*sym;			// - symbol for   i/o action

  StreamDepthIAEdge (ActionKind actionKind_i=ACTION_INT,
		     bool       eosKind_i   =false,
		     Symbol     *sym_i      =NULL)
    : actionKind(actionKind_i), eosKind(eosKind_i), sym(sym_i) {}
};

typedef GRAPH<StreamDepthIANode,StreamDepthIAEdge> StreamDepthIAGraph;

class StreamDepthIA {
public:
  StreamDepthIAGraph g;
  node               start;

  StreamDepthIA () : start(NULL) {}
  StreamDepthIA& operator=(const StreamDepthIA &sdia);
};


StreamDepthIA& StreamDepthIA::operator= (const StreamDepthIA &sdia)
{
  // - copy a StreamDepthIA: /     g = duplicate of sdia.g 
  //                         \ start = node in g corresponding to sdia.start

  // cerr << "--- StreamDepthIA operator= ---\n";
  g.clear();
  start=NULL;
  node_array<node> oldn_to_newn(sdia.g);	// map nodes: sdia.g --> g
  node oldn;
  forall_nodes (oldn,sdia.g) {
    node newn = g.new_node(sdia.g[oldn]);
    if (sdia.start==oldn)
      start=newn;
    oldn_to_newn[oldn]=newn;
  }
  // assert(start);
  // assert(g[start].state);
  edge olde;
  forall_edges (olde,sdia.g) {
    edge newe = g.new_edge(oldn_to_newn[sdia.g.source(olde)],
			   oldn_to_newn[sdia.g.target(olde)],
			   sdia.g[olde]);
    if (newe);
  }
  return *this;
}


////////////////////////////////////////////////////////////////
//  Print StreamDepthIA

string printStreamDepthIA (StreamDepthIA *sdia)
{
  StreamDepthIAGraph &sdiag=sdia->g;
  node               &start=sdia->start;

  string res("Stream-Depth IA [%d nodes, %d edges] {\n",
	     sdiag.number_of_nodes(),sdiag.number_of_edges());
  node n;
  forall_nodes (n,sdiag) {
    res += "  node " + sdiag[n].state->getName() +
           (sdiag.outdeg(n)==0 ? " [DEADLOCK]"  : "") +
           (sdiag.indeg(n)==0  ? " [NO INPUTS]" : "") +
           (n==start           ? " [START]"     : "") +
	   "\n";
  }
  edge e;
  forall_edges (e,sdiag) {
    StreamDepthIAEdge sdiae=sdiag[e];
    res += "  edge ";
    Symbol *sym = sdiae.sym;
    switch (sdiae.actionKind) {
      case ACTION_IN :	res += "IN  " + sym->toString() + " : ";  break;
      case ACTION_OUT:	res += "OUT " + sym->toString() + " : ";  break;
      case ACTION_INT:	res += "INT " + (sym ? (sym->toString()+" : ")
					     : (string(": "))) ;  break;
    }
    node ns=sdiag.source(e);
    node nd=sdiag.target(e);
    res +=  sdiag[ns].state->getName()      + " --> " +
            sdiag[nd].state->getName()      +
            (sdiae.eosKind ? " (eos)" : "") + "\n";
  }
  res += "}\n";
  return res;
}


void printStreamDepthIA (StreamDepthIA *sdia, ostream &out)
{
  // - print directly to ostream, if single string would be too large

  StreamDepthIAGraph &sdiag=sdia->g;
  node               &start=sdia->start;

  out << string("Stream-Depth IA [%d nodes, %d edges] {\n",
		sdiag.number_of_nodes(),sdiag.number_of_edges());
  node n;
  forall_nodes (n,sdiag) {
    out << ("  node " + sdiag[n].state->getName() +
	    (sdiag.outdeg(n)==0 ? " [DEADLOCK]"  : "") +
	    (sdiag.indeg(n)==0  ? " [NO INPUTS]" : "") +
	    (n==start           ? " [START]"     : "") +
	    "\n");
  }
  edge e;
  forall_edges (e,sdiag) {
    StreamDepthIAEdge sdiae=sdiag[e];
    string line="  edge ";
    Symbol *sym = sdiae.sym;
    switch (sdiae.actionKind) {
      case ACTION_IN :	line += "IN  " + sym->toString() + " : ";  break;
      case ACTION_OUT:	line += "OUT " + sym->toString() + " : ";  break;
      case ACTION_INT:	line += "INT " + (sym ? (sym->toString()+" : ")
					      : (string(": "))) ;  break;
    }
    node ns=sdiag.source(e);
    node nd=sdiag.target(e);
    line +=  sdiag[ns].state->getName()      + " --> " +
             sdiag[nd].state->getName()      +
             (sdiae.eosKind ? " (eos)" : "") + "\n";
    out << line;
  }
  out << "}\n";
}


////////////////////////////////////////////////////////////////
//  Create StreamDepthIA for Queue

void createStreamDepthIA_queue (StreamDepthIA *sdia, int depth,
				Symbol *in, Symbol *out, string basename)
{
  // - initialize *sdia to a stream-depth interface automaton
  //     for a queue of the given depth
  // - actions use the given input/output symbols
  // - states are name "basenameN" where N = #elements in the queue

  assert(depth>=1);

  StreamDepthIAGraph &sdiag=sdia->g;
  node               &start=sdia->start;
  
  sdiag.clear();
  start=NULL;
  node n_prev=NULL;

  // - create + connect each node at depth i
  for (int i=0; i<=depth; i++) {
    string  stateName=basename+string("%d",i);
    State  *state=new State(NULL,stateName);		// - dummy state
    state->setParent(NULL);
    node n=sdiag.new_node(StreamDepthIANode(state));
    if (i==0) {
      // - first node
      start=n;
    }
    else {  // (i>0)
      // - connect to previous node
      edge ew=sdiag.new_edge(n_prev,n,
			     StreamDepthIAEdge(ACTION_IN,false,in));
      edge er=sdiag.new_edge(n,n_prev,
			     StreamDepthIAEdge(ACTION_OUT,false,out));
      if (ew); if (er);
    }
    n_prev=n;
  }
}


/*
void streamdepth_queue_test ()
{
  SymbolStream in (NULL,"in ",type_none,STREAM_IN);
  SymbolStream out(NULL,"out",type_none,STREAM_OUT);

  for (int depth=1; depth<=4; depth++) {
    string basename("queue%d_",depth);
    StreamDepthIA q;
    createStreamDepthIA_queue(&q,depth,&in,&out,basename);
    cerr << printStreamDepthIA(&q);
  }
}
*/


////////////////////////////////////////////////////////////////
//  Create StreamDepthIA for Behavioral Op

typedef dmap<Symbol*,Symbol*> SymbolMap;

class StreamDepthIA_info {
public:
  StreamDepthIA      *sdia;		// - the automaton
  OperatorBehavioral *op;		// - the TDF operator
  map<State*,node>   *stateToNode;	// - map: TDF state --> IA node
  node                n;		// - node most recently added to IA
					//     (attach more nodes to this one)
  int                 nodenum;		// - # of node most recently added,
					//     starting from 0  (new nodes
					//     get numbers starting from 1)
  string	      basename;		// - base name for new states created
					//     (typically name of state)
  SymbolMap *localToComposStream;	// - map local op's streams to
					//     compos op's streams;
					//     actions use compos streams if
					//     this is provided, else use local

  StreamDepthIA_info (StreamDepthIA *sdia_i, OperatorBehavioral *op_i,
		      map<State*,node> *stateToNode_i,
		      node n_i, int nodenum_i, string basename_i,
		      SymbolMap *localToComposStream_i=NULL)
    : sdia(sdia_i), op(op_i), stateToNode(stateToNode_i),
      n(n_i), nodenum(nodenum_i), basename(basename_i),
      localToComposStream(localToComposStream_i)	{}
};


bool createStreamDepthIA_map (Tree *t, void *i)
{
  // - add to *sdia representing a node sequence corresponding
  //     to a state-case, with sequenced inputs, outputs, gotos.
  // - assume the following order of tree traversal:
  //     * state-case (top level)
  //     * input specs
  //     * stmts in order  (note assignments, close())
  //     * gotos in order  (single or 2 in "if")

  StreamDepthIA_info *info=(StreamDepthIA_info*)i;

  StreamDepthIAGraph &sdiag=info->sdia->g;
//node               &start=info->sdia->start;

  switch (t->getKind())
  {
    case TREE_STATECASE:  {
			    // - top level: state case
			    StateCase *sc=(StateCase*)t;
			    // - assert sequential firing rules
			    assert(sc->getInputs()->size()<=1);
			    return true;
			  }
    case TREE_INPUTSPEC:  {
			    // - add input transition(s) to IA
			    InputSpec *ispec=(InputSpec*)t;
			    int numTokens=1;
			    Expr *nt=ispec->getNumTokens();
			    if (nt) {
			      assert(nt->getExprKind()==EXPR_VALUE);
			      numTokens=((ExprValue*)nt)->getIntVal();
			    }
			    for (int i=0; i<numTokens; i++) {
			      Symbol *sym=ispec->getStream();
			      Symbol *asym=info->localToComposStream ?
				   (*info->localToComposStream)[sym] : sym;
			      // - create dummy state
			      string new_name = info->basename
				+ string("_ia%d",++info->nodenum);
			      State *new_st = new State(NULL,new_name);
			             new_st->setParent(NULL);
			      node   new_n = sdiag.new_node
				      (StreamDepthIANode(new_st));
			      edge   new_e = sdiag.new_edge
				      (info->n,new_n,
				       StreamDepthIAEdge(ACTION_IN,
							 ispec->isEosCase(),
							 asym));
			      if (new_e);
			      info->n = new_n;
			    }
			    return false;
			  }
    case TREE_STMT:	  {
			    switch (((Stmt*)t)->getStmtKind()) {
			      case STMT_ASSIGN: {
				StmtAssign *asst=(StmtAssign*)t;
				Symbol *sym=asst->getLValue()->getSymbol();
				if (sym->isStream()) {
				  // - add output transition to IA
				  Symbol *asym=info->localToComposStream ?
				       (*info->localToComposStream)[sym] : sym;
				  // - create dummy state
				  string new_name = info->basename
				    + string("_ia%d",++info->nodenum);
				  State *new_st = new State(NULL,new_name);
				         new_st->setParent(NULL);
				  node new_n = sdiag.new_node
				    (StreamDepthIANode(new_st));
				  edge new_e = sdiag.new_edge
				    (info->n,new_n,
				     StreamDepthIAEdge(ACTION_OUT,false,asym));
				  if (new_e);
				  info->n = new_n;
				}
				return false;
			      }
			      case STMT_BUILTIN: {
				StmtBuiltin *blts=(StmtBuiltin*)t;
				ExprBuiltin *blte=blts->getBuiltin();
				// Nachiket: 10/6/2009: Not sure if FRAMECLOSE should be supported or not..
				if (((OperatorBuiltin*)blte->getOp())->
					getBuiltinKind()==BUILTIN_CLOSE ||
					((OperatorBuiltin*)blte->getOp())->
					getBuiltinKind()==BUILTIN_FRAMECLOSE) {
				  // - add output EOS transition to IA
				  Expr *arg=blte->getArgs()->head();
				  assert(arg->getExprKind()==EXPR_LVALUE);
				  Symbol *sym=((ExprLValue*)arg)->getSymbol();
				  Symbol *asym=info->localToComposStream ?
				       (*info->localToComposStream)[sym] : sym;
				  // - create dummy state
				  string new_name = info->basename
				    + string("_ia%d",++info->nodenum);
				  State *new_st = new State(NULL,new_name);
				         new_st->setParent(NULL);
				  node new_n = sdiag.new_node
				    (StreamDepthIANode(new_st));
				  edge new_e = sdiag.new_edge
				    (info->n,new_n,
				     StreamDepthIAEdge(ACTION_OUT,true,asym));
				  if (new_e);
				  info->n = new_n;
				}
				return false;
			      }
			      case STMT_GOTO: {
				StmtGoto *sg = (StmtGoto*)t;
				// - add internal transition for goto
				// - for unconditional goto (no "if"),
				//     IA could be shrunk by collapsing
				//     this internal transition (merge endpts)
				// - find node for next state
				node new_n = (*info->stateToNode)
							[sg->getState()];
				assert(new_n);
				edge new_e = sdiag.new_edge
				    (info->n,new_n,
				     StreamDepthIAEdge(ACTION_INT,false,NULL));
				if (new_e);
				// - do NOT set "info->n=new_n",
				//     so other goto in an if stmt (if exists)
				//     remembers recent node to attach to
				return false;
			      }
			      case STMT_IF:
			      case STMT_BLOCK: {
				return true;
			      }
			      default: {
				return false;
			      }
			    }  // switch (((Stmt*)t)->getStmtKind()) 
			  }
    default:		  {
			    return false;
			  }
  }  // switch (t->getKind())

  assert(!"never get here");
  return 0;
}


/*
static void merge_nodes (graph& G, node src, node dst)
{
  // - merge node dst into node src, remove node dst
  // - collapsed edges become self feedback

  list<edge> G_in_edges_dst =G.in_edges(dst);
  list<edge> G_out_edges_dst=G.out_edges(dst);
  edge e;
  forall (e,G_in_edges_dst) {		// - convert e=(s,dst) --> e=(s,src)
    node s=G.source(e);
    G.move_edge(e,s,src);
  }
  forall (e,G_out_edges_dst) {		// - convert e=(dst,d) --> e=(src,d)
    node d=G.target(e);
    G.move_edge(e,src,d);
  }
  G.del_node(dst);
}
*/


void createStreamDepthIA (StreamDepthIA *sdia, OperatorBehavioral *op,
			  SymbolMap *localToComposStream=NULL)
{
  // - initialize *sdia to a stream-depth interface automaton for *op
  // - assume that op has sequential firing rules (dismantled stream read)
  // - create state nodes
  // - for each state
  //     * for each state-case
  //         + input transition to new node
  //         + output transition sequence to new node(s)
  //         + internal transitions to goto target states
  // - *localToComposeStream maps from op's local streams
  //     to top-level compositional op's streams;
  //     if map is provided, IA actions will use compositional streams
  //     instead of local streams (otherwise use local streams).

  StreamDepthIAGraph &sdiag=sdia->g;
  node               &start=sdia->start;

  sdiag.clear();
  start=NULL;

  // - create base state nodes
  map<State*,node> stateToNode(NULL);	// - map: op state --> sdia node
  State *state;
  forall (state,*op->getStates()) {
    node n=sdiag.new_node(StreamDepthIANode(state));
    stateToNode[state]=n;
  }
  start = stateToNode[op->getStartState()];

  // - create transitions + intermediate nodes
  forall (state,*op->getStates()) {
    StateCase *sc;
    forall (sc,*state->getCases()) {
      // - build IA
      StreamDepthIA_info info(sdia,op,&stateToNode,stateToNode[state],0,
			      state->getName(),localToComposStream);
      sc->map(createStreamDepthIA_map,(TreeMap)NULL,&info);
    }  // forall (sc,*state->getCases())
  }  // forall (state,*op->getStates())

  // - get rid of useless internal transitions
  list<edge> edges=sdiag.all_edges();
  edge e;
  forall (e,edges) {
    node src=sdiag.source(e), dst=sdiag.target(e);
    if (   src!=dst
	&& sdiag[e].actionKind==ACTION_INT
	&& sdiag.outdeg(src)==1
	&& sdiag.indeg (dst)==1) {
      // cerr << "*** MERGING " << sdiag[src].state->getName() << " --> "
      //                        << sdiag[dst].state->getName() << "\n";
      if (src==start)
	sdiag.merge_nodes(src,dst);		// - src swallows dst
      else
	sdiag.merge_nodes(dst,src);		// - dst swallows src
      sdiag.del_edge(e);
    }
  }  // forall (e,edges)

}


////////////////////////////////////////////////////////////////
//  Destroy StreamDepthIA

void destroyStreamDepthIA (StreamDepthIA *sdia)
{
  // - delete dummy states used in sdia->g nodes  (from creation/composition)
  //     NOTE: assumes that dummy states have nil token, nil parent,
  //           and 0 cases; deletes all such states!
  // - clear sdia->g graph
  //     (sdia->g graph gets destroyed/deleted later, when *sdia gets deleted)
  // - do NOT delete *sdia here!

  StreamDepthIAGraph &sdiag=sdia->g;
  int numdestroyed=0;
  node n;
  forall_nodes (n,sdiag) {
    State *st=sdiag[n].state;
    if (st && st->getParent()==NULL
	   && st->getToken()==NULL
	   && st->getCases()->empty()) {
      // - st is a "dummy" state made by createStreamDepthIA()/_queue()
      delete st;
      // sdiag[n].state=NULL;	// - avoid dangling pointer
      numdestroyed++;
    }
  }
  warn(string("destroyStreamDepthIA:  deleted %d states",numdestroyed));
  sdiag.clear();
}


////////////////////////////////////////////////////////////////
//  Compose StreamDepthIA

template<class E>
set<E> listToSet (list<E> l)
{
  set<E> s;
  E e;
  forall (e,l)
    s.insert(e);
  return s;
}


node composeStreamDepthIANodes (StreamDepthIAGraph &sdiag_1,
				StreamDepthIAGraph &sdiag_2,
				StreamDepthIAGraph &sdiag_compose,
				node n_1, node n_2)
{
  // - create and return new node in sdiag_compose
  //     representing composition of n_1 (in sdiag_1) x n_2 (in sdiag_2)

  // warn ("composing node "+sdiag_1[n_1].state->getName()+
  //       " x "            +sdiag_2[n_2].state->getName());

  string new_name = sdiag_1[n_1].state->getName() + "_x_" +
		    sdiag_2[n_2].state->getName();
  State *new_st = new State(NULL,new_name);	// - dummy state
         new_st->setParent(NULL);
  node   new_n  = sdiag_compose.new_node(StreamDepthIANode(new_st));
  return new_n;
}


node findOrComposeStreamDepthIANodes (StreamDepthIAGraph &sdiag_1,
				      StreamDepthIAGraph &sdiag_2,
				      StreamDepthIAGraph &sdiag_compose,
				      node n_1, node n_2,
				      dmap<two_tuple<node,node>,node>
								&pairToNode)
{
  // - find and return node in sdiag_compose
  //     representing composition of n_1 (in sdiag_1) x n_2 (in sdiag_2)
  // - if composition does not exist, then create and return it,
  //     and augment pairToNode to represent it
  // - pairToNode is defined for existing composed nodes
  //   pairToNode : (n_1,n_2) --> n_1 x n_2

  two_tuple<node,node> pair(n_1,n_2);
  node n_compose = pairToNode[pair];
  if (!n_compose) {
    n_compose = composeStreamDepthIANodes(sdiag_1,sdiag_2,
					  sdiag_compose,n_1,n_2);
    pairToNode[pair] = n_compose;
  }
  return n_compose;
}


set<edge> findIOActions (StreamDepthIAGraph &sdiag,
			 node n, Symbol *sym)
{
  // - find + return all outgoing edges of node n
  //     that have an I/O action using *sym

  set<edge> ret;

  list<edge> out_edges=sdiag.out_edges(n);
  edge e;
  forall (e,out_edges) {
    StreamDepthIAEdge sdiae=sdiag[e];
    if ((sdiae.sym==sym) &&
	(sdiae.actionKind==ACTION_IN || sdiae.actionKind==ACTION_OUT))
      ret.insert(e);
  }
  return ret;
}


void composeStreamDepthIA (StreamDepthIA *sdia_1,
			   StreamDepthIA *sdia_2,
			   StreamDepthIA *sdia_compose,
			   set<Symbol*>  *shared_syms,
			   dmap<node,set<Symbol*> > *illegal_syms=NULL)
{
  // - build in *sdia_compose the automata composition of *sdia_1, *sdia_2
  // - *shared_syms contains symbols for shared actions
  //     (i.e. input/output of sdia_1 == output/input of sdia_2)
  // - if *illegal_syms is provided, fill it with map
  //     (illegal composn node) --> (set of symbols that make it illegal)
  // - def: for illegal node n=(u,v), a symbol that "makes it illegal"
  //        is a symbol for which there is an output transition from u
  //        and no input transition from v, or vice versa

  StreamDepthIAGraph &sdiag_1       = sdia_1->g;
  StreamDepthIAGraph &sdiag_2       = sdia_2->g;
  StreamDepthIAGraph &sdiag_compose = sdia_compose->g;
  node               &start_1       = sdia_1->start;
  node               &start_2       = sdia_2->start;
  node               &start_compose = sdia_compose->start;

  sdiag_compose.clear();
  start_compose=NULL;

  if (illegal_syms)
      illegal_syms->clear();

  // - working node sets
  set< two_tuple<node,node> >     donePairs;	// - composn nodes, done
  set< two_tuple<node,node> >     workPairs;	// - composn nodes, not done
  dmap<two_tuple<node,node>,node> pairToNode(NULL);	// - map to comp nodes

  // - compose start nodes
  start_compose = composeStreamDepthIANodes(sdiag_1,sdiag_2,sdiag_compose,
					    start_1,start_2);
  two_tuple<node,node> pair_start(start_1,start_2);
  workPairs.insert(pair_start);
  pairToNode[pair_start] = start_compose;

  // - compose nodes on frontier
  while (!workPairs.empty())
  {
    two_tuple<node,node> pair = workPairs.choose();
    workPairs.del(pair);
    node n_1=pair.first(), n_2=pair.second(), n_compose=pairToNode[pair];
    // warn("processing "+sdiag_1[n_1].state->getName()+" x "+
    //                    sdiag_2[n_2].state->getName()       );

    // - identify orig outgoing edges
    set<edge> edges_1=listToSet(sdiag_1.out_edges(n_1));
    set<edge> edges_2=listToSet(sdiag_2.out_edges(n_2));

    /*
    // - identify shared actions on orig outgoing edges
    set<Symbol*> syms_1, syms_2, syms_12;
    edge e;
    forall (e,edges_1) {
      Symbol *sym=sdiag_1[e].sym;
      if (sym)
	syms_1.insert(sym);
    }
    forall (e,edges_2) {
      Symbol *sym=sdiag_2[e].sym;
      if (sym)
	syms_2.insert(sym);
    }
    syms_12 = syms_1.intersect(syms_2);
    */

    // - create transitions outgoing from n_1 x n_2
    edge e_1;
    forall (e_1,edges_1) {
      StreamDepthIAEdge sdiae_1=sdiag_1[e_1];
      Symbol *sym_1=sdiae_1.sym;
      if ((sdiae_1.actionKind==ACTION_IN || sdiae_1.actionKind==ACTION_OUT)
	  && (*shared_syms).member(sym_1)) {
	// - possibly shared action  (from n_1)
	set<edge> edges_2_compat=findIOActions(sdiag_2,n_2,sym_1);
	/*
	string msg="shared actions "+sdiag_1[n_1].state->getName()+" x "+
	                             sdiag_2[n_2].state->getName()+" { ";
	edge e;
	forall (e,edges_2_compat)
	  msg += sdiag_1[e].sym->getName() + " ";
	warn(msg+"}");
	*/
	edge e_2_compat;
	forall (e_2_compat,edges_2_compat) {
	  // - take matching transitions (if any) in both automata
	  node n_1_dst=sdiag_1.target(e_1);
	  node n_2_dst=sdiag_2.target(e_2_compat);
	  node n_compose_dst=findOrComposeStreamDepthIANodes
	    (sdiag_1,sdiag_2,sdiag_compose,n_1_dst,n_2_dst,pairToNode);
	  edge e_compose=sdiag_compose.new_edge
				(n_compose,n_compose_dst,
				 StreamDepthIAEdge(ACTION_INT,false,sym_1));
	  if (e_compose);
	  two_tuple<node,node> pair_dst(n_1_dst,n_2_dst);
	  if (!donePairs.member(pair_dst))
	       workPairs.insert(pair_dst);
	  edges_2.del(e_2_compat);	  // - do not process edge again later
	}
	if (edges_2_compat.empty() && illegal_syms
				   && sdiae_1.actionKind==ACTION_OUT)
	  (*illegal_syms)[n_compose].insert(sym_1);
      }
      else {
	// - non-shared action  (from n_1)
	  node n_1_dst=sdiag_1.target(e_1);
	  node n_2_dst=n_2;
	  node n_compose_dst=findOrComposeStreamDepthIANodes
            (sdiag_1,sdiag_2,sdiag_compose,n_1_dst,n_2_dst,pairToNode);
	  edge e_compose=sdiag_compose.new_edge
	    (n_compose,n_compose_dst,sdiag_1[e_1]);
	  if (e_compose);
	  two_tuple<node,node> pair_dst(n_1_dst,n_2_dst);
	  if (!donePairs.member(pair_dst))
	       workPairs.insert(pair_dst);
      }
    }  // forall_edges (e_1,edges_1)

    edge e_2;
    forall (e_2,edges_2) {
      StreamDepthIAEdge sdiae_2=sdiag_2[e_2];
      Symbol *sym_2=sdiae_2.sym;
      if ((sdiae_2.actionKind==ACTION_IN || sdiae_2.actionKind==ACTION_OUT)
	  && (*shared_syms).member(sym_2)) {
	// - possibly shared action  (from n_2)
	// - actually, we did all actual shared actions above from n_1,
	//     so there should be none remaining
	set<edge> edges_1_compat=findIOActions(sdiag_1,n_1,sym_2);
	if (!edges_1_compat.empty())
	  assert(!"internal inconsistency");
	if (edges_1_compat.empty() && illegal_syms
				   && sdiae_2.actionKind==ACTION_OUT)
	  (*illegal_syms)[n_compose].insert(sym_2);
      }
      else {
	// - non-shared action  (from n_2)
	  node n_1_dst=n_1;
	  node n_2_dst=sdiag_2.target(e_2);
	  node n_compose_dst=findOrComposeStreamDepthIANodes
            (sdiag_1,sdiag_2,sdiag_compose,n_1_dst,n_2_dst,pairToNode);
	  edge e_compose=sdiag_compose.new_edge
	    (n_compose,n_compose_dst,sdiag_2[e_2]);
	  if (e_compose);
	  two_tuple<node,node> pair_dst(n_1_dst,n_2_dst);
	  if (!donePairs.member(pair_dst))
	       workPairs.insert(pair_dst);
      }
    }  // forall_edges (e_2,edges_2)

    donePairs.insert(pair);

  }  // while (!workNodes.empty())

  /*
  // - print statistics  (composition)
  int nn1=sdiag_1.number_of_nodes(),       ne1=sdiag_1.number_of_edges();
  int nn2=sdiag_2.number_of_nodes(),       ne2=sdiag_2.number_of_edges();
  int nnc=sdiag_compose.number_of_nodes(), nec=sdiag_compose.number_of_edges();
  int nnmax=nn1*nn2, nemax=ne1*nn2+ne2*nn1;
  warn(string("  --> [%d nodes, %d edges] / [%d, %d] = [%.2f%%, %.2f%%]",
	      nnc, nec, nnmax, nemax, 100.*nnc/nnmax, 100.*nec/nemax));
  */
}


string printCompositionStatistics (StreamDepthIA            *sdia_1,
				   StreamDepthIA            *sdia_2,
				   StreamDepthIA            *sdia_compose,
				   set<Symbol*>             *shared_syms,
				   dmap<node,set<Symbol*> > *illegal_syms=NULL)
{
  StreamDepthIAGraph &sdiag_1       = sdia_1->g;
  StreamDepthIAGraph &sdiag_2       = sdia_2->g;
  StreamDepthIAGraph &sdiag_compose = sdia_compose->g;

  // - general #nodes, #edges info
  Operator *op_1=sdiag_1[sdiag_1.first_node()].state->getOperator();
  Operator *op_2=sdiag_2[sdiag_2.first_node()].state->getOperator();
  string op_1_name = op_1 ? op_1->getName() : string("<nil>");
  string op_2_name = op_2 ? op_2->getName() : string("<nil>");
  int nn1=      sdiag_1.number_of_nodes(), ne1=      sdiag_1.number_of_edges();
  int nn2=      sdiag_2.number_of_nodes(), ne2=      sdiag_2.number_of_edges();
  int nnc=sdiag_compose.number_of_nodes(), nec=sdiag_compose.number_of_edges();
  int nncmax=nn1*nn2, necmax=nn1*ne2+nn2*ne1;

  string res = "compose " +
               op_1_name + string(" [%d, %d] x ",           nn1,ne1) +
               op_2_name + string(" [%d, %d] share %d --> ",nn2,ne2,
						shared_syms->size()) +
               string("[%d, %d] / [%d, %d] = [%.2f%%, %.2f%%]",
		      nnc,nec,nncmax,necmax,100.*nnc/nncmax,100.*nec/necmax);

  // - find illegal deadlocked nodes
  int numdeadlocks=0;
  node nc;
  forall_nodes (nc,sdiag_compose)
    if (sdiag_compose.outdeg(nc)==0 &&
	illegal_syms && illegal_syms->defined(nc))
      numdeadlocks++;
  if (numdeadlocks>0)
    res += string(" deadlock %d",numdeadlocks);

  return res;
}


void printCompositionStatistics_allPairs (ostream &out, PageDFG *pdfg,
					  map<edge,int> *streamdepths)
{
  out << "Pair-wise composition stream depths (per pair) {\n";
  set<edge> streams=listToSet(pdfg->all_edges());
  while (!streams.empty()) {
    edge stream = streams.choose();
    streams.del(stream);
    node src=pdfg->source(stream), dst=pdfg->target(stream);
    OperatorBehavioral *op_src=(OperatorBehavioral*)(*pdfg)[src];
    OperatorBehavioral *op_dst=(OperatorBehavioral*)(*pdfg)[dst];
    if (   op_src->getKind()==TREE_OPERATOR
	&& op_src->getOpKind()==OP_BEHAVIORAL
	&& op_dst->getKind()==TREE_OPERATOR
	&& op_dst->getOpKind()==OP_BEHAVIORAL) {
      set<edge> shared_edges = (  (  listToSet(pdfg->out_edges(src))
				   & listToSet(pdfg->in_edges (dst)))
				+ (  listToSet(pdfg->out_edges(dst))
				   & listToSet(pdfg->in_edges (src))));
      edge e;
      forall (e,shared_edges)
	streams.del(e);
      cerr << "  { ";
      forall (e,shared_edges)
	out << string("%2d ",(*streamdepths)[e]);
      out << "}  " << op_src->getName()
	  << " x " << op_dst->getName()
		   << (shared_edges.size()==1 ? "  [TRIVIAL]\n" : "\n");
    }
  }  // while (!streams.empty())
  out << "}\n";

  out << "Pair-wise composition stream depths (per stream) {\n";
  edge stream;
  forall_edges(stream,*pdfg) {
    node src=pdfg->source(stream), dst=pdfg->target(stream);
    set<edge> shared_edges = (  (  listToSet(pdfg->out_edges(src))
				 & listToSet(pdfg->in_edges (dst)))
			      + (  listToSet(pdfg->out_edges(dst))
				 & listToSet(pdfg->in_edges (src))));
    out << string("  %2d  ",(*streamdepths)[stream])
	<< (*pdfg)[stream]->getName()
        << (shared_edges.size()==1 ? "  [TRIVIAL]\n" : "\n");
  }
  out << "}\n";
}


////////////////////////////////////////////////////////////////
//  Local-to-Compositional Stream Map

bool buildStreamMap_map (Tree *t, void *i)
{
  // - fill in local-to-compositional stream map in (map<Symbol*,Symbol*>*)i
  //     for streams connected to behavioral operators
  // - called on compositional operator's stmts

  SymbolMap *localToComposStream = (SymbolMap*)i;

  switch (t->getKind())
  {
    case TREE_OPERATOR:	{
			  return (((Operator*)t)->getOpKind()==OP_COMPOSE);
			}
    case TREE_STMT:	{
			  switch (((Stmt*)t)->getStmtKind()) {
			    case STMT_CALL:
			    case STMT_ASSIGN:	return true;
			    default:		return false;
			  }
			}
    case TREE_EXPR:	{
			  if (((Expr*)t)->getExprKind()==EXPR_CALL) {
			    ExprCall *call=(ExprCall*)t;
			    // - process argument list
			    list<Expr*>   *args   =call->getArgs();
			    list<Symbol*> *formals=call->getOp()->getArgs();
			    list_item ie, isym;
			    int i;
			    for (ie=args->first(), isym=formals->first(), i=1;
				 ie && isym;
				 ie=args->succ(ie), isym=formals->succ(isym)) {
			      Expr   *arg=args->inf(ie);
			      Symbol *formalsym=formals->inf(isym);
			      if (formalsym->isStream()) {
				if (arg->getExprKind()!=EXPR_LVALUE)
				  fatal(1,string("Stream depth analysis: "
						 "cannot identify stream in "
						 "arg #%d of call to ",i)+
					  call->getOp()->getName()+" of op "+
					  call->getOperator()->getName());
				Symbol *argsym=((ExprLValue*)arg)->getSymbol();
				(*localToComposStream)[formalsym]=argsym;
			      }
			    }
			    // - process return symbol
			    Symbol *formalretsym=call->getOp()->getRetSym();
			    if (formalretsym) {
			      Tree *p=call->getParent();
			      if (p->getKind()==TREE_STMT &&
				  ((Stmt*)p)->getStmtKind()==STMT_ASSIGN) {
				StmtAssign *asst=(StmtAssign*)p;
				Symbol *argretsym=asst->getLValue()
						      ->getSymbol();
				(*localToComposStream)[formalretsym]=argretsym;
			      }
			    }
			    return false;
			  }
			}
    default:		{
			  return false;
			}
  }
}


void buildStreamMap (OperatorCompose *iop,
		     SymbolMap *localToComposStream)
{
  localToComposStream->clear();
  iop->map(buildStreamMap_map,(TreeMap)NULL,(void*)localToComposStream);
}


////////////////////////////////////////////////////////////////
//  Top Level

Operator* streamDepth_instance_all_ops (OperatorCompose *iop,
					list<OperatorBehavioral*> *page_ops)
{
  PageDFG pdfg;
  buildPageDFG(iop,&pdfg);
  warn(iop->getName()+" "+printPageDFG(&pdfg));

  // - build interface automaton for every behavioral op
  OperatorBehavioral *bop;
  forall (bop,*page_ops) {
    StreamDepthIA sdia;
    createStreamDepthIA(&sdia,bop);
    cerr << bop->getName() << " " << printStreamDepthIA(&sdia);
  }

  return iop;
}


Operator* streamDepth_instance_all_pairs (OperatorCompose *iop,
					  list<OperatorBehavioral*> *page_ops)
{
  PageDFG pdfg;
  buildPageDFG(iop,&pdfg);
  warn(iop->getName()+" "+printPageDFG(&pdfg));

  // - build local-to-compositional stream map for action symbols
  SymbolMap localToComposStream(NULL);
  buildStreamMap(iop,&localToComposStream);
  /*
  string msg="Local-to-compositional stream map:\n";
  Symbol *formalsym;
  forall_defined (formalsym,localToComposStream)
    msg += formalsym->getName() + " --> " +
           localToComposStream[formalsym]->getName() + "\n";
           // string(" (%p)\n",localToComposStream[formalsym]);
  warn(msg);
  */

  // - build interface automata for every pair of connected behavioral ops
  set<edge> streams=listToSet(pdfg.all_edges());
  while (!streams.empty()) {
    edge stream=streams.choose();
    streams.del(stream);
    node src=pdfg.source(stream);
    node dst=pdfg.target(stream);
    OperatorBehavioral *op_src=(OperatorBehavioral*)pdfg[src];
    OperatorBehavioral *op_dst=(OperatorBehavioral*)pdfg[dst];
    if (   op_src->getKind()==TREE_OPERATOR
	&& op_src->getOpKind()==OP_BEHAVIORAL
	&& op_dst->getKind()==TREE_OPERATOR
	&& op_dst->getOpKind()==OP_BEHAVIORAL) {
      // - try composing op_src & op_dst

      // - find all shared streams op_src <--> op_dst
      set<edge> src_edges = listToSet(pdfg.out_edges(src)) +
			    listToSet(pdfg.in_edges (src));
      set<edge> dst_edges = listToSet(pdfg.out_edges(dst)) +
			    listToSet(pdfg.in_edges (dst));
      set<edge> shared_edges=src_edges.intersect(dst_edges);
      set<Symbol*> shared_syms;
      edge e;
      forall (e,shared_edges) {
	Symbol *sym=pdfg[e];
	assert(sym);
	shared_syms.insert(sym);
      }
      /*
      string msg = "shared streams { ";
      Symbol *sym;
      forall (sym,shared_syms)
	msg += sym->getName() + " ";
      warn(msg+" }");
      */

      // - do not visit any other shared streams op_src<-->op_dst
      //    for subsequent pair composition
      forall (e,shared_edges)
        streams.del(e);

      // - compose automata
      // warn("composing "+op_src->getName()+" x "+op_dst->getName());
      StreamDepthIA sdia_src, sdia_dst, sdia_compose;
      createStreamDepthIA(&sdia_src,op_src,&localToComposStream);
      // warn(op_src->getName()+" "+printStreamDepthIA(&sdia_src));
      createStreamDepthIA(&sdia_dst,op_dst,&localToComposStream);
      // warn(op_dst->getName()+" "+printStreamDepthIA(&sdia_dst));
      dmap<node,set<Symbol*> > illegal_syms;
      composeStreamDepthIA(&sdia_src,&sdia_dst,&sdia_compose,
			   &shared_syms,&illegal_syms);
      // - printing composition as single string is too large
      // warn(op_src->getName()+" x "+
      //      op_dst->getName()+" "  +printStreamDepthIA(&sdia_compose));
      // warn(op_src->getName()+" x "+op_dst->getName());
      // printStreamDepthIA(&sdia_compose,cerr);
      warn(printCompositionStatistics(&sdia_src,&sdia_dst,&sdia_compose,
				      &shared_syms,&illegal_syms));

    }  // if (...) compose src x dst
  }  // while (!streams.empty())

  return iop;
}


template <class T>
void swap_ (T& a, T& b) { T tmp=a; a=b; b=tmp; }


Operator* streamDepth_instance_queues (OperatorCompose *iop,
				       list<OperatorBehavioral*> *page_ops)
{
  warn("Stream depth analysis for "+iop->getName()+
       string(", maximum depth %d",gStreamDepthMax));

  PageDFG pdfg;
  buildPageDFG(iop,&pdfg);
  warn(iop->getName()+" "+printPageDFG(&pdfg));

  // - build local-to-compositional stream map for action symbols
  SymbolMap localToComposStream(NULL);
  buildStreamMap(iop,&localToComposStream);
  string msg="Local-to-compositional stream map:\n";
  Symbol *formalsym;
  forall_defined (formalsym,localToComposStream)
    msg += formalsym->getName() + " --> " +
           localToComposStream[formalsym]->getName() + "\n";
           // string(" (%p)\n",localToComposStream[formalsym]);
  warn(msg);

  // - build interface automata for every pair of connected behavioral ops
  map<edge,int> streamdepths(0);	// - stream depths being computed
					//   for entire pdfg  (-1 := \infty)
  set<edge> streams=listToSet(pdfg.all_edges());
  while (!streams.empty())
  {
    // - try all connected op pairs
    edge stream=streams.choose();
    streams.del(stream);
    node src=pdfg.source(stream);
    node dst=pdfg.target(stream);
    OperatorBehavioral *op_src=(OperatorBehavioral*)pdfg[src];
    OperatorBehavioral *op_dst=(OperatorBehavioral*)pdfg[dst];
    if (   op_src->getKind()==TREE_OPERATOR
	&& op_src->getOpKind()==OP_BEHAVIORAL
	&& op_dst->getKind()==TREE_OPERATOR
	&& op_dst->getOpKind()==OP_BEHAVIORAL)
    {
      // - try composing op_src & op_dst  (with queues)
      warn("trying to compose pair "+
	   op_src->getName()+" x "+op_dst->getName());

      // - find all shared streams op_src <--> op_dst
      set<edge> src_edges_out = listToSet(pdfg.out_edges(src));
      set<edge> src_edges_in  = listToSet(pdfg.in_edges (src));
      set<edge> dst_edges_out = listToSet(pdfg.out_edges(dst));
      set<edge> dst_edges_in  = listToSet(pdfg.in_edges (dst));
      set<edge> shared_edges  = (src_edges_out.intersect(dst_edges_in)) +
				(dst_edges_out.intersect(src_edges_in));

      /*
      // DEFUNCT
      set<Symbol*> shared_syms;
      edge ee;
      forall (ee,shared_edges) {
        Symbol *sym=pdfg[ee];
	assert(sym);
	shared_syms.insert(sym);
      }
      string msg = "shared streams { ";
      Symbol *sym;
      forall (sym,shared_syms)
	msg += sym->getName() + " ";
      warn(msg+" }");
      */

      // - do not visit any other shared streams op_src<-->op_dst
      //    for subsequent pair composition
      edge e;
      forall (e,shared_edges)
        streams.del(e);

      // - create op_src automaton (independent of queueing)
      StreamDepthIA sdia_src;
      createStreamDepthIA(&sdia_src,op_src,&localToComposStream);
      // warn("source op "+op_src->getName()+" "+printStreamDepthIA(&sdia_src));

      // - running compositions
      //     compose:    *sdia_compos_pnext = *sdia_compos_p x sdia
      //     then swap:  swap_(sdia_compos_p,sdia_compos_pnext)
      StreamDepthIA sdia_compose_1;
      StreamDepthIA sdia_compose_2;
      StreamDepthIA *sdia_compose_p    =&sdia_compose_1;  // - running composn
      StreamDepthIA *sdia_compose_pnext=&sdia_compose_2;  // - scratch

      bool got_finite_stream_depths=true;
      bool done=false;
      while (!done)
      {
	// - compose with successively larger queues

	string msg="shared stream depths {\n";
	forall (e,shared_edges) {
	  msg += string("  %2d ",streamdepths[e]) + pdfg[e]->getName() + "\n";
	}
	warn(msg+"}");

	// - prepare composition with queues
	*sdia_compose_p = sdia_src;	  // - initalize running composition
	//map<edge,StreamDepthIA> queues; // - queues for each compos stream
	SymbolMap QinToQout;		  // - map queue in to queue out syms
					  //     for each queued compos stream
	set<Symbol*> shared_syms_qout;	  // - streams w/queue (at queue out)
	set<Symbol*> shared_syms_noqueue; // - streams w/no queue are shared
					  //     for op_src x op_dst composn
	map<Symbol*,edge> symToComposStream;  // - compos stream for each sym

	forall (e,shared_edges) {
	  // - compose with queues
	  int streamdepth=streamdepths[e];
	  if (streamdepth==-1) {
	    // - infinite stream depth: treat as non-shared stream
	  }
	  else if (streamdepth==0) {
	    // - zero stream depth (no queue):  treat as shared stream
	    Symbol *sym=pdfg[e];
	    assert(sym);
	    shared_syms_noqueue.insert(sym);
	    symToComposStream[sym]=e;
	  }
	  else {  // (streamdepth>=1)
	    // - non-zero stream depth:  create queue
	    Symbol *sym=pdfg[e];	// - queue input stream (compositional)
	    assert(sym);

	    // - create queue output stream (compositional level)
	    Symbol *sym_queued=(Symbol*)sym->duplicate();
	    sym_queued->setName(sym->getName()+string("_q%d",streamdepth));
	    shared_syms_qout.insert(sym_queued);
	    symToComposStream[sym]=e;
	    symToComposStream[sym_queued]=e;
	    QinToQout[sym]=sym_queued;		// - later used to fix
						//     localToComposStream

	    // - create + compose queue automaton
	    string sdia_q_name("queue%d_",streamdepth);
	    //StreamDepthIA &sdia_q = queues[e];    // - save queue in queues
	    StreamDepthIA sdia_q;
	    createStreamDepthIA_queue(&sdia_q,streamdepth,sym,sym_queued,
				      sdia_q_name);
	    // warn("queue "+sym->getName()+" "+printStreamDepthIA(&sdia_q));
	    set<Symbol*> shared_syms_qin;
	    shared_syms_qin.insert(sym);	// - only shared sym is q input
	    composeStreamDepthIA(sdia_compose_p,&sdia_q,sdia_compose_pnext,
				 &shared_syms_qin);
	    // warn("intermediate composition with queue "+sym->getName()+" ");
	    // printStreamDepthIA(sdia_compose_pnext,cerr);
	    // destroyStreamDepthIA(sdia_compose_p);  // - destroy prev composn
	    swap_(sdia_compose_p,sdia_compose_pnext); // - new composn in _p
	    // destroyStreamDepthIA(&sdia_q);
	  }
	}  // forall (e,shared_edges)  // - compose with queues

	// - create copy of localToComposStream reflecting queue outputs
	SymbolMap localToComposStream_queued=localToComposStream;
	Symbol *localSym;
	forall_defined (localSym,localToComposStream) {
	  Symbol *composSym=localToComposStream[localSym];
	  if (QinToQout.defined(composSym)) {
	    Symbol *queuedComposSym=QinToQout[composSym];
	    localToComposStream_queued[localSym]=queuedComposSym;
	  }
	}

	// - create op_dst automaton using queued symbols
	StreamDepthIA sdia_dst;
	createStreamDepthIA(&sdia_dst,op_dst,&localToComposStream_queued);
	// warn("dest op "+op_dst->getName()+" "+printStreamDepthIA(&sdia_dst));
	// - compose with op_dst
	set<Symbol*> shared_syms = shared_syms_noqueue + shared_syms_qout;
	dmap<node,set<Symbol*> > illegal_syms;	// - illegal composition nodes
	composeStreamDepthIA(sdia_compose_p,&sdia_dst,sdia_compose_pnext,
			     &shared_syms,&illegal_syms);
	// - printing composition as single string is too large
	// warn(op_src->getName()+" x "+
	//      op_dst->getName()+" " +printStreamDepthIA(sdia_compose_pnext));
	// warn(op_src->getName()+" x queues x "+op_dst->getName());
	// printStreamDepthIA(sdia_compose_pnext,cerr);
	warn(printCompositionStatistics(sdia_compose_p,&sdia_dst,
					sdia_compose_pnext,
					&shared_syms,&illegal_syms));
	// destroyStreamDepthIA(sdia_compose_p);    // - destroy prev composn
	swap_(sdia_compose_p,sdia_compose_pnext);   // - new composn in _p

	// - check for deadlock and increase queue depths
	bool no_deadlock=true;
	set<Symbol*> illegal_syms_done;
	node illegalIAnode;
	forall_defined (illegalIAnode,illegal_syms) {	// illegal compsn nodes
	  if (sdia_compose_p->g.outdeg(illegalIAnode)==0) {
	    // - found deadlock
	    no_deadlock = false;
	    set<Symbol*> &illegalset=illegal_syms[illegalIAnode];
	    Symbol *illegal_sym;
	    forall (illegal_sym,illegalset) {
	      // - visit all symbols that deadlock composition when output
	      if (illegal_syms_done.member(illegal_sym))
		// - already did this symbol
		;
	      else {
		illegal_syms_done.insert(illegal_sym);
		edge improperly_queued_edge = symToComposStream[illegal_sym];
		int streamdepth = streamdepths[improperly_queued_edge];
		if (streamdepth!=-1) {
		  // - increase stream depth, unless it's already \infty
		  if (streamdepth>=gStreamDepthMax)
		    streamdepths[improperly_queued_edge] = -1;	// - give up
		  else
		    streamdepths[improperly_queued_edge]++;	// - 1 deeper
		  // warn("stream "+illegal_sym->getName()+
		  //      string(" depth <-- %d at deadlock node ",
		  //             streamdepths[improperly_queued_edge])+
		  //      sdia_compose_p->g[illegalIAnode].state->getName());
		}
	      }
	    }  // forall (illegal_sym,illegalset)
	  }  // if (sdia_compose_p->g.outdeg(illegalIAnode)==0), i.e. deadlock
	}  // forall (illegalset,illegal_syms)

	// - done?
	if (no_deadlock) {
	  // - success:  composition has no deadlock
	  //             (i.e. some streams have bounded depth)
	  got_finite_stream_depths = true;
	  done=true;
	  // break;  // while (!done)
	}
	else {
	  // - check for failure
	  //     (true if all shared edges have depth -1, i.e. \infty)
	  bool fail=true;
	  edge e;
	  forall (e,shared_edges) {
	    int streamdepth=streamdepths[e];
	    if (streamdepth!=-1) {
	      fail=false;
	      break;  // forall (e,shared_edges)
	    }
	  }
	  if (fail) {
	    // - failure:  all streams have unbounded depth
	    got_finite_stream_depths=false;
	    done=true;
	    // break;	// while (!done)
	  }
	}

	// - clean up this iter
	// destroyStreamDepthIA(sdia_compose_p);
	// destroyStreamDepthIA(&sdia_dst);
	// StreamDepthIA dummy, &sdia_q=dummy;
	// forall (sdia_q,queues)
	//   destroyStreamDepthIA(&sdia_q);
	// - StreamDepthIA's in queues[] get deleted
	//   when queues[] goes out of scope at end of 'while (!done)' loop

      }  // while (!done)

      // destroyStreamDepthIA(&sdia_src);

      warn("done composing pair "+op_src->getName()+" x "+op_dst->getName());
      if (got_finite_stream_depths) {
	warn("SUCCESS, SOME FINITE STREAM DEPTHS");
      }
      else {
	warn("FAIL, NO FINITE STREAM DEPTHS");
      }
      
    }  // if (...) then compose src x dst x queues

  }  // while (!streams.empty())

  // - print resulting stream depths
  printCompositionStatistics_allPairs(cerr,&pdfg,&streamdepths);

  return iop;
}


Operator* streamDepth_instance (Operator *iop,
				list<OperatorBehavioral*> *page_ops)
{
  if (iop->getOpKind()==OP_COMPOSE) {
    // return streamDepth_instance_all_ops   ((OperatorCompose*)iop, page_ops);
    // return streamDepth_instance_all_pairs ((OperatorCompose*)iop, page_ops);
    return streamDepth_instance_queues	     ((OperatorCompose*)iop, page_ops);
  }
  else {
    // - iop is single behavioral op
    warn("No stream depth analysis done since top-level op " +
	 iop->getName() + " is not a composition");
  }
  return iop;
}


////////////////////////////////////////////////////////////////
//  Make gcc happy

istream& operator>> (istream &i, StreamDepthIANode&)		{return i;}
istream& operator>> (istream &i, StreamDepthIAEdge&)		{return i;}
ostream& operator<< (ostream &o, const StreamDepthIANode&)	{return o;}
ostream& operator<< (ostream &o, const StreamDepthIAEdge&)	{return o;}


////////////////////////////////////////////////////////////////
//  Notes on Automata Composition
//
// - traditional automata composition
//     * either automaton may stall at any shared action
//         (non-deterministic branching on multiple actions ok)
//     * compatibility = no new deadlock
//     * state explosion (50-100% of max #states)
//
// - interface automata composition
//     * consumer may stall at shared input  (multi input ok)
//     * producer stall at shared output is illegal
//     * compatibility = no reachable illegal states after composition w/env
//     * least restrictive env:  accepts all outputs, produces no inputs
//         + optimistic compatibility = compatibility w/least restrictive env
//         + any state leading to producer stall
//             via output/internal transitions is incompatible
//       --> optimistic compatibility = non-empty composition
//                                      after pruning illeal+incompat
//     * no state explosion (100-200% of orig #states?)
//
// - SCORE automata composition  [not implemented]
//     * consumer may stall at shared input  (multi input ok)
//     * producer stall at shared output is illegal
//         (fixed by queue with stutter output transitions)
//     * compatibility = no reachable illegal states
//     * compatibility = no reachable illegal states after composition
//         with ``open'' env (accepts all outputs, produces all inputs)
//     * BUT under this definition, there are unnecessary illegal states
//         that might have been avoided by earlier non-deterministic branch
//     * how to prune transitions leading to illegal?
//         + conjecture:  prune single-transition chains back to
//                        state w/non-deterministic branch b/w actions of
//                        either automaton; take branch away from illegal
//                        (but this is not minimal composition)
//         + conjecture:  prune internal / output transitions;
//                        prune input transitions as above?
