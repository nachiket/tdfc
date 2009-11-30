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
// SCORE TDF compiler:  states
// $Revision: 1.142 $
//
//////////////////////////////////////////////////////////////////////////////


#include "parse.h"
#include "state.h"
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "blockdfg.h" // Added by Nachiket on 11/29/2009

using leda::list_item;
using std::cout;
using std::cerr;
using std::endl;

////////////////////////////////////////////////////////////////
//  constructors

State::State (Token *token_i, const string &name_i,
	      set<StateCase*> *cases_i)
  : Tree(token_i,TREE_STATE),
    name(name_i),
    cases(cases_i ? cases_i : new set<StateCase*>)
{
  StateCase *c;
  forall (c,*cases)
    c->setParent(this);

  // - pedantic: should call State::addCase(c) / associateScopes()
}


StateCase::StateCase (Token *token_i, list<InputSpec*> *inputs_i,
		      list<Stmt*> *stmts_i)
  : Tree(token_i,TREE_STATECASE),
    inputs(inputs_i),
    stmts(stmts_i)
{
  InputSpec *i;
  forall (i,*inputs)
    i->setParent(this);
  Stmt *s;
  forall (s,*stmts)
    s->setParent(this);
}


InputSpec::InputSpec (Token *token_i, Symbol *stream_i,
		      Expr  *numTokens_i, bool eos_i, bool eofr_i)
  : Tree(token_i,TREE_INPUTSPEC),
    stream(stream_i),
    numTokens(numTokens_i ? numTokens_i : (Expr*)expr_1->duplicate()),
    eos(eos_i),
    eofr(eofr_i)
{
  assert(stream);		// typeCheck() verifies it is an input-stream 
  numTokens->setParent(this);
  if (eos)
    assert(numTokens->equals((Expr*)expr_1));
}


////////////////////////////////////////////////////////////////
//  setParent

void State::setParent (Tree *p)
{
  assert(    ( p==NULL )
	  || ( p->getKind()==TREE_OPERATOR &&
	       ((Operator*)p)->getOpKind()==OP_BEHAVIORAL ) );
  parent=p;
}


void StateCase::setParent (Tree *p)
{
  assert(p==NULL || p->getKind()==TREE_STATE);
  parent=p;
}


void InputSpec::setParent (Tree *p)
{
  assert(p==NULL || p->getKind()==TREE_STATECASE);
  parent=p;
}


////////////////////////////////////////////////////////////////
//  thread

void State::thread (Tree *p)
{
  setParent(p);
  StateCase *c;
  forall (c,*cases)
    c->thread(this);
}


void StateCase::thread (Tree *p)
{
  setParent(p);
  InputSpec *i;
  forall (i,*inputs)
    i->thread(this);
  Stmt *s;
  forall (s,*stmts)
    s->thread(this);
}


void InputSpec::thread (Tree *p)
{
  setParent(p);
  numTokens->thread(this);
}


////////////////////////////////////////////////////////////////
//  link

bool State::link ()
{
  bool linked=true;
  StateCase *c;
  forall (c,*cases)
    linked = linked && c->link();
  return linked;
}


bool StateCase::link ()
{
  bool linked=true;
  InputSpec *i;
  forall (i,*inputs)
    linked = linked && i->link();
  Stmt *s;
  forall (s,*stmts)
    linked = linked && s->link();
  return linked;
}


bool InputSpec::link ()
{
  // - first, find symbol in scope
  Operator *op     = getOperator();
  SymTab   *symtab = op ? op->getSymtab() : NULL;   // formals, NOT vars symtab
  if (symtab)
  {
    Symbol *visibleSym = symtab->lookup(stream->getName());
    if (visibleSym)
      stream=visibleSym;
    else
    {
      Operator	*op_ispec  =         getOperator(),
		*op_stream = stream->getOperator();
      fatal(1, string("symbol \"")+stream->getName()+"\" not found"+
	       ", in state-signature from operator \""+
	       (op_ispec?op_ispec->getName():string("<none>"))+
	       string("\" (0x%p), ",op_ispec)+
	       "symbol from operator \""+
	       (op_stream?op_stream->getName():string("<none>"))+
	       string("\" (0x%p)",op_stream),
	    token);
    }
  }
  return stream->link() && numTokens->link();
}


////////////////////////////////////////////////////////////////
//  typeCheck

Type* State::typeCheck ()
{
  StateCase *c;
  forall (c,*cases)
    c->typeCheck();
  return type_none;
}


Type* StateCase::typeCheck ()
{
  InputSpec *i;
  forall (i,*inputs)
    i->typeCheck();
  Stmt *s;
  forall (s,*stmts)
    {
      s->typeCheck();
      if (gDeleteType)
	rmType(s);
    }
  return type_none;
}


Type* InputSpec::typeCheck ()
{
  stream->typeCheck();
  if (   stream->getSymKind()!=SYMBOL_STREAM
      || ((SymbolStream*)stream)->getDir()!=STREAM_IN)
    fatal(1, string("state input \"")+stream->getName()+
	     string("\" is not an input stream"), token);
  numTokens->typeCheck();
  return type_none;
}


////////////////////////////////////////////////////////////////
//  state+case managment

void State::addCase (StateCase *case_i)
{
  assert(case_i);
  cases->insert(case_i);
  case_i->setParent(this);

  // - pedantic:  make all local symtabs children of state's scope
  SymTab *scope=getScope();
  if (scope)
    associateScopes(case_i,scope);
}


void State::removeCase (StateCase *case_i)
{
  // - warning - does NOT chase down ExprGoto nodes
  //           - removing all cases from a state is presently allowed

  assert(case_i);
  cases->del(case_i);
  case_i->setParent(NULL);
  // if (cases->empty())
  //   warn(string("state \"")+name+"\" now has no state-cases");

  // - pedantic:  dissociate all local symtabs from state's scope
  SymTab *scope=getScope();
  if (scope)
    dissociateScopes(case_i,scope);
}


bool State::setName (const string &newName)
{
  // - warning - does NOT chase down ExprGoto nodes;
  //             they will point to the correct renamed state
  //             but retain a local copy of the old name

  OperatorBehavioral *op=(OperatorBehavioral*)parent;
  if (op)
  {
    if (op->lookupState(newName))
      return false;
    op->removeState(this);
    name=newName;
    op->addState(this);
    return true;
  }
  else
  {
    name=newName;
    return true;
  }
}

// Added by Nachiket on 11/29/2009 to remember the dataflow graph extracted per state case
void StateCase::addDataflowGraph(BlockDFG dfg_i) {
	if(&dfg_i==NULL) {
		cerr << "Cannot accept a NULL dataflow graph" << endl;
	}
	dfg=dfg_i;
}

void StateCase::addInput (InputSpec *ispec_i)
{
  inputs->append(ispec_i);
  ispec_i->setParent(this);
}


////////////////////////////////////////////////////////////////
//  getBindTime  (stubs for now)

BindTime State::getBindTime () const	    { return BindTime(BIND_UNKNOWN); }
BindTime StateCase::getBindTime () const    { return BindTime(BIND_UNKNOWN); }
BindTime InputSpec::getBindTime () const    { return BindTime(BIND_UNKNOWN); }


////////////////////////////////////////////////////////////////
//  getScope

SymTab* State::getScope () const
{
  // - states at top level of operator are actually scoped inside
  //   "vars" symtab, not inside operator's top level symtab
  if (parent && parent->getKind()==TREE_OPERATOR)
  {
    if (((Operator*)parent)->getOpKind()==OP_BEHAVIORAL)
      return ((OperatorBehavioral*)parent)->getVars();
    // if (((Operator*)parent)->getOpKind()==OP_COMPOSE)
    //   return ((OperatorCompose*)parent)   ->getVars();
  }
  return parent ? parent->getScope() : NULL;
}


////////////////////////////////////////////////////////////////
//  duplicate

Tree* State::duplicate () const
{
  State *s	= new State(*this);
  s->setParent(NULL);
  s->cases	= new set<StateCase*>;
  StateCase *c;
  forall (c,*cases)
  {
    StateCase *cCopy = (StateCase*)c->duplicate();
    cCopy->setParent(s);
    s->cases->insert(cCopy);
  }
  return s;
}


Tree* StateCase::duplicate () const
{
  StateCase *c	= new StateCase(*this);
  c->setParent(NULL);
  c->inputs	= new list<InputSpec*>;
  InputSpec *i;
  forall (i,*inputs)
  {
    InputSpec *iCopy = (InputSpec*)i->duplicate();
    iCopy->setParent(c);
    c->inputs->append(iCopy);
  }
  c->stmts	= new list<Stmt*>;
  Stmt *m;
  forall (m,*stmts)
  {
    Stmt *mCopy = (Stmt*)m->duplicate();
    mCopy->setParent(c);
    c->stmts->append(mCopy);
  }
  return c;
}


Tree* InputSpec::duplicate () const
{
  InputSpec *i	= new InputSpec(*this);
  i->setParent(NULL);
  i->stream	= (SymbolStream*)i->stream   ->duplicate();	// - see below
  i->numTokens	=         (Expr*)i->numTokens->duplicate();
  i->numTokens->setParent(i);
  return i;

  // - warning:  stream symbol is duplicated but has no scope.
  //             must link() InputSpec later to fix symbol reference.
}


////////////////////////////////////////////////////////////////
//  toString

string State::toString () const
{
  string     casesStr;
  int        casesDone=0;
  StateCase *c;
  forall (c,*cases)
    casesStr += string(casesDone++?"\n":"") + c->toString();
  return casesStr;
}


string printStateCaseHead (const StateCase *sc)
{
  // - print state-case header "state foo (...)"  (name + input sig)

  string inputsStr;
  const list<InputSpec*> *inputs=sc->getInputs();
  if (!inputs->empty()) {
    list_item i=inputs->first();
    inputsStr = inputs->inf(i)->toString();
    while ((i=inputs->succ(i)))
      inputsStr += string(",") + inputs->inf(i)->toString();
  }

  Tree *parent=sc->getParent();
  string nameStr = parent ? ((State*)parent)->getName() : string("<unknown>");

  return "state " + nameStr + " (" + inputsStr + ")";
}


string StateCase::toString () const
{
  string sigStr = printStateCaseHead(this);
  indentPush();
  string stmtsStr;
  Stmt *s;
  forall (s,*stmts)
    stmtsStr += s->toString();
  indentPop();

  return indent + sigStr + " :\n" + stmtsStr;
}


string InputSpec::toString () const
{
  string numStr  = (eos || numTokens->equals((Expr*)expr_1))
		     ? string()
		     : (string("#")+numTokens->toString());
  string eosPre  = eos ? string("eos(") : string();
  string eosPost = eos ? string(")")    : string();
  return eosPre + stream->getName() + numStr + eosPost;
}


////////////////////////////////////////////////////////////////
//  map

void State::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    StateCase *c;
    forall (c,*cases)
      c->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void State::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		  void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    set<StateCase*> oldCases, newCases;
    StateCase *c;
    forall (c,*cases)
    {
      StateCase *cOrig=c;
      c->map2((Tree**)&c,pre,post,i);
      if (c!=cOrig)
      {
	oldCases.insert(cOrig);
	newCases.insert(c);
      }
    }
    if (!newCases.empty())
    {
      forall(c,oldCases)
	removeCase(c);
      forall(c,newCases)
	addCase(c);
    }
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void StateCase::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    InputSpec *input;
    forall (input,*inputs)
      input->map(pre,post,i);
    Stmt *stmt;
    forall (stmt,*stmts)
      stmt->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void StateCase::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		      void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    list_item it;
    forall_items (it,*inputs)
    {
      InputSpec *input=inputs->inf(it);
      input->map2((Tree**)&input,pre,post,i);
      inputs->assign(it,input);
    }
    forall_items (it,*stmts)
    {
      Stmt *stmt=stmts->inf(it);
      stmt->map2((Tree**)&stmt,pre,post,i);
      stmts->assign(it,stmt);
    }
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void InputSpec::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    numTokens->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void InputSpec::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		      void *i, bool skipPre)
{
  assert(this==*h);
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  bool descend=true;
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this!=*h)
    {
      (*h)->setParent(p);
      if (descend)
      {
	(*h)->map2(h,pre,post,i,true);
	return;
      }
    }
  }
  if (descend)
  {
    numTokens->map2((Tree**)&numTokens,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}

