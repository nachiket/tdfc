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
// SCORE TDF compiler:  Min-cut / max-flow
// $Revision: 1.124 $
//
//////////////////////////////////////////////////////////////////////////////


// LEDA BUG
//   - symptom:      iterate "forall (e,G.out_edges(v))" destroys graph!!!
//   - work-around:  iter over copy "list<edge> G_out_edges_v=G.out_edges(v);"


// #include <assert.h>
#include "mincut.h"


using std::cerr;
using std::endl;

////////////////////////////////////////////////////////////////
//  Debug Printing

void printNode (graph& G, node v)
{
  GRAPH<string,string>& Gss = (GRAPH<string,string>&)G;
  cerr << "node " << Gss[v] << " incoming={";
  edge e;
  list<edge> G_in_edges_v=G.in_edges(v);
  // forall (e,G.in_edges(v))
  forall (e,G_in_edges_v)
    cerr << (e ? Gss[Gss.source(e)] : string("nil")) << ' ';
  cerr << "} outgoing={";
  list<edge> G_out_edges_v=G.out_edges(v);
  // forall (e,G.out_edges(v))
  forall (e,G_out_edges_v)
    cerr << (e ? Gss[Gss.target(e)] : string("nil")) << ' ';
  cerr << "}\n";
}


void printPathArray (graph& G, node_array<edge>& path)
{
  GRAPH<string,string>& Gss = (GRAPH<string,string>&)G;
  node v;
  cerr << "Path Array: ";
  forall_nodes (v,Gss) {
    edge e=path[v];
    cerr << Gss[v]
	 << (e ? ("<-" + Gss[e] + "-" + Gss[Gss.source(e)]) : string(""))
         << ' ';
  }
  cerr << '\n';
}


void printPath (graph& G, node_array<edge>& path, node src, node dst)
{
  GRAPH<string,string>& Gss = (GRAPH<string,string>&)G;
  node v;
  edge e;
  cerr << "Path: {" << Gss[dst] << "} <-- ";
  for (v=dst; (e=path[v]); v=Gss.source(e))
    cerr << Gss[e] << " ";
  cerr << " <-- {" << Gss[src] << "}\n";
}


////////////////////////////////////////////////////////////////
//  Min-Cut

void reachableByWeight (graph& G, edge_array<int>& weights, node src,
			node_array<bool>& reached)
{
  // Visit each unreached node v of G that is reachable from src
  // along path of positive-weight edges (using DFS),
  // and mark each v reached.

  reached[src]=true;
  edge e;
  list<edge> G_out_edges_src=G.out_edges(src);
  // forall (e,G.out_edges(src)) {		// - (leda bug)
  forall (e,G_out_edges_src) {
    node v=G.target(e);
    if (weights[e]>0 && !reached[v])
      reachableByWeight(G,weights,v,reached);
  }
}


bool shortestPathUnitWeight (graph& G, edge_array<int>& weights,
			     node src, node dst, node_array<edge>& path)
{
  // Finds shortest path in G along positive-weight edges,
  // with weights treated as unit weight (uses BFS).
  // Forms reverse path:  path[dst]=(last edge of path), path[src]=nil
  // Returns true if finds a path, false if no path.

  // assert(src!=dst);
  path.init(G,NULL);
  list<node> frontier;
  node_array<int> color(G,0);		  // 0=unvisited, 1=frontier, 2=done
  color[src]=1;
  frontier.append(src);
  while (!frontier.empty()) {
    node v=frontier.pop();		  // - grab frontier node v
    // cerr << " - shortest paths at node " << v << '\n';
    edge e;
    list<edge> G_out_edges_v=G.out_edges(v);
    // forall (e,G.out_edges(v)) {	  // - (leda bug)
    forall (e,G_out_edges_v) {		  // - check neighbors d of v
      node d=G.target(e);
      if (weights[e]>0 && color[d]==0) {  // - add reachable neighbors to frntr
	path[d]=e;
	if (d==dst)			  // - bail early if find dst
	  return true;
	color[d]=1;
	frontier.append(d);
	// cerr << " - shortest paths adding node " << d
	//      << " thru edge " << e << " weight " << weights[e] << '\n';
      }
    }
    color[v]=2;
  }
  return false;				  // - no path if get here
}


bool augmentFlow (graph& G, edge_array<int>& flows,
			    edge_array<int>& residual, node src, node dst)
{
  // Add residual flow along shortest (unit-weight) path from src to dst,
  // adding to flows and removing from residual.

  node_array<edge> path(G,NULL);
  shortestPathUnitWeight(G,residual,src,dst,path);

  // cerr << "Augmenting "; printPath(G,path,src,dst);
  // cerr << "Augmenting "; printPathSG(G,path,src,dst);

  if (path[dst]) {
    // - found augmenting path
    // - first, find constraining residual flow along path (aug_flow)
    node v;
    edge e;
    int aug_flow=residual[path[dst]];
    for (v=dst; (e=path[v]); v=G.source(e)) {
      int r=residual[e];
      if (aug_flow>r)
	aug_flow=r;
    }
    // - second, add constraining residual flow to actual flow
    for (v=dst; (e=path[v]); v=G.source(e)) {
      edge re=G.reversal(e);
      residual[e] -=aug_flow;
      residual[re]+=aug_flow;
      flows[e]    +=aug_flow;
      flows[re]   -=aug_flow;
    }

    // cerr <<"Augmenting "<<aug_flow <<" along "; printPath(G,path,src,dst);
    // cerr <<"Augmenting "<<aug_flow <<" along "; printPathSG(G,path,src,dst);

    return true;
  }
  else {
    // - no augmenting path
    return false;
  }
}


void minCutMaxFlow (graph& G, edge_array<int>& weights, node src, node dst,
		    node_array<bool>& cutset)
{
  // Partition graph G by min-cut s.t. src is in cutset, dst is not.
  // Cut-set is returned in cutset as array of boolean membership indicators.
  // Uses Ford-Fulkerson augmenting paths method, Edmonds Karp algorithm.

  list<edge> added_edges;		// - reverse edges added to G for flow
  G.make_map(added_edges);		// - add reverse edges to G
  
  edge_array<int> flows(G,0);		// - initial flows = 0  [not used]
  edge_array<int> residual(G);		// - initial residual flows = caps (w)
  edge e;
  forall_edges (e,G)
    if (added_edges.search(e))
      residual[e]=0;
    else
      residual[e]=weights[e];

  while (augmentFlow(G,flows,residual,src,dst))
					// - repeatedly add augmenting paths
  {
    cerr << "Residual Capacities: ";
    forall_edges (e,G)
      cerr << ((GRAPH<string,string>&)G)[e] << ":" << residual[e] << " ";
    cerr << "\n\n";
  }

  forall (e,added_edges)		// - remove added reverse edges from G
    G.del_edge(e);

  cerr << "Residual Capacities in orig graph: ";
  forall_edges (e,G)
    cerr << ((GRAPH<string,string>&)G)[e] << ":" << residual[e] << " ";
  cerr << "\n\n";

  cutset.init(G,false);
  reachableByWeight(G,residual,src,cutset);
					// - create cutset as nodes reachable
					//   in residual flow graph

}


////////////////////////////////////////////////////////////////
//  Min-Cut In Parts
//    (parts of minCutMaxFlow() available separately for modified algorithms)

void minCutMaxFlow_init (MinCutMaxFlow_Parts& p)
{
  // minCutMaxFlow() in parts:  initialization

  p.G.make_map(p.added_edges);		// - add reverse edges to G
  
  p.flows.init(p.G,0);			// - initial flows = 0  [not used]
  p.residual.init(p.G);			// - initial residual flows = caps (w)
  edge e;
  forall_edges (e,p.G)
    if (p.added_edges.search(e))
      p.residual[e]=0;
    else
      p.residual[e]=p.weights[e];
}


bool minCutMaxFlow_augment (MinCutMaxFlow_Parts& p)
{
  // minCutMaxFlow() in parts:  augment flow

  return augmentFlow(p.G, p.flows, p.residual, p.src, p.dst);
}


void minCutMaxFlow_cutset (MinCutMaxFlow_Parts& p)
{
  // minCutMaxFlow() in parts:  get cutset from residual graph

  p.cutset.init(p.G,false);
  reachableByWeight(p.G, p.residual, p.src, p.cutset);
					// - create cutset as nodes reachable
					//   in residual flow graph
}


void minCutMaxFlow_exit (MinCutMaxFlow_Parts& p)
{
  // minCutMaxFlow() in parts:  clean up

  edge e;
  forall (e,p.added_edges)		// - remove added reverse edges from G
    p.G.del_edge(e);
}


void minCutMaxFlow_inParts (graph& G, edge_array<int>& weights,
			    node src, node dst, node_array<bool>& cutset)
{
  // minCutMaxFlow() in parts.
  // This is a skeleton for modified min-cut/max-flow algorithms.

  edge_array<int> flows, residual;
  MinCutMaxFlow_Parts p(G,weights,src,dst,flows,residual,cutset);
  minCutMaxFlow_init(p);
  while (minCutMaxFlow_augment(p));
  minCutMaxFlow_cutset(p);
  minCutMaxFlow_exit(p);
}
