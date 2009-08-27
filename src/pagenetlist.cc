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
// $Revision: 1.148 $
//
//////////////////////////////////////////////////////////////////////////////


#include <fstream>
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "pagedfg.h"
#include "pagenetlist.h"

using leda::node_array;
using leda::edge;
using leda::node;
using std::ofstream;

////////////////////////////////////////////////////////////////
//  globals

bool gEmitPageNetlist;		// - emit page netlist, "-epn" option


////////////////////////////////////////////////////////////////

void printPageNetlist (OperatorCompose *iop, PageDFG *dfg)
{
  // - print a page-level netlist for a compose-op instance
  //     into file "<iopname>.pagenetlist"
  // - Randy Huang's netlist format:  "numOfNets
  //                                   src snk
  //                                   src snk ..."
  //   where src, snk are integers, >=0 for pages/segments, -1 for primary IO

  // - number nodes:  op nodes count up from 0, sym (pio) nodes -1
  node_array<int> nodenums(*dfg);
  node n;
  int i=0;
  forall_nodes (n,*dfg) {
    if ((*dfg)[n]->getKind()==TREE_OPERATOR)
      nodenums[n]=i++;
    else
      nodenums[n]=-1;
  }

  // - open output file
  string filename=iop->getName()+".pagenetlist";
  ofstream fout(filename);
  // ofstream fout=cout;

  fout << "### PAGE NETLIST: " << iop->getName() << "\n";

  // - emit comments showing node number, op name, sym name
  // node n;
  forall_nodes (n,*dfg) {
    Tree *t=(*dfg)[n];
    if (t->getKind()==TREE_OPERATOR)
      fout << "# node " << nodenums[n] << " (" << n << ")"
	   << " = op  " << ((Operator*)t)->getName() << '\n';
    else if (t->getKind()==TREE_SYMBOL)
      fout << "# node " << nodenums[n] << " (" << n << ")"
	   << " = sym " << ((Symbol*)t)->getName()   << '\n';
  }

  // - emit netlist
  fout << dfg->number_of_edges() << '\n';
  edge e;
  forall_edges (e,*dfg)
    fout << nodenums[dfg->source(e)] << ' '
	 << nodenums[dfg->target(e)] << '\n';

  // - close output file
  fout.close();
}


OperatorCompose* page_netlist_instance (OperatorCompose *iop,
					list<OperatorBehavioral*> *page_ops)
{
  // - compute + print a page-level netlist (DFG)
  //     for a flattened compose-op instance (i.e. flat page graph)

  PageDFG dfg;
  buildPageDFG(iop,&dfg);
  printPageNetlist(iop,&dfg);
  return iop;	// - dummy
}
