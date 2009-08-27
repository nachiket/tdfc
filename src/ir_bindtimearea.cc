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
// SCORE TDF compiler:  Area calc., DFG scheduling and Pipeline Extraction
// $Revision 1.122$
//
//////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
//#include <LEDA/graph/graph_alg.h>
#include "ir_graph.h"
#include "operator.h"
#include "stmt.h"
#include "ir_misc.h"
#include "bindvalues.h"
#include "annotes.h"

using leda::dic_item;
using std::ofstream;
using std::cerr;
using std::endl;

extern OFGraph gOFG;
void PrintOFG();

/*void MoveNodeintoPipe(node n,OFNode* pipe, DFGraph orig_DFG){
  DFGraph new_DFG=pipe->getDFG();
  //transfer the node from ofn to pipe
  node extracted_node = new_DFG->new_node(orig_DFG->inf(n)); 
  //attach pointer to new node to AST expression:
  orig_DFG->inf(n)->getExpr()->setAnnote(TO_NEWREP_IMAGE_NODE,(void*) extracted_node);
  //if necessary, fix boundary:
  edge e;
  forall_in_edges(e,n){
    DFEdge* orig_dfe=orig_DFG->inf(e);
    if (orig_dfe->isReadyIPExtr()){  //then it means it was a boundary: lift edge
      //cerr<<"getting annote from expression "<<orig_DFG->inf(source(e))->getExpr()->toString()<<endl;
      node extracted_source_node = (node) 
	orig_DFG->inf(source(e))->getExpr()->getAnnote(TO_NEWREP_IMAGE_NODE);
      new_DFG->new_edge(extracted_source_node,extracted_node,orig_dfe);
      //delete it from gOFG:
      gOFG->del_edge(orig_dfe->getImageEdge());
    }
  }   
  //  return new_DFG->inf(extracted_node);
  }*/

/*void MakeEdgeaBoundary(edge e,node pipe_node, node ofn_node, DFGraph orig_DFG,DFNode* dfn ){
  string  orig_edge_name = orig_DFG->inf(e)->getName();
  cerr<<"makeedge with edge " <<orig_edge_name<< endl;
  OFEdge* ofe = new OFEdge(orig_edge_name);
  edge new_ofg_edge = gOFG->new_edge(pipe_node,ofn_node,ofe);
  dfn->setOFEdge(new_ofg_edge);
  orig_DFG->inf(e)->setImageEdge(new_ofg_edge);
  }*/

void PrintSchedule(SCCell* scc,DFGraph DFG){
  Schedule the_schedule = scc->getSchedule();
  set<node> *step_set;
  int step=1;
  int extracted_IP_area = 0;
  int extracted_OP_area = 0;
  int unextracted_area = 0;
  //cerr << "SCHEDULE:" << endl;
  forall(step_set, * the_schedule ){
    //cerr << "STEP " << step++<<endl;
    node node_el;
    forall(node_el,*step_set){
      DFNode * dfn  =  DFG->inf(node_el);
      //cerr << *dfn->getExpr();
      //cerr << "(CAN";
      //if (! dfn->isIPExtracted())
      //cerr << "NOT";
      //cerr << " IP extract)  ";
      if (dfn->isIPExtracted())
	extracted_IP_area += dfn->getArea();
      //cerr << "(CAN";
      //if (! dfn->isOPExtracted())
      //cerr << "NOT";
      //cerr << " OP extract)\n";
      if (dfn->isOPExtracted())
	extracted_OP_area += dfn->getArea();
      if ( ! dfn->isOPExtracted() && ! (dfn->isIPExtracted())){
	unextracted_area += dfn->getArea();
	//cerr << "cannot extract at all" <<endl;

	/////////////////////////////////////////////////////////////////
	//
	//  HERE I CALCULATE THE STRINGS FROM EXTRACTED TO UNEXTRACTED 
	//  AND THE STREAMS WHICH WERE LEFT IN
	//
	/////////////////////////////////////////////////////////////
	edge in_edge;
	forall_in_edges(in_edge,node_el){
	  DFNode* dfnode= DFG->inf(DFG->source(in_edge));
	  string boundary_stream = dfnode->getName();
	  if (dfnode->isIPExtracted()){
	      scc->getIPInterm()->insert(boundary_stream);
	      //cerr << "IP INTERM " << boundary_stream<<endl;
	  }
	  if(dfnode->getDFNodeKind()== DFN_STR){
	    scc->getInStrLeftIn()->insert(dfnode->getName());
	    //cerr<<"LEFT IN " << boundary_stream<<endl;
	  }
	}
	edge out_edge;
	forall_out_edges(out_edge,node_el){
	  node out_node = DFG->target(out_edge);
	  if (DFG->inf(out_node)->isOPExtracted()){
	      string boundary_stream = DFG->inf(node_el)->getName();
	      scc->getOPInterm()->insert(boundary_stream);
	      //cerr << "OP INTERM " << boundary_stream<<endl;
	  }
	}
      }
    }
  }
  scc->setUnextractedArea(unextracted_area);
}
int EvaluateBW(Expr* ex){
  int bw =  ex->getType()->getWidth();
  if (bw  == -1){
    Expr* extype = ex->getType()->getWidthExpr();
    Expr* solved_expr = EvaluateExpr(extype);
    switch(solved_expr->getExprKind()){
    case EXPR_VALUE:
      bw = ((ExprValue*)solved_expr)->getIntVal();
      break;
    case EXPR_CAST:
      bw = ((ExprCast*)solved_expr)->getExpr()->getType()->getWidth();
      break;
    default:
      //can't resolve
      bw=3; //obviously this is a joke, need to get back to it.
      break;
    }
  }
  return bw;
}    

int lookup_area(Expr*exp, int abw){
  //cerr << "looking area for expression " << exp->toString() <<endl;
  bool is_const1=0;
  bool is_const2=0;
  int bw1;
  int bw2;
  if(exp->getExprKind() == EXPR_BOP){
    ExprBop* bexp = (ExprBop*)exp;
    bw1=EvaluateBW(bexp->getExpr1());
    bw2=EvaluateBW(bexp->getExpr2());
    if (bexp->getExpr1()->getExprKind()==EXPR_VALUE)
      is_const1=1;
    if (bexp->getExpr2()->getExprKind()==EXPR_VALUE)
      is_const2=1;
    int ar = ArchDepArea(bexp->getOp(),bw1,is_const1,bw2,is_const2,1,abw); 
    //cerr <<"bw1 "<<bw1<<"\tbw2 "<<bw2<<"\tASSto "<<abw<<endl;
    //cerr << "partial AREA " << ar<< endl<< endl; 
    return ar; 
  }
  else{ //UOP    
    ExprUop*uexp = (ExprUop*)exp;
    bw1=EvaluateBW(uexp->getExpr()); 
    if (uexp->getExpr()->getExprKind()==EXPR_VALUE) 
      return 0;
    int ar = ArchDepArea(uexp->getOp(),bw1,is_const1,0,0,0,abw); 
    return ar; 
  }
  return 0;
}

void CalculateArea (OFNode* ofn){
  SFGraph SFG;
  int sfg_area = 0;
  if((SFG=ofn->getSFG())){           //if there is an SFG
    node sn;
    forall_nodes(sn,*SFG){           //for every state
      SFNode* sfn =  SFG->inf(sn);
      int state_area = 0;
      SCCell* scc;
      forall(scc,*sfn->getSCCells()){
	DFGraph DFG=scc->getDFG();
	int dfg_area = 0;
	if(DFG){                 //if there is a dfg, bind area
	  node dn;
	  forall_nodes(dn,*DFG){           
	    int assignment_bw = 0;
	    DFNode* dfn = DFG->inf(dn); 
	    int dfn_area=0;
	    DFNodeKind kind = dfn->getDFNodeKind();
	    if(kind == DFN_OP){  //TODO take care of DFN_SELECTOR
	      Expr* ex=dfn->getExpr();
	      // see if assigns, if so see bw of assignment
	      edge feo; //first edge out. also 'ugly' in spanish
	      if ((feo =DFG->first_adj_edge(dn))){
		DFNode* dfn_next = DFG->inf(target(feo));
		if(dfn_next->getDFNodeKind() == DFN_ASSIGN)
		  assignment_bw= EvaluateBW(dfn_next->getAssigned());
	      }
	      dfn_area = lookup_area(ex, assignment_bw);
	      dfn->setArea(dfn_area);
	    }
	    dfg_area += dfn_area;
	  }
	}
	state_area+=dfg_area;
	scc->setArea(dfg_area);
      }
      sfg_area += state_area;
      sfn->setArea(state_area);
    }
  }
  ofn->setArea(sfg_area);
}
void CalculateMuxesArea(OFNode* ofn,int*mux_area_acc_reg, int*mux_area_acc_str ){
  dictionary <string,list<list<Tree*>*>*> * muxes = ofn->getExprsToMuxes();
  dictionary <string,Symbol*>* registers = ofn->getRegisters();
  dictionary <string,Symbol*>* streams = ofn->getStreamsOut();
  dictionary <string,Symbol*>* dictionary_to_look_in; 
    dic_item dic_it;
    forall_items(dic_it,*muxes){
      string reg_or_str = muxes->key(dic_it);
      //cerr<<"\n\n Next item in muxes "<< reg_or_str<<endl;
      int mux_length = muxes->inf(dic_it)->length();
      //cerr <<"MUX LENGTH "<<mux_length<<endl;
      //check the constant case:
      // .... todo for now only trivial case of const+contr<=4
      if(mux_length<=4){
	//list<list<Tree*>*>* exprs = muxes->inf(dic_it);
	list<Tree*>* fl_ex;
	forall(fl_ex,*muxes->inf(dic_it)){
	  if (fl_ex->length()==1){
	    Tree* t = fl_ex->front();
	    if(t->getKind()==TREE_EXPR &&  ((Expr*)t)->getExprKind()==EXPR_VALUE){
	      mux_length--;
	      //cerr<<"CONST, muxlength is now "<<mux_length<<endl;
	    }
	  }
	}
      }
      int number_of_luts = (mux_length<=2)?1:((mux_length<=4)?3:7);//THIS IS ARCH DEP....
      //calculate BW of register
      bool look_into_muxarea=1;
      //cerr <<reg_or_str<<endl;
      dic_item str_it = streams->lookup(reg_or_str);
      dic_item reg_it = registers->lookup(reg_or_str);
      if(str_it!= (dic_item)nil){
	dictionary_to_look_in = streams;
      }
      else if (reg_it!= (dic_item)nil)
	dictionary_to_look_in = registers;
      else
	  look_into_muxarea=0;
      if(look_into_muxarea!= 0){
	//cerr<<"\nDICT " << ((registers==dictionary_to_look_in)?"registers":"streams")<<endl;
	Symbol * reg_sym=dictionary_to_look_in->access(reg_or_str);
	int bw = reg_sym->getType()->getWidth();
	//cerr << "WIDTH = " <<bw<<endl;
	if(bw==-1){
	  Expr* extype = reg_sym->getType()->getWidthExpr();
	  bw=((ExprValue*)EvaluateExpr(extype))->getIntVal();
	    //cerr<<"BW NOW "<<bw<<endl;
	}
	if (bw>0){ 
	  int area_to_add = number_of_luts*bw;
	  if(dictionary_to_look_in==registers)
	    *mux_area_acc_reg+=area_to_add;
	  else
	    *mux_area_acc_str+=area_to_add;
	  //cerr<<"ACC REG= "<<mux_area_acc_reg <<"ACC STR= "<<mux_area_acc_str<<endl;
	}
      }
    }
}
void ScheduleandExtract(node on){
  set<string>*                 op_level_IPinputs = new set<string>;
  //set<string>*                 op_level_OPoutputs = new set<string>;
  set<string>*                 op_level_IPinterm = new set<string>;
  set<string>*                 op_level_OPinterm = new set<string>;
  set<string>*                 op_level_in_str_left_in = new set<string>;
  OFNode* ofn = gOFG->inf(on);
  Operator* op = ofn->getOperator();
  //string fname = op->getName()+".IO";
  //ofstream *fout = new ofstream(fname);
  node pipe_node=gOFG->new_node(new OFNode(op,OFN_PIPELINE));
  op->setAnnote(TO_IN_PIPE, (void*) pipe_node);
  OFNode* pipe_ofn = gOFG->inf(pipe_node);
  //pipe_node=gOFG->new_node(new OFNode(op,OFN_PIPELINE));
  //op->setAnnote(TO_OUT_PIPE, (void*) pipe_node);
  SFGraph SFG=ofn->getSFG();
  node sn;
  forall_nodes(sn,*SFG){
    SFNode* sfn =  SFG->inf(sn);
    //cerr<<"state name for schedextr "<<sfn->getState()->getName() << endl;
    int rank=0;   //this is used for PrintDFG, to give the rank of the statecase
    SCCell* scc;
    forall (scc,*sfn->getSCCells()){
      DFGraph DFG=scc->getDFG();
      if(DFG){   // if there is a dfg
	set<node> * step = new set<node>;
	
	//****************************************************/
	// INITIAL ROUND
	//****************************************************/
	
	//to label inputs/outputs as ready to sched, IPex, OPex
	node n;
	forall_nodes(n,*DFG){  
	  DFNodeKind kind = DFG->inf(n)->getDFNodeKind();
	  if ( kind==DFN_INVAL  //constant, ready to be scheduled + IPextr.
	       || kind==DFN_STR){ //stream, ready to be scheduled + IPextr.
	    //DFG->inf(n)->makeIPExtracted(); maybe not strictly necessary
	    //MoveNodeintoPipe(n,pipe_ofn,DFG);
	    edge e;
	    if((e = DFG->first_adj_edge(n))){  //if there is an outedge
	      DFG->inf(e)->makeReadySched();
	      DFG->inf(e)->makeReadyIPExtr();
	      //MakeEdgeaBoundary(e,pipe_node,on, DFG,DFG->inf(n));
	    }
	  }
	  if (kind == DFN_TOP_REG){
	    edge out_edge1;
	    forall_out_edges(out_edge1,n){
	      edge out_edge2;
	      if((out_edge2 = DFG->first_adj_edge(target(out_edge1))))
		DFG->inf(out_edge2)->makeReadySched();
	    }
	  }
	  else if (kind == DFN_ASSIGN &&
		   DFG->inf(n)->getDest()->getSymbol()->isStream()){
	    edge ed;
	    if ((ed= DFG->first_in_edge(n))){
	      DFEdge* dfe = DFG->inf(ed);
	      dfe->makeReadyOPExtr();
	      scc->getOPOutputs()->insert(DFG->inf(n)->getName());
	    }
	  }
	  
	} //end initial round
	
	//****************************************************/
	// SCHEDULE
	//****************************************************/
	
	int step_id = 0;
	while(1){  //it breaks when nothing new has been scheduled
	  step_id++;
	  forall_nodes(n,*DFG){ //later rounds, to schedule ops in "step"
	    DFNode* dfn = DFG->inf(n);
	    DFNodeKind kind = dfn->getDFNodeKind();
	    if (kind == DFN_OP||kind==DFN_ASSIGN||kind==DFN_SELECTOR)
	      if(!dfn->isScheduled()){
		edge e;
		bool ready_sched=1;
		forall_in_edges(e,n)  //are edges ready?
		  if(!DFG->inf(e)->isReadySched())
		    ready_sched=0;
		if(ready_sched){ 
		  step->insert(n);
		  //cerr <<"scheduling "<<*dfn->getExpr() <<"\tlevel "<<step_id<<endl;
		  dfn->makeScheduled(step_id);
		}
	      }
	  }    //schedule created
	  
	  //****************************************************/
	  // IP EXTRACTION
	  //****************************************************/
	  
	  //now see among all nodes in STEP, which can be IPextracted
	  forall(n,*step){
	    set<string>* instreams = new set<string>;
	    DFNode* dfn = DFG->inf(n);
	    // if assignment to register, we cannot extract
	    if(dfn->getDFNodeKind()==DFN_ASSIGN &&
	       dfn->getDest()->getSymbol()->isReg()){
	      ;  //need cleanup
	    }
	    else{ 
	      edge e;
	      bool ready_ex=1;
	      forall_in_edges(e,n){  //are edges ready?
		DFEdge *dfe=DFG->inf(e);
		//ExprLValue*lv=dfe->getLValue();
		DFNode*  source=DFG->inf(DFG->source(e));
		if(!dfe->isReadyIPExtr())
		  ready_ex=0;
		else // the node has been extracted, was it a stream?
		  if(source->getDFNodeKind()==DFN_STR){
		    instreams->insert(source->getName()); //input is stream
		  }
	      }
	      if(ready_ex){ 
		//cerr <<"IPExtracting "<<*dfn->getExpr()<<endl;
		string in_st;
		forall(in_st,*instreams)
		  scc->getIPInputs()->insert(in_st);//INPUT streams to pipe added
		dfn->makeIPExtracted();
		//MoveNodeintoPipe(n,pipe_ofn,DFG);
		scc->addIPArea(dfn->getArea());
		//make following edges ready to extract
		edge e;
		if ((e= DFG->first_adj_edge(n))){
		  DFG->inf(e)->makeReadyIPExtr();
		  //MakeEdgeaBoundary(e,pipe_node,on,DFG, DFG->inf(n));
		}
	      }
	    }
	  }
	  //****************************************************/
	  // MAKE NEXT EDGES READY FOR IP EXTR and SCHED
	  //****************************************************/
	  
	  forall(n,*step){//make following edges ready to sched
	    edge e;
	    forall_out_edges(e,n){
	      node nn=DFG->target(e);
	      DFNode* nextnode = DFG->inf(nn);
	      DFG->inf(e)->makeReadySched();
	      if(nextnode->getDFNodeKind() == DFN_INLVAL){
		edge e2;
		if((e2=DFG->first_adj_edge(nn))){
		  DFG->inf(e2)->makeReadySched();
		  if(DFG->inf(n)->isIPExtracted()){
		    //MoveNodeintoPipe(nn,pipe_ofn,DFG);
		    DFG->inf(e2)->makeReadyIPExtr();
		    //MakeEdgeaBoundary(e2,pipe_node,on,DFG, DFG->inf(nn));
		  }
		}
	      }
	    }
	  }
	  //****************************************************/
	  // GO TO NEXT SCHEDULING STEP
	  //****************************************************/
	  if(!step->empty()){
	    scc->getSchedule()->append(step);
	    step = new set<node>;
	  }
	  else break;
	}
	
	//****************************************************/
	//GRAPH SCHEDULED; NOW:
	//OP EXTRACTION
	//****************************************************/
	
	
	scc->getSchedule()->reverse();
	forall(step,*scc->getSchedule()){
	  forall(n,*step){
	    bool ready_ex=1;
	    edge out_ed;
	    forall_out_edges(out_ed,n)
	      if(!DFG->inf(out_ed)->isReadyOPExtr())
		ready_ex=0;
	    if(ready_ex){
	      DFNode* dfn = DFG->inf(n);
	      DFNodeKind kind = dfn->getDFNodeKind();
	      if(kind==DFN_OP ||
		 (kind==DFN_ASSIGN && 
		  !(dfn->getDest()->getSymbol()->isReg()))){
		//cerr <<"OPExtracting "<<*dfn->getExpr()<<endl;
		dfn->makeOPExtracted();
		//scc->addOPArea(dfn->getArea());
		
		/******************************************/
		// make previous edges ready to extract
		/******************************************/
		edge ine;
		forall_in_edges(ine,n){
		  DFNodeKind dk =DFG->inf(DFG->source(ine))->getDFNodeKind();
		  if(dk!=DFN_REG){
		    DFG->inf(ine)->makeReadyOPExtr();
		    if(dk==DFN_INLVAL){
		      edge ine2;
		      forall_in_edges(ine2,DFG->source(ine))
			DFG->inf(ine2)->makeReadyOPExtr();
		    }
		  }
		}
	      }
	    }
	  }
	}
	scc->getSchedule()->reverse();
	PrintSchedule(scc,DFG);
	
	
	//ofn->addIPArea(scc->getIPArea());
	//ofn->addOPArea(scc->getOPArea());
	ofn->addUnextractedArea(scc->getUnextractedArea());
	// here a check to see which streams have been extracted 
	// and which intermediates we have
	
	/*if(!scc->getIPInputs()->empty()|| !scc->getInStrLeftIn()->empty() ||
	   !scc->getIPInterm()->empty()|| !scc->getOPInterm()->empty()   || 
	   !scc->getOPOutputs()->empty()){
	  *fout << "\nSTRINGS EXTRACTED in state "<<sfn->getState()->getName()
		<<" (inputs: ";
	  string str;
	  forall(str,*sfn->getConsumedStreams())
	    *fout << str<<" ";
	  *fout<<"):  ";
	  forall(str,*scc->getIPInputs())
	    *fout<< str<<" ";
	  *fout <<endl;
	  *fout << "STRINGS LEFT IN : ";
	  forall(str,*scc->getInStrLeftIn())
	    *fout<< str<<" ";
	  
	  *fout<<"\nINTERM FROM IP TO SFM: ";
	  forall(str,*scc->getIPInterm())
	    *fout<< str<<" ";
	  *fout<<"\nINTERM FROM SFM TO OP: ";
	  forall(str,*scc->getOPInterm())
	    *fout<< str<<" ";
	  *fout<<"\nOP EXTRACTED: ";
	  forall(str,*scc->getOPOutputs())
	    *fout<< str<<" ";
	  *fout<<endl<<endl;
	  }*/
	// end check
	*op_level_IPinputs+=*scc->getIPInputs();
	*op_level_IPinterm+=*scc->getIPInterm();
	*op_level_OPinterm+=*scc->getOPInterm();
	*op_level_in_str_left_in+=*scc->getInStrLeftIn();
	
	//*////////////////////////////////////////////////////////*//
	//    EMIT DFG GRAPH, WITH EXTRACTION INFORMATION  
	//*///////////////////////////////////////////////////////*//
	
	if(gEmitVCG==1)  //emit the dataflowgraphs of every state
	  if (DFG->number_of_nodes()>0)
	    PrintDFG(scc, ofn, rank++,0);  //o means it's not a pipeline
	
      }
    }
  }
  //FINAL STATS
  string str;
  /**fout << "\n\nSTATS FOR OPERATOR " <<ofn->getOperator()->getName(); 
  *fout<<"\nOP LEVEL IPINP: ";
  forall(str,*op_level_IPinputs)
    *fout<<str<<"\t";
  *fout<<"\nOP LEVEL IPINTERM: ";
  forall(str,*op_level_IPinterm)
    *fout<<str<<"\t";
  *fout<<"\nOP LEVEL OPINTERM: ";
  forall(str,*op_level_OPinterm)
    *fout<<str<<"\t";
  *fout<<"\nOP LEVEL IN STR LEFT IN: ";
  forall(str,*op_level_in_str_left_in)
    *fout<<str<<"\t";
  *fout<<endl<<endl;
  */
  //**************************************************************//
  //  CHECK IF EXTRACTION HAS BEEN DONE WHICH extracts, ex, 
  //  (a,b) in one state, and (a,c) in another
  //**************************************************************//
  
  // STILL TO BE ADAPTED TO SCCELL ///////////////////////
  // IT ASSUMES FOR THE MOMENT that we extract only from
  // non eos cases
  
  set<SCCell*>*de_extracted_set= new set<SCCell*>;
  node sn1;
  forall_nodes (sn1,*SFG){
    SCCell* scc1 = SFG->inf(sn1)->getRegSCCell();
    set<string>* IPinputs1 = scc1->getIPInputs();
    int size1 = IPinputs1->size();
    if(size1>0){
      node sn2;
      forall_nodes (sn2,*SFG){
	SCCell* scc2 =SFG->inf(sn2)->getRegSCCell();
	set<string>* IPinputs2 =scc2->getIPInputs();
	int size2 = IPinputs2->size();
	if (size2>0){
	  set<string> intersect = *IPinputs1 & *IPinputs2;
	  if (!intersect.empty() && intersect.size() != size1){
	    //*fout<< "\nEXTRACTION PROBLEM, states: \n";
	    //*fout << SFG->inf(sn1)->getState()->getName() << " and "
	    //	  << SFG->inf(sn2)->getState()->getName();
	    //experimental
	    if(!de_extracted_set->member(scc1)){
	      de_extracted_set->insert(scc1);
	      int de_extracted = scc1->getIPArea();
	      scc1->addIPArea(-de_extracted);
	      ofn->addUnextractedArea(de_extracted);
	    }
	    if(!de_extracted_set->member(scc2)){
	      de_extracted_set->insert(scc2);
	      int de_extracted = scc2->getIPArea();
	      scc2->addIPArea(-de_extracted);
	      ofn->addUnextractedArea(de_extracted);
	    }
	  }
	}
      }
    }
  }
  
  ///////////////////////////////////////
  // NUMBERS FOR EYLON TOKEN DISTR/COLL
  ///////////////////////////////////////
  /*ofstream *foutt = new ofstream(ofn->getOperator()->getName()+".tok");
   *foutt<< "\n\n-------------\nOPERATOR " <<  ofn->getOperator()->getName()<<endl;
  
   //forall in streams
   dictionary <string,Symbol*>*  DicIn = ofn->getStreamsIn();
   dictionary <string,Symbol*>*  Dicout = ofn->getStreamsOut();
   dic_item di;
   forall_items (di,*DicIn){
   string str = DicIn->key(di);
   *foutt<< "\nString " <<  str;
   int bw = DicIn->inf(di)->getType()->getWidth();
   if (bw == -1) 
   bw= ((ExprValue*)DicIn->inf(di)->getType()->getWidthExpr())->getIntVal();
   *foutt << "\twidth " <<bw<<endl;
   int consumed_counter=0;
   int used_counter=0;
   node sn;
   forall_nodes(sn,*SFG){
   SFNode* sfn = SFG->inf(sn);
   if(sfn->getConsumedStreams()->member(str))
   consumed_counter++;
   SCCell* scc;
   forall(scc,*sfn->getSCCells())
   if(scc->getUsedStreams()->lookup(str))
   used_counter++;
   }
   *foutt<< "  consumed " <<consumed_counter<<"\tused "<<used_counter<<endl;
   }
   forall_items (di,*Dicout){
   string str = DicIn->key(di);
   *foutt<< "\nString " <<  str <<endl;//<<" width "<< Dic->inf(di)->getType()->getWidth()<<endl;
   int def_counter=0;
   node sn;
   forall_nodes(sn,*SFG){
   SCCell* scc;
   forall(scc,*SFG->inf(sn)->getSCCells())
   if(scc->getWrittenStreams()->lookup(str))
   def_counter++;
   }
   *foutt<<"\tdefined "<<def_counter<<endl;
   }
	     */
}

void CleanUpPipeline(DFGraph DFG){
  node n;
  forall_nodes(n,*DFG)
    if (DFG->degree(n)==0){  //unconnected node
      edge oe = DFG->inf(n)->getOFEdge();
      DFG->del_node(n);     // delete node from pipe_DFG, but do not delete
                            // DFNode, which is shared with the orig_DFN 
      gOFG->del_edge(oe);   // delete edge from gOFG, but not DFEdge (as above)
    }
  //HERE CREATE THE OFG LEVEL EDGES

  //else if(DFG->inf(n)->getDFNodeKind()==DFN_STR)
  //  gOFG->new
}

void BindTimeArea(){
  node on;
  forall_nodes(on,*gOFG){
    OFNode* ofn =  gOFG->inf(on);
    if(ofn->getKind()==OFN_BEHAVIORAL){
      /*************************************************************/
      // CALCULATE AREA for all DF operations (but muxes, see later.)
      /*************************************************************/
      CalculateArea(ofn);  
      
      /***************************************/
      //SCHEDULE, and do PIPELINE EXTRACTION
      /***************************************/
      ScheduleandExtract(on);
    }
  }
  forall_nodes(on,*gOFG){
    OFNode* ofn =  gOFG->inf(on);
    if(ofn->getKind()==OFN_PIPELINE){ 
      CleanUpPipeline(ofn->getDFG());
      if(ofn->getDFG()->number_of_nodes()==0){
	delete ofn;
	gOFG->del_node(on);
      }
      else
	if(gEmitVCG==1)               // emit the dataflowgraphs of pipelines
	  PrintDFG(nil, ofn, 0, 1);   // 0 means no eos, 1 means it's a pipeline
    }
  }
  if( gEmitVCG ==1)
    PrintOFG();
  //FINAL STATS
  int tot_area = 0;
  int tot_unex_area = 0;
  int tot_mux_str_area = 0;
  int tot_mux_reg_area = 0;
  string fname = gOFG->inf(gOFG->first_node())->getOperator()->getName()+".df";
  ofstream *fout = new ofstream(fname);
  forall_nodes(on,*gOFG){
    int mux_area_acc_reg=0;
    int mux_area_acc_str=0;  
    OFNode* ofn =  gOFG->inf(on);
    if(ofn->getKind()==OFN_BEHAVIORAL){
      CalculateMuxesArea(ofn, &mux_area_acc_reg, &mux_area_acc_str);
      *fout << "name "     << ofn->getOperator()->getName()
	//<< "\tDF (no muxes) area "  << ofn->getArea()
	//<< "\ttotmuxes_area "   << mux_area_acc_reg+mux_area_acc_str
	    << "\ttotDF+muxes_area "<< ofn->getArea()+mux_area_acc_reg+mux_area_acc_str
	    << "\ttot extr_area "   << ofn->getArea()-ofn->getUnextractedArea()
	                               +mux_area_acc_str 
	    << "\ttot_unextracted " << ofn->getUnextractedArea()+mux_area_acc_reg<<endl;
      tot_area += ofn->getArea();
      tot_mux_str_area +=mux_area_acc_str ;
      tot_mux_reg_area +=mux_area_acc_reg ;
      tot_unex_area += ofn->getUnextractedArea();
    }
  }
  *fout << "name total\t\t\ttot_area "<<tot_area+tot_mux_str_area+tot_mux_reg_area
	<< "\textr_area " <<tot_area-tot_unex_area+tot_mux_str_area
	<< "\ttotal unex "<< tot_unex_area+tot_mux_reg_area<<endl<<endl<<endl;
  fout->close();
  cerr << "Done.\n\nArea information to be found in *.df file.\n";
  cerr << "Kiss2 format of FSM to be found in *.fsm* files.\n";
  if(gEmitVCG==1) 
    cerr << "vcg format of graphs to be found in *.vcg files.\n";
}










