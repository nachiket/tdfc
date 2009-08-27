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
// SCORE TDF compiler:  operator suites
// $Revision: 1.74 $
//
//////////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include <LEDA/core/string.h>
#include <LEDA/core/dictionary.h>
#include "operator.h"
#include "suite.h" // allow compare to be defined..
#include <LEDA/core/set.h> // allow compare to be defined..
#include "symbol.h"
#include "misc.h"
#include "gc.h"


////////////////////////////////////////////////////////////////
//  constructors

Suite::Suite (set<Operator*> *operators_i)
  : Tree(NULL,TREE_SUITE),
    symtab(new SymTab(SYMTAB_SUITE)),
    operators(operators_i ? operators_i : new set<Operator*>)
{
  symtab->setParent(this);
  symtab->setScope(NULL);
  Operator *op;
  forall(op,*operators)
  {
    symtab->addSymbol(new SymbolOp(op->getToken(),op));
    symtab->addChild(op->getSymtab());
    op->setParent(this);
  }
}


Suite::Suite (SymTab *symtab_i)
  : Tree(NULL,TREE_SUITE),
    symtab(symtab_i),
    operators(new set<Operator*>)
{
  symtab->setParent(this);
  symtab->setScope(NULL);
  assert(symtab->getSymtabKind()==SYMTAB_SUITE);
  dictionary<string,Symbol*> *symbols=symtab->getSymbols();
  Symbol *s;
  forall (s,*symbols)
  {
    Operator *op=((SymbolOp*)s)->getOp();
    op->setParent(this);
    operators->insert(op);
  }
}


////////////////////////////////////////////////////////////////
//  operator suite management

Operator* Suite::lookupOperator (const string &name) const
{
  SymbolOp *symOp = (SymbolOp*)symtab->lookup(name);
  return symOp ? symOp->getOp() : NULL;
}


void Suite::addOperator (Operator *op)
{
  assert(op);
  SymbolOp *sym=(SymbolOp*)symtab->lookup(op->getName());
  if (sym)
  {
    fatal(1, string("redefinition of operator \"")+op->getName()+"\"",
	  op->getToken());
  }
  else
  {
    symtab->addSymbol(new SymbolOp(op->getToken(),op));	// - adds child symtab
    symtab->addChild(op->getSymtab());
    op->setParent(this);
    operators->insert(op);
  }
}


void Suite::removeOperator(Operator *op)
{
  assert(op);
  SymbolOp *sym=(SymbolOp*)symtab->lookup(op->getName());
  if (sym)
  {
    symtab->removeSymbol(sym);
    symtab->removeChild(op->getSymtab());
    op->setParent(NULL);
    operators->del(op);
  }
  else
  {
    fatal(1, string("Cannot remove operator \"")+op->getName()+
	     string("\", not in suite"), op->getToken());
  }
}


////////////////////////////////////////////////////////////////
//  thread

void Suite::thread (Tree *p)
{
  setParent(p);
  symtab->thread(this);
  Operator *op;
  forall(op,*operators)
    op->thread(this);
}


////////////////////////////////////////////////////////////////
//  link

bool Suite::link ()
{
  bool linked = symtab->link();
  Operator *op;
  forall (op,*operators)
    linked = linked && op->link();
  return linked;
}


////////////////////////////////////////////////////////////////
//  typeCheck

Type* Suite::typeCheck ()
{
  symtab->typeCheck();
  Operator *op;
  forall (op,*operators)
  {
    op->typeCheck();		// - generate+check types
    if (gEnableGC)		// - gc waste from type generation
      TreeGC::gc();
  }
  return type_none;
}


////////////////////////////////////////////////////////////////
//  duplicate

Tree* Suite::duplicate () const
{
  Suite *s = new Suite(*this);
  s->setParent(NULL);
  s->symtab	= new SymTab(SYMTAB_SUITE);
  s->operators	= new set<Operator*>;
  Operator *op;
  forall (op,*operators)
  {
    Operator *opCopy=(Operator*)op->duplicate();
    opCopy->setParent(s);
    s->operators->insert(opCopy);
    s->symtab->addSymbol(new SymbolOp(opCopy->getToken(),opCopy));
    s->symtab->addChild(opCopy->getSymtab());
  }
  return s;
}


////////////////////////////////////////////////////////////////
//  toString

string Suite::toString () const
{
  string suiteStr;
  Operator *o;
  forall (o,*operators)
    suiteStr += o->toString() + "\n";
  return suiteStr;
}


////////////////////////////////////////////////////////////////
//  map

void Suite::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    symtab->map(pre,post,i);
    Operator *op;
    forall (op,*operators)
      op->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void Suite::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    set<Operator*> oldOps, newOps;
    Operator *op;
    forall (op,*operators)
    {
      Operator *origOp=op;
      op->map2((Tree**)&op,pre,post,i);
      if (op!=origOp)
      {
	oldOps.insert(origOp);
	newOps.insert(op);
      }
    }
    if (!newOps.empty())
    {
      forall (op,oldOps)
	removeOperator(op);
      forall (op,newOps)
	addOperator(op);
    }
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}

