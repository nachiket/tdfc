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
// SCORE TDF compiler:  State clustering for partitioning
// $Revision: 1.140 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_CLUSTERSTATES_
#define _TDF_CLUSTERSTATES_

#include <LEDA/core/list.h>
#include <LEDA/core/set.h>
#include <LEDA/graph/graph.h>
#include "ir_graph.h"

using leda::node_array;
using leda::edge_array;

extern float gDFSFmix;	// mix frac [0..1] for data-flow/state-flow edge weight

class Operator;
class OperatorBehavioral;
class OperatorCompose;

list<set<node> > clusterStates (GRAPH<SFNode*,SFEdge*> &sfg,
				int page_area_min, int page_area_max,
				edge_array<int> *Freqs=NULL,
				node_array<int> *Areas=NULL);

Operator* clusterStates_instance (OperatorCompose *iop,
				  list<OperatorBehavioral*> *page_ops,
				  int page_area_min, int page_area_max);

void printStateClusterStats ();

#endif	// #ifndef _TDF_CLUSTERSTATES_
