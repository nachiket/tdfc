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
// SCORE TDF compiler:  Internal Representation
// $Revision: 1.131 $
//
//////////////////////////////////////////////////////////////////////////////


#include "ir_graph.h"
#include "operator.h"

OFNode::OFNode(Operator* op, OFNodeKind k, ExprCall* ex): //=nil?
    oper(op),kind(k), exprcall(ex){
  to_SFG=           new GRAPH<SFNode*,SFEdge*>;
  to_DFG=           new GRAPH<DFNode*,DFEdge*>;
  registers =       new dictionary <string,Symbol*>; 
  ini_reg_values =  new dictionary <string,ExprValue*>; 
  streams_in =      new dictionary <string,Symbol*>;
  streams_out =     new dictionary <string,Symbol*>;
  non_bool_conds =  new list <list<Tree*>*>; 
  exprs_to_muxes=   new dictionary <string,list<list<Tree*>*>*>;
  mux_bw =          new dictionary <string,int>;
  unex_area=        0; 
  area=             0;
  ip_ex_area=       0;
  op_ex_area=       0;
  name_for_graph=   "";
  defUsePairs=      new dictionary <string,DefUsePairList*>;
}
SCCell::SCCell(SFNode* sfn,StateCase* sc): to_SFN(sfn),statecase(sc){
  to_DFG = new GRAPH<DFNode*,DFEdge*>; 
  defined_registers=new dictionary <string,DFNode*>; 
  used_registers=new dictionary<string,node>;
  written_streams=new dictionary <string,DFNode*>;
  used_streams=new dictionary <string,node>;
  the_schedule = new list< set<node>* >;  unex_area=0;
  IPinputs = new set<string>;
  OPoutputs = new set<string>;
  IPinterm = new set<string>;
  OPinterm = new set<string>;
  in_str_left_in = new set<string>;
  area=0;
  ip_ex_area=0;
  op_ex_area=0;
  unex_area=0;
  done=0;
  hidden=0;
}
SFNode::SFNode(State* st):state(st){
  sccells=        new list<SCCell*>;
  sccells->append(new SCCell(this)); //create reg_case cell
  regular_sccell = sccells->front();
  consumed_streams = new set<string>;
}

DFEdge::DFEdge(string st):name(st){
  ready_sched=0;
  ready_IPextr=0;
  ready_OPextr=0;
}
DFNode::DFNode(Expr* ex,DFNodeKind dk,string nm,ExprLValue* dst):
  ast_node_expr(ex), dfnkind(dk),name(nm),dest (dst){
  IP_extracted=0;
  OP_extracted=0;
  assigned=nil;
  area=0;
  scheduled=0;
  name_for_graph="";
  inputs = new (list<ExprLValue*>);
  to_ofedge=nil;
  to_dfe = new DFEdge(name);
}
SFEdge::SFEdge(SCCell* stcs, Expr* cond, bool outc):
    sc_cell(stcs), condition(cond),outcome(outc){
  cond_string="in";
  all_bool=1;
  rank_mux = new dictionary<string,int>;
}
//extern OFGraph gOFG;

/*void print_OFG(OFGraph OFG){
  node n;
  int node_cont=0;
  forall_nodes(n,*OFG){
    cout<< "node " << node_cont << ": op. \"" << 
      OFG->inf(n)->getOperator()->getName()<< "\""<< endl;
    node_cont++;
    edge e;
    forall_out_edges(e,n){
      node target_node =  OFG->target(e);
      cout<< "\tedge: to op. \"" << OFG->inf(target_node)->getOperator()->getName() <<
	"\"\t\""<<  OFG->inf(e)->getSymOut()->toString() << "\"\tto \"" <<
	OFG->inf(e)->getSymIn()->toString() << "\"\tvia \"" <<  
	OFG->inf(e)->getArg()->toString() <<  "\"" <<endl;
    }
  }
}
*/


