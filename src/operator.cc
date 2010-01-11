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
// SCORE TDF compiler:  operators
// $Revision: 1.136 $
//
//////////////////////////////////////////////////////////////////////////////

#include "compares.h"
#include "parse.h"
#include "state.h"
#include "operator.h"
#include "stmt.h"
#include "gc.h"
#include "misc.h"
#include "blockdfg.h"

using leda::dic_item;
using leda::list_item;
using leda::list;
using leda::set;
using leda::h_array;

using std::cerr;
using std::endl;

////////////////////////////////////////////////////////////////
// pre-defined operators
//  - initialized by OperatorBuiltin::init()

OperatorBuiltin *builtin_attn	 = NULL;	// raise attention
OperatorBuiltin *builtin_bitsof	 = NULL;	// get representation of expr
OperatorBuiltin *builtin_cat	 = NULL;	// concatenate bits
OperatorBuiltin *builtin_done	 = NULL;	// self-terminate operator
OperatorBuiltin *builtin_close	 = NULL;	// close stream
OperatorBuiltin *builtin_frameclose	 = NULL;	// close frame on stream
OperatorBuiltin *builtin_copy	 = NULL;	// copy stream to fanout
//OperatorBuiltin *builtin_eos	 = NULL;	// end-of-stream predicate
OperatorBuiltin *builtin_widthof = NULL;	// get bit-width of expr
OperatorBuiltin *builtin_terminate = NULL;	// terminate stream (no cnsumr)
OperatorBuiltin *builtin_printf  = NULL;	// call printf (debug)


////////////////////////////////////////////////////////////////
//  init

void OperatorBuiltin::init ()
{
  // builtin_attn (unsigned)
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolVar(NULL,"attnCode",
				  (Type*)type_uintx->duplicate()));
    Symbol *retSym_i=(Symbol*)sym_none->duplicate();
    builtin_attn=new OperatorBuiltin(NULL,"attn",
				     retSym_i,symList,BUILTIN_ATTN);
    TreeGC::addRoot(builtin_attn);
  }

  // builtin_close (output)
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolStream(NULL,"stream",
				     type_any,STREAM_IN));	// stream name
    Symbol *retSym_i=(Symbol*)sym_none->duplicate();
    builtin_close=new OperatorBuiltin(NULL,"close",
				      retSym_i,symList,BUILTIN_CLOSE);
    TreeGC::addRoot(builtin_close);
  }

  // builtin_frameclose (output)
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolStream(NULL,"stream",
				     type_any,STREAM_IN));	// stream name
    Symbol *retSym_i=(Symbol*)sym_none->duplicate();
    builtin_frameclose=new OperatorBuiltin(NULL,"frameclose",
				      retSym_i,symList,BUILTIN_FRAMECLOSE);
    TreeGC::addRoot(builtin_frameclose);
  }


  // builtin_done ()
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolStream(NULL,"sync",
				     (Type*)type_uint0->duplicate(),
				     STREAM_IN));
    Symbol *retSym_i=new SymbolStream(NULL,"done",
				      (Type*)type_uint0->duplicate(),
				      STREAM_OUT);
    builtin_done=new OperatorBuiltin(NULL,"done",
				     retSym_i,symList,BUILTIN_DONE);
    TreeGC::addRoot(builtin_done);
  }

  /*
  // builtin_eos (input) --> bool
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolStream(NULL,"stream",
				     type_any,STREAM_IN));
    Symbol *retSym_i=new SymbolStream(NULL,"eos",
				      (Type*)type_bool->duplicate(),
				      STREAM_OUT);
    builtin_eos=new OperatorBuiltin(NULL,"eos",
				    retSym_i,symList,BUILTIN_EOS);
    TreeGC::addRoot(builtin_eos);
  }
  */

  // builtin_cat (expr,...) --> unsigned
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolVar(NULL,"expr",
				  (Type*)type_uintx->duplicate()));
    Symbol *retSym_i=new SymbolStream(NULL,"cat",
				      (Type*)type_uintx->duplicate(),
				      STREAM_OUT);
    builtin_cat=new OperatorBuiltin(NULL,"cat",
				    retSym_i,symList,BUILTIN_CAT);
    TreeGC::addRoot(builtin_cat);
  }

  // builtin_copy (istream,ostream1,ostream2,...) 
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolVar(NULL,"stream",
				  (Type*)type_uintx->duplicate()));
    Symbol *retSym_i=(Symbol*)sym_none->duplicate();
    builtin_copy=new OperatorBuiltin(NULL,"copy",
				    retSym_i,symList,BUILTIN_COPY);
    TreeGC::addRoot(builtin_copy);
  }

  // builtin_widthof (expr) --> unsigned
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolVar(NULL,"expr",type_any));
    Symbol *retSym_i=new SymbolStream(NULL,"widthof",
				      (Type*)type_uintx->duplicate(),
				      STREAM_IN);
    builtin_widthof=new OperatorBuiltin(NULL,"widthof",
					retSym_i,symList,BUILTIN_WIDTHOF);
    TreeGC::addRoot(builtin_widthof);
  }

  // builtin_terminate (expr) 
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolStream(NULL,"stream",
				     type_any,STREAM_IN));
    Symbol *retSym_i=(Symbol*)sym_none->duplicate();
    builtin_terminate=new OperatorBuiltin(NULL,"terminate",
					retSym_i,symList,BUILTIN_TERMINATE);
    TreeGC::addRoot(builtin_terminate);
  }

  // builtin_bitsof (expr) --> unsigned
  {
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolVar(NULL,"expr",type_any));
    Symbol *retSym_i=new SymbolStream(NULL,"bitsof",
				      (Type*)type_uintx->duplicate(),
				      STREAM_IN);
    builtin_bitsof=new OperatorBuiltin(NULL,"bitsof",
				       retSym_i,symList,BUILTIN_BITSOF);
    TreeGC::addRoot(builtin_bitsof);
  }

  // builtin_printf (str,expr,...)
  {
    // - format string is NOT in argument list
    // - format string is in annote ANNOTE_PRINTF_STRING_TOKEN (type Token*)
    //     on each ExprBuiltin (call)
    list<Symbol*> *symList=new list<Symbol*>;
    symList->append(new SymbolVar(NULL,"expr",
				  (Type*)type_uintx->duplicate()));
    Symbol *retSym_i=(Symbol*)sym_none->duplicate();
    builtin_printf=new OperatorBuiltin(NULL,"printf",
				       retSym_i,symList,BUILTIN_PRINTF);
    TreeGC::addRoot(builtin_printf);
  }
}


////////////////////////////////////////////////////////////////
//  constructors

using std::cout;
using std::endl;

Operator::Operator (Token  *token_i,  const string &name_i,
		    Symbol *retSym_i, list<Symbol*> *args_i,
		    OperatorKind opKind_i, Token *exception_i)
  : Tree(token_i,TREE_OPERATOR),
    name(name_i),
    opKind(opKind_i),
    retSym(retSym_i),
    args(args_i),
    symtab(new SymTab(SYMTAB_OP)),
    exception(exception_i)
{
  assert(retSym);
  assert(args);

  if(symtab==NULL) {
	  symtab = new SymTab(SYMTAB_OP);
  }

  // - return symbol's name must match operator name
  TypeKind retTypeKind = retSym->getType()->getTypeKind();
  if (   retTypeKind!=TYPE_NONE
      && retTypeKind!=TYPE_ANY )
    assert(retSym->getName()==name);

  // - add symbols to symtabs
  symtab->setParent(this);
  symtab->setScope(NULL);
  symtab->addSymbol(retSym);
  Symbol *sym;
  forall (sym,*args) {
	    symtab->addSymbol(sym);
  }
}


OperatorBehavioral::OperatorBehavioral (Token  *token_i, const string &name_i,
					Symbol *retSym_i,list<Symbol*> *args_i,
					SymTab *vars_i,
					dictionary<string,State*> *states_i,
					State  *startState_i,
					Token  *exception_i)
  : Operator(token_i,name_i,retSym_i,args_i,OP_BEHAVIORAL,exception_i),
    states(states_i),
    startState(startState_i),
    vars(vars_i)
{
  vars->setParent(this);
  symtab->addChild(vars_i);
  // - assume that state symtabs are already children of vars_i symtab
  // - on second thought, that is a bad assumption for state code
  //     not built by parser (*)

  State *s;
  forall(s,*states)
  {
    s->setParent(this);
    associateScopes(s,vars);	// (*)
  }
}


OperatorCompose::OperatorCompose (Token  *token_i,  const string &name_i,
				  Symbol *retSym_i, list<Symbol*> *args_i,
				  SymTab *vars_i,   list<Stmt*> *stmts_i,
				  Token  *exception_i)
  : Operator(token_i,name_i,retSym_i,args_i,OP_COMPOSE,exception_i),
    stmts(stmts_i),
    vars(vars_i)
{
  vars->setParent(this);
  symtab->addChild(vars_i);
  // assume that block symtabs are already children of vars_i symtab

  Stmt *s;
  forall(s,*stmts)
    s->setParent(this);
}


OperatorBuiltin::OperatorBuiltin (Token  *token_i,  const string &name_i,
				  Symbol *retSym_i, list<Symbol*> *args_i,
				  BuiltinKind builtinKind_i,
				  Token  *exception_i)
  : Operator(token_i,name_i,retSym_i,args_i,OP_BUILTIN,exception_i),
    builtinKind(builtinKind_i)
{
}


OperatorSegment::OperatorSegment (Token  *token_i, const string &name_i,
				  SegmentKind segmentKind_i,
				  Type *dataType_i, list<Symbol*> *args_i)
  : OperatorBuiltin(token_i,name_i,
		    (Symbol*)sym_none->duplicate(),args_i,BUILTIN_SEGMENT),
    segmentKind(segmentKind_i),
    dataType(dataType_i)
{
}


////////////////////////////////////////////////////////////////
//  makeOperatorSegment
//    - creates OperatorSegment builtin given a CMB mode and data type
//    - max 64-bit addresses

OperatorSegment* makeOperatorSegment (Token *token_i,
				      SegmentKind segmentKind_i,
				      const Type *dataType_i)
{
  assert(dataType_i->getTypeKind()!=TYPE_ARRAY);
  if (dataType_i->getTypeKind()==TYPE_FIXED)
    fatal(1,"fixed-type segment operators not supported",token_i);

  // - create symbols:  dwidth, awidth, nelems, contents
  Symbol    *dwidthSym	= new SymbolVar(NULL,"dwidth",
					new Type(TYPE_INT,7,false));
					// - HACK: max dwidth=64
  Symbol    *awidthSym	= new SymbolVar(NULL,"awidth",
					new Type(TYPE_INT,7,false));
					// - HACK: max awidth=64
  Symbol    *nelemsSym	= new SymbolVar(NULL,"nelems",
  					// new Type(TYPE_INT,1<<6,false));
					// - HACK: max nelems=2^32
					new Type(TYPE_INT,
						 new ExprBop(NULL,'+',
						 new ExprLValue(NULL,awidthSym),
						 constIntExpr(1)),
						 false));
  Type      *dataType	= (dataType_i->getTypeKind()==TYPE_BOOL)
			? new Type(TYPE_BOOL)
			: new Type(dataType_i->getTypeKind(),
				   new ExprLValue(NULL,dwidthSym),
				   dataType_i->isSigned());
  TypeArray *arrayType	= new TypeArray((Type*)dataType->duplicate(),
					new ExprLValue(NULL,nelemsSym));
  Symbol    *contentsSym=new SymbolVar(NULL,"contents",arrayType);
  list<Symbol*> *args_i = new list<Symbol*>;
  args_i->append(dwidthSym);
  args_i->append(awidthSym);
  args_i->append(nelemsSym);
  args_i->append(contentsSym);

  // - create symbols:  addr
  switch (segmentKind_i)
  {
    case SEGMENT_R:
    case SEGMENT_W:
    case SEGMENT_RW:
      {
	Type   *addrType = new Type(TYPE_INT,
				    new ExprLValue(NULL,awidthSym),false);
	Symbol *addrSym  = new SymbolStream(NULL,"addr",addrType,
					    STREAM_IN);
	args_i->append(addrSym);
	break;
      }

    default:
      break;
  }

  // - create symbols:  data, dataIn, dataOut
  switch (segmentKind_i)
  {
    case SEGMENT_R:
    case SEGMENT_SEQ_R:
      {
	Symbol *dataSym  = new SymbolStream(NULL,"data",
					    (Type*)dataType->duplicate(),
					    STREAM_OUT);
	args_i->append(dataSym);
	break;
      }

    case SEGMENT_W:
    case SEGMENT_SEQ_W:
      {
	Symbol *dataSym  =new SymbolStream(NULL,"data",
					   (Type*)dataType->duplicate(),
					   STREAM_IN);
	args_i->append(dataSym);
	break;
      }

    case SEGMENT_RW:
    case SEGMENT_SEQ_RW:
      {
	Symbol *dataRSym = new SymbolStream(NULL,"dataR",
					    (Type*)dataType->duplicate(),
					    STREAM_OUT);
	Symbol *dataWSym = new SymbolStream(NULL,"dataW",
					     (Type*)dataType->duplicate(),
					     STREAM_IN);
	args_i->append(dataRSym);
	args_i->append(dataWSym);
	break;
      }
  }

  // - add symbols:  write
  if (segmentKind_i==SEGMENT_RW)
  {
    Symbol *writeSym = new SymbolStream(NULL,"write",
					(Type*)type_bool->duplicate(),
					STREAM_IN);
    args_i->append(writeSym);
  }

  // - create operator:
  OperatorSegment *o;
  switch (segmentKind_i)
  {
    case SEGMENT_R:
      {
	o=new OperatorSegment(token_i,"segment_r",SEGMENT_R,
			      dataType,args_i);
	break;
      }
    case SEGMENT_W:
      {
	o=new OperatorSegment(token_i,"segment_w",SEGMENT_W,
			      dataType,args_i);
	break;
      }
    case SEGMENT_RW:
      {
	o=new OperatorSegment(token_i,"segment_rw",SEGMENT_RW,
			      dataType,args_i);
	break;
      }
    case SEGMENT_SEQ_R:
      {
	o=new OperatorSegment(token_i,"segment_seq_r",SEGMENT_SEQ_R,
			      dataType,args_i);
	break;
      }
    case SEGMENT_SEQ_W:
      {
	o=new OperatorSegment(token_i,"segment_seq_w",SEGMENT_SEQ_W,
			      dataType,args_i);
	break;
      }
    case SEGMENT_SEQ_RW:
      {
	o=new OperatorSegment(token_i,"segment_seq_rw",SEGMENT_SEQ_RW,
			      dataType,args_i);
	break;
      }
  }
  TreeGC::addRoot(o);		// - HACK: seg-op not reachable from anywhere
  o->link();
  o->typeCheck();
  // cerr << "- makeOperatorSegment() created:\n" << o->toString();
  return o;
}


////////////////////////////////////////////////////////////////
//  setParent

void Operator::setParent (Tree *p)
{
  assert(p==NULL || p->getKind()==TREE_SUITE);
  parent=p;
}


////////////////////////////////////////////////////////////////
//  thread

void OperatorBehavioral::thread (Tree *p)
{
  setParent(p);
  symtab->thread(this);
  vars->thread(this);
  dic_item i;
  forall_items(i,*states)
  {
    State *s=states->inf(i);
    s->thread(this);
  }
}


void OperatorCompose::thread (Tree *p)
{
  setParent(p);
  symtab->thread(this);
  vars->thread(this);
  Stmt *s;
  forall(s,*stmts)
    s->thread(this);
}


void OperatorBuiltin::thread (Tree *p)
{
  setParent(p);
  symtab->thread(this);
}


void OperatorSegment::thread (Tree *p)
{
  setParent(p);
  symtab->thread(this);
  dataType->thread(this);
}


////////////////////////////////////////////////////////////////
//  link

bool OperatorBehavioral::link ()
{
  bool linked = symtab->link();
  linked = linked && vars->link();
  dic_item i;
  forall_items(i,*states)
    linked = linked && states->inf(i)->link();
  return linked;
}


bool OperatorCompose::link ()
{
  bool linked = symtab->link();
  linked = linked && vars->link();
  Stmt *s;
  forall(s,*stmts)
    linked = linked && s->link();
  return linked;
}


bool OperatorBuiltin::link ()
{
  return symtab->link();
}


bool OperatorSegment::link ()
{
  bool linked = symtab->link();
  linked = linked && dataType->link();
  return linked;
}


////////////////////////////////////////////////////////////////
//  typeCheck

Type* OperatorBehavioral::typeCheck ()
{
  symtab->typeCheck();
  dic_item i;
  forall_items(i,*states)
    states->inf(i)->typeCheck();
  return retSym->typeCheck();
}


Type* OperatorCompose::typeCheck ()
{
  symtab->typeCheck();
  Stmt *s;
  forall(s,*stmts)
    {
      s->typeCheck();
      if (gDeleteType)
	rmType(s);
    }
  return retSym->typeCheck();
}


Type* OperatorBuiltin::typeCheck ()
{
  symtab->typeCheck();
  return retSym->typeCheck();
}


Type* OperatorSegment::typeCheck ()
{
  OperatorBuiltin::typeCheck();
  dataType->typeCheck();
  return type_none;
}


////////////////////////////////////////////////////////////////
//  symbol managment

SymbolOp* Operator::getSymbol () const
{
  // - returns this operator's SymbolOp (NULL if not-found)

  SymTab *scope = symtab->getScope();
  if (scope)
    return (SymbolOp*)(scope->lookup(name));
  else
    return NULL;
}


bool Operator::setName (const string &newName)
{
  // - names/renames an operator + its retSym, updating suite's and own symtabs
  // - note, calling SymbolOp::setName from here recurses (calls back)
  //   so we explicitly call Symbol::setName to update SymbolOp

  bool updatedSym=false;
  Symbol *sym = getSymbol();
  if (sym)
  {
    if (sym->Symbol::setName(newName))
      updatedSym=true;
    else
      return false;
  }
  if (retSym)
  {
    if (retSym->setName(newName))
      ;
    else
    {
      if (updatedSym)
	sym->Symbol::setName(name);		// - back out
      return false;
    }
  }
  name=newName;
  return true;  
}


////////////////////////////////////////////////////////////////
//  state managment

State* OperatorBehavioral::lookupState (const string &name) const
{
  dic_item i=states->lookup(name);
  return i ? states->inf(i) : NULL;
}


void OperatorBehavioral::addState (State *s)
{
  string stateName = s->getName();
  if (states->lookup(stateName))
    fatal(1, "duplicate addition of state \""+stateName+
	     "\" to operator \""+name+"\"");
  else
  {
    states->insert(stateName,s);
    s->setParent(this);
    associateScopes(s,vars);
  }
}


void OperatorBehavioral::removeState (State *s)
{
  string stateName=s->getName();
  if (states->lookup(stateName))
  {
    states->del(stateName);
    s->setParent(NULL);
    dissociateScopes(s,vars);
  }
  else
    fatal(1, "cannot remove state \""+stateName+
	     "\" from operator \""+name+"\", state not found");
}


////////////////////////////////////////////////////////////////
//  okInComposeOp

bool OperatorBuiltin::okInComposeOp () const
{
  return (   builtinKind==BUILTIN_CAT
	  || builtinKind==BUILTIN_BITSOF
	  || builtinKind==BUILTIN_COPY
	  || builtinKind==BUILTIN_WIDTHOF
	  || builtinKind==BUILTIN_SEGMENT
          || builtinKind==BUILTIN_TERMINATE // kinda special -- amd
	     );
  // - warning:  not checking binding times!  op ok only for bound args
}


////////////////////////////////////////////////////////////////
//  getBindTime  (stubs for now)

BindTime OperatorBehavioral::getBindTime () const   { return BIND_UNKNOWN; }
BindTime OperatorCompose::getBindTime () const	    { return BIND_UNKNOWN; }
BindTime OperatorBuiltin::getBindTime () const	    { return BIND_UNKNOWN; }


////////////////////////////////////////////////////////////////
//  duplicate

void Operator::duplicateHelper ()
{
  // duplicate parts of Operator
  // - duplicate symtab, then fish new retSym and args from it
  symtab = (SymTab*)symtab->duplicate();
  symtab->setParent(this);
  if (retSym)
  {
    retSym = symtab->lookup(retSym->getName());
    assert(retSym);
  }
  list<Symbol*> *argsOrig=args;
  args = new list<Symbol*>;
  Symbol *s;
  forall (s,*argsOrig)
  {
    Symbol *sCopied = symtab->lookup(s->getName());
    assert(sCopied);
    args->append(sCopied);
  }
}

Tree* OperatorBehavioral::duplicate () const
{
  assert(this);
  OperatorBehavioral *o = new OperatorBehavioral(*this);
  o->setParent(NULL);
  o->duplicateHelper();
  o->vars = (SymTab*)o->vars->duplicate();
  o->vars->setParent(o);
  o->symtab->addChild(o->vars);
  o->states = new dictionary<string,State*>;
  dic_item i;
  forall_items (i,*states)
  {
    State *state     = states->inf(i),
          *stateCopy = (State*)state->duplicate();
    stateCopy->setParent(o);
    o->states->insert(state->getName(), stateCopy);
  }
  if (startState)
  {
    dic_item i    = o->states->lookup(startState->getName());
    o->startState = o->states->inf(i);
  }
  return o;
}


Tree* OperatorCompose::duplicate () const
{
  assert(this);
  OperatorCompose *o = new OperatorCompose(*this);
  o->setParent(NULL);
  o->duplicateHelper();
  o->vars = (SymTab*)o->vars->duplicate();
  o->vars->setParent(o);
  o->symtab->addChild(o->vars);
  o->stmts = new list<Stmt*>;
  Stmt *s;
  forall (s,*stmts)
  {
    Stmt *sCopy = (Stmt*)s->duplicate();
    sCopy->setParent(o);
    o->stmts->append(sCopy);
  }
  return o;
}


Tree* OperatorBuiltin::duplicate () const
{
  assert(this);
  OperatorBuiltin *o = new OperatorBuiltin(*this);
  o->setParent(NULL);
  o->duplicateHelper();
  return o;
}


Tree* OperatorSegment::duplicate () const
{
  assert(this);
  OperatorSegment *o = new OperatorSegment(*this);
  o->setParent(NULL);
  o->duplicateHelper();
  o->dataType = (Type*)o->dataType->duplicate();
  o->dataType->setParent(o);
  return o;
}


////////////////////////////////////////////////////////////////
//  toString

string Operator::declToString () const
{
  Type  *retType    = retSym->getType();
  string retTypeStr = (retType->getTypeKind()==TYPE_NONE)
			? string() : (retType->toString()+" ");
  string argsStr;
  if (args->size()>0)
  {
    list_item i=args->first();
    argsStr = args->inf(i)->declToString();
    while ((i=args->succ(i)))
      argsStr += ", " + args->inf(i)->declToString();
  }
  return retTypeStr + name + " (" + argsStr + ")";
}


string OperatorBehavioral::toString () const
{
//cout << "Nachiket is writing out behavioral operators" << endl;
  indentPush();
  string varsStr   = vars->toString();
  if (varsStr.length()>0)
    varsStr += "\n";
  string statesStr = startState->toString();
  dic_item i;
  forall_items (i,*states)
  {
    State *s = states->inf(i);
    if (s!=startState)
      statesStr += "\n" + s->toString();
  }
  indentPop();
  return declToString() + "\n{\n" + varsStr + statesStr + "}\n";
}

// Added by Nachiket on 11/29/2009 to build dataflow graphs
void OperatorBehavioral::buildDataflowGraph() const
{

	// - create dfg + residual stmts
	set<StateCase*> statecases;
	StateCase *sc;
	dic_item i;
	forall_items (i,*states) {
		State *s = states->inf(i);
		string sname=s->getName();
		//cout << "CREATING DFG for State=" << sname << "[" << s <<"]" << endl;
		forall (sc,*s->getCases()) {
			//cout << "Case=" << sc->toString() << endl;
			statecases.insert(sc);
			sc->setStateName(s->getName()); // stupid!! 12/27/2009 4.53pm
		}
	}

	StateCase *sc1;
	forall (sc1,statecases) {

		BlockDFG dfg;
		list<Stmt*> stmts1 = *sc1->getStmts();

		h_array<node, Symbol*> symbolmap = createBlockDfgSimple(sc1,&dfg,&stmts1,vars);
		sc1->addDataflowGraph(dfg); // Remember the dataflow graph for this state case..
		sc1->addSymbolMap(symbolmap); // Whoa?
		//cout << "Statecase=" << sc1 << " DFG=" << &dfg << endl;
		cout << printBlockDFG(sc1->getStateName(), &dfg, NULL, NULL, NULL) << endl;
	}

}

string Operator::toDOTString (string offset) const
{
	string str;
	if(getOpKind()==OP_BEHAVIORAL) {
		str+=string(offset+"node "+getName()+";\n");
		/*
		// IOs
		str+=string("\t\t// IOs\n");
		Symbol* sym;
		forall(sym, *(getArgs())) {
			if(sym->isStream()) {
				str+=string("\t\t//"+sym->getName()+"\n");
			}
		}
		*/
		return str;
	}

	// internal streams
	if(getOpKind()==OP_COMPOSE) {
		str+=string(offset+"subgraph "+getName()+" {\n");
		str+=string(offset+"\t// Internals\n");
		SymTab *symtab=((OperatorCompose*)this)->getVars();
		list<Symbol*>* lsyms=symtab->getSymbolOrder();
		Symbol* sym;
		forall(sym, *lsyms) {
			str+=string(offset+"\t"+sym->getName()+"\n");
		}

		// Now process call statements
		set<Expr*>* ops = new set<Expr*>();
		Stmt* statement;
		str+= offset+"\t// Operators\n";
		forall(statement,*(((OperatorCompose*)this)->getStmts())) {
			if ((statement->getStmtKind()==STMT_CALL)) {
				Expr *expr=((StmtCall *)statement)->getCall();
				ExprCall *ecall=(ExprCall *)expr;
				Operator *cop=ecall->getOp();
				str+= cop->toDOTString(offset+"\t");
				ops->insert(ecall);
			}
		}


		// Match our internal signal list against this...
		forall(sym, *lsyms) {
			Expr *ecall;
			forall(ecall, *ops) {
				list<Expr*>* args=((ExprCall*)ecall)->getArgs();
				Operator *cop=((ExprCall*)ecall)->getOp();
				Expr *earg;
				forall(earg,*args) {
					if(earg->getExprKind()==EXPR_LVALUE) {
						if(((ExprLValue *)earg)->getSymbol()==sym) {
							str+=offset+"\t matched="+sym->getName()+" oneend="+cop->getName()+"\n";
							list<Symbol*>* lsyms=cop->getArgs();
							Symbol* sym;
							forall(sym, *lsyms) {
								str+=string(offset+"\t"+sym->getName()+"\n");
							}

						}
					}
				}
			}
		}
		
		str+=offset+"}\n";
		return str;
	}

}

string OperatorBehavioral::toDFGString () const
{

// Added by Nachiket on 11/3/2009 to test where the state-machine variables are stored!
//	cout << "Global variables=" << endl;
//	cout << vars->toString() << endl;
//	cout << "Finished..." <<  endl;


  // - create dfg + residual stmts
  	int total_states=0;
	set<StateCase*> statecases;
	StateCase *sc;
  	dic_item i;
	forall_items (i,*states) {
    		State *s = states->inf(i);
		forall (sc,*s->getCases()) {
			statecases.insert(sc);
			sc->setStateName(s->getName());
			total_states++;
		}
	}

	std::stringstream out;
	out << total_states;
	string totalStr="ScoreOperator "+getName()+" ("+string(out.str().c_str())+" states)\n";

	StateCase *sc1;
	forall (sc1,statecases) {

		//cout << "State=" << printStateCaseHead(sc1) << endl;

		BlockDFG dfg;
		list<Stmt*> stmts1 = *sc1->getStmts();

		/*
		// debug start 10/28
		Stmt* stmt;
		forall(stmt, stmts1) {
			cout << "Statement=\n" << stmt->toString() << endl;
		}
		// debug end
		 */

		createBlockDfgSimple(sc1,&dfg,&stmts1,vars);
		sc1->addDataflowGraph(dfg); // Remember the dataflow graph for this state case..
		string dfgStr = printBlockDFG(sc1->getStateName(), &dfg, NULL, NULL, NULL);
		//cout << dfgStr << endl;

		totalStr+=dfgStr;
	}

	return totalStr;
}


string OperatorCompose::toString () const
{
  indentPush();
  string varsStr = vars->toString();
  if (varsStr.length()>0)
    varsStr += "\n";
  string stmtsStr;
  if (stmts->size()>0)
  {
    list_item i=stmts->first();
    stmtsStr = stmts->inf(i)->toString();
    while ((i=stmts->succ(i)))
      stmtsStr += stmts->inf(i)->toString();
  }
  indentPop();
  return declToString() + "\n{\n" + varsStr + stmtsStr + "}\n";
}


string OperatorBuiltin::toString () const
{
  return declToString() + " {...}\n";
}


////////////////////////////////////////////////////////////////
//  map

void OperatorBehavioral::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    symtab->map(pre,post,i);	// visits return symbol
    vars->map(pre,post,i);
    State *state;
    forall (state,*states)
      state->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void OperatorBehavioral::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    vars  ->map2((Tree**)&vars,  pre,post,i);
    set<State*> oldStates, newStates;
    State *state;
    forall (state,*states)
    {
      State *stateOrig=state;
      state->map2((Tree**)&state,pre,post,i);
      if (state!=stateOrig)
      {
	oldStates.insert(stateOrig);
	newStates.insert(state);
      }
    }
    if (!newStates.empty())
    {
      forall (state,oldStates)
	removeState(state);
      forall (state,newStates)
	addState(state);
    }
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void OperatorCompose::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    symtab->map(pre,post,i);	// visits return symbol
    vars->map(pre,post,i);
    Stmt *stmt;
    forall (stmt,*stmts)
      stmt->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void OperatorCompose::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    vars  ->map2((Tree**)&vars,  pre,post,i);
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


void OperatorBuiltin::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    symtab->map(pre,post,i);	// visits return symbol
  }
  if (post)
    post(this,i);
}


void OperatorBuiltin::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void OperatorSegment::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    symtab->map(pre,post,i);
    dataType->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void OperatorSegment::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    dataType->map2((Tree**)&dataType,pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}
