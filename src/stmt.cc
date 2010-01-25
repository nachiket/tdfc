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
// SCORE TDF compiler:  statments
// $Revision: 1.139 $
//
//////////////////////////////////////////////////////////////////////////////


#include "parse.h"
#include "stmt.h"
#include "symbol.h"
#include "operator.h"
#include "state.h"

using leda::string;
using leda::list_item;

////////////////////////////////////////////////////////////////
//  constructors

Stmt::Stmt (Token *token_i, StmtKind stmtKind_i)
  : Tree(token_i,TREE_STMT),
    stmtKind(stmtKind_i)
{
}

StmtIf::StmtIf (Token *token_i,   Expr *cond_i,
		Stmt *thenPart_i, Stmt *elsePart_i)
  : Stmt(token_i,STMT_IF),
    cond(cond_i),
    thenPart(thenPart_i),
    elsePart(elsePart_i)
{
  cond    ->setParent(this);
  thenPart->setParent(this);
  if (elsePart)			// NULL elsePart is ok
    elsePart->setParent(this);
}


StmtGoto::StmtGoto (Token *token_i, const string &stateName_i)
  : Stmt(token_i,STMT_GOTO),
    stateName(stateName_i),
    state(NULL)
{
  // unlinked, target state name in stateName (leaves NULL state)
}


StmtGoto::StmtGoto (Token *token_i, State *state_i)
  : Stmt(token_i,STMT_GOTO),
    stateName(state_i!=NULL ? state_i->getName() : "<stay>"),
    state(state_i)
{
  
  // NULL state is unlinked "stay" with stateName "<stay>",
  //   changed to goto-enclosing-state during link
}


StmtCall::StmtCall (Token *token_i, ExprCall *call_i)
  : Stmt(token_i,STMT_CALL),
    call(call_i)
{
  call->setParent(this);
}


StmtBuiltin::StmtBuiltin (Token *token_i, ExprBuiltin *builtin_i)
  : Stmt(token_i,STMT_BUILTIN),
    builtin(builtin_i)
{
  builtin->setParent(this);
}


StmtAssign::StmtAssign (Token *token_i, ExprLValue *lvalue_i, Expr *rhs_i)
  : Stmt(token_i,STMT_ASSIGN),
    lvalue(lvalue_i),
    rhs(rhs_i),
    rhsnode(NULL)
{
  lvalue->setParent(this);
  rhs   ->setParent(this);
}

StmtAssign::StmtAssign (Token *token_i, ExprLValue *lvalue_i, node rhs_i)
  : Stmt(token_i,STMT_ASSIGN),
    lvalue(lvalue_i),
    rhs(NULL),
    rhsnode(rhs_i)
{
  lvalue->setParent(this);
}

void StmtAssign::setRhsnode(node rhs_i) {
	rhsnode=rhs_i;
}


StmtBlock::StmtBlock (Token *token_i, SymTab *symtab_i, list<Stmt*> *stmts_i)
  : Stmt(token_i,STMT_BLOCK),
    symtab(symtab_i),
    stmts(stmts_i)
{
  symtab->setParent(this);
  // symtab->setScope(NULL);	// HACK - oops, dissociation screws up parser
  Stmt *s;
  forall (s,*stmts)
    s->setParent(this);
}


////////////////////////////////////////////////////////////////
//  setParent

void Stmt::setParent (Tree *p)
{
  assert(   ( p==NULL )
	 || ( p->getKind()==TREE_STATECASE )
	 || ( p->getKind()==TREE_STMT &&
	      (((Stmt*)p)->getStmtKind()==STMT_BLOCK ||
	       ((Stmt*)p)->getStmtKind()==STMT_IF) )
	 || ( p->getKind()==TREE_OPERATOR &&
	      ((Operator*)p)->getOpKind()==OP_COMPOSE ) );
  parent=p;
}


////////////////////////////////////////////////////////////////
//  thread

void StmtIf::thread (Tree *p)
{
  setParent(p);
  cond->thread(this);
  thenPart->thread(this);
  if (elsePart)
    elsePart->thread(this);
}


void StmtGoto::thread (Tree *p)
{
  setParent(p);
}


void StmtCall::thread (Tree *p)
{
  setParent(p);
  call->thread(this);
}


void StmtBuiltin::thread (Tree *p)
{
  setParent(p);
  builtin->thread(this);
}


void StmtAssign::thread (Tree *p)
{
  setParent(p);
  lvalue->thread(this);
  rhs->thread(this);
}


void StmtBlock::thread (Tree *p)
{
  setParent(p);
  symtab->thread(this);
  Stmt *s;
  forall (s,*stmts)
    s->thread(this);
}


////////////////////////////////////////////////////////////////
//  link

bool StmtIf::link ()
{
  Operator *op=getOperator();
  if (!op || op->getOpKind()!=OP_BEHAVIORAL)
    fatal(1, string("if-statement permitted only in behavioral operator"),
	  token);

  bool linked = cond->link() && thenPart->link();
  if (elsePart)
    linked = linked && elsePart->link();
  return linked;
}


bool StmtGoto::link ()
{
  Operator *op=getOperator();
  if (!op || op->getOpKind()!=OP_BEHAVIORAL)
    fatal(1, string("goto-statement permitted only in behavioral operator"),
	  token);

  if (state==NULL)
  {
    if (stateName=="<stay>")
    {
      // - find own state for "stay"
      Tree *s;
      for (s=parent; s && s->getKind()!=TREE_STATE; s=s->getParent());
      if (s)
      {
	state     = (State*)s;
	stateName = state->getName();
      }
      else
	fatal(1, string("cannot find enclosing state during link"), token);
    }
    else
    {
      // - find state by name
      state = ((OperatorBehavioral *)op)->lookupState(stateName);
      if (state==NULL)
	fatal(1, string("state ")+stateName+" not found", token);
    }
  }
  else {
    // - relink target state
    //     (already pointing to a state, but it may not be correct
    //      if the stmtGoto was transplanted from another operator)
    string newStateName = state->getName();
    stateName = newStateName;
    state = ((OperatorBehavioral *)op)->lookupState(newStateName);
    if (state==NULL)
      fatal(1, string("state ")+stateName+" not found", token);
  }
  return true;
}


bool StmtCall::link ()
{
  return call->link();
}


bool StmtBuiltin::link ()
{
  Operator *op=getOperator();
  if (op->getOpKind()!=OP_BEHAVIORAL && !builtin->okInComposeOp())
  {
    OperatorBuiltin *op=(OperatorBuiltin*)builtin->getOp();
    fatal(1, string("this use of ")+op->getName()+
	     string(" permitted only in behavioral operator")+
	     (op->okInComposeOp() ? " (bad arguments)" : ""),
	  token);
  }
  return builtin->link();
}


bool StmtAssign::link ()
{
  return lvalue->link() && rhs->link();
}


bool StmtBlock::link ()
{
  Operator *op=getOperator();
  if (!op || op->getOpKind()!=OP_BEHAVIORAL)
    fatal(1,"statement block permitted only in behavioral operator",token);

  bool linked = symtab->link();
  Stmt *s;
  forall (s,*stmts)
    linked = linked && s->link();
  return linked;
}


////////////////////////////////////////////////////////////////
//  typeCheck

Type* StmtIf::typeCheck ()
{
  cond->typeCheck();
  Type *condType=cond->getType();
  if (condType->getTypeKind()!=TYPE_BOOL)
    fatal(1, string("incompatible type for \"if\" conditional, is ")+
	     condType->toString()+", should be boolean", cond->getToken());
  thenPart->typeCheck();
  if (elsePart)
    elsePart->typeCheck();
  return type_none;
}


Type* StmtGoto::typeCheck ()
{
  return type_none;
}


Type* StmtCall::typeCheck ()
{
  call->typeCheck();
  if (call->getType()->getTypeKind()!=TYPE_NONE)
    warn(string("ignoring return value of call to \"")+
	 call->getOp()->getName()+"\"", call->getToken());
  return type_none;
}


Type* StmtBuiltin::typeCheck ()
{
  builtin->typeCheck();
  if (builtin->getType()->getTypeKind()!=TYPE_NONE)
    warn(string("ignoring return value of builtin \"")+
	 builtin->getOp()->getName()+"\"", builtin->getToken());
  return type_none;
}


Type* StmtAssign::typeCheck ()
{
  Symbol *sym=lvalue->getSymbol();
  if (!lvalue->isLValue())
    fatal(1, string("cannot assign to \"")+sym->getName()+"\"",
	  lvalue->getToken());
  if (!rhs->isRValue())
    fatal(1, string("cannot assign from \"")+sym->getName()+"\"",
	  rhs->getToken());

  Type *rhsType     =    rhs->typeCheck();
  Type *lhsType     = lvalue->typeCheck();
  Type *upgradeType = rhsType->upgrade(lhsType);
  if (upgradeType==NULL)
  {
    if (   lhsType->getTypeKind()==rhsType->getTypeKind()
	&& lhsType->isSigned()   ==rhsType->isSigned() )
      // - allow assigning to narrower type
      // - WARNING - should sub-range from local, this violates auto-upgrade
      warn(string("assignment from type ")+rhsType->toString()+
	   " to type "+lhsType->toString()+" may lose information", token);
    else
      fatal(1, string("incompatible type for assignment, from type ")+
	       rhsType->toString()+", to "+lhsType->toString(), token);
  }
  return type_none;
}


Type* StmtBlock::typeCheck ()
{
  symtab->typeCheck();
  Stmt *s;
  forall (s,*stmts)
    s->typeCheck();
  return type_none;
}


////////////////////////////////////////////////////////////////
//  getBindTime  (stubs for now)

BindTime StmtIf::getBindTime () const		{ return BIND_UNKNOWN; }
BindTime StmtGoto::getBindTime () const		{ return BIND_UNKNOWN; }
BindTime StmtCall::getBindTime () const		{ return BIND_UNKNOWN; }
BindTime StmtBuiltin::getBindTime () const	{ return BIND_UNKNOWN; }
BindTime StmtAssign::getBindTime () const	{ return BIND_UNKNOWN; }
BindTime StmtBlock::getBindTime () const	{ return BIND_UNKNOWN; }


////////////////////////////////////////////////////////////////
//  getScope

SymTab* Stmt::getScope () const
{
  // - stmts at top level of operator are actually scoped inside
  //   "vars" symtab, not inside operator's top level symtab
  if (parent && parent->getKind()==TREE_OPERATOR)
  {
    if (((Operator*)parent)->getOpKind()==OP_BEHAVIORAL)
      return ((OperatorBehavioral*)parent)->getVars();
    if (((Operator*)parent)->getOpKind()==OP_COMPOSE)
      return ((OperatorCompose*)parent)   ->getVars();
  }
  return parent ? parent->getScope() : NULL;
}


////////////////////////////////////////////////////////////////
//  duplicate

Tree* StmtIf::duplicate () const
{
  StmtIf *s   = new StmtIf(*this);
  s->setParent(NULL);
  s->cond     = (Expr*)s->cond    ->duplicate();
  s->cond     ->setParent(s);
  s->thenPart = (Stmt*)s->thenPart->duplicate();
  s->thenPart ->setParent(s);
  if (s->elsePart) // 7/28/99 -- amd 
    {
      s->elsePart = (Stmt*)s->elsePart->duplicate();
      s->elsePart ->setParent(s);
    }
  return s;
}


Tree* StmtGoto::duplicate () const
{
  StmtGoto *s = new StmtGoto(*this);
  s->setParent(NULL);
  return s;
}


Tree* StmtCall::duplicate () const
{
  StmtCall *s = new StmtCall(*this);
  s->setParent(NULL);
  s->call = (ExprCall*)s->call->duplicate();
  s->call->setParent(s);
  return s;
}


Tree* StmtBuiltin::duplicate () const
{
  StmtBuiltin *s = new StmtBuiltin(*this);
  s->setParent(NULL);
  s->builtin = (ExprBuiltin*)s->builtin->duplicate();
  s->builtin->setParent(s);
  return s;
}


Tree* StmtAssign::duplicate () const
{
  StmtAssign *s = new StmtAssign(*this);
  s->setParent(NULL);
  s->lvalue = (ExprLValue*)s->lvalue->duplicate();
  s->lvalue ->setParent(s);
  s->rhs    = (Expr*)      s->rhs   ->duplicate();
  s->rhs    ->setParent(s);
  return s;
}


Tree* StmtBlock::duplicate() const
{
  StmtBlock *s = new StmtBlock(*this);
  s->setParent(NULL);
  s->symtab = (SymTab*)s->symtab->duplicate();
  s->symtab ->setParent(s);
  s->stmts  = new list<Stmt*>;
  Stmt *stmt;
  forall (stmt,*stmts)
  {
    Stmt *stmtCopy = (Stmt*)stmt->duplicate();
    stmtCopy->setParent(s);
    s->stmts->append(stmtCopy);
  }
  return s;
}


////////////////////////////////////////////////////////////////
//  toString


string StmtIf::toString	() const
{
  indentPush();
  string condStr = cond->toString();
  string thenStr = thenPart->toString();
  string elseStr = elsePart ? elsePart->toString() : string();
  indentPop();
  return indent + "if (" + condStr + ")\n" + thenStr +
	 (elsePart ? (indent + "else\n" + elseStr) : string());
}


string StmtGoto::toString () const
{
  // - identify "goto my-own-state" as "stay"
  Tree *s;
  for (s=parent; s && s->getKind()!=TREE_STATE; s=s->getParent());
  if (state==NULL)
    return indent + "goto <unknown>;\n";	// unlinked
  else if (state==s)
    return indent + "stay;\n";
  else      
    return indent + "goto " + state->getName() + ";\n";
}


string StmtCall::toString () const
{
  return indent + call->toString() + ";\n";
}


string StmtBuiltin::toString () const
{
  return indent + builtin->toString() + ";\n";
}


string StmtAssign::toString () const
{
	if(rhs!=(Expr*)0) {
		return indent + lvalue->toString() + "=" + rhs->toString() + ";\n";
	} else {
		return indent + lvalue->toString() + "(RHSNODE);\n";
	}
}


string StmtBlock::toString () const
{
  indentPush();
  string varsStr = symtab->toString();
  string stmtsStr;
  Stmt *s;
  forall (s,*stmts)
    stmtsStr += s->toString();
  indentPop();
  return indent + "{\n" + varsStr + stmtsStr + indent + "}\n";
}


////////////////////////////////////////////////////////////////
//  map

void StmtIf::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    cond->map(pre,post,i);
    thenPart->map(pre,post,i);
    if (elsePart)
      elsePart->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void StmtIf::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    cond    ->map2((Tree**)&cond,    pre,post,i);
    thenPart->map2((Tree**)&thenPart,pre,post,i);
    if (elsePart)
      elsePart->map2((Tree**)&elsePart,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void StmtGoto::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    // do NOT map target state
  }
  if (post)
    post(this,i);
}


void StmtGoto::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    // do NOT map target state
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void StmtCall::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    call->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void StmtCall::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    call->map2((Tree**)&call,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void StmtBuiltin::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    builtin->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void StmtBuiltin::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    builtin->map2((Tree**)&builtin,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void StmtAssign::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    lvalue->map(pre,post,i);
    rhs->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void StmtAssign::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    lvalue->map2((Tree**)&lvalue,pre,post,i);
    rhs   ->map2((Tree**)&rhs,   pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void StmtBlock::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    symtab->map(pre,post,i);
    Stmt *stmt;
    forall (stmt,*stmts)
      stmt->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void StmtBlock::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    symtab->map2((Tree**)&symtab,pre,post,i);
    list_item it;
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

  string stmtkindToString(StmtKind t) {
          switch(t) {
  		case STMT_IF: return string("STMT_IF");
  		case STMT_GOTO: return string("STMT_GOTO");
  		case STMT_CALL: return string("STMT_CALL");
  		case STMT_BUILTIN: return string("STMT_BUILTIN");
  		case STMT_ASSIGN: return string("STMT_ASSIGN");
  		case STMT_BLOCK: return string("STMT_BLOCK");
  		default: return string("bs");
  	}
  }

