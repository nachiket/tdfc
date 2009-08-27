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


#ifndef _TDF_MINCUT_
#define _TDF_MINCUT_

#include <LEDA/graph/graph.h>

using leda::list;
using leda::graph;
using leda::GRAPH;
using leda::node;
using leda::edge;
using leda::node_array;
using leda::edge_array;
using leda::string;

class MinCutMaxFlow_Parts;

extern void minCutMaxFlow (graph& G, edge_array<int>& weights,
			   node src, node dst, node_array<bool>& cutset);

extern void minCutMaxFlow_inParts (graph& G, edge_array<int>& weights,
				   node src, node dst,
				   node_array<bool>& cutset);

extern void minCutMaxFlow_init    (MinCutMaxFlow_Parts& p);
extern bool minCutMaxFlow_augment (MinCutMaxFlow_Parts& p);
extern void minCutMaxFlow_cutset  (MinCutMaxFlow_Parts& p);
extern void minCutMaxFlow_exit    (MinCutMaxFlow_Parts& p);


class MinCutMaxFlow_Parts
{
public:
  graph		    &G;
  node		    &src,     &dst;
  edge_array<int>   &weights, &flows, &residual;
  node_array<bool>  &cutset;
  list<edge>	    added_edges;

  MinCutMaxFlow_Parts (graph& Gi, edge_array<int>& wi, node& si, node& di,
		       edge_array<int>& fi, edge_array<int>& ri,
		       node_array<bool>& ci)
    : G(Gi), src(si), dst(di), weights(wi),
      flows(fi), residual(ri), cutset(ci) {}
};

#endif	// #ifndef _TDF_MINCUT_
