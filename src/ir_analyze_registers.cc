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
// SCORE TDF compiler:  Eliminates false registers, creates def-use chains
// $Revision 1.122$
//
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// false registers are those assigned a constant, only once: they are 
// replaced by the constant
/////////////////////////////////////////////////////////////////////////////

#include "ir_graph.h"
#include "operator.h"
#include "bindvalues.h"
#include "ops.h"
#include "type.h"
#include "tree.h"
#include "ir_analyze_registers.h"

using leda::dic_item;
using std::cerr;
using std::endl;

//struct VAL;
extern OFGraph gOFG;

void fold(Tree**h  ){
  Tree* t = *h;
   int out;
   if (((Expr*)t)->getExprKind() == EXPR_BOP){
     long long in1 = ((ExprValue*)((ExprBop*)t)->getExpr1())->getIntVal();  //get back and deal with frac....
     long long in2 = ((ExprValue*)((ExprBop*)t)->getExpr2())->getIntVal();
     int bop = ((ExprBop*)t)->getOp();
     switch (bop)
       {
       case '+': {out=in1 +  in2; break;}
       case '-': {out=in1 -  in2; break;}
       case '*': {out=in1 *  in2; break;}
       case '/': {out=in1 /  in2; break;}
       case '%': {out=in1 %  in2; break;}
       case '&': {out=in1 &  in2; break;}
       case '|': {out=in1 |  in2; break;}
       case '^': {out=in1 ^  in2; break;}
       case LEFT_SHIFT:  {out=in1 <<  in2; break;}
       case RIGHT_SHIFT: {out=in1 >>  in2; break;}
       case LOGIC_AND:   {out=in1 &&  in2; break;}
       case LOGIC_OR:    {out=in1 ||  in2; break;}
       case EQUALS:      {out=in1 ==  in2; break;}
       case NOT_EQUALS:  {out=in1 !=  in2; break;}
       case GTE:		{out=in1 >=  in2; break;}
       case LTE:		{out=in1 <=  in2; break;}
       case '>':		{out=in1 >   in2; break;}
       case '<':		{out=in1 <   in2; break;}
       }
     *h = (Tree*) new ExprValue(NULL,((ExprBop*)t)->getExpr1()->getType(),out,0); 
   }
   else if (((Expr*)t)->getExprKind() == EXPR_COND){
     ExprCond* econd = (ExprCond*)t;
     ExprValue* ev = (ExprValue*) econd->getCond();
     bool cond =  (bool) ev->getIntVal();
     //Operator op;
     Type* type;
     if (cond){
       out=((ExprValue*) econd->getThenPart())->getIntVal();
       type = ((ExprValue*) econd->getThenPart())->getType();
     }
     else{
       out = ((ExprValue*) econd->getElsePart())->getIntVal();
       type =( (ExprValue*)econd->getElsePart())->getType();
     }
     *h = (Tree*) new ExprValue(NULL,type,out,0); 
   }
   //cerr << "written out value " << out <<" in tree " << *t<< endl;
   //THIS MIGHT BE CREATING THAT TYPE INCONSISTENCY!!!!!!!!!
}
bool const_fold_post(Tree** h, void* i){
  Tree* t = *h;
  if (t->getKind() == TREE_EXPR && ((Expr*)t)->getExprKind() == EXPR_BOP &&
      ((ExprBop*)t)->getExpr1()->getExprKind() == EXPR_VALUE &&  
      ((ExprBop*)t)->getExpr2()->getExprKind() == EXPR_VALUE ){
    //cerr<<"can fold again " << t->toString() << endl;
    fold(h);
  }
  else if (t->getKind() == TREE_EXPR && ((Expr*)t)->getExprKind() == EXPR_COND){
    ExprCond* cexpr= (ExprCond*)t;
    Expr *cpart=cexpr->getCond();
    Expr *tpart=cexpr->getThenPart();
    Expr *epart=cexpr->getElsePart();
    if (cpart->getExprKind() == EXPR_VALUE &&
	tpart->getExprKind() == EXPR_VALUE &&
	epart->getExprKind() == EXPR_VALUE ){
      //cerr << "here we can fold " << t->toString() <<  endl;
      fold(h);
    }
  }
  return true;
}
bool const_fold_pre(Tree** h, void* i){
  Tree* t = *h;
  // Nachiket: WTF???
  if (t->getKind() == TREE_OPERATOR)
    //cerr << "while FOLDING, visited operator " << ((Operator*)t)->getName() << endl;


  if (t->getKind() == TREE_EXPR && ((Expr*)t)->getExprKind() == EXPR_BOP &&
      ((ExprBop*)t)->getExpr1()->getExprKind() == EXPR_VALUE &&  
      ((ExprBop*)t)->getExpr2()->getExprKind() == EXPR_VALUE ){
    //cerr << "here we can fold " << t->toString() <<  endl;
    fold(h);
    return 0;
  }
  else if (t->getKind() == TREE_EXPR && ((Expr*)t)->getExprKind() == EXPR_COND){
    ExprCond* cexpr= (ExprCond*)t;
    Expr *cpart=cexpr->getCond();
    Expr *tpart=cexpr->getThenPart();
    Expr *epart=cexpr->getElsePart();
    if (cpart->getExprKind() == EXPR_VALUE &&
	tpart->getExprKind() == EXPR_VALUE &&
	epart->getExprKind() == EXPR_VALUE ){
      //cerr << "here we can fold " << t->toString() <<  endl;
      fold(h);
      return 0;
    }
  }
  return true;
}
bool replace (Tree** h, void* vstr){
  Tree* t = *h;
  to_pass * str = (to_pass*) vstr;
  
  if (t->getKind() == TREE_EXPR && ((Expr*)t)->getExprKind() == EXPR_LVALUE &&
      ((ExprLValue*)t)->toString() == str->reg ){
    *h = (Tree*) new ExprValue(NULL,str->val->getType(),str->val->getIntVal());
    //cerr << "CHANGED INTO " << (*h)->toString()<< endl;
  }
  return (t->getKind() == TREE_EXPR|| t->getKind() == TREE_STMT);
}

void ConstantFold(Tree* t){
  // - not used, superceded by constant folding in "instance.cc"
  // - not strict about typing rules, does not mask result to proper width

  t->map2(&t,const_fold_pre,const_fold_post,nil);
}

void lookup_state(node curr_node, State* where_last_assigned, SFGraph SFG,
	     list<DefUsePair*> * def_use_list, string reg_string,  
	     dictionary <State*, set<State*> *> * dic) {
  SFNode* sfn = SFG->inf(curr_node);
  SCCell* scc = sfn->getRegSCCell();
  State* curr_state = sfn->getState();
  //cerr << "State " << curr_state->getName()<< endl;
  //cerr << "where last assigned ";
  //if (where_last_assigned)
    //cerr << where_last_assigned->getName();
  //cerr << "\n\n";
  dic_item it = dic->lookup(curr_state);
  if ( it  &&              // already visited state &&
       dic->inf(it)->member(where_last_assigned)){ // already seen where_last_assigned
    //cerr << "I am returning becasue already seen state-last_assign pair\n";
    return;
  }
  bool first_time=0;
  bool new_last_defined = 0;
  set<State*> * state_set;
  if (it == nil ){
    first_time=1;
    dic->insert(curr_state, (state_set = new set<State*>));
    state_set->insert(where_last_assigned);
  }
  else {
    state_set = dic->inf(it);
    if (! state_set->member(where_last_assigned)){
    new_last_defined = 1;
    state_set->insert(where_last_assigned);
    }
  }
  if(scc->getUsedReg()->lookup(reg_string)){
    //cerr << " reg \"" <<reg_string<<"\" used, state " << curr_state->getName() << endl;
    DefUsePair* pair = new DefUsePair;
    pair->def=where_last_assigned;
    pair->use=curr_state;
    //if (def_use_list->rank(pair))
    //cerr << "FOUND A def-use already there!!\n";
    def_use_list->append(pair);
    //cerr << "def-use ";
    //if(!where_last_assigned)
    //  cerr << "INIT";
    //else
    //  cerr << where_last_assigned->getName();
    //cerr << " - " << curr_state->getName()<<endl;
  }
  if(scc->getDefReg()->lookup(reg_string)){
    where_last_assigned = curr_state;
    //cerr << " reg \"" <<reg_string<<"\" defined, state " << curr_state->getName() << endl;
  }
  if(!(first_time||new_last_defined)){
    //cerr << "I am returning becasue !(first_time||new_last_defined)\n";
    // it seems like the above NEVER happens .... GET BACK TO THIS!!!!!!!!
    return;
  }
  //go on to next states
  node next_node;
  forall_adj_nodes(next_node,curr_node){
    //node next_node = SFG->target(e);
    lookup_state (next_node,where_last_assigned, SFG,def_use_list, reg_string,dic );
  }
}
void PrintDefUseChain(string reg_string, list<DefUsePair*> * def_use_list){
  cerr << "reg " << reg_string<<endl;
  DefUsePair* du;
  forall(du, *def_use_list){
    cerr << "from ";
    if(! du->def)
      cerr << "INIT";
    else
      cerr << du->def->getName();
    cerr << " to " << du->use->getName()<<endl;
  }
  cerr <<"\n";
}

void AnalyzeRegisters(){
  node ofnode;
  forall_nodes(ofnode,*gOFG){ 
    OFNode* ofn = gOFG->inf(ofnode);
    SFGraph SFG=ofn->getSFG();
    dic_item it;
    forall_items(it,*(ofn->getRegisters())) {   //for all registers in the operator
      string reg_string = ofn->getRegisters()->key(it);
      node nodei;
      int reg_defined = 0;
      forall_nodes(nodei,*SFG){ //for all states
	SFNode * sfn = SFG->inf(nodei);
	SCCell * scc;
	forall(scc,*sfn->getSCCells()){
	  if (scc->getDefReg()->lookup(reg_string)){
	    //cerr << "REG " << reg_string << " defined in " <<sfn->getState()->getName() << endl;
	    reg_defined++;
	  }
	}
      }
      if (reg_defined==0){ //let's eliminate the register
	//cerr << "register "<<reg_string << " eliminated" << endl;
	forall_nodes(nodei,*SFG){ //for all states
	  SFNode * sfn = SFG->inf(nodei);
	  SCCell * scc;
	  forall(scc,*sfn->getSCCells()){
	    DFGraph DFG = scc->getDFG();
	    if (dic_item itemi = scc->getUsedReg()->lookup(reg_string)){
	      node regnode = scc->getUsedReg()->inf(itemi);
	      node adj_node;
	      int length = DFG->out_edges(regnode).length();
	      forall_adj_nodes(adj_node, regnode){
		ExprLValue* lv = DFG->inf(adj_node)->getDest();
		//cerr <<"LVALUE POINTER " << lv << endl;
		Expr* exp = (Expr*) lv->getParent();
		if (ofn->getInitialvalues()->lookup(reg_string)){
		//value we want to substitute:
		  ExprValue* value = ofn->getInitialvalues()->access(reg_string); 
		  Tree* t = (Tree*) exp;
		  to_pass thing = { value,reg_string};
		  to_pass* point=&thing;
		  t->map2(&t,replace,nil,(void*) point ); 
		  //node where reg went in:
		  node in_node=DFG->target(DFG->first_adj_edge(adj_node)); 
		  //new node with ExprValue
		  node valnode=
		    DFG->new_node(new DFNode((Expr*)value,DFN_INVAL,value->toString())); 
		  DFG->new_edge(valnode,in_node,DFG->inf(valnode)->getDFEdge());
		  DFG->del_node(adj_node);  //the old reg node
		}
		//the following line is for a bug in leda! if it has 
		//to iterate only once, it goes on instead
		if (length==1) break;
	      }
	      DFG->del_node(regnode);  //the Top_reg node
	      scc->getUsedReg()->del_item(itemi);
	      // - EC:  HACK, direct manipulation of symbol dic is bad,
	      //        especially since we added symbol order list
	      // ((OperatorBehavioral*)ofn->getOperator())->getVars()->getSymbols()->del(reg_string);//from AST
	      ((OperatorBehavioral*)ofn->getOperator())->getVars()->removeSymbol(
	      ((OperatorBehavioral*)ofn->getOperator())->getVars()->lookup(reg_string)
	      );
	      ofn->getRegisters()->del(reg_string);
	    }
	  }
	}
      }
    }
  }
  
  ///////////////////////////////////////////////////////////////
  ///// NOW Create DEF-USE CHAINS
  ///// PROBLEM: I am only considering Regular SCCells, i.e. not the eos cases....
  ///// TO BE ADDED sometime   laurap 1/31/01
  /////////////////////////////////////////////////////////////// 
  forall_nodes(ofnode,*gOFG){ 
    OFNode* ofn = gOFG->inf(ofnode);
    //cerr <<"for def-use, operator " <<ofn->getOperator()->getName() <<endl;
    SFGraph SFG = ofn->getSFG();
    dic_item it;
    forall_items(it,*(ofn->getRegisters())) {   //for all registers in the operator
      State* where_last_assigned = nil;
      string reg_string = ofn->getRegisters()->key(it);
      //cerr << "analyzing reg \"" << reg_string << "\"\n";
      if (ofn->getInitialvalues()->lookup(reg_string)){
	where_last_assigned= nil;  //what do I call a not last_assigned????
	//cerr << "has been initialised\n";
      }
      dictionary <State *, set<State*> *> * dic = new dictionary <State *, set<State*> *> ;
      list<DefUsePair*> * def_use_list = new list <DefUsePair*>;
      lookup_state(SFG->first_node(), where_last_assigned, SFG, 
		   def_use_list, reg_string, dic);
      ofn->setDefUsePairs(reg_string,def_use_list);
      //PrintDefUseChain(reg_string,def_use_list);
    }
  }
}
