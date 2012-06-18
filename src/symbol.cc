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
// SCORE TDF compiler:  symbols
// $Revision: 1.141 $
//
//////////////////////////////////////////////////////////////////////////////


#include <assert.h>

#include "parse.h"
#include "symbol.h"
#include "operator.h"
#include "expr.h"
#include "misc.h"
#include "gc.h"

using leda::dic_item;

////////////////////////////////////////////////////////////////
//  pre-defined symbols

const SymbolVar *sym_none = NULL;	// for unlinked syms & non-valued trees


////////////////////////////////////////////////////////////////
//  init

void Symbol::init ()
{
  sym_none = new SymbolVar(NULL, string("<none>"), new Type(TYPE_NONE));
  TreeGC::addRoot(sym_none);
}


////////////////////////////////////////////////////////////////
//  constructors

SymTab::SymTab (SymTabKind symtabKind_i,
		dictionary<string,Symbol*> *symbols_i,
		list<Symbol*> *symbolOrder_i,
		set<SymTab*> *children_i)
  : Tree(NULL,TREE_SYMTAB),
    symtabKind(symtabKind_i),
    symbols(symbols_i ? symbols_i : new dictionary<string,Symbol*>),
    symbolOrder(symbolOrder_i ? symbolOrder_i : new list<Symbol*>),
    children(children_i ? children_i : new set<SymTab*>),
    scope(NULL)
{
  Symbol *s;
  forall (s,*symbols)
    s->setParent(this);

  // - complete *symbolOrder,
  //     so it contains all the symbols in *symbols, but no others
  forall (s,*symbols)
    if (!symbolOrder_i || !symbolOrder->rank(s))
      symbolOrder->append(s);
  assert(symbolOrder->size()==symbols->size());

  assert(children);
  SymTab *t;
  forall (t,*children)
    t->setScope(this);
}


/*
SymTab::~SymTab ()
{
  // dissociate all parent/child pointers
  if (scope)
    scope->removeChild(this);
  SymTab *c;
  forall (c,*children)
    if (c->scope==this)		// dissociate child only if it still thinks
      c->scope=NULL;		//  this is its parent (may have been xfered)
  dic_item i;
  forall_items (i,*symbols)
  {
    Symbol *s=symbols->inf(i);
    if (s->getParent()==this)	// dissociate sym only if it still thinks
      s->setParent(NULL);	//  this is its parent (may have been xfered)
  }
}
*/


Symbol::Symbol (Token *token_i, SymbolKind symKind_i,
		const string &name_i, Type *type_i)
  : Tree(token_i,TREE_SYMBOL),
    symKind(symKind_i),
    name(name_i),
    type(type_i)
{
  type->setParent(this);
}

void Symbol::setStreamValid() {
 symKind=SYMBOL_STREAM_VALID;
}

SymbolStream::SymbolStream (Token *token_i, const string &name_i,
			    Type *type_i,   StreamDir dir_i, string low, string high)
  : Symbol(token_i,SYMBOL_STREAM,name_i,type_i),
    dir(dir_i)
{
	if (low != "" && high != "")
	{
		//cout << "range = low : " << low << " \t high : " << high   << endl;
		range = "[" + low + "," + high + "]";
		range_left = low;
		range_right = high;
	}
	else {
		range = "";
		range_left = "";
		range_right = "";
	}
}


SymbolVar::SymbolVar (Token *token_i, const string &name_i,
		      Type *type_i, Expr *value_i, Expr *depth_i)
  : Symbol(token_i,SYMBOL_VAR,name_i,type_i),
    value(value_i),
    depth(depth_i)
{
  if (value)
    value->setParent(this);
  if (depth)
    depth->setParent(this);
}

SymbolVar::SymbolVar (Token *token_i, const string &name_i, 
						Type *type_i,  string num)
	: Symbol(token_i,SYMBOL_VAR,name_i,type_i)
{
	number = num;
	cout << "number = " << num << endl;
}

SymbolOp::SymbolOp (Token *token_i, Operator *op_i)
  : Symbol(token_i,SYMBOL_OP,op_i->getName(),
	   (Type*)op_i->getType()->duplicate()),
    op(op_i)
{
  assert(op);
}


////////////////////////////////////////////////////////////////
//  setParent

void Symbol::setParent (Tree *p)
{
  assert(p==NULL || p->getKind()==TREE_SYMTAB);
  parent=p;
}


////////////////////////////////////////////////////////////////
//  thread

void SymTab::thread (Tree *p)
{
  setParent(p);
  dic_item i;
  forall_items (i,*symbols)
    symbols->inf(i)->thread(this);
  // - child symtabs thread thru scope, not parent.  rethreading is redundant
  // SymTab *c;
  // forall (c,*children)
  //   c->setScope(this);
}

void SymbolStream::thread (Tree *p)
{
  setParent(p);
  type->thread(this);
}


void SymbolVar::thread (Tree *p)
{
  setParent(p);
  type->thread(this);
  if (value)
    value->thread(this);
  if (depth)
    depth->thread(this);
}

void SymbolOp::thread (Tree *p)
{
  setParent(p);
  type->thread(this);
  // do NOT thread op
}


////////////////////////////////////////////////////////////////
//  link

bool SymTab::link ()
{
  // - link to enclosing symtab (scope, but getScope() would be wrong)
  if (parent)
  {
    Tree *pp;
    if (   (parent->getKind()==TREE_OPERATOR)
	&& (   (   ((Operator*)parent)->getOpKind()==OP_BEHAVIORAL
		&& ((OperatorBehavioral*)parent)->getVars()==this)
	    || (   ((Operator*)parent)->getOpKind()==OP_COMPOSE
		&& ((OperatorCompose*)parent)->getVars()==this)))
      // - this is vars from operator,
      // - parent->getScope() is suite symtab, get operator symtab instead
      scope=((Operator*)parent)->getSymtab();
    else if ((pp=parent->getParent()))
      // - this is embedded symtab,
      // - parent->getScope() is this, bypass to get grandparent's scope
      scope=pp->getScope();
    else
      scope=NULL;
  }
  if (scope)
  {
    set<SymTab*> *scopesChildren = scope->getChildren();
    if (!scopesChildren->member(this))
      scope->addChild(this);
  }

  // - link components
  bool linked=true;
  dic_item i;
  forall_items (i,*symbols)
    linked = linked && symbols->inf(i)->link();
  /*
   * do not link child symtabs, done thru subtree later
   *
  SymTab *c;
  forall (c,*children)
    linked = linked && c->link();
  */
  return linked;
}


bool SymbolStream::link ()
{
  return type->link();
}


bool SymbolVar::link ()
{
  bool linked = type->link();
  if (value)
  {
    Operator *op=getOperator();
    if (op && op->getOpKind()==OP_COMPOSE		// - is in compos-op
	   && ((OperatorCompose*)op)->getVars()==parent // - is a declared var
	&& type->getTypeKind()!=TYPE_ARRAY)		// - is a stream
      // - in compositional op, allow initialization of arrays, not of streams
      fatal(1, string("initial value not permitted for streams"),
	       value->getToken());
    linked = linked && value->link();
  }
  if (depth)
  {
    Operator *op=getOperator();
    if (op && op->getOpKind()!=OP_COMPOSE)
      fatal(1, string("depth hint permitted only for local stream "
		      "declarations inside compositional operator"),
	    depth->getToken());
    linked = linked && depth->link();
  }
  return linked;
}


bool SymbolOp::link ()
{
  // - HACK - do not link type, since its symbols lack an environment
  //        - this should be revisited when implementing scoped operators
  // return type->link();
  return true;

  // do NOT link op (Suite's job)
}


////////////////////////////////////////////////////////////////
//  typeCheck

Type* SymTab::typeCheck ()
{
  dic_item i;
  forall_items (i,*symbols)
    symbols->inf(i)->typeCheck();
  SymTab *c;
  forall (c,*children)
    c->typeCheck();
  return type_none;
}


Type* SymbolStream::typeCheck ()
{
  if (isArray())
    fatal(1, string("cannot declare array of streams"), token);
  return type->typeCheck();
}


Type* SymbolVar::typeCheck ()
{
  type->typeCheck();
  if (value)
  {
    Type *t = value->typeCheck();
    if (!t->upgradable(type))
    {
      if (   t->getTypeKind()==type->getTypeKind()
	  && t->isSigned()   ==type->isSigned())
	warn(string("initial value assignment of type ")+t->toString()+
	     " to symbol \""+name+"\" of type "+type->toString()+
	     " may lose information", value->getToken());
      else
	fatal(1, string("incompatible type for initial value of \"")+name+
	      string("\", is ")+t->toString()+", should be "+type->toString(),
	      value->getToken());
    }
  }
  if (depth)
  {
    Type *t = depth->typeCheck();
    if (t->getTypeKind()!=TYPE_INT)
      fatal(1, string("depth hint for a stream must be an integer type"),
	    depth->getToken());
  }
  return type;
}


Type* SymbolOp::typeCheck ()
{
  // - HACK - do not type-check type, since its symbols lack an environment
  //        - this should be revisited when implementing scoped operators
  // return type->typeCheck();
  return type;

  // do NOT type-check operator (Suite's job)
}


////////////////////////////////////////////////////////////////
//  symbol table management

void SymTab::addChild (SymTab *child_i)
{
  assert(child_i);

  if (children->member(child_i))
    fatal(1, string("Cannot insert child symtab, already present"), NULL);
  else
  {
    children->insert(child_i);
    child_i->setScope(this);
  }
}


void SymTab::removeChild (SymTab *child_i)
{
  assert(child_i);

  if (children->member(child_i))
  {
    children->del(child_i);
    child_i->setScope(NULL);
  }
  else
    fatal(1, string("Cannot remove child symtab, not found"), NULL);
}

using std::cout;
using std::endl;

void SymTab::addSymbol (Symbol *sym_i)
{
  assert(sym_i);

  string symName = sym_i->getName();

  if ((symtabKind==SYMTAB_SUITE) != (sym_i->getSymKind()==SYMBOL_OP))
  {
    if (symtabKind==SYMTAB_SUITE)
      fatal(1, string("Cannot insert non-operator symbol \"")+symName+
	       "\" into suite symbol table", NULL);
    else
      fatal(1, string("Cannot insert operator symbol \"")+symName+
	       "\" into non-suite symbol table", NULL);
  }
  else if ((symtabKind==SYMTAB_BLOCK) && (sym_i->getSymKind()==SYMBOL_STREAM))
    fatal(1, string("Cannot insert stream symbol \"")+symName+
	     "\" into block symbol table", NULL);

// 8/8/2011 Put to rest speculation.. LEDA 6.3 vs 6.2 it is!
// Nachiket's debugging of dictionary!!
//cout << "Attempting to add name " << symName << " for symbol " << sym_i->toString() <<  " with pointer " << symbols << endl;
//cout << "Symbol size=" << symbols->size() << endl;
//if(symName=="<none>") {
//	cout << "null being added?" << endl;
//}

  dic_item i=symbols->lookup(symName);
  if (i)
    fatal(1, string("redefinition of symbol \"")+symName+"\"", NULL);
  else
  {
    symbols->insert(string(symName),sym_i);
    symbolOrder->append(sym_i);
    sym_i->setParent(this);
    // do NOT add SymbolOp's symtab as child (done elsewhere if necessary)
  }
}


void SymTab::removeSymbol (Symbol *sym_i)
{
  assert(sym_i);

  string symName = sym_i->getName();
  dic_item i=symbols->lookup(symName);
  if (i)
  {
    symbols->del_item(i);
    symbolOrder->remove(sym_i);
    sym_i->setParent(NULL);
    // do NOT remove SymbolOp's symtab as child
  }
  else
    fatal(1, string("Cannot delete Symbol \"")+symName+
	     "\" since it is not in this symtab", NULL);
}


Symbol* SymTab::lookup (const string &name) const
{
  dic_item i=symbols->lookup(name);
  if (i)					// search here
    return symbols->inf(i);
  else if (scope)				// search in ancestors
    return scope->lookup(name);
  else						// give up
    return NULL;
}


bool Symbol::setName (const string &newName)
{
  // - names/renames a symbol, updating its parent symtab
  // - returns true on success, false on name conflict
  // - note, renaming may expose/hide names from outer scopes

  SymTab *symtab = (SymTab*)parent;
  if (symtab)
  {
    Symbol *existingSym=symtab->lookup(newName);
    if (existingSym && existingSym->getParent()==symtab)
      return false;
    else
    {
      // - warning: direct access to symtab's symbol dict is dangerous
      // dictionary<string,Symbol*> *symbols = symtab->getSymbols();
      // symbols->del(name);
      // name=newName;
      // symbols->insert(newName,this);

      // - safer but slower:
      symtab->removeSymbol(this);
      name=newName;
      symtab->addSymbol(this);
      return true;
    }
  }
  else
  {
    name=newName;
    return true;
  }
}


bool SymbolOp::setName (const string &newName)
{
  // - let operator rename itself (will call Symbol::setName())
  return op->setName(newName);
}


////////////////////////////////////////////////////////////////

bool Symbol::isReg () const
{
  Operator *op=getOperator();
  return    symKind==SYMBOL_VAR
         && op
         && op->getOpKind()==OP_BEHAVIORAL
	 && parent
	 && parent==((OperatorBehavioral*)op)->getVars();
}


bool Symbol::isLocal () const
{
  return    symKind==SYMBOL_VAR
	 && parent
	 && (   (   ((SymTab*)parent)->getSymtabKind()==SYMTAB_BLOCK
		 && !isReg() )
	     || (   ((SymTab*)parent)->getSymtabKind()==SYMTAB_OP
		 && getOperator()->getOpKind()        ==OP_COMPOSE   ) );
}


bool Symbol::isParam () const
{
  Operator *op=getOperator();
  return    symKind==SYMBOL_VAR
	 && op
	 && parent
	 && parent==op->getSymtab();
}


bool Symbol::isArray () const
{
  return    symKind==SYMBOL_VAR
         && type->getTypeKind()==TYPE_ARRAY;
}


////////////////////////////////////////////////////////////////
//  iterators

void SymTab::foreach_sym (void (*f)(Symbol *s, void *i), void *i,
			  bool preorder,  bool descend)
{
  // fill me
  fatal(1,"SymTab::foreach_sym() not implemented",token);
}


////////////////////////////////////////////////////////////////
//  equals

bool Symbol::equals (const Symbol *s) const
{
  if (this==s)					// exact match
    return true;
  if (   symKind==s->symKind			// match dummy symbol "<none>"
      &&    type->getTypeKind()==TYPE_NONE
      && s->type->getTypeKind()==TYPE_NONE)
    return true;
  return false;
}


////////////////////////////////////////////////////////////////
//  getBindTime  (stubs for now)

BindTime SymTab::getBindTime   	   () const  { return BindTime(BIND_UNKNOWN); }
BindTime SymbolStream::getBindTime () const  { return BindTime(BIND_UNKNOWN); }
BindTime SymbolVar::getBindTime	   () const  { return BindTime(BIND_UNKNOWN); }
BindTime SymbolOp::getBindTime	   () const  { return BindTime(BIND_UNKNOWN); }


////////////////////////////////////////////////////////////////
//  duplicate

Tree* SymTab::duplicate () const
{
  SymTab *s = new SymTab(*this);
  s->setParent(NULL);
  s->setScope(NULL);

  s->children = new set<SymTab*>;
  /*
   * do not duplicate child symtabs, they are duped elsewhere, added by link()
   *
  SymTab *c;
  forall (c,*children)
  {
    SymTab *cCopy = (SymTab*)c->duplicate();
    cCopy->setScope(s);
    s->children->insert(cCopy);
  }
  */

  /*
   * the following code is from before symbolOrder existed
   *
  s->symbols = new dictionary<string,Symbol*>;
  dic_item i;
  forall_items (i,*symbols)
  {
    Symbol *sym     = symbols->inf(i),
           *symCopy = (Symbol*)sym->duplicate();
    symCopy->setParent(s);
    s->symbols->insert(sym->getName(),symCopy);
  }
  */

  s->symbols     = new dictionary<string,Symbol*>;
  s->symbolOrder = new list<Symbol*>;
  Symbol *sym;
  forall (sym,*symbolOrder) {
    Symbol *symCopy = (Symbol*)sym->duplicate();
    symCopy->setParent(s);
    s->symbols->insert(sym->getName(),symCopy);
    s->symbolOrder->append(symCopy);
  }

  // - sanity check:  was symbolOrder complete?
  if (s->symbols->size() != symbols->size())
    assert(!"internal error, incomplete symbol order while duplicating symtab");

  return s;
}


Tree* SymbolStream::duplicate () const
{
  SymbolStream *s = new SymbolStream(*this);
  s->setParent(NULL);
  s->type = (Type*)s->type->duplicate();
  s->type->setParent(s);
  return s;
}


Tree* SymbolVar::duplicate () const
{
  SymbolVar *s = new SymbolVar(*this);
  s->setParent(NULL);
  s->type = (Type*)s->type->duplicate();
  s->type->setParent(s);
  if (value)
  {
    s->value = (Expr*)s->value->duplicate();
    s->value->setParent(s);
  }
  if (depth)
  {
    s->depth = (Expr*)s->depth->duplicate();
    s->depth->setParent(s);
  }
  return s;
}


Tree* SymbolOp::duplicate () const
{
  SymbolOp *s = new SymbolOp(*this);
  s->setParent(NULL);
  s->type = (Type*)s->type->duplicate();
  s->type->setParent(s);
  s->op = (Operator*)s->op->duplicate();
  return s;
}


////////////////////////////////////////////////////////////////
//  toString

string SymTab::toString	() const
{
  string symsStr;
  Symbol *s;
  forall (s,*symbolOrder)
    symsStr += indent + s->declToString() + ";\n";
  return symsStr;
}


string SymbolStream::declToString () const
{
  return ( dir==STREAM_IN  ? "input "  :
	   dir==STREAM_OUT ? "output " : "stream " ) +
	 type->toString() + " " + name;
}


string SymbolVar::declToString () const
{
  if (isParam())
    return "param " + type->toString() + " " + name;
  else
    return type->toString() + " " + name +
	   (depth ? (string("(")+depth->toString())+string(")") : string()) +
	   (value ? (string("=")+value->toString())             : string());
}


string SymbolOp::declToString () const
{
  return op->declToString();
}


////////////////////////////////////////////////////////////////
//  map

void SymTab::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    Symbol *sym;
    forall (sym,*symbolOrder)
      sym->map(pre,post,i);

    // - do NOT map over child symtabs, they are not direct AST descendants
    // SymTab *child;
    // forall (child,*children)
    //   child->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void SymTab::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    set<Symbol*> oldSyms, newSyms;
    Symbol *sym;
    forall (sym,*symbolOrder)
    {
      Symbol *symOrig=sym;
      sym->map2((Tree**)&sym,pre,post,i);
      if (sym!=symOrig)
      {
	oldSyms.insert(symOrig);
	newSyms.insert(sym);
      }
    }
    if (!newSyms.empty())
    {
      forall (sym,oldSyms)
	removeSymbol(sym);
      forall (sym,newSyms)
	addSymbol(sym);
    }
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void SymbolStream::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    type->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void SymbolStream::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    type->map2((Tree**)&type,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void SymbolVar::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    type->map(pre,post,i);
    if (value)
      value->map(pre,post,i);
    if (depth)
      depth->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void SymbolVar::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    type->map2((Tree**)&type,pre,post,i);
    if (value)
      value->map2((Tree**)&value,pre,post,i);
    if (depth)
      depth->map2((Tree**)&depth,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void SymbolOp::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    type->map(pre,post,i);
    // do NOT map op
  }
  if (post)
    post(this,i);
}


void SymbolOp::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
		     void *i, bool skipPre)
{
  assert(this==*h);
  bool descend=true;
  Tree *p=parent;	// save for replacement node in case pre/post modify it
  if (pre && !skipPre)
  {
    descend=pre(h,i);
    if (this==*h)
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
    type->map2((Tree**)&type,pre,post,i);
    // do NOT map op
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}

