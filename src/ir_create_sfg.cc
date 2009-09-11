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
// SCORE TDF compiler:  Creates the State Flow Graph. Also, emits FSM.
// $Revision 1.126$
//
//////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include "operator.h"
#include "state.h"
#include "ir_graph.h"
#include "stmt.h"
#include "annotes.h"
#include <math.h>
#include "bindvalues.h"

using std::ofstream;
using std::endl;
using leda::list_item;
using leda::dic_item;

void CleanUpSFG(SFGraph SFG){

  node n;
  forall_nodes(n, *SFG){
    SFNode* sfn = SFG->inf(n);
    if (sfn->getSCCells()->length()==1){   // there are no eos cases
      SCCell* rcase=sfn->getRegSCCell();
      if (sfn->getConsumedStreams()->empty() && !rcase->getDone()){
	DFGraph DFG = rcase->getDFG();
	if (DFG->number_of_nodes() == 0){
	  //cerr<< "State "<<sfn->getState()->getName() << "THROW!\n";
	  if (SFG->out_edges(n).length() != 1)
	    fatal(-1,"why is this not a simple GOTO??");
	  node new_target = SFG->adj_nodes(n).front();
	  edge e;
	  forall_in_edges(e,n) { //new edge. The old SFedge is passed as an arg.
	    SFG->new_edge(e,new_target,SFG->inf(e),1); 
	  }
	  SFG->del_node(n);
	}
      }
    }
  }
}


bool pre_cond(Tree** h, void* vav){
  Tree* t = *h;
  list<bool> * av = (list<bool> *) vav;
  if (t->getKind() == TREE_EXPR &&  ((Expr*)t)->getExprKind() == EXPR_LVALUE){
    //cerr << "EXPR_LVALUE " << *t<< endl;
    *h=(Tree*) new ExprValue(NULL,t->getType(),(int)av->pop(),0);
    return 0;
  }
  return 1;
}
bool FlattenTree(Tree* t, void* vnew_cond){
  //cerr << "IN FLATTEN with tree: "<< *t<<endl;
  TreeKind kind = t->getKind();
  if  (kind==TREE_EXPR){
    //cerr << "IN FLATTEN appending tree "<< *t<<endl;
    ((list<Tree*>*) vnew_cond)->append(t);
    return 1;
  }
  else
    return 0;              
}
bool Equal(Tree* t1, Tree* t2){
  //cerr << "IN Equal with t1 "<<*t1<<" t2 "<<*t2<<"\n";
  TreeKind kind1 = t1->getKind();
  TreeKind kind2 = t2->getKind();
  if (kind1!=kind2)
    return 0;
  switch(kind1)
    {
    case TREE_EXPR:{        // to be modified a bit: it can only be a tree expr as I built it now!
      //cerr << "equal treeexpr\n";
      ExprKind ekind1=((Expr*)t1)->getExprKind();
      ExprKind ekind2=((Expr*)t2)->getExprKind();
      if(ekind1!=ekind2)
	return 0;
      switch(ekind1)
	{
	case EXPR_VALUE: {
	  //cerr << "exprVAL\n";
	  return 
	    (((ExprValue*)t1)->getIntVal()==((ExprValue*)t2)->getIntVal()
	     &&((ExprValue*)t1)->getIntVal() == ((ExprValue*)t2)->getIntVal());}
	case EXPR_LVALUE: {
	  //cerr << "exprLVAL\n";
	  return
	    (((ExprLValue*)t1)->getSymbol() ==
	     ((ExprLValue*)t2)->getSymbol());}
	//case EXPR_CALL:    //I don't think this can be in a cond
	//case EXPR_BUILTIN: //this neither?
	case EXPR_COND: {
	  //cerr << "exprCOND\n";
	  return 1;}
	case EXPR_BOP:{
	  //cerr << "exprBop\n";
	  return (((ExprBop*)t1)->getOp() == ((ExprBop*)t2)->getOp()); }
	case EXPR_UOP: {
	  //cerr << "exprUop\n";
	  return (((ExprUop*)t1)->getOp() == ((ExprUop*)t2)->getOp()); }
	case EXPR_CAST: {//cerr << "exprCast\n";
	  return 1; }
	case EXPR_ARRAY: {//cerr << "exprArray\n";
	  return 1; }
	default: fatal(-1,"Error on ExprKind");
	}
    }
    case TREE_STMT:  
      fatal(-1,"I thought there couldn't be a statement inside a condition (obviously wrong!");
    case TREE_SYMBOL:{
      //cerr<<"treesymbol\n";
      return (((Symbol*)t1)->getName() ==((Symbol*)t2)->getName());}
    case TREE_TYPE: return 1; //if we got here,  types should be equal now.

    default:fatal(-1,"Error on TreeKind");
    }
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// CheckUnicity checks unicity of an expression by flattening a tree to a list of tree nodes.
// and then comparing each expression, by comparing each tree node.
// if CheckUnicity finds "ex" to be unnique, it inserts it in the list, and returns its rank; 
// if it was not unique, it returns the rank 
///////////////////////////////////////////////////////////////////////////////////

bool CheckUnicity(Expr* ex, list <list<Tree * > *> * flattened_exs, int* rank){
  
  /////////////////////////////////////
  //first flatten expression ex.
  /////////////////////////////////////
  list<Tree*> * new_ex = new list<Tree*>;
  ex->map(FlattenTree,nil, (void*) new_ex);
  
  //////////////////////////////////////////
  // then compare the flattened expressions
  //////////////////////////////////////////
  int length_of_new=new_ex->length();
  int count=0;
  list<Tree * > * old_ex;
  forall(old_ex,*flattened_exs){
    int length_of_old=old_ex->length();
    if (length_of_old == length_of_new){
      bool equal=1;
      list_item new_ex_item = new_ex->first();
      Tree* node_old_ex;
      forall(node_old_ex,*old_ex){
	Tree* node_new_ex= new_ex->inf(new_ex_item);
	if (!Equal(node_new_ex,node_old_ex)){
	  equal=0;
	  break;
	}
	else
	  new_ex_item=new_ex->succ(new_ex_item);
      }
      if (equal == 1){             //////EXPRESSION WAS NOT UNIQUE
	//cerr<<"NOT UNIQUE " <<*ex<<endl;
	*rank = count;
	return 0;
      }
    }
    count++;
  }
  flattened_exs->append(new_ex);  ///////EXPRESSION WAS UNIQUE, append it.
  //cerr<<"UNIQUE " <<*ex<<endl;
  *rank = count;
  return 1;
}

void SetConditionInputs(list<string>* cond_inputs,OFNode* ofn,ofstream *fout,ofstream *foutfire){
  SFGraph SFG = ofn->getSFG();
  *fout << "## INPUTS\n## ";
  *foutfire << "## INPUTS\n## ";
  int fsm_inp_index=0;
  node n;
  forall_nodes(n,*SFG){
    edge ed;
    forall_out_edges(ed,n){     //CONDITION INPUTS (bool or not)
      SFEdge * sfe = SFG->inf(ed);
      Expr* cond= sfe->getCond();
      if (cond!=(Expr*)nil && sfe->getOutcome()){
	node dfnode = (node) cond->getAnnote(TO_NEWREP_NODE);
	DFNode* dfn = sfe->getSCCell()->getDFG()->inf(dfnode);
	ExprLValue* ev;
	bool allbool=true;
	forall(ev,*dfn->getInputs())
	  if (ev->getType()->getWidth()!=1){
	    allbool=false;
	    sfe->setAllBool(0);
	    edge succ;
	    if ((succ =  SFG->adj_succ(ed)) &&(SFG->inf(succ)->getOutcome() == 0));
	      SFG->inf(succ)->setAllBool(0); // sets the else edge 
	    break;
	  }
	if (allbool){
	  //insert all boolean inputs in input_list (if they are not there)
	  //*fout <<  "state " << sfn->getState()->getName() << " all bool!\n";
	  forall(ev,*dfn->getInputs()){
	    if (!cond_inputs->rank(ev->toString())){
	      cond_inputs->append(ev->toString());
	      *fout<< ev->toString() << "  ";
	      *foutfire<< ev->toString() << "  ";
	    }
	  }
	}
	else{
	  //else the input will be a condition bit, asserted by the condition
	  // itself outside the fsm
	  int rank;
	  string cond_string;
	  if(CheckUnicity(cond,ofn->getConditions(), &rank)){
	    cond_string = string("FSMcond%d",fsm_inp_index);
	    cond_inputs->append(cond_string); 
	    *fout <<cond_string<<" = "<<cond->toString()<<" ";
	    *foutfire <<cond_string<<" = "<<cond->toString()<<" ";
	    fsm_inp_index++;
	  }
	  else
	    cond_string = string("FSMcond%d",rank);
	  sfe->setConditionString(cond_string);
	  edge succ;   // if there is an else, set the condition there as well
	  if ((succ = SFG->adj_succ(ed)) && (SFG->inf(succ)->getOutcome() == 0 ))
	    SFG->inf(succ)->setConditionString(cond_string);
	}
      }
    }
  }
}

void SetOperatorInputs(list<string>* op_inputs,list<string>* op_outputs,Operator* op, ofstream *fout, ofstream *foutfire){
  list<Symbol*>* op_args = op->getArgs();
  Symbol* sym;
  forall(sym, *op_args)    // append to input list the inputs streams for EOS
    if (sym->isStream())
      if (((SymbolStream*)sym)->getDir() == STREAM_IN){
	op_inputs->append(sym->toString());
	*fout <<sym->toString() + "_EOS"<<" ";
	*foutfire <<sym->toString() + "_EOS"<<" ";
      }
      else if (((SymbolStream*)sym)->getDir() == STREAM_OUT)
	op_outputs->append(sym->toString());
  string inp_string;
  forall(inp_string,*op_inputs)
    *foutfire <<inp_string + "_PRES"<<" ";  // presence inputs for INside firing logic 
  *foutfire << "backpr ";
  *fout << "FIRE ";  //fire input for OUTSIDE firing logic (not presence anymore)
}

void EmitConsumeorWantOutputs(list<string>* op_inputs, SFGraph SFG, edge ed,ofstream *fout, ofstream *foutfire){
  SFNode* sfn = SFG->inf(SFG->source(ed));
  SFNode* sfn_next = SFG->inf(SFG->target(ed));

  list<string>* this_state_inputs= new list<string>;
  string str;
  
  ////// NOTE, for now, ConsumedStreams are the same for every state case
  ////// So they belong to the SFNode rather than the SCCell
  ////// But after SFM partitionig, we could have arbitrary signatures,
  ////// So ConsumedStreams might have to move into SCCells....
  ////// laurap 1/31/01

  forall(str,* sfn->getConsumedStreams())
    this_state_inputs->append(str);
  
  list<string>* next_state_inputs= new list<string>;
  forall(str,* sfn_next->getConsumedStreams()){
    next_state_inputs->append(str);
  }
  string op_input_string;
  forall(op_input_string,*op_inputs){          //EMIT consume outputs
    int rank_this_state = this_state_inputs->rank(op_input_string);
    int rank_next_state = next_state_inputs->rank(op_input_string);
    if (rank_this_state!= 0)  //(stream does trigger this state)  CONSUME CASE
      *foutfire<<"1";
    else  //does not trigger
      *foutfire<<"0";
    if (rank_next_state!= 0)  //WANT CASE
      *fout<<"1";
    else  //does not trigger
      *fout<<"0";
  }
}

void EmitMuxOutputs(OFNode* ofn , edge ed,ofstream* fout, ofstream *foutfire){
  SFEdge* sfe = ofn->getSFG()->inf(ed);
  dictionary<string,int>* rm= sfe->getRankMux();
  dictionary <string,list<list<Tree*>*>*> * op_level=ofn->getExprsToMuxes();
  
  string str;
  dic_item it_op;
  forall_items(it_op,*op_level){
    str = op_level->key(it_op);
    dic_item it =rm->lookup(str);  //is it written here?
    dic_item it_bw =ofn->getMuxBw()->lookup(str); //bw of that reg
    int bw =ofn->getMuxBw()->inf(it_bw);
    if(it!=nil){  //it is
      int dec = rm->inf(it);
      int iter = (int) pow(2,bw);
      //cerr << "BW " << bw<<" dec " << dec << "BIN " << endl;
	for(int bp=0;bp<bw;bp++){ //bit position
	  bool bin;
	  bin = (dec >= (iter=iter/2));
	  //cerr << "new dec " << new_dec<< endl;
	  *fout<<bin;
	  *foutfire<<bin;
	  dec=dec-(bin*iter);
	  //cerr << "NEW DEC " << dec << endl; 
	}
	//cerr<<endl;
    }
    else
      for(int i=0;i<bw;i++){
	*fout<<"1";
 	*foutfire<<"1";
      }
 }
  //cerr<<endl;
}
void EmitEosandFiringInputs(list<string>* op_inputs,list<string>* op_outputs, 
			    SFGraph SFG, edge ed, 
			    ofstream *fout, ofstream *foutfire){
  SFEdge* sfe = SFG->inf(ed);
  list<InputSpec*>* input_spec = sfe->getSCCell()->getStateCase()->getInputs();
  list<string>* state_inputs= new list<string>;
  InputSpec* is;
  forall(is,* input_spec){
    state_inputs->append(is->getStream()->toString());
  }
  //*fout<<endl;
  if (state_inputs->length() != input_spec->length()) 
    fatal(-1,"Error here");
  string op_input_string;
  forall(op_input_string,*op_inputs){          //EMIT EOS
    int rank = state_inputs->rank(op_input_string);
    if (rank!= 0){   //(stream does trigger state)
      if (input_spec->inf(input_spec->get_item(rank-1))->isEosCase()){
	*fout << "0";    // isEos corresponds to bit ZERO 
	*foutfire << "0";  } 
      else {
	*fout << "1";    // not an Eos case
	*foutfire << "1";  }
    }
    else{                // (stream does not trigger state)
      *fout << "-";
      *foutfire << "-";}
  } 
  // ***  EMIT FIRING, for firing logic INSIDE the FSM
  forall(op_input_string,*op_inputs){       //EMIT FIRING
    int rank = state_inputs->rank(op_input_string);
    if (rank!= 0)  //(stream does trigger state)
      *foutfire << "1";
    else           // (stream does not trigger state)
      *foutfire << "-";
  }
  if (op_outputs->length()!=0)
    *foutfire << "1"; //this is for the backpressure bit
  else
    *foutfire << "-"; //this is for the backpressure bit: do not bother looking 
                      //at it if the state does not generate anything.
  
  // ***  EMIT FIRING, for firing logic OUTSIDE the FSM
  *fout << "1";  //firing for the firing logic OUTSIDE the FSM
}

void SetMuxesStuff(OFNode* ofn,list<string>*mux_outputs,  ofstream *fout,ofstream *foutfire ){
  SFGraph SFG = ofn->getSFG();
  dictionary <string,list<list<Tree*>*>*>* dic_muxes_op_level =
    ofn->getExprsToMuxes();
  //cerr<<"MUXES LENGTH: "<<dic_muxes_op_level->size()<<endl;
  node n;
  forall_nodes(n,*SFG){
    //SFNode * sfn = SFG->inf(n);
    edge ed;
    forall_out_edges(ed,n){            
      SFEdge * sfe = SFG->inf(ed);
      if(sfe->getOutcome()){
	SCCell* sc = sfe->getSCCell();
	//dictionary <string,DFNode*>* dic_str = sc->getWrittenStreams();
	dic_item it;
 	dictionary <string,DFNode*>* dic = sc->getDefReg();  //first case is reg
	for(int count=0;count<2;count++){ //do twice, for regs and for streams
	  forall_items(it,*dic){
	    string str = dic->key(it);
	    DFNode* dfn = dic->inf(it); 
	    Expr* ex = dfn->getAssigned();      //laurap !!!!!! not getExpr!!!!!!
	    dic_item it_op =dic_muxes_op_level->lookup(str);
	    list<list<Tree*>*> * flattened_list;
	    if (it_op==nil){ // reg/str is not yet inserted
	      flattened_list = new list<list<Tree*>*>;
	      dic_muxes_op_level->insert(str,flattened_list);
	    }
	    else {
	      flattened_list =dic_muxes_op_level->inf(it_op);
	    }
	    int rank; 
	    CheckUnicity(ex,flattened_list,&rank); // sets the rank of the expression in 'rank'
	    sfe->getRankMux()->insert(str,rank);  
	  }
	  dic=sc->getWrittenStreams(); //now do it for streams
	}
      }
    }  
    
  } //all nodes
  
  int output_size=1;
  dic_item it;
  forall_items(it,*dic_muxes_op_level){
    string str = dic_muxes_op_level->key(it);
    int length =dic_muxes_op_level->inf(it)->length();
    int cardinality  = (int) ceil(log10(length+1)/log10(2)); 
    ofn->getMuxBw()->insert(str,cardinality);
    //+1 to add the option, to the mux, of unmodified reg (or string)
    for (int iter = 0;iter < cardinality ; iter++ ){
      *fout<<str+"_MUX"+string("_%d",iter)<< " ";//"FSMcond%d",fsm_inp_index)
      *foutfire<<str+"_MUX"+string("_%d",iter)<< " ";//"FSMcond%d",fsm_inp_index)
      mux_outputs->append(str+"_MUX"+string("_%d",iter));
      output_size++;
    }
  } 
  *fout<<endl;
  *foutfire<<endl;
  
}
void PrintFSM(OFNode* ofn){
  string opname= ofn->getOperator()->getName();
  string fname = opname+".vcg";
  ofstream *fout = new ofstream(fname);
  *fout<<"graph: { title: \""<<opname<<"\"\n";
  *fout<<"color: lightgray\n";
  SFGraph SFG = ofn->getSFG();
  node n;
  forall_nodes(n,*SFG){
    string source_name=SFG->inf(n)->getState()->getName();
    *fout<<"node: { title: \""<<source_name<<"\" ";
    /*switch (SFG->inf(n)->getHidden()){
      case 0:{*fout<<"color: lightyellow ";break;}
      case 1:{*fout<<"color: red ";break;}
      case 2:{*fout<<"color: lightgrey ";break;}
      case 3:{*fout<<"color: white ";break;}
      case 4:{*fout<<"color: blue ";break;}
      case 5:{*fout<<"color: magenta ";break;}
      case 6:{*fout<<"color: aquamarine ";break;}
      case 7:{*fout<<"color: yellow ";break;}
      case 8:{*fout<<"color: darkmagenta ";break;}
      case 9:{*fout<<"color: orange ";break;}
      default: *fout<<"color: pink ";
      }*/
    *fout<<"color: lightyellow ";
    *fout<<"textcolor: darkgreen }\n";
    edge ed;
    forall_out_edges(ed,n){
      string target_name = SFG->inf(SFG->target(ed))->getState()->getName();
      *fout<<"edge: { sourcename: \""<<source_name<<
	"\" targetname: \""<<target_name<<"\" }\n";
    }
  }
  *fout<<"\n}\n";
  fout->close();

}

// The following function emits an STG description of the operator FSM
// in KISS2 format (to be fed to SIS tools for synthesis)
// First it figures out which input fields the STG will need, and then which
// outputs fields. 

void EmitFSM(OFNode* ofn){
  Operator* op = ofn->getOperator();
  //cerr<< "FSM for operator \""<< op->getName() << "\"\n";
  SFGraph SFG = ofn->getSFG();
  //list<string>* outputs = new list<string>;
  list<string>* cond_inputs = new list<string>;
  list<string>* op_inputs = new list<string>;  // for eos inputs *and* firing inputs
  list<string>* op_outputs = new list<string>;  // for backpressure inputs
  list<string>* mux_outputs = new list<string>;
  
  string fname = op->getName()+".fsm";
  ofstream *fout = new ofstream(fname);
  fname = op->getName()+".fsmfire.fsm";
  ofstream *foutfire = new ofstream(fname);
  SetConditionInputs(cond_inputs,ofn, fout, foutfire);  // fills list cond_inputs
  SetOperatorInputs(op_inputs,op_outputs,op,fout, foutfire);        // fills list op_inputs
  *fout <<endl;                                // end of list of inputs
  *foutfire <<endl;                                // end of list of inputs
  string str;
  *fout<<"## OUTPUTS\n## ";
  *foutfire<<"## OUTPUTS\n## ";
  forall(str,*op_inputs){
    *fout<<string(str+"WNT")<< " ";      //wants
    *foutfire<<string(str+"CNS")<< " ";  //consumes
  }
  SetMuxesStuff(ofn,mux_outputs, fout,foutfire);
  int cond_input_size = cond_inputs->length();
  int op_input_size = op_inputs->length();
  *fout<< ".i " << cond_input_size+op_input_size+1 
       << "\n.o "<<op_input_size+mux_outputs->length()<<endl;
  *foutfire<< ".i " << cond_input_size+2*op_input_size +1
	   << "\n.o "<<op_input_size+mux_outputs->length()<<endl;
  node n;
  forall_nodes(n,*SFG){
    edge ed;
    forall_out_edges(ed,n){              // EMIT ALL CONDITION INPUTS
      SFNode * sfn = SFG->inf(n);
      SFEdge * sfe = SFG->inf(ed);
      Expr* cond = sfe->getCond();
      if (cond == (Expr*) nil){         // there is NO Condition
	
	for(int i = 0; i<cond_input_size ;i++){ // EMIT CONDITION INPUTS for no cond
	  *fout<< "-";
	  *foutfire<< "-";
	}
	
	EmitEosandFiringInputs(op_inputs,op_outputs, SFG, ed, fout,foutfire);      // EMIT EOS INPUTS
	
	*fout<<" "<<sfn->getState()->getName();
	*fout<<" " <<  SFG->inf(target(ed))->getState()->getName()<<" ";
	*foutfire<<" "<<sfn->getState()->getName();
	*foutfire<<" " <<  SFG->inf(target(ed))->getState()->getName()<<" ";
	
	EmitConsumeorWantOutputs(op_inputs, SFG, ed, fout, foutfire);
	EmitMuxOutputs(ofn, ed, fout,foutfire);
	*fout<<endl;
	*foutfire<<endl;
      }
      else if (! sfe->getAllBool()){  // CONDITION INPUTS FOR non bool CONDITION
	string current_cond=sfe->getConditionString();
	//cerr << "state "<<sfn->getState()->getName()<<" to state "<<SFG->inf(SFG->target(ed))->getState()->getName()<<" CONDITION STRING "<<current_cond<<endl;
	string s;
	forall(s,*cond_inputs){
	  if(s!= current_cond){
	    *fout << "-"; 
	    *foutfire << "-";}
	  else { // the condition field
	    *fout << sfe->getOutcome();  
	    *foutfire << sfe->getOutcome(); }
	}      
	EmitEosandFiringInputs(op_inputs,op_outputs, SFG, ed, fout,foutfire);      // EMIT EOS INPUTS
	
	*fout<<" "<<sfn->getState()->getName();   //Current State
	*fout<<" "<< SFG->inf(target(ed))->getState()->getName()<<" ";  //Next State
	*foutfire<<" "<<sfn->getState()->getName();   //Current State
	*foutfire<<" "<< SFG->inf(target(ed))->getState()->getName()<<" ";  //Next State
	
	EmitConsumeorWantOutputs(op_inputs, SFG, ed, fout,foutfire);
	EmitMuxOutputs(ofn, ed, fout,foutfire);
	*fout << endl;
	*foutfire << endl;
      }
      else{   // CONDITION INPUTS FOR BOOLEAN CONDITION
	if (sfe->getOutcome()){  //only process "then" edges
	  Tree* cond_copy;
	  list <bool> * assigned_values = new (list <bool>);
	  node dfnode = (node) cond->getAnnote(TO_NEWREP_NODE);
	  DFNode* dfn = sfe->getSCCell()->getDFG()->inf(dfnode);
	  int length = dfn->getInputs()->length();
	  int iter = (int) pow(2,length);
	  int new_iter = iter;
	  //cerr << "ITER " << iter << endl;
	  for (int i_dec=0;i_dec<iter;i_dec++){    // DEC to BIN translation
	    int dec = i_dec;
	    for(int bp=0;bp<length;bp++){ //bit position
	      bool bin;
	      bin = (dec >= (new_iter=new_iter/2));
	      //cerr << "new ITER " << new_iter<< endl;
	      assigned_values->append(bin);
	      //cerr << "BIN " << bin << endl;
	      dec=dec-(bin*new_iter);
	      //cerr << "NEW DEC " << dec << endl; 
	    }
	    cond_copy =  cond->duplicate();
	    list <bool> assigned_val_copy = *assigned_values;
	    cond_copy->map2(&cond_copy,pre_cond,nil,(void*) &assigned_val_copy);
	    ExprValue* out_come_ex= (ExprValue*) EvaluateExpr((Expr*)cond_copy);
	    //cerr << " assigned to " << *out_come_ex << endl;
	    string s;
	    forall(s,*cond_inputs){
	      bool was_in_cond=0;
	      ExprLValue *ev;
	      list_item it=assigned_values->first();
	      forall(ev,*dfn->getInputs()){
		if (ev->toString()==s){
		  *fout<< assigned_values->inf(it);
		  *foutfire<< assigned_values->inf(it);
		  was_in_cond=true;
		  break;
		}
		it=assigned_values->succ(it);
	      }
	      if (!was_in_cond){
		*fout<<"-";
		*foutfire<<"-";}
	    }
	    
	    EmitEosandFiringInputs(op_inputs,op_outputs, SFG, ed, fout,foutfire); 
	    
	    *fout << " "<<sfn->getState()->getName();
	    *foutfire << " "<<sfn->getState()->getName();
	    if (out_come_ex->getIntVal() == 1){
	      *fout<<" " << SFG->inf(SFG->target(ed))->getState()->getName()<<" ";
	      *foutfire<<" " << SFG->inf(SFG->target(ed))->getState()->getName()<<" ";
	    }
	    else{
	      *fout<<" " << SFG->inf(SFG->target(SFG->adj_succ(ed)))->getState()->getName()<<" ";
	      *foutfire<<" " << SFG->inf(SFG->target(SFG->adj_succ(ed)))->getState()->getName()<<" ";
	    }	    
	    EmitConsumeorWantOutputs(op_inputs, SFG, ed, fout,foutfire); 
	    EmitMuxOutputs(ofn, ed, fout,foutfire);
	    *fout << endl;
	    *foutfire << endl;
	    assigned_values->clear();
	  }
	}
      }
    }
  }
  *fout << "## NO-FIRE STG LINES\n";
  *foutfire << "## NO-FIRE STG LINES\n";

  forall_nodes(n,*SFG){   //NEW Emit NO-FIRE STG LINES
    SFNode * sfn = SFG->inf(n);
    list<InputSpec*>* input_spec= sfn->getState()->getCases()->choose()->getInputs();
    //list<string>* state_inputs= new list<string>;
    InputSpec* is;
    forall(is,* input_spec){   //for all input strings of that state
      for(int i=0;i<cond_input_size+op_input_size;i++)//COND + EOS INPUTS
	*foutfire <<"-";
      string state_input_string=is->getStream()->toString();
      string op_input_string;
      forall(op_input_string,*op_inputs){          // PRES INPUTS
	if (state_input_string==op_input_string)   //(stream triggers state)
	  *foutfire<<"0";      //but it is not present.
	else               //stream does not trigger state
	  *foutfire<<"-";      //and so we don't care if it's not present.
      }
      *foutfire << "-";  //this is for backpressure bit
      *foutfire << " "<<sfn->getState()->getName()<< " "<<sfn->getState()->getName()<<" ";
      for (int i=0;i<op_input_size;i++){ //CONSUME OUTPUTS
	*foutfire <<"0";  
      }         
      for (int i=0;i<mux_outputs->length();i++){ //MUXES OUTPUTS
	*foutfire <<"1"; 
      }
      *foutfire<<endl;
    }
      
    // now for fout and the backpressure line for *foutfire, in parallel
    for (int i=0;i<cond_input_size+op_input_size;i++){//COND INPUTS + EOS INPUTS
      *fout <<"-";
      *foutfire<<"-";
    }      
    string op_input_string;
    forall(op_input_string,*op_inputs)          // PRES INPUTS
      *foutfire<<"-";
    //the backpressure bit in *foutfire AND the FIRE bit in *fout are zero
    *fout << "0 "<<sfn->getState()->getName()<< " "<<sfn->getState()->getName()<<" ";    
    *foutfire << "0 "<<sfn->getState()->getName()<< " "<<sfn->getState()->getName()<<" ";     
    for (int i=0;i<op_input_size;i++){ //CONSUME OUTPUTS
      *fout <<"0";                    // do not consume
      *foutfire<<"0";
    }
    for (int i=0;i<mux_outputs->length();i++){ //MUXES OUTPUTS
      *fout <<"1";    //all 1s in muxes (means: stay as you are)
      *foutfire<<"1";
    }
    *fout<<endl; 
    *foutfire<<endl;
  }
  fout->close();
  foutfire->close();
}      
bool descend_operator(Tree *t,void *vstates){
  list <State*> * states = (list <State*> *) vstates; //cast only
  if(t->getKind()==TREE_STATE){
    states->append((State*)t);
    //cerr << "STATE " << ((State*)t)->getName()<<endl;
    return false;
  }
  else return true;
}
 
struct pass_to_map{
  dictionary <string,State*> * dict;
  SFGraph SFG;
  node curr_node;
  SCCell* sccell;
  State* state;
  Expr* last_condition;
  bool is_there_else;
  bool then;
};

// note: in the following I am using a dictionary because 
// when I am in search for nexstates ( in descend_statecase ) 
// the expression ((StmtGoto*)t)->getState() gives me a pointer
// NOT TO the new state of the instance
// BUT TO the original TDF (unique) state
// I confess I do not understand why (I am descending from the NEW 
// (duplicated) state, not the old one...)

bool descend_statecase(Tree *t,void * vpass){
  TreeKind kind = t->getKind();
  pass_to_map * pass = (pass_to_map * ) vpass;
  SFGraph SFG = pass->SFG;
  dictionary <string,State*> * state_dict = pass->dict;
  node  currSFnode = pass->curr_node; 
  if (kind == TREE_STMT){
    bool outcome;
    StmtKind stkind = ((Stmt*)t)->getStmtKind();
    if (stkind==STMT_IF){
      pass->last_condition = ((StmtIf*)t)->getCond();  // will be stored in SFEdge
      pass->then = 1;//next thing you meet is surely the then branch
      pass->is_there_else = (((StmtIf*)t)->getElsePart()!=(Stmt*)nil);
     }
    else if (stkind==STMT_GOTO){
      State* nextstate = state_dict->access(((StmtGoto*)t)->getState()->getName());
      node  nextSFnode = (node) nextstate->getAnnote(TO_NEWREP_NODE);
      //cerr<<"from state "<<SFG->inf(currSFnode)->getState()->getName()<<" to state "<<nextstate->getName()<<endl;
      Expr* cond=pass->last_condition;
      if (cond==(Expr*)nil)// not in if-then-else
	outcome=1;
      else{ //in if-then-else
	if(pass->then){  //in then
	  outcome=1;
	  pass->then=0;  //next is else, (if there is)
	  if(!pass->is_there_else) // no else
	    pass->last_condition=(Expr*)nil;  // null condition for next round
	}
	else{ //we are in else
	  outcome=0;
	  pass->last_condition=(Expr*)nil;
	}
      }
      //cerr<<" outc "<<outcome<<endl;
      SFG->new_edge(currSFnode,nextSFnode, new SFEdge(pass->sccell,cond, outcome)); //create edge
    }
    else if (stkind == STMT_BUILTIN){
      Operator* op = ((StmtBuiltin*)t)->getBuiltin()->getOp();
      if (op->getOpKind()==OP_BUILTIN &&
	  ((OperatorBuiltin*)op)->getBuiltinKind()==BUILTIN_DONE)
	pass->sccell->setDone();
    }
  }
  return ( kind==TREE_STATECASE
	   ||  kind==TREE_STMT);
}
// CreateSFG creates the State transition graph. It first creates a node  
// for each state, then it descends every state (descend_statecase 
// function) in search  for nextstates, then it creates the SFedges 
// (i.e. the gotos).

void CreateSFG(OFNode * ofn){
  OperatorBehavioral* opb = (OperatorBehavioral*)  ofn->getOperator();
  SFGraph SFG = ofn->getSFG();
  
  //////////////////////////////////////////////////
  //// CREATE STATE DICTIONARY
  /////////////////////////////////////////////////
  list<State*> * states= new list<State*>;
  opb->map(descend_operator, nil, (void*) states);
  dictionary <string,State*> * state_dict = new dictionary <string,State*>;
  State* st;
  forall(st, *states)
    state_dict->insert(st->getName(),st);

  //////////////////////////////////////////////////
  ////// CREATE all SFNodes
  //////////////////////////////////////////////////
  forall(st, *states){ 
    node newnode = SFG->new_node(new SFNode(st));
    st->setAnnote(TO_NEWREP_NODE, (void*) newnode);
    SFNode* sfn = SFG->inf(newnode);
    
    //////////////////////////////////////////////////////////////////////////////
    // SEARCH FOR THE 'REGULAR' STATECASE, and also create SCCells for other cases
    ///////////////////////////////////////////////////////////////////////////////
    StateCase* sc;
    bool reg_case=1;
    forall(sc,*st->getCases()){
      InputSpec* is;
      forall(is,*sc->getInputs()){
	if (is->isEosCase()){   //is eos, create SCCell and break
	  reg_case=0;
	  sfn->getSCCells()->append(new SCCell(sfn,sc));
	  break;   //out of forall inputspec, to statecases
	}
      } //end of forall inputs
      if (reg_case)
	sfn->getRegSCCell()->setStateCase(sc); //SCCell.statecase now points to regular statecase
    }
    
    //////////////////////////////////////////////////////////
    ////// CREATE sfn->getConsumedStreams()
    //////////////////////////////////////////////////////////
    
    InputSpec* is;
    forall(is,*sc->getInputs()) //using the last sc visited, which is fine
      sfn->getConsumedStreams()->insert(is->getStream()->toString());
  }  //forall states
  
  /////////////////////////////////////////////////////
  //////////// CREATE SFEdges
  ////////////////////////////////////////////////////
  pass_to_map pass;
  pass.dict = state_dict;
  pass.SFG = SFG;
  pass.last_condition = (Expr*) nil;
  pass.is_there_else = 0;
  pass.then=0;
  node n;
  forall_nodes(n,*SFG){ 
    SFNode* sfn = SFG->inf(n);
    st=sfn->getState();
    //cerr << "\nfrom state " << st->getName() << "  "<< st<< endl;
    pass.state = st;
    pass.curr_node=n;
    SCCell* sccell;
    forall(sccell,*(sfn->getSCCells())){
      pass.sccell=sccell; 
      sccell->getStateCase()->map(descend_statecase, nil, (void*) &pass); // detects nextst
    }
  }
}




