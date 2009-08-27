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
// SCORE TDF compiler:  Page-level data-flow graph for AT&T graphviz
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#include <LEDA/graph/graph.h>
#include <fstream>
#include "operator.h"
#include "misc.h"
#include "everilog.h"		// - for instanceSegmentOps()
#include "pagedfg.h"
#include "pagenetlistdot.h"

using leda::node;
using leda::edge;
using leda::node_array;
using leda::graph;
using leda::set;
using std::ofstream;

////////////////////////////////////////////////////////////////
//  find SCCs

string nodeToString (node n, graph *g)
{
  // - debug findSCC() when applied to PageDFG *g

  if ((*g).all_nodes().rank(n)==0)
    return "<ERROR>";
  else {
    Tree *t = (*(PageDFG*)g)[n];
    return (t ? (t->getKind()==TREE_OPERATOR ? ((Operator*)t)->getName()
					     : (  (Symbol*)t)->getName())
	      : string("<start>"));
  }
}


void sccVisitNode (node n, graph *g,
		   set<node> *visited, list<node> *dfsStack,
		   node_array<int> *nodeNum,
		   node_array<int> *earliestReachableNum,
		   int *nodeCounter, list<set<node>*> *SCCs)
{
  // - recursive part of findSCC()

  // cerr << "### visit " + nodeToString(n,g)
  //      << " (stack contains " << (*dfsStack).size() << ")\n";	// ***
  // if ((*visited).member(n))
  //   cerr << "### REVISITING\n";					// ***

  (*visited).insert(n);
  (*dfsStack).push(n);
  (*earliestReachableNum)[n] = (*nodeNum)[n] = (*nodeCounter);
  (*nodeCounter)++;

  node t;
  list<node> adjNodes = (*g).adj_nodes(n);
  forall (t, adjNodes) {
    if (!(*visited).member(t)) {
      sccVisitNode(t,g,visited,dfsStack,
		   nodeNum,earliestReachableNum,nodeCounter,SCCs);
      if ((*earliestReachableNum)[n] > (*earliestReachableNum)[t])
	  (*earliestReachableNum)[n] = (*earliestReachableNum)[t];
    }
    else {
      if ((*earliestReachableNum)[n] > (*nodeNum)[t])
	  (*earliestReachableNum)[n] = (*nodeNum)[t];
    }
  }

  if ((*earliestReachableNum)[n]==(*nodeNum)[n]) {
    // - found SCC, recorded in dfsStack from n to top
    //     (n is head of SCC subtree of DFS tree)
    set<node> *scc = new set<node>;
    node x;
    // cerr << "--- new component { ";				// ***
    do {
      x = (*dfsStack).pop();
      // cerr << nodeToString(x,g) << " ";			// ***
      scc->insert(x);
      (*g).hide_node(x);
    } while (x!=n);
    (*SCCs).append(scc);
    // cerr << "}\n";						// ***
    // cerr << "--- now have " << (*SCCs).size() << " SCCs\n";	// ***
  }
}


void findSCCs (graph *g, list<set<node>*> *SCCs)
{
  // - Find strongly connected components of *g, return in SCCs
  // - WARNING:  modifies g's hidden nodes;  unhides all nodes on exit
  // - http://www.ics.uci.edu/~eppstein/161/960220.html , tdfc/info/scc.html

  (*SCCs).clear();

  // - add fake start node, connected to every other node
  // - this connects all components and creates a natural start for DFS
  node start = (*g).new_node();
  node n;
  forall_nodes (n, *g)
    (*g).new_edge(start,n);

  // - DFS-based SCC recognition
  // - sccVisitNode recurses, builds SCCs, and hides nodes
  set<node>  visited;
  list<node> dfsStack;
  node_array<int> nodeNum(*g,0), earliestReachableNum(*g,0);
  int nodeCounter=0;
  sccVisitNode(start,g,&visited,&dfsStack,
	       &nodeNum,&earliestReachableNum,&nodeCounter,SCCs);

  // - get rid of SCC of fake start
  set<node> *scc;
  forall (scc, *SCCs) {
    if (scc->size()==1 && scc->choose()==start) {
      // cerr << "--- deleting component of <start>\n";		// ***
      (*SCCs).remove(scc);
      break;
    }
  }

  // - restore *g:  unhide nodes/edges, delete fake start
  (*g).restore_all_nodes();
  (*g).restore_all_edges();
  (*g).del_node(start);

  // forall (scc, *SCCs) {
  //   cerr << "=== component { ";				// ***
  //   node n;
  //   forall (n, *scc)
  //     cerr << nodeToString(n,g) << " ";			// ***
  //   cerr << "}\n";
  // }

}


void deleteSCCs (graph *g, list<set<node>*> *SCCs)
{
  // - deallocate SCCs allocated by findSCCs()
  set<node> *scc;
  forall (scc, (*SCCs))
    delete scc;
  (*SCCs).clear();
}


////////////////////////////////////////////////////////////////
//  compare() for LEDA containers

inline int compare (set<node>* const& a, set<node>* const& b) {
  return a==b ? 0 : a<b ? -1 : 1;
}


////////////////////////////////////////////////////////////////
// dot emission

string pagdDFGtoDotString (OperatorCompose *op, PageDFG *dfg,
			   bool showSCC=true)
{
  string ret;

  ret +=   "# Page DFG for " + op->getName()
      +  "\n# " + tdfcComment() + "\n";

  ret += "digraph " + op->getName() + " {\n"
      +  "  graph [label=\"" + op->getName() + "\""
  //  +  ", fillcolor=\"lightgrey\""
      +  "];\n";

  list<set<node>*> SCCs;	// - set<set<node>*> is buggy, so use this
  node_array<set<node>*> nodeToSCC(*dfg,NULL);
  if (showSCC) {
    // - find SCCs and initialize nodeToSCC
    findSCCs(dfg,&SCCs);
    set<node> *scc;
    forall (scc, SCCs) {
      node n;
      forall (n, *scc)
	nodeToSCC[n] = scc;
    }
  }

  // - create dot graph's operator nodes (boxes)
  node n;
  forall_nodes (n,*dfg) {
    Tree *t=(*dfg)[n];
    if (t->getKind()==TREE_OPERATOR) {
      string color = (showSCC && nodeToSCC[n]->size()>1)
			? ", color=\"red\"" : "";
      string fill  = ", style=\"filled\", fillcolor=\"lightgrey\"";
      ret += "  " + ((Operator*)t)->getName()
	  +  " [shape=\"box\"" + color + fill + "];\n";
    }
    else
      ; // no box for primary I/O nodes
  }

  // - create dot graph's stream nodes (diamonds) + arcs
  edge e;
  forall_edges (e,*dfg) {
    node srcNode = (*dfg).source(e);
    node dstNode = (*dfg).target(e);
    Symbol *sym = (*dfg)[e];
    Tree   *src = (*dfg)[srcNode];
    Tree   *dst = (*dfg)[dstNode];
    bool symIsPIO = op->getSymtab()->lookup(sym->getName());
    bool symIsPI  = symIsPIO && src->getKind()==TREE_SYMBOL;
    bool symIsPO  = symIsPIO && dst->getKind()==TREE_SYMBOL;
    string fill   = symIsPI ? ", style=\"filled\", fillcolor=\"lightblue\""
		  : symIsPO ? ", style=\"filled\", fillcolor=\"lightpink\""
		  : "" ;
    bool   srcDstInSameSCC = (nodeToSCC[srcNode]==nodeToSCC[dstNode]);
    string color     = (showSCC && srcDstInSameSCC) ? ", color=\"red\""  : "";
    string edgecolor = (showSCC && srcDstInSameSCC) ? " [color=\"red\"]" : "";
    ret += "  " + sym->getName() +" [shape=\"diamond\""
                + color + fill + "];\n";
    if (src->getKind()==TREE_OPERATOR)
      ret += "  " + ((Operator*)src)->getName() + " -> "
	          + sym->getName()              + edgecolor + ";\n";
    if (dst->getKind()==TREE_OPERATOR)
      ret += "  " + sym->getName()              + " -> "
	          + ((Operator*)dst)->getName() + edgecolor + ";\n";
  }

  // - clean up
  if (showSCC)
    deleteSCCs(dfg,&SCCs);

  ret += "}\n";
  return ret;
}

OperatorCompose* page_netlist_dot_instance (OperatorCompose *iop,
					    list<OperatorBehavioral*> *page_ops)
{
  string fileName = iop->getName() + ".dot";
  ofstream fout(fileName);
  if (!fout)
    fatal(1,"tdfc could not open output file "+fileName);

  OperatorCompose *iop_copy = (OperatorCompose*)iop->duplicate();
  iop_copy->link();
  instanceSegmentOps(iop_copy);
  PageDFG dfg;
  buildPageDFG(iop_copy,&dfg);
  fout << pagdDFGtoDotString(iop_copy,&dfg);
  // delete iop_copy;		// - WARNING:  crashes?  memory leak?

  fout.close();
  return iop;	// - dummy
}
