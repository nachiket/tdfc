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
// SCORE TDF compiler:  Creates operator-level data-flow graph
// $Revision: 1.140 $
//
//////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include "operator.h"
#include "stmt.h"
#include "annotes.h"
#include "ir_graph.h"


using std::ofstream;
using leda::list_item;

extern OFGraph gOFG;

////NOTE  does not allow strings to be replicated from one out to many in.
////TODO: automatically generate the fanout operator.
////TODO: does not care about assignments for now: (a = iDSUB(a,b,c)) in compose op.

void PrintOFG(){
  string opname= gOFG->inf(gOFG->first_node())->getOperator()->getName();
  string fname = "OFG.vcg";
  ofstream *fout = new ofstream(fname);
  *fout<<"graph: { title: \""<<opname<<"\"\n";
  *fout<<"display_edge_labels: yes\n";
  *fout<<"color: lightgray\n";
  int counter=0;
  node n;
  forall_nodes(n,*gOFG){
    OFNode* ofn = gOFG->inf(n);
    OFNodeKind ok=ofn->getKind();
    string source_label=ofn->getOperator()->getName();
    string source_title=string("%d",counter++);
    ofn->setNameforGraph(source_title);
    *fout<<"node: { title: \""<<source_title<<"\" label:\""<<source_label<<"\" ";
    if (ok==OFN_BEHAVIORAL)
      *fout<<"color: orange ";
    else
      *fout<<"color: lightyellow ";
    *fout<<"textcolor: darkgreen }\n";
  }
  forall_nodes(n,*gOFG){
    edge ed;
    forall_out_edges(ed,n){
      string target_name = gOFG->inf(gOFG->target(ed))->getNameforGraph();
      *fout<<"edge: { sourcename: \""<<gOFG->inf(n)->getNameforGraph()<<
	"\" targetname: \""<<target_name<<"\" label: \""
	   <<gOFG->inf(ed)->getName() <<"\" color: red}\n";
    }
  }
  *fout<<"\n}\n";
  fout->close();
}


void CreateOFG(Operator* op, node n_in, node n_out){
  //cerr << "IN OPER " << op->getName() <<endl ;
  list<Symbol*>* values = ((OperatorCompose*)op)->getVars()->getSymbolOrder();
  list<Symbol*> *value_list = new list<Symbol*>;
  Symbol *sym;
  forall (sym, *values) {
    value_list->append(sym);
  }
  list<ExprCall*> * exprcall_list = new list<ExprCall*> ;
  Stmt* stmt;
  forall(stmt, *((OperatorCompose*)op)->getStmts())
    if (stmt->getStmtKind() == STMT_CALL){
      ExprCall* exprcall = ((StmtCall*) stmt)->getCall();
      exprcall_list->append(exprcall);
      
      // 1/23/01 the following cleaned up: all operators are behavioural now, 
      // because the whole graph has been already flattened in Andre's code
      
      //OperatorKind opkind = exprcall->getOp()->getOpKind();
      //cerr <<"call found to "<< exprcall->getOp()->getName()<<endl;
      //if(opkind == OP_BEHAVIORAL){
      node newnode =  gOFG->new_node(new OFNode(exprcall->getOp(),OFN_BEHAVIORAL,exprcall));
      exprcall->getOp()->setAnnote(TO_BEH_OP_INSTANCE, (void*) newnode);  
	
	
      ///////////////////////////////////
      /// CREATE ofn->StreamsIn()
      //////////////////////////////////
      list <Symbol*> * op_args = ((OperatorBehavioral*)exprcall->getOp())->getArgs();
      Symbol* sym;
      forall(sym, *op_args){
	if (sym->isStream() && ((SymbolStream*)sym)->getDir()==STREAM_IN){
	  gOFG->inf(newnode)->getStreamsIn()->insert(sym->toString(),sym);
	  //cerr << "STREAM_IN "<<sym->toString()<<endl;
	}
      }
    }
  //else if (stmt->getStmtKind() == STMT_ASSIGN) ....  // NOTE THIS CAN BE AN ASSIGN....TODO
  
  list<Symbol*>* streams = op->getArgs();
  Symbol* sym_stream;
  
  list<Symbol*>* arg_sym_out;
  list<Symbol*>* arg_sym_in;
  list<Expr*>* arg_val_out;
  list<Expr*>* arg_val_in;
  ExprCall* ec_out;
  ExprCall* ec_in;
  list_item it_val_out;
  list_item it_val_in;
  forall(ec_out, *exprcall_list){       // for all calls
    arg_sym_out = ec_out->getOp()->getArgs();
    arg_val_out = ec_out->getArgs();
    forall(ec_in,*exprcall_list){       // for all calls
      arg_sym_in = ec_in->getOp()->getArgs();
      arg_val_in = ec_in->getArgs(); 
      Symbol* sym_out;
      it_val_out = arg_val_out->first();
      forall(sym_out, *arg_sym_out){     // forall (out) streams of the call
	//cerr << "OUT " << sym_out->toString() << endl;
	if(sym_out->isStream() && ((SymbolStream*)sym_out)->getDir()==STREAM_OUT){
	  Symbol* val_out = ((ExprLValue*)arg_val_out->inf(it_val_out))->getSymbol();
	  Symbol* sym_in;
	  it_val_in = arg_val_in->first();
	  forall(sym_in, *arg_sym_in){     //forall (in) streams of succ calls
	    if(sym_in->isStream() && ((SymbolStream*)sym_in)->getDir()==STREAM_IN){
	      Symbol* val_in = ((ExprLValue*)arg_val_in->inf(it_val_in))->getSymbol();
	      if ( val_out == val_in){
		if(! value_list->rank(val_in))   
		  fatal (-1,string("\n\n\nillegal multiple use or definition of stream \""+val_in->toString()+"\" in operator \""+op->getName()+"\""));
		value_list->remove(val_in);
                node node_out;
		node node_in;
		node_out = (node) ec_out->getOp()->getAnnote(TO_BEH_OP_INSTANCE); //behavioural out
		//fatal (-1,string("DO GET HERE"));
		node_in = (node) ec_in->getOp()->getAnnote(TO_BEH_OP_INSTANCE); //behavioural in
		gOFG->new_edge(node_out,node_in, new OFEdge(val_in->toString())); 
		//new OFEdge(sym_out,sym_in, val_in)); this is what the above used to be
	      }
	    }
	    it_val_in = arg_val_in->succ(it_val_in);
	  }   //for all sym_in
	} //if OUT
	it_val_out = arg_val_out->succ(it_val_out);
      } //forall sym_out
    } // forall ec_in
    forall(sym_stream, *streams)
      if(sym_stream->isStream()){  // no parameters
        Expr* arg_val;
	forall(arg_val,*arg_val_out)
	  if (arg_val->getExprKind() == EXPR_LVALUE &&  
	      ((ExprLValue*)arg_val)->getSymbol() == sym_stream){
	    node node_from;
	    node node_to;
	    if (!arg_val->isLValue()){
	      //cerr << "STREAMin " << sym_stream->toString() << endl;
	      node_from=n_in; //top level compose_out
	      node_to =(node)ec_out->getOp()->getAnnote(TO_BEH_OP_INSTANCE); //behav. out
	    }
	    else{
	      //cerr << "STREAMout " << sym_stream->toString() << endl;
	      node_to = n_out; //top level compose_in!
	      node_from = (node) ec_out->getOp()->getAnnote(TO_BEH_OP_INSTANCE); //behavioural in
	    }
	    //fatal (-1,string("DO GET HERE"));
	    gOFG->new_edge(node_from,node_to, new OFEdge(sym_stream->toString()));
	    //new OFEdge(sym_stream,sym_stream,sym_stream); this is what the above used to be
	  }
      }
  } //forallec_out
  //gOFG->print("OFG graph");
}




