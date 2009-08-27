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
// SCORE TDF compiler: creates the operator graph
// $Revision 1.122$
//
//////////////////////////////////////////////////////////////////////////////
#include "suite.h"
#include "operator.h"
#include "ir_graph.h"
#include "ir_misc.h"
#include "stmt.h"
#include "annotes.h"
#include "misc.h"

using std::cout;
using std::cerr;
using std::endl;
using leda::dic_item;

OFGraph gOFG = new GRAPH <OFNode*,OFEdge*>; //  top level operator graph

Tree* CreateNewRep(Operator* op, list<OperatorBehavioral*> *op_inst_list){
  cerr << "\n=====\ngenerating new representation for instance...\n";
  gOFG = new GRAPH<OFNode*,OFEdge*>;
  if (op_inst_list->length()>0){ //there exist a top level compositional operator
    node n_in = gOFG->new_node(new OFNode(op,OFN_MEMORY));
    //op->setAnnote(TO_COMP_OP_NODE_IN, (void*) n_in);  
    node n_out = gOFG->new_node(new OFNode(op, OFN_MEMORY));
    //op->setAnnote(TO_COMP_OP_NODE_OUT, (void*) n_out);  
    CreateOFG(op, n_in, n_out); // build OFG inside top level compose op
  }
  // op_inst_list->pop(); //eliminate top level compos op -- DEFUNCT EC 2/1/01
  else{ //one behavioural only
    node n = gOFG->new_node(new OFNode(op, OFN_BEHAVIORAL));
    op->setAnnote(TO_BEH_OP_INSTANCE, (void*) n);
    op_inst_list->append((OperatorBehavioral*)op);
  }
  OperatorBehavioral *bop;
  forall (bop,*op_inst_list){
    ////////////////////////////////////////////////////////
    // CREATE Initial Values list (ofn->getInitialvalues())
    ////////////////////////////////////////////////////////
    SymTab* vars = bop->getVars();
    node n = (node) bop->getAnnote(TO_BEH_OP_INSTANCE);
    OFNode * ofn = gOFG->inf(n);
    //cerr <<"INITVAL OP "<< ofn->getOperator()->getName()<<endl;
    // - EC:  The following code  (vars->getSymbols(), etc.)
    //        does not respect order of symbol declaration/initialization.
    //        It should use vars->getSymbolOrder(),
    //        but correction is not simple, since the dictionary
    //        (vars->getSymbols()) is stored and manipulated in the ofn IR.
    dictionary <string,Symbol*>* D = vars->getSymbols();
    ofn->setRegisters(D);
    //cerr<<"getRegsize "<< ofn->getRegisters()->size()<<endl;
    dic_item itemi;
    forall_items(itemi, *D){
      Symbol* sym = D->inf(itemi);
      //cerr <<"SYM " <<sym->toString()<<endl;
      if (sym->getSymKind() == SYMBOL_VAR){
	Expr* expr = (((SymbolVar*)sym)->getValue());
	if (expr && expr->getExprKind() == EXPR_VALUE){
	  ofn->getInitialvalues()->insert(sym->toString(), (ExprValue*)expr);
	  //cerr<<"INSERTING in INITVAL " << sym->toString() <<" "<< *expr<<endl;
	}
      }
    }
    /////////////////////////////////////
    // CREATE ofn->getStreamsOut()
    /////////////////////////////////////
    list <Symbol*>* arg_list = ofn->getOperator()->getArgs();
    Symbol* sym_arg;
    forall(sym_arg, *arg_list)
      if (sym_arg->isStream()&& ((SymbolStream*)sym_arg)->getDir()==STREAM_OUT)
	ofn->getStreamsOut()->insert(sym_arg->toString(),sym_arg);
    
    //////////////////////////
    // CREATE SFG 
    ////////////////////////// 
    CreateSFG(ofn);
    SFGraph SFG = ofn->getSFG();
    forall(n,SFG->all_nodes()){
      SCCell* scc;
      forall(scc,*SFG->inf(n)->getSCCells())
	//////////////////////  
	//CREATE DFG 
	//////////////////////  
	CreateDFG(scc,ofn);
    }
    
    //////////////////////////////////////////// 
    // ELIMINATE EMPTY DUMB STATES FROM SFG 
    ////////////////////////////////////////////
    CleanUpSFG(SFG);
    
    ////////////////////////////////////////////////////////////
    // ELIMINATE FALSE REGISTERS, CREATE DEF_USE CHAINS ... 
    ////////////////////////////////////////////////////////////
    AnalyzeRegisters();   
    
    ////////////////////////////  
    // EMIT FSM in KISS2 FORMAT 
    ////////////////////////////  
    
    if(SFG->number_of_nodes() >1            // if more than one state, or
       || ofn->getRegisters()->size()>0){   //presence of registers
      EmitFSM(ofn);     //kiss2 (.fsm)
      if( gEmitVCG ==1)
	PrintFSM(ofn);  //.vcg 
    }
  }
  
  return op;
}







