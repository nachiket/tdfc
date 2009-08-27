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
// SCORE TDF compiler: Creates Data Flow Graph for each statecase  
// $Revision 1.126$
//
//////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include "stmt.h"
#include "ir_graph.h"
#include "annotes.h"
#include "operator.h"
#include "misc.h"

using std::ofstream;
using std::cerr;
using std::endl;

using leda::dic_item;

void PrintDFG(SCCell* scc, OFNode* ofn,int rank, bool it_is_pipe){
  DFGraph DFG=(it_is_pipe)?ofn->getDFG():scc->getDFG();
  string eosname=!rank?string(""):string("___eos%d",rank);
  string statename=it_is_pipe?string("PIPE"):scc->getSfn()->getState()->getName();
  string fname = ofn->getOperator()->getName()+"___"+statename+eosname+".vcg";
  node n;
  int counter=0;
  ofstream *fout = new ofstream(fname);
  *fout<<"graph: { title: \""<<fname<<"\"\n";
  *fout<<"color: lightgray\n";
  forall_nodes (n,*DFG){
    DFNode* dfn=DFG->inf(n);
    string source_name=string("%d",counter++);
    dfn->setNameforGraph(source_name);
    DFNodeKind dfnk = dfn->getDFNodeKind();
    *fout<<"node: { title: \""<<source_name<<"\" label:\""
	 <<((dfnk==DFN_OP)?(dfn->getExpr()->toString()):dfn->getName())+"___"
	 <<dfn->isScheduled()<<"\"  color: ";
    switch (dfnk)
      {
      case 0:{*fout<<"lightyellow ";break;}
      case 1:{*fout<<"lightred ";break;}
      case 2:{*fout<<"green ";break;}
      case 3:{*fout<<"lightgrey ";break;}
      case 4:{*fout<<"white ";break;}
      case 5:{*fout<<"yellowgreen ";break;}
      case 6:{*fout<<"lightmagenta ";break;}
      case 7:{*fout<<"aquamarine ";break;}
      case 8:{*fout<<"orange ";break;}
      case 9:{*fout<<"magenta ";break;}
      default: *fout<<"pink ";
      }
    string bc = (dfn->isIPExtracted()||dfn->isOPExtracted())? "red":"darkgreen";
    *fout<<"textcolor: darkgreen bordercolor: "<<bc<<" borderwidth: 4 ";
    int step;
    if ((step=dfn->isScheduled()))
      *fout << "level: "<< 2*step;
      *fout<< "}\n";
  }
  forall_nodes(n,*DFG){
    edge ed;
    forall_out_edges(ed,n){
      DFEdge* dfe = DFG->inf(ed);
      bool IP=dfe->isReadyIPExtr();bool OP=dfe->isReadyOPExtr();
      string target_name = DFG->inf(DFG->target(ed))->getNameforGraph();
      string color = 
	(IP&&OP)?"red" : ((IP&&!OP)?"blue":((!IP&&OP)?"orange":"darkgreen"));
      *fout<<"edge: { sourcename: \""<<DFG->inf(n)->getNameforGraph()<<
	"\" targetname: \""<<target_name<<"\" color: "<<color<<" }\n";
    }
    
  }
  *fout<<"\n}\n";
  fout->close();
  
}

void CreateTopLevelNodesEdges(node curr_node, DFGraph DFG, SCCell* scc){
  DFNode * dfn = DFG->inf(curr_node);
  DFNodeKind kind = dfn->getDFNodeKind();  //can only be reg or stream
  bool is_reg = (kind == DFN_REG);  // 1 if it's reg, 0 if it's stream
  ExprLValue* dest=dfn->getDest();
  string str = dfn->getName();
  //WAS THE TOPREG or TOPSTR node already created?
  dic_item used_it=(is_reg)?
    scc->getUsedReg()->lookup(str):
    scc->getUsedStreams()->lookup(str);
  node top_node;
  if(used_it == (dic_item)nil){ // not in UsedReg or UsedStreams yet == no TOP level node yet
    if (is_reg){
      top_node = DFG->new_node(new DFNode((Expr*)dest,DFN_TOP_REG,str,dest));
      scc->getUsedReg()->insert(str, top_node);//create the entry in the dictionary
    }
    else {
      top_node = DFG->new_node(new DFNode((Expr*)dest,DFN_TOP_STR,str,dest));
      scc->getUsedStreams()->insert(str, top_node);
    }
  }
  else{ //TOPLEV node already exists
    if(is_reg)
      top_node=scc->getUsedReg()->inf(used_it);
    else
      top_node=scc->getUsedStreams()->inf(used_it);
    //create edge from topnode to reg/stream
  }
  DFG->new_edge(top_node,curr_node,DFG->inf(top_node)->getDFEdge());
}


void set_dependences_for_following_stmts(node def_node, SCCell* scc){
  //cerr << "into setdep\n";
  DFGraph DFG=scc->getDFG();
  ExprLValue* dest = DFG->inf(def_node)->getDest();
  node curr_node = def_node;
  while (DFG->succ_node(curr_node)){
    curr_node = DFG->succ_node(curr_node);
    DFNode * dfn = DFG->inf(curr_node);
    DFNodeKind kind = dfn->getDFNodeKind();
    //first check if registers need to be put in SCCell:defined_registers
    //this is true if a register is found as input, AND it has not been defined 
    //previously in this state:
    if (kind == DFN_INLVAL || kind == DFN_REG){
      //warn(string("*** assignment ")+*dest);	// EC: assign to self bug?
      //warn(string(" <-- ")+*dfn->getExpr());	// EC: " "
      ExprLValue* val=((ExprLValue*)dfn->getExpr());
      if (val == dest){
	//cerr<<"DEST "<<dest->toString() << " IN  "<< *(dfn->getExpr()) << endl;
	node preass_node = source(DFG->first_in_edge(def_node));
	DFG->new_edge(preass_node,curr_node, DFG->inf(preass_node)->getDFEdge());
	//cerr<< "NEW EDGE FROM: "<< *DFG->inf(preass_node)->getExpr() <<" TO: " <<  *(dfn->getExpr()) << endl;
      }
    }
    else if (kind == DFN_ASSIGN){
      ExprLValue* new_dest = DFG->inf(curr_node)->getDest();
      if (new_dest == dest){  // destination redefined
	//cerr << "destination redefined" << endl;
	return;
      }
    }
    //wlist->pop();
  }
}
void CreateGraph(node assign_node,node parent, Expr* ex,
		 DFGraph DFG,list<ExprLValue*>* inputs, string statename, int* pindex){
  DFG->inf(parent)->getExpr()->toString();
  //cerr << "into creategraph, expr "<<ex->toString();
  node newnode;
  string tmpname;
  string edgename;
  ExprKind kind=ex->getExprKind(); //cerr<< " KIND: " << kind <<endl;
  switch (kind){
  case EXPR_BOP: {
    tmpname = statename+"TMP"+string("_%d",(*pindex)++);
    //cerr << "TMP NAME " << tmpname<<endl;
    newnode = DFG->new_node(assign_node,new DFNode(ex, DFN_OP, tmpname),1);
    CreateGraph(assign_node,newnode,((ExprBop*)ex)->getExpr1(),DFG, inputs, 
		statename, pindex);
    CreateGraph(assign_node,newnode,((ExprBop*)ex)->getExpr2(),DFG, inputs,
		statename, pindex);
    edgename=tmpname;
    break;    
  }
  case EXPR_UOP:{
    tmpname = statename+"TMP"+string((*pindex)++);
    newnode = DFG->new_node(assign_node,new DFNode(ex, DFN_OP, tmpname),1);
    CreateGraph(assign_node,newnode,((ExprUop*)ex)->getExpr(),DFG, inputs,
		statename, pindex);
    edgename=tmpname;
    break;
  }
  case EXPR_VALUE:{
    newnode = DFG->new_node(assign_node,new DFNode(ex,DFN_INVAL,ex->toString()),1);
    edgename=ex->toString();
    break;
  }
  case EXPR_LVALUE:{
    ExprLValue* exlv = (ExprLValue*)ex;
    inputs->append(exlv);  //here I create getInputs()
    //cerr << "appending input " << *ex<<endl;
    DFNodeKind dfnk;
    if (exlv->getSymbol()->isReg())
      dfnk=DFN_REG;
    else if (exlv->getSymbol()->isStream())
      dfnk=DFN_STR;
    else //LValue, not register (local value)
	dfnk=DFN_INLVAL;
    if(exlv->usesAllBits())
      newnode = DFG->new_node(assign_node,new DFNode(ex,dfnk,exlv->toString(),exlv),1);
    else{  //create bit selector node.
      newnode = DFG->new_node(assign_node,new DFNode(ex,DFN_SELECTOR,exlv->toString(),exlv),1);
      node lv_node = DFG->new_node(assign_node,new DFNode(ex,dfnk,exlv->getSymbol()->toString(),exlv),1);
      DFG->new_edge(lv_node,newnode,DFG->inf(lv_node)->getDFEdge());
      CreateGraph(assign_node,newnode,exlv->getPosLow(),DFG, inputs,
		  statename, pindex);
      if (exlv->getPosHigh())
	CreateGraph(assign_node,newnode,exlv->getPosHigh(),DFG, inputs,
		    statename, pindex);
    }
    edgename=ex->toString();
    break;
  }
  case EXPR_CAST:{
    newnode = DFG->new_node(assign_node,new DFNode(ex,DFN_CAST,ex->toString()),1);
    //cerr<<"EXPR CAST\n";
    CreateGraph(assign_node,newnode,((ExprCast*)ex)->getExpr(),DFG, inputs,
		statename, pindex);
    edgename="cast";
    break;
  }
  case EXPR_ARRAY:{
    newnode = DFG->new_node(assign_node,new DFNode(ex,DFN_CAST,ex->toString()),1);
    cerr<<"EXPR ARRAY\n";
    Expr* exp_arr;
    forall(exp_arr,*((ExprArray*)ex)->getExprs()){
      CreateGraph(assign_node,newnode,exp_arr,DFG, inputs,
		  statename, pindex);
      edgename="array";
    }
    break;
  }
  case EXPR_BUILTIN:{
    // - HACK:  (EC) strip bitsof()
    if (((OperatorBuiltin*)((ExprBuiltin*)ex)->getOp())->getBuiltinKind()
	== BUILTIN_BITSOF) {
      // - builtin()
      CreateGraph(assign_node,parent,
		  ((ExprBuiltin*)ex)->getArgs()->head(),DFG,inputs,
		  statename, pindex);
      newnode=nil;
      edgename="bitsof";
    }
    else {
      // - not builtin() -- ignore
      newnode=nil;
    }
    break;
  }
  default: 
    {
      //get back to this.
      newnode=nil;
    }
  }
  if (newnode != (node)nil){ 
    DFG->new_edge(newnode,parent,DFG->inf(newnode)->getDFEdge());
    //ex->setAnnote(TO_NEWREP_NODE, (void*) newnode);
  }
}

void EvalStmt(Stmt* st, SCCell* scc, OFNode* ofn, int *pindex){
  //cerr << "into eval, statement: \n" << st->toString() << endl;
  Expr* expr_passed=nil;	// added init to nil (EC 5/6/02)
  node parent_node =nil;	// ''
  ExprLValue* dest =nil;	// ''
  DFGraph DFG = scc->getDFG();
  switch (st->getStmtKind()){
  case STMT_BLOCK:
    {
      //dest=nil;
      //expr_passed=nil;
      Stmt* stat;
      forall (stat,*((StmtBlock*)st)->getStmts()){
	//cerr<<"block here"<<endl;
	EvalStmt(stat,scc,ofn, pindex);
      }
      break;
    }
  case STMT_IF:
    {
      //dest=nil;
      StmtIf* st_if =  (StmtIf*) st; 
      expr_passed=st_if->getCond();
      parent_node = DFG->new_node
	(new DFNode(expr_passed, DFN_ENDCOND,expr_passed->toString()));
      expr_passed->setAnnote(TO_NEWREP_NODE, (void*) parent_node);
      break;
    }
  case STMT_ASSIGN:
    { 
      expr_passed = ((StmtAssign*)st)->getRhs();
      Expr* expr_to_node = (Expr*) ((StmtAssign*)st)->getLValue();
      dest=((StmtAssign*)st)->getLValue ();
      parent_node=DFG->new_node(new DFNode(expr_to_node, DFN_ASSIGN,dest->toString(),dest));
      DFG->inf(parent_node)->setAssigned(expr_passed);
      st->setAnnote(TO_NEWREP_NODE, (void*) parent_node);
      if (dest->getSymbol()->isStream())
	scc->getWrittenStreams()->insert(dest->toString(),DFG->inf(parent_node));
      break;
    }
  default:// STMT_GOTO:STMT_CALL:STMT_BUILTIN... later
    {
      //dest=nil;
      //cerr << "in default with " << *st;
      expr_passed=nil;
    }
  }
  // if (parent_node==(node)nil) fatal (-1,"PARENT NODE NIL");
						// moved below      (EC 5/6/02)
  
  if (expr_passed != (Expr*)nil){
    if (parent_node==(node)nil) fatal (-1,"PARENT NODE NIL");
						// moved from above (EC 5/6/02)
    CreateGraph(parent_node,parent_node,expr_passed,DFG,DFG->inf(parent_node)->getInputs(),
		scc->getSfn()->getState()->getName(),pindex);
    //if(DFG->inf(parent_node)->getDFNodeKind() == DFN_ENDCOND)
    //cerr << DFG->inf(parent_node)->getInputs()->length() <<" inps for expr "<<*expr_passed<<endl;
  }
}



void CreateDFG(SCCell* scc, OFNode* ofn){
  
  ///////////////////////////////////////////////////
  // CREATE NODES
  ///////////////////////////////////////////////////
  DFGraph  DFG = scc->getDFG();
  int index=0;
  Stmt* stmt;
  forall(stmt, *scc->getStateCase()->getStmts()){  // stmts in state case
    //if the stmt below is not here, it core dumps, and I CANNOT understand why!!! laurap
    stmt->toString();      
    EvalStmt(stmt, scc, ofn, &index); // create nodes (really, subgraphs, one per stmt)
  }
    
  ///////////////////////////////////////////////////
  // CREATE EDGES
  ///////////////////////////////////////////////////
  node n;
  forall(n , DFG->all_nodes()){ 
    DFNode*dfn=DFG->inf(n);
    if (dfn->getDFNodeKind() == DFN_ASSIGN){
      ExprLValue* dst=dfn->getDest();
      if (dst->getSymbol()->isReg())
	scc->getDefReg()->insert(dfn->getName(),dfn);
      set_dependences_for_following_stmts(n, scc);
    }
  }
  ///////////////////////////////////////////
  //NOW CREATE THE TOP LEVEL NODES AND EDGES
  ///////////////////////////////////////////
  forall(n , DFG->all_nodes()){
    //DFNode * dfn = DFG->inf(n);
    DFNodeKind dfkind = DFG->inf(n)->getDFNodeKind();
    if ((dfkind == DFN_REG &&  DFG->in_edges(n).empty()) || dfkind == DFN_STR)
      CreateTopLevelNodesEdges(n, DFG,scc);
  }
}

 




