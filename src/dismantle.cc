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
// SCORE TDF compiler:  dismantle register/stream access to have own state
// $Revision: 1.145 $
//
//////////////////////////////////////////////////////////////////////////////


#include <string.h>
#include "annotes.h"
#include "state.h"
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "canonical.h"
#include <LEDA/core/list.h>
#include <LEDA/core/dictionary.h>
#include <LEDA/core/set.h>
#include <LEDA/core/map.h>

using leda::list_item;
using leda::map;


////////////////////////////////////////////////////////////////
//  unique rename

int findRenamingSuffix (const string& s, int *val)
{
  // - check if s has a suffix "_%d"
  // - if yes, return position of '_', and set val to '%d' value
  // - if no,  return -1

  int tval=0;	// - '%d' value
  int pos;	// - position of last '_' in origName, -1 if none found
  int nextpos;	// - scratch
  for (pos=-1, nextpos=s.pos("_"); nextpos!=-1;
       pos=nextpos, nextpos=s.pos("_",pos+1));
  if (pos!=-1) {
    // - found '_', try to match subsequent '%d'
    for (int i=pos+1; i<s.length(); i++) {
      char c=s[i];
      if (c>='0' && c<='9')
	tval = tval*10 + c-'0';
      else {
	pos   = -1;
	break;	// for
      }
    }
  }
  if (pos!=-1) {
    *val=tval;
    return pos;
  }
  else
    return -1;
}


void uniqueRenameSym_inc (Symbol *sym, SymTab *scope)
{
  // - renames sym (if necessary) to be unique in the given scope
  // - renaming is by addition of "_i" where i is smallest possible int >=1
  // - existing suffix "_i" is recognized and incremented

  string origName=sym->getName(), baseName, newName;
  int    origNameCnt, newNameCnt;

  if (scope->lookup(origName)==NULL)	// - symbol name is already unique
    return;

  int pos=findRenamingSuffix(origName,&origNameCnt);
  if (pos==-1) {
    baseName=origName;
    newNameCnt=1;
  }
  else {
    baseName=origName(0,pos-1);
    newNameCnt=origNameCnt+1;
  }

  for (newName=baseName+string("_%d",newNameCnt);
       scope->lookup(newName);
       newName=baseName+string("_%d",++newNameCnt));
  sym->setName(newName);

  // if (newName!=origName)
  //  warn(string("renamed symbol ")+origName+" to "+newName, sym->getToken());
}


void uniqueRenameState_inc (State *state, dictionary<string,State*> *states)
{
  // - renames state (if necessary) to be unique in the given dict of states
  // - renaming is by addition of "_i" where i is smallest possible int >=1
  // - existing suffix "_i" is recognized and incremented

  string origName=state->getName(), baseName, newName;
  int    origNameCnt, newNameCnt;

  if (states->lookup(origName)==NULL)	// - state name is already unique
    return;

  int pos=findRenamingSuffix(origName,&origNameCnt);
  if (pos==-1) {
    baseName=origName;
    newNameCnt=1;
  }
  else {
    baseName=origName(0,pos-1);
    newNameCnt=origNameCnt+1;
  }

  for (newName=baseName+string("_%d",newNameCnt);
       states->lookup(newName);
       newName=baseName+string("_%d",++newNameCnt));
  state->setName(newName);

  // if (newName!=origName)
  // warn(string("renamed state ")+origName+" to "+newName, state->getToken());
}


////////////////////////////////////////////////////////////////
//  Utilities

list<Stmt*>* getEnclosingStmtList (Tree *t)
{
  while (t && t->getKind()!=TREE_STMT)	// - find enclosing stmt
    t=t->getParent();
  if (!t)
    return NULL;
  Tree *p = t->getParent();		// - check kind of stmt's parent
  if (p->getKind()==TREE_STMT) {
    assert(((Stmt*)p)->getStmtKind()==STMT_BLOCK);
    return ((StmtBlock*)p)->getStmts();
  }
  else if (p->getKind()==TREE_STATECASE)
    return ((StateCase*)p)->getStmts();
  else
    return NULL;
}


template<class E>
string print_list (list<E>& L)
{
  // - print a list of Tree* elements  (class E: public Tree)
  string str;
  E e;
  forall (e,L) {
    str += e->toString();
    if (str.tail(1)!=string('\n'))
      str += " ";
  }
  return str;
}


template<class E>
void split_list (const E& e, list<E>& L1, list<E>& L2, int dir=LEDA::before)
{
  // - similar to LEDA list::split(...)
  // - split L1 at the first occurence of e,
  //     truncating L1 and transferring tail to L2 (insert at front)
  // - truncation before or after e depends on dir
  // - no action if e not found

  // warn("before split:  {"+print_list(L1)+"} {"+print_list(L2)+"}");

  list_item ie;				// - find ie = 1st occurence of e in L1
  for (ie=L1.first(); ie && L1[ie]!=e; ie=L1.succ(ie));
  if (ie) {
    while (L1.last()!=ie)		// - transfer items after e
      L2.push_front(L1.pop_back());
    if (dir==LEDA::before)		// - transfer e
      L2.push_front(L1.pop_back());
    // warn("after  split:  {"+print_list(L1)+"} {"+print_list(L2)+"}");
  }
  else {
    // warn("split element not found");
    assert(!"internal inconsistency");
  }
}


////////////////////////////////////////////////////////////////
//  DismantleStreamAccess

class Ispec_Info {		// - count appearances of an input stream
public:				//     in the input-specs of a state's cases
  Symbol *symbol;		// - stream's symbol
  int data_appearances;		// - # of times stream appears as data input
  int  eos_appearances;		// - # of times stream appears as eos  input
  Ispec_Info () : data_appearances(0), eos_appearances(0) {}
};

enum InputStreamRank {	// - rank of input stream for input-spec ordering
  ALL_CASES_DATA=1,	//     1: appears as data input  in all state-cases
  ALL_CASES     =2,	//     2: appears as data or eos in all state-cases
  SOME_CASES    =3	//     3: appears in only some of the state-cases
};

static
map<Symbol*,InputStreamRank> *gInputStreamRanks;    // - used for cmp below

int cmp_inputStreamRanks (InputSpec *const &i1, InputSpec *const &i2)
{
  // - compare input-specs by stream rank, then by stream name  (return -1,0,1)
  Symbol *s1=i1->getStream();
  Symbol *s2=i2->getStream();
  int r1=(int)(*gInputStreamRanks)[s1];
  int r2=(int)(*gInputStreamRanks)[s2];
  return (r1<r2) ? -1 :
	 (r1>r2) ?  1 :
	 strcmp(s1->getName(),s2->getName());
}


int canonicalizeInputSpecOrder (State *s)
{
  // - Reorder input-specs of state-cases in state *s into matching order:
  //     * first, list input-specs that appear in all state-cases as data input
  //     * second, list input-specs that appear in all state-cases w/eos
  //     * third, list input-specs that appear only in some state-cases
  //     * within each category, secondarily order by symbol name
  // - return number of "common inputs" that appear in all state-cases
  //     (1st + 2nd category)

  set<StateCase*> *stateCases=s->getCases();
  int numCases=stateCases->size();
  if (numCases==1)	// - order already canonical if only 1 state-case
    return stateCases->choose()->getInputs()->size();
  else if (numCases==0)
    return 0;

  int commonInputs=0;	// - return value

  // - tally data/eos appearances of input streams across all state-cases
  map<Symbol*,Ispec_Info> ispec_infos;
  StateCase *sc;
  forall (sc,*stateCases) {
    InputSpec *ispec;
    forall (ispec,*sc->getInputs()) {
      Symbol *sym=ispec->getStream();
      Ispec_Info &info=ispec_infos[sym];
      info.symbol=sym;
      if (ispec->isEosCase())
	info.eos_appearances++;
      else
	info.data_appearances++;
    }
  }

  // - rank input streams into classes
  map<Symbol*,InputStreamRank> inputStreamRanks;
  Ispec_Info info;
  forall (info,ispec_infos) {
    InputStreamRank rank
      = (info.data_appearances==numCases)		       ? ALL_CASES_DATA
      : (info.data_appearances+info.eos_appearances==numCases) ? ALL_CASES
      :								 SOME_CASES;
    inputStreamRanks[info.symbol]=rank;
    // warn("Input stream rank for "+info.symbol->getName()+
    //      string(" is %d",(int)rank));
    if (rank!=SOME_CASES)
      commonInputs++;
  }

  // - reorder input-specs of each state-case, sort by rank then by name
  gInputStreamRanks = &inputStreamRanks;
  forall (sc,*stateCases)
    sc->getInputs()->sort(cmp_inputStreamRanks);
    // - HACK: modifies input-spec list directly

  return commonInputs;
}


void dismantleStreamAccess_op (OperatorBehavioral *op)
{
  // - dismantle *op s.t. each stream input happens in its own state
  // - original:    state foo (i,...) : ...
  // - dismantled:  state foo () : goto input_i_for_foo;
  //                state input_i_for_foo (i) : goto foo_1;
  //                state foo_1 (...) : ...
  // - correctly handles states where multiple state-cases
  //     input different input streams or same streams in different order
  //     (does not solve or report ambiguity in firing cases,
  //      but should lead to equivalent ambiguity in dismantled code).
  // - state-case inputs are reordered as per canonicalizeInputSpecOrder()

  // warn ("dismantleStreamAccess_op: op "+op->getName());

  list<State*> states;		// - working set of states to dismantle
  State *s;
  forall (s,*op->getStates())
    states.append(s);
  set<State*> statesNamedCase;	// - set of states with "_case" suffix
  map<State*,int> commonInputs;	// - # of input streams common to all cases

  forall (s,states)
    commonInputs[s] = canonicalizeInputSpecOrder(s);
  // return;

  while (!states.empty()) {
    State *s=states.pop();
    // warn("dismantleStreamAccess_op: dismantle state "+s->getName()+
    //      string(" with %d common inputs",commonInputs[s]));
    if (commonInputs[s]>0)
    {
      // - dismantle 1st input stream in each state-case (is same stream)

      State     *newState0=NULL, *newState1=NULL;   // - with multiple cases
      State     *inputState=NULL;		    // - with 2 cases
      StateCase *inputStateCase0=NULL, *inputStateCase1=NULL;
      StateCase *presentStateCase=NULL;

      set<StateCase*> *cases=s->getCases();
      StateCase *sc;
      forall (sc,*cases) {
	list<InputSpec*> *ispecs=sc->getInputs();
	if (!ispecs->empty()) {
	  // warn("dismantleStreamAccess_op: dismantle 1 in state-case\n"+*sc);

	  InputSpec *ispec   = ispecs->head();		// - input to dismantle
	  bool       isEos   = ispec->isEosCase();
	  Symbol    *sym     = ispec->getStream();
	  string     symName = sym->getName();

	  // - create continuation state
	  //     <state>_case0 or <state>_case1 with multiple cases
	  s->removeCase(sc);
	  StateCase *newStateCase=sc;
	  newStateCase->getInputs()->pop();	// - HACK: modifying directly
	  State *newState = isEos?newState1:newState0;
	  if (!newState) {
	    string stateName    = s->getName();
	    string newStateName = stateName + 
				  string(statesNamedCase.member(s)?"":"_case")+
				  string(isEos?"1":"0");
	    newState = new State(NULL,newStateName);
	    statesNamedCase.insert(newState);
	    // - add state to operator later, so case0 precedes case1
	    if (isEos)
	      newState1=newState;
	    else
	      newState0=newState;
	  }
	  newState->addCase(newStateCase);
	  // warn("dismantleStreamAccess_op: new state\n"+*newStateCase);

	  // - create stream input state
	  //     input_<stream>_for_<state> (stream) or (eos(stream))
	  StateCase *inputStateCase = isEos?inputStateCase1:inputStateCase0;
	  if (!inputStateCase) {
	    list<InputSpec*> *inputISpecs=new list<InputSpec*>;
	    inputISpecs->append(ispec);
	    StmtGoto *inputGoto=new StmtGoto(NULL,newState);
	    list<Stmt*> *inputStmts=new list<Stmt*>;
	    inputStmts->append(inputGoto);
	    inputStateCase=new StateCase(NULL,inputISpecs,inputStmts);
	    if (isEos)
	      inputStateCase1=inputStateCase;
	    else
	      inputStateCase0=inputStateCase;
	    if (!inputState) {
	      string stateName     =s->getName();
	      string inputStateName="input_"+symName+"_for_"+stateName;
	      inputState = new State(NULL,inputStateName);
	      // - add state to operator later
	    }
	    inputState->addCase(inputStateCase);
	    // warn("dismantleStreamAccess_op: input state\n"+*inputStateCase);
	  }

	  // - modify present state  <state>
	  if (!presentStateCase) {
	    list<InputSpec*> *presentIspecs=new list<InputSpec*>;
	    StmtGoto *presentGoto=new StmtGoto(NULL,inputState);
	    list<Stmt*> *presentStmts=new list<Stmt*>;
	    presentStmts->append(presentGoto);
	    presentStateCase=new StateCase(NULL,presentIspecs,presentStmts);
	    s->addCase(presentStateCase);
	    // warn("dismantleStreamAccess_op: residual state\n"+*presentStateCase);
	  }
	}
      }  // forall (sc,*cases)
      if (newState1) {
	op->addState(newState1);
	states.push(newState1);
	commonInputs[newState1]=commonInputs[s]-1;
      }
      if (newState0) {
	op->addState(newState0);
	states.push(newState0);
	commonInputs[newState0]=commonInputs[s]-1;
      }
      if (inputState) {
	op->addState(inputState);
      }
      commonInputs[s]=0;
    }

    else  // (commonInputs[s]==0)
    {
      // - dismantle remaining inputs in each state-case
      // - remaining inputs will be different, dismantled as group per case

      State     *inputState=NULL;	// - with multiple cases
      StateCase *presentStateCase=NULL;

      set<StateCase*> *cases=s->getCases();
      StateCase *sc;
      forall (sc,*cases) {
	list<InputSpec*> *ispecs=sc->getInputs();
	if (!ispecs->empty()) {
	  // warn("dismantleStreamAccess_op: dismantle all in state-case\n"+*sc);

	  list<InputSpec*> *inputISpecs=new list<InputSpec*>;	// - inputs to
	  *inputISpecs=*sc->getInputs();			//   dismantle

	  // - create continuation state:  <state>_case_N
	  s->removeCase(sc);
	  StateCase *newStateCase=sc;
	  newStateCase->getInputs()->clear();	// - HACK: modifying directly
	  string stateName    = s->getName();
	  string newStateName = stateName + 
				string(statesNamedCase.member(s)?"X_1"
								:"_caseX_1");
	  State *newState = new State(NULL,newStateName);
	  uniqueRenameState_inc(newState,op->getStates());
	  newStateName=newState->getName(); 
	  newState->addCase(newStateCase);
	  statesNamedCase.insert(newState);
	  op->addState(newState);
	  // warn("dismantleStreamAccess_op: new state\n"+*newStateCase);

	  // - create stream input state:  input_for_<state>
	  // - list<InputSpec*> *inputISpecs - created above
	  StmtGoto *inputGoto=new StmtGoto(NULL,newState);
	  list<Stmt*> *inputStmts=new list<Stmt*>;
	  inputStmts->append(inputGoto);
	  StateCase *inputStateCase=new StateCase(NULL,inputISpecs,inputStmts);
	  if (!inputState) {
	    string stateName     =s->getName();
	    string inputStateName="input_for_"+stateName;
	    inputState = new State(NULL,inputStateName);
	    op->addState(inputState);
	  }
	  inputState->addCase(inputStateCase);
	  // warn("dismantleStreamAccess_op: input state\n"+*inputStateCase);

	  // - modify present state  <state>
	  if (!presentStateCase) {
	    list<InputSpec*> *presentIspecs=new list<InputSpec*>;
	    StmtGoto *presentGoto=new StmtGoto(NULL,inputState);
	    list<Stmt*> *presentStmts=new list<Stmt*>;
	    presentStmts->append(presentGoto);
	    presentStateCase=new StateCase(NULL,presentIspecs,presentStmts);
	    s->addCase(presentStateCase);
	    commonInputs[s]=0;
	    // warn("dismantleStreamAccess_op: residual state\n"+*presentStateCase);
	  }
	}
      }  // forall (sc,*cases)
    }
  }  // while (!states.empty())
}


void dismantleStreamAccess_op_commonInputsOnly (OperatorBehavioral *op)
{
  // - dismantle *op s.t. each stream input happens in its own state
  // - original:    state foo (i,...) : ...
  // - dismantled:  state foo () : goto input_i_for_foo;
  //                state input_i_for_foo (i) : goto foo_1;
  //                state foo_1 (...) : ...
  // - assumes that in states with multiple state-cases,
  //     every state-case inputs same input streams in same order
  //     (differing only in whether input is data or eos())
  // - this is presently not used-- see dismantleStreamAccess_op()

  // warn ("dismantleStreamAccess_op: op "+op->getName());

  list<State*> states;		// - working set of states to dismantle
  State *s;
  forall (s,*op->getStates())
    states.append(s);
  set<State*> statesNamedCase;	// - set of states with "_case" suffix
  map<State*,int> commonInputs;	// - # of input streams common to all cases

  forall (s,states)
    commonInputs[s] = canonicalizeInputSpecOrder(s);
  // return;

  while (!states.empty()) {
    State *s=states.pop();
    set<StateCase*> *cases=s->getCases();
    State     *newState0=NULL, *newState1=NULL;
    State     *inputState=NULL;
    StateCase *inputStateCase0=NULL, *inputStateCase1=NULL;
    StateCase *presentStateCase=NULL;
    StateCase *sc;
    forall (sc,*cases) {
      list<InputSpec*> *ispecs=sc->getInputs();
      if (!ispecs->empty())
      {
	// - dismantle 1st input stream for this state

	// warn("dismantleStreamAccess_op: dismantle state-case\n"+*sc);

	InputSpec *ispec   = ispecs->head();
	bool       isEos   = ispec->isEosCase();
	Symbol    *sym     = ispec->getStream();
	string     symName = sym->getName();

	// - create continuation state
	//     <state>_case0 or <state>_case1 with multiple cases
	s->removeCase(sc);
	StateCase *newStateCase=sc;
	newStateCase->getInputs()->pop();	// - HACK: modifying directly
	State *newState = isEos?newState1:newState0;
	if (!newState) {
	  string stateName    = s->getName();
	  string newStateName = stateName + 
				string(statesNamedCase.member(s)?"":"_case") +
				string(isEos?"1":"0");
	  set<StateCase*> *newStateCases = new set<StateCase*>;
	  newState = new State(NULL,newStateName,newStateCases);
	  if (isEos)
	    newState1=newState;
	  else
	    newState0=newState;
	  statesNamedCase.insert(newState);
	}
	newState->addCase(newStateCase);
	// warn("dismantleStreamAccess_op: new state\n"+*newStateCase);
	
	// - create stream input state
	//     input_<stream>_for_<state> (stream) or (eos(stream))
	StateCase *inputStateCase = isEos?inputStateCase1:inputStateCase0;
	if (!inputStateCase) {
	  list<InputSpec*> *inputISpecs=new list<InputSpec*>;
	  inputISpecs->append(ispec);
	  StmtGoto *inputGoto=new StmtGoto(NULL,newState);
	  list<Stmt*> *inputStmts=new list<Stmt*>;
	  inputStmts->append(inputGoto);
	  inputStateCase=new StateCase(NULL,inputISpecs,inputStmts);
	  if (isEos)
	    inputStateCase1=inputStateCase;
	  else
	    inputStateCase0=inputStateCase;
	  if (!inputState) {
	    string stateName     =s->getName();
	    string inputStateName="input_"+symName+"_for_"+stateName;
	    set<StateCase*> *inputStateCases = new set<StateCase*>;
	    inputState = new State(NULL,inputStateName,inputStateCases);
	  }
	  inputState->addCase(inputStateCase);
	  // warn("dismantleStreamAccess_op: input state\n"+*inputStateCase);
	}

	// - modify present state  <state>
	if (!presentStateCase) {
	  list<InputSpec*> *presentIspecs=new list<InputSpec*>;
	  StmtGoto *presentGoto=new StmtGoto(NULL,inputState);
	  list<Stmt*> *presentStmts=new list<Stmt*>;
	  presentStmts->append(presentGoto);
	  presentStateCase=new StateCase(NULL,presentIspecs,presentStmts);
	  s->addCase(presentStateCase);
	  // warn("dismantleStreamAccess_op: residual state\n"+*presentStateCase);
	}
      }
    }
    if (newState1) {
      op->addState(newState1);
      states.push(newState1);
    }
    if (newState0) {
      op->addState(newState0);
      states.push(newState0);
    }
    if (inputState) {
      op->addState(inputState);
    }
  }
}


////////////////////////////////////////////////////////////////
//  DismantleRegisterAccess

class DismantleRegisterAccess_info
{
  // - info for dismantling shared reg/stream access in a particular state-case
public:
  OperatorBehavioral	 *op;		// - enclosing op
  list<StateCase*>	 *scases;	// - state-cases not yet dismantled
  StateCase		 *scase;	// - state-case being dismantled
  Stmt			 *stmt;		// - stmt being visited
  bool			  done;		// - true if done with this state-case
  DismantleRegisterAccess_info (OperatorBehavioral *op_i,
				list<StateCase*> *scases_i,
				StateCase *scase_i, Stmt *stmt_i)
    : op(op_i), scases(scases_i), scase(scase_i), stmt(stmt_i), done(0) {}
};


bool dismantleRegisterAccess_reads_map (Tree *t, void *i)
{
  // - dismantle the first register/stream-history read encountered,
  //     modifying the present state and creating/installing new states
  // - original:    state foo (...) : ...=reg; ...
  // - dismantled:  state foo (...) : goto read_reg_to_foo;
  //                state read_reg_from_foo () : reg_to_foo=reg; goto foo_1;
  //                state foo_1 ()  : ...=reg_to_foo; ...
  // - map over a stmt in basic-block code (-xc)

  DismantleRegisterAccess_info *info = (DismantleRegisterAccess_info*)i;

  switch (t->getKind())
  {
    case TREE_STMT:
    {
      info->stmt=(Stmt*)t;
      return true; 
    }

    case TREE_EXPR:
    {
      // warn("dismantleRegisterAccess_reads_map: examine expr "+t->toString());

      // - should only get here in a stmt (not in type, iospec, etc.)
      if (((Expr*)t)->getExprKind()==EXPR_LVALUE) {
	if (                (t->getParent())->getKind()    ==TREE_STMT
	    &&       ((Stmt*)t->getParent())->getStmtKind()==STMT_ASSIGN
	    && ((StmtAssign*)t->getParent())->getLValue()  ==t            ) {
	  // - skip (but descend into) lvalue of assignment stmt
	  return true;
	}
	ExprLValue *e  =(ExprLValue*)t;
	Symbol     *sym=e->getSymbol();
	if (   ( sym->isReg() &&
	        !sym->getAnnote(ANNOTE_IS_SIMPLE_REG))
	    || ( sym->isStream() &&
		((SymbolStream*)sym)->getDir()==STREAM_IN) ) {
	  // - found register read / input stream read to dismantle

	  // - create transfer register <reg>_to_<state>
	  string stateName = ((State*)info->scase->getParent())->getName();
	  string newSymName = sym->getName() + "_to_" + stateName;
	  Symbol *newSym=new SymbolVar(NULL,newSymName,
				       (Type*)e->getType()->duplicate());
	  //			       (Type*)sym->getType()->duplicate());
	  uniqueRenameSym_inc(newSym,e->getScope());
	  newSymName=newSym->getName();
	  newSym->setAnnote_(ANNOTE_IS_SIMPLE_REG,true);
	  info->op->getVars()->addSymbol(newSym);

	  // - modify stmt to reference transfer register instead of <reg>
	  Tree *p=e->getParent();
	  ExprLValue *e_copy=(ExprLValue*)e->duplicate();
	  *e=ExprLValue(NULL,newSym);		// - HACK: overwriting a tree
	  e->thread(p);				//         (setParent not suff)

	  // - create continuation state
	  //     state <state>_N () : 
	  //       <present stmt>
	  //       <subsequent stmts>
	  // - this cuts present state to include only prev. stmts
	  SymTab *localSymtab = info->stmt->getParent()->getKind()==TREE_STMT
			      ? ((StmtBlock*)info->stmt->getParent())
						       ->getSymtab() : NULL;
	  list<Stmt*> *stmts    =getEnclosingStmtList(info->stmt);
	  list<Stmt*> *postStmts=new list<Stmt*>;
	  // warn("dismantleRegisterAccess_reads_map: splitting stmt list {"
	  //      +print_list(*stmts)+"} before stmt "+info->stmt->toString());
	  split_list(info->stmt,*stmts,*postStmts,LEDA::before);
			// - NOTE, postStmts are removed from stmts
			// - HACK: modifying stmt list directly
	  StateCase *newStateCase=new StateCase(NULL,new list<InputSpec*>,
						postStmts);
	  // warn("stmt parent is "+
	  //      string((info->stmt->getParent())->getKind()==TREE_STMT
	  //             ?"":"NOT ") + "TREE_STMT");
	  if (localSymtab && !localSymtab->getSymbols()->empty())
	    // - if continuation state references locals from orig state,
	    //     then promote those locals into regs (too conservative)
	    liftOrphanedLocals(newStateCase,info->op);
	  set<StateCase*> *newStateCases=new set<StateCase*>;
	  newStateCases->insert(newStateCase);
	  string newStateName=stateName;
	  State *newState=new State(NULL,newStateName,newStateCases);
	  uniqueRenameState_inc(newState,info->op->getStates());
	  newStateName=newState->getName();
	  info->op->addState(newState);
	  info->scases->push(newStateCase);
	  // warn("dismantleRegisterAccess_reads_map: new state\n"+*newState);

	  // - create reg read state
	  //     state read_<reg>_to_<state> () :
	  //       <reg>_to_<state>=<reg>;
	  //       goto <state>_N;
	  StmtAssign  *readAssign=new StmtAssign(NULL,
						 new ExprLValue(NULL,newSym),
						 e_copy);
	  StmtGoto    *readGoto  =new StmtGoto(NULL,newState);
	  list<Stmt*> *readStmts =new list<Stmt*>;
	  readStmts->append(readAssign);
	  readStmts->append(readGoto);
	  StateCase *readStateCase=new StateCase(NULL,new list<InputSpec*>,
						 readStmts);
	  set<StateCase*> *readStateCases=new set<StateCase*>;
	  readStateCases->insert(readStateCase);
	  string readStateName = "read_" + newSymName;
	  State *readState=new State(NULL,readStateName,readStateCases);
	  info->op->addState(readState);
	  // warn("dismantleRegisterAccess_reads_map: read state\n"+*readState);

	  // - modify present state  (truncation already done above; add goto)
	  StmtGoto *preGoto=new StmtGoto(NULL,readState);
	  preGoto->setParent(info->stmt->getParent());
	  stmts->append(preGoto);	// - HACK: modifying stmt list directly
	  // warn("dismantleRegisterAccess_writes_map: residual state-case\n"+
	  //      *info->scase->getParent());

	  // - done with state-case  (process no stmts after this one)
	  info->done=true;
	  return true;		// - continue w/this stmt
	}
      }
      return true;
    }

    default:
    {
      return false;
    }
  }
}


bool dismantleRegisterAccess_writes_map (Tree *t, void *i)
{
  // - dismantle the first register/stream write encountered,
  //     modifying the present state and creating/installing new states
  // - original:    state foo (...) : reg=...; ...
  // - dismantled:  state foo (...) : reg_from_foo=...; goto write_reg_from_foo
  //                state write_reg_from_foo () : foo=reg_from_foo; goto foo_1;
  //                state foo_1 ()  : ...
  // - map over a stmt in basic-block code (-xc)

  DismantleRegisterAccess_info *info = (DismantleRegisterAccess_info*)i;

  switch (t->getKind())
  {
    case TREE_STMT:
    {
      info->stmt=(Stmt*)t;
      if (((Stmt*)t)->getStmtKind()==STMT_ASSIGN) {
	StmtAssign *s  =(StmtAssign*)t;
	ExprLValue *e  =s->getLValue();
	Symbol     *sym=e->getSymbol();
	if (   ( sym->isReg() &&
		!sym->getAnnote(ANNOTE_IS_SIMPLE_REG))
	    || ( sym->isStream() &&
		((SymbolStream*)sym)->getDir()==STREAM_OUT) ) {
	  // - found register write / output stream write to dismantle

	  // - create transfer register <reg>_from_<state>
	  string stateName = ((State*)info->scase->getParent())->getName();
	  string newSymName = sym->getName() + "_from_" + stateName;
	  Symbol *newSym=new SymbolVar(NULL,newSymName,
				       (Type*)e->getType()->duplicate());
	  //			       (Type*)sym->getType()->duplicate());
	  uniqueRenameSym_inc(newSym,e->getScope());
	  newSymName=newSym->getName();
	  newSym->setAnnote_(ANNOTE_IS_SIMPLE_REG,true);
	  info->op->getVars()->addSymbol(newSym);

	  // - modify stmt to reference transfer register instead of <reg>
	  Tree *p=e->getParent();	// p==s
	  ExprLValue *e_copy=(ExprLValue*)e->duplicate();
	  *e=ExprLValue(NULL,newSym);		// - HACK: overwriting a tree
	  e->thread(p);				//         (setParent not suff)

	  // - create continuation state
	  //     state <state>_N () : 
	  //       <subsequent stmts>
	  // - this cuts present state to include prev. + present stmts
	  SymTab *localSymtab = info->stmt->getParent()->getKind()==TREE_STMT
			      ? ((StmtBlock*)info->stmt->getParent())
						       ->getSymtab() : NULL;
	  list<Stmt*> *stmts    =getEnclosingStmtList(info->stmt);
	  list<Stmt*> *postStmts=new list<Stmt*>;
	  // warn("dismantleRegisterAccess_write_map: splitting stmt list {"
	  //      +print_list(*stmts)+"} after stmt "+info->stmt->toString());
	  split_list(info->stmt,*stmts,*postStmts,LEDA::after);
			// - NOTE, postStmts are removed from stmts
			// - HACK: modifying stmt list directly
	  StateCase *newStateCase=new StateCase(NULL,new list<InputSpec*>,
						postStmts);
	  // warn("stmt parent is "+
	  //      string((info->stmt->getParent())->getKind()==TREE_STMT
	  //             ?"":"NOT ") + "TREE_STMT");
	  if (localSymtab && !localSymtab->getSymbols()->empty())
	    // - if continuation state references locals from orig state,
	    //     then promote those locals into regs (too conservative)
	    liftOrphanedLocals(newStateCase,info->op);
	  set<StateCase*> *newStateCases=new set<StateCase*>;
	  newStateCases->insert(newStateCase);
	  string newStateName=stateName;
	  State *newState=new State(NULL,newStateName,newStateCases);
	  uniqueRenameState_inc(newState,info->op->getStates());
	  newStateName=newState->getName();
	  info->op->addState(newState);
	  info->scases->push(newStateCase);
	  // warn("dismantleRegisterAccess_writes_map: new state\n"+*newState);

	  // - create reg write state
	  //     state write_<reg>_from_<state> () :
	  //       <reg>_from_<state>=<reg>;
	  //       goto <state>_N;
	  StmtAssign  *writeAssign=new StmtAssign(NULL,e_copy,
						  new ExprLValue(NULL,newSym));
	  StmtGoto    *writeGoto  =new StmtGoto(NULL,newState);
	  list<Stmt*> *writeStmts =new list<Stmt*>;
	  writeStmts->append(writeAssign);
	  writeStmts->append(writeGoto);
	  StateCase *writeStateCase=new StateCase(NULL,new list<InputSpec*>,
						  writeStmts);
	  set<StateCase*> *writeStateCases=new set<StateCase*>;
	  writeStateCases->insert(writeStateCase);
	  string writeStateName = "write_" + newSymName;
	  State *writeState=new State(NULL,writeStateName,writeStateCases);
	  info->op->addState(writeState);
	  // warn("dismantleRegisterAccess_writes_map: write state\n"+*writeState);

	  // - modify present state  (truncation already done above; add goto)
	  StmtGoto *preGoto=new StmtGoto(NULL,writeState);
	  preGoto->setParent(info->stmt->getParent());
	  stmts->append(preGoto);	// - HACK: modifying stmt list directly
	  // warn("dismantleRegisterAccess_writes_map: residual state-case\n"+
	  //      *info->scase->getParent());

	  // - done with state-case  (process no stmts after this one)
	  info->done=true;
	  return true;		// - continue w/this stmt
	}
      }
      else if (((Stmt*)t)->getStmtKind()==STMT_BLOCK)
	return true;
      else
	return false; 
    }

    default:
    {
      return false;
    }
  }
}


void dismantleRegisterAccess_op (OperatorBehavioral *op)
{
  // - dismantle *op s.t. each register access (read/write)
  //     and each stream access (read history value / emit output value)
  //     happens in its own state.
  // - assumes state code is in basic-block form (-xc),
  //     with or without enclosing stmt block.

  // warn ("dismantleRegisterAccess_op: op "+op->getName());

  // - create working lists of all StateCases
  list<StateCase*> stateCases;
  State *s;
  forall (s,*op->getStates()) {
    StateCase *sc;
    forall (sc,*s->getCases())
      stateCases.append(sc);
  }

  while (!stateCases.empty()) {
    StateCase *sc=stateCases.pop();		// - dismantle reads in *sc
    // warn("dismantleRegisterAccess_op: state "+
    //      ((State*)sc->getParent())->getName());
    list<Stmt*> *stmts=sc->getStmts();
    if (stmts->length()==1 && stmts->head()->getStmtKind()==STMT_BLOCK)
      stmts=((StmtBlock*)stmts->head())->getStmts();
    Stmt *stmt;
    forall (stmt,*stmts) {
      // - dismantle reads in all stmts
      // warn ("dismantleRegisterAccess_op: read stmt "+stmt->toString());
      DismantleRegisterAccess_info info(op,&stateCases,sc,stmt);
      stmt->map(dismantleRegisterAccess_reads_map,(TreeMap)NULL,&info);
      if (info.done)
	break;	// forall (stmt,*stmts)
    }
    forall (stmt,*stmts) {
      // - dismantle writes in remaining stmts
      // warn ("dismantleRegisterAccess_op: write stmt "+stmt->toString());
      DismantleRegisterAccess_info info(op,&stateCases,sc,stmt);
      stmt->map(dismantleRegisterAccess_writes_map,(TreeMap)NULL,&info);
      if (info.done)
	break;	// forall (stmt,*stmts)
    }
  }	// while (!stateCases.empty())
}


bool dismantleRegStreamAccess_map (Tree *t, void *i)
{
  switch (t->getKind()) {
    case TREE_SUITE:	return true;
    case TREE_OPERATOR:	if (((Operator*)t)->getOpKind()==OP_BEHAVIORAL) {
			  OperatorBehavioral *op=(OperatorBehavioral*)t;
			  if (op->getStates()->size()==1)
			    op->setAnnote_(ANNOTE_WAS_SINGLE_STATE_OP,true);
			  dismantleStreamAccess_op  (op);
			  dismantleRegisterAccess_op(op);
			  op->link();	// - fix goto stmts since munged FSMs
			}
			return false;
    default:		return false;
  }
}


void dismantleRegStreamAccess (Tree *t)
{
  // - dismantle code s.t. each register access, stream access,
  //     and stream input happens in its own state.

  t->map(dismantleRegStreamAccess_map);

  checkGotos(t);
  removeDumbStates(t);
}
