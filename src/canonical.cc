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
// SCORE TDF compiler:  canonical-form xforms (exlining calls/mem, etc.)
// $Revision: 1.146 $
//
//////////////////////////////////////////////////////////////////////////////


#include "state.h"
//#include "symbol.h"
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "canonical.h"
#include "annotes.h"
#include "time.h"
#include "gc.h"
#include "suite.h"
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/sortseq.h>
#include <LEDA/core/array.h>

using leda::array;
using leda::sortseq;
using leda::list_item;
using leda::dic_item;
using std::ostream;

////////////////////////////////////////////////////////////////
// utilities

template<class T>
void pushList (list<T> *l1, list<T> *l2)
{
  list<T> l2_dup(*l2);
  l1->conc(l2_dup,LEDA::before);
}


template<class T>
void appendList (list<T> *l1, list<T> *l2)
{
  list<T> l2_dup(*l2);
  l1->conc(l2_dup,LEDA::after);
}


template <class T>
string listToString (list<T> *l)
{
  string str;
  T t;
  forall (t,*l)
    str+=t->toString();			// T is subclass of Tree
  return str;
}


////////////////////////////////////////////////////////////////
//  lift inlines

static
bool isInAsstContext (Tree *t)
{
  // - return true iff t is in an assignment context,
  //   i.e. either on left side of assignment,
  //   or as a call-arg receiving an output stream

  Tree *p=t->getParent();
  BuiltinKind    builtinKind;
  int		 argPos;
  list<Symbol*>	*formals;
  Symbol	*formal;

  if (   (   p
	  && p->getKind()==TREE_STMT
	  && ((Stmt*)p)->getStmtKind()==STMT_ASSIGN
	  && ((StmtAssign*)p)->getLValue()==t)
      || (   p
	  && p->getKind()==TREE_EXPR
	  && (   (   ((Expr*)p)->getExprKind()==EXPR_CALL
		  && (argPos =((ExprCall*)p)->getArgs()->rank((Expr*)t)-1) >= 0
		  && (formals=((ExprCall*)p)->getOp()->getArgs())
		  && (formal =formals->inf(formals->get_item(argPos)))
		  && formal->getSymKind()==SYMBOL_STREAM
		  && ((SymbolStream*)formal)->getDir()==STREAM_OUT)
	      || (   ((Expr*)p)->getExprKind()==EXPR_BUILTIN
		  && (argPos =((ExprCall*)p)->getArgs()->rank((Expr*)t)-1) >= 0
		  && ((ExprCall*)p)->getOp()->getOpKind()==OP_BUILTIN
		  && (   (   (builtinKind= ((OperatorBuiltin*)((ExprCall*)p)
					    ->getOp())->getBuiltinKind()), 0)
		      || (    builtinKind==BUILTIN_COPY && argPos>0)
		      || (    builtinKind!=BUILTIN_CAT
			  && (formals=((ExprCall*)p)->getOp()->getArgs())
			  && (formal =formals->inf(formals->get_item(argPos)))
			  && formal->getSymKind()==SYMBOL_STREAM
			  && ((SymbolStream*)formal)->getDir()==STREAM_OUT)))
	      || (   ((Expr*)p)->getExprKind()==EXPR_CAST
		  && isInAsstContext(p)))))
    return true;
  else
    return false;
}


static
Stmt* getEnclosingStmt (Tree *t)
{
  // - returns nearest stmt enclosing t (possibly t itself), NULL if none found

  while (t && t->getKind()!=TREE_STMT)
    t=t->getParent();
  return (Stmt*)t;
}


static
bool isLocalScalar (Tree *t)
{
  // - returns true iff t is an ExprLValue referring to a scalar
  if (   t->getKind()==TREE_EXPR
      && ((Expr*)t)->getExprKind()==EXPR_LVALUE)
  {
    Symbol *sym=((ExprLValue*)t)->getSymbol();
    return sym->isLocal() && !sym->isArray();
  }
  else
    return false;
}


static
bool isArrayRef (Tree *t)
{
  // - returns true iff t is an ExprLValue which accesses memory
  return (   t->getKind()==TREE_EXPR
	  && ((Expr*)t)->getExprKind()==EXPR_LVALUE
	  && ((ExprLValue*)t)->getSymbol()->isArray()	// redundant
	  && ((ExprLValue*)t)->getArrayLoc()!=NULL);
}


static
bool isLiftableCall (Tree *t)
{
  // - returns true iff t is an exlinable call/builtin
  // - note, segment ops are exlinable but never inlined (are stmts only)
  return (   t->getKind()==TREE_EXPR
	  && ((Expr*)t)->getExprKind()==EXPR_CALL);
}


bool liftInlines_preMap (Tree **h, void *i)
{
  // - descend into behavioral ops' stmts + exprs (not types, symbols)
  Tree *t=*h;
  TreeKind kind=t->getKind();
  return (    kind==TREE_SUITE
	  || (kind==TREE_OPERATOR
		&& ((Operator*)t)->getOpKind()==OP_BEHAVIORAL)
	  ||  kind==TREE_STATE
	  ||  kind==TREE_STATECASE
	  ||  kind==TREE_STMT
	  ||  kind==TREE_EXPR);
}


bool liftInlines_postMap (Tree **h, void *i)
{  
  // - post-pass map to lift inlined mem-accesses/calls (see liftInlines())

  Tree *t=*h, *p=t->getParent();
  bool tIsArrayRef	= isArrayRef(t),
       tIsLiftableCall	= isLiftableCall(t);

  if (tIsArrayRef)
  {
    if (isInAsstContext(t))
      ((ExprLValue*)t)->getSymbol()->setAnnote_(ANNOTE_ARRAY_WRITTEN,true);
    else
      ((ExprLValue*)t)->getSymbol()->setAnnote_(ANNOTE_ARRAY_READ,true);
  }

  if (   (   tIsArrayRef
	  && !(   p
	       && p->getKind()==TREE_STMT
	       && ((Stmt*)p)->getStmtKind()==STMT_ASSIGN
	       && (   (   t==((StmtAssign*)p)->getLValue()
		       && isLocalScalar(((StmtAssign*)p)->getRhs()))
		   || (   t==((StmtAssign*)p)->getRhs()
		       && isLocalScalar(((StmtAssign*)p)->getLValue())))))
      || (   tIsLiftableCall
	  && !(p
	       && p->getKind()==TREE_STMT
	       && (   (   ((Stmt*)p)->getStmtKind()==STMT_CALL   )
		   || (   ((Stmt*)p)->getStmtKind()==STMT_BUILTIN)
		   || (   ((Stmt*)p)->getStmtKind()==STMT_ASSIGN
		       && (   t==((StmtAssign*)p)->getRhs()
			   && isLocalScalar(((StmtAssign*)p)->getLValue()))))))
	)
    {
      // - found expr to lift
      
      // - (1) create tmp lvalue to replace this expr
      static long liftTmpCount=0;
      Type	*tmpType=(Type*)t->getType()->duplicate();
      SymbolVar	*tmpSym =new SymbolVar(NULL,
				       string("_score_inline_tmp_%d",
					      liftTmpCount++),
				       tmpType);
      Expr	*tmpExpr=new ExprLValue(NULL,tmpSym);
      // *h=tmpExpr;	// - defer to later for consistency

      // - (2) get/initialize enclosing stmt's info about new block
      Stmt *enclosingStmt=getEnclosingStmt(t);
      SymTab *symtab		= (SymTab*)enclosingStmt->
					getAnnote(ANNOTE_LIFT_SYMTAB);
      list<Stmt*> *preStmts	= (list<Stmt*>*)enclosingStmt->
					getAnnote(ANNOTE_LIFT_PRE_STMTS);
      list<Stmt*> *postStmts	= (list<Stmt*>*)enclosingStmt->
					getAnnote(ANNOTE_LIFT_POST_STMTS);
      if (symtab==NULL)
	enclosingStmt->setAnnote(ANNOTE_LIFT_SYMTAB,
				 symtab=new SymTab(SYMTAB_BLOCK));
      if (preStmts==NULL)
	enclosingStmt->setAnnote(ANNOTE_LIFT_PRE_STMTS,
				 preStmts=new list<Stmt*>);
      if (postStmts==NULL)
	enclosingStmt->setAnnote(ANNOTE_LIFT_POST_STMTS,
				 postStmts=new list<Stmt*>);
      symtab->addSymbol(tmpSym);

      // - (3) add tmp decl + lifted assignment to new block (to replace stmt)
      if (isInAsstContext(t))
      {
	// assert(   t->getKind()==TREE_EXPR
	//	&& ((Expr*)t)->getExprKind()==EXPR_LVALUE);
	StmtAssign *tmpAssign=new StmtAssign(NULL,(ExprLValue*)t,
					     (Expr*)tmpExpr->duplicate());
	tmpAssign->map2((Tree**)&tmpAssign,
			liftInlines_preMap,liftInlines_postMap,i);  // recurse
	postStmts->push_front(tmpAssign);
	// warn(string("lifting asst. ")+tmpAssign->toString()+
	//      string(" from ")+t->toString(), t->getToken());
      }
      else
      {
	StmtAssign *tmpAssign=new StmtAssign(NULL,
					     (ExprLValue*)tmpExpr->duplicate(),
					     (Expr*)t);
	tmpAssign->map2((Tree**)&tmpAssign,
			liftInlines_preMap,liftInlines_postMap,i);  // recurse
	preStmts->append(tmpAssign);
	// warn(string("lifting value ")+tmpAssign->toString()+
	//      string(" from ")+t->toString(), t->getToken());
      }

      // - (4) replace lifted expr by tmp lvalue (deferred for consistency)
      *h=tmpExpr;
    }
  else if (   t->getKind()==TREE_STMT
	   && t->getAnnote(ANNOTE_LIFT_SYMTAB)
	   && t->getAnnote(ANNOTE_LIFT_PRE_STMTS)
	   && t->getAnnote(ANNOTE_LIFT_POST_STMTS))
    {
      // - found a stmt containing lifted exprs
      // - replace stmt by (pre-computed) block containing
      //     tmp decls, lifted assignments (pre,post), and orig stmt
      Stmt	  *enclosingStmt= (Stmt*)t;
      SymTab	  *symtab	= (SymTab*)enclosingStmt->
					getAnnote(ANNOTE_LIFT_SYMTAB);
      list<Stmt*> *preStmts	= (list<Stmt*>*)enclosingStmt->
					getAnnote(ANNOTE_LIFT_PRE_STMTS);
      list<Stmt*> *postStmts	= (list<Stmt*>*)enclosingStmt->
					getAnnote(ANNOTE_LIFT_POST_STMTS);
      list<Stmt*> *allStmts	= new list<Stmt*>;
      appendList(allStmts,preStmts);
      allStmts->append(enclosingStmt);
      appendList(allStmts,postStmts);
      StmtBlock *newBlock=new StmtBlock(NULL,symtab,allStmts);
      *h=newBlock;			    // - patch-in new block
      SymTab *scope=t->getScope();	    // - link block to enclosing symtab
      if (scope)
	scope->addChild(symtab);
      // warn(string("replaced ")+t->toString()+" by "+newBlock->toString(),
      //      t->getToken());
      // newBlock->link();
      // newBlock->typeCheck();
      delete preStmts;
      delete postStmts;
    }
  /*
  else if (   t->getKind()==TREE_OPERATOR
	   && ((Operator*)t)->getOpKind()==OP_BEHAVIORAL)
    t->link();		// link after modifying states
  */
  return true;	// dummy
}


Tree* liftInlines (Tree *t)
{
  // - lift mem-access/inlined-calls to statement level
  // - afterwards, mem-accesses may appear only as:
  //    * asst lvalue w/tmp rvalue
  //    * asst rvalue w/tmp lvalue
  // - afterwards, calls & builtin-ops (segments) may appear only as:
  //    * stmt
  //    * asst rvalue w/tmp lvalue

  t->map2(&t, liftInlines_preMap, liftInlines_postMap);
  return t;
}


////////////////////////////////////////////////////////////////
//  basic blocks

// - break behavioral operators into "basic block" state actions
// - each resulting state action is:
//    * stmt block, possibly with locals
//    * straight-line code, gotos and ifs appearing only at end
//    * if branches contain only gotos


typedef struct {
  SymTab	  *saveDecls;		// decls to leave in this state-case
  list<Stmt*>     *saveStmts;		// stmts to leave in this state-case
  list<Stmt*>     *deferStmts;		// stmts defered to continuation state
  State		  *deferState;		// continuation state
  set<State*>     newStates;		// new states (residues)
} BasicBlockInfo;


static
void uniqueRenameSym (Symbol *sym, SymTab *scope)
{
  // - renames sym (if necessary) to be unique in the given scope
  // - renaming is by addition of "_i" where i is smallest possible int >=1
  string origName=sym->getName(), newName;
  int newNameCnt;
  for (newName=origName, newNameCnt=1;
       scope->lookup(newName);
       newName=origName+string("_%d",newNameCnt++));
  sym->setName(newName);
  // if (newName!=origName)
  // warn(string("renamed symbol ")+origName+" to "+newName, sym->getToken());
}


static
void uniqueRenameState (State *state, sortseq<string,State*> *states)
{
  // - renames state (if necessary) to be unique in the given set of states
  // - renaming is by addition of "_i" where i is smallest possible int >=1
  string origName=state->getName(), newName;
  int newNameCnt;
  for (newName=origName, newNameCnt=1;
       states->lookup(newName);
       newName=origName+string("_%d",newNameCnt++));
  state->setName(newName);
  // if (newName!=origName)
  // warn(string("renamed state ")+origName+" to "+newName,state->getToken());
}


static
void uniqueRenameOperator (Operator *op, SymTab *scope)
{
  // - renames op (if necessary) to be unique in the given scope
  // - renaming is by addition of "_i" where i is smallest possible int >=1
  string origName=op->getName(), newName;
  int newNameCnt;
  for (newName=origName, newNameCnt=1;
       scope->lookup(newName);
       newName=origName+string("_%d",newNameCnt++));
  op->setName(newName);
  // if (newName!=origName)
  // warn(string("renamed operator ")+origName+" to "+newName, op->getToken());
}


static
void mergeSymtabs (SymTab *symtab1, SymTab *symtab2)
{
  // - merge symbols from symtab2 into symtab1, renaming for uniqueness
  // - symtab1 symbol order is extended with symtab2's symbol order
  Symbol *sym;
  forall (sym,*symtab2->getSymbolOrder())
  {
    uniqueRenameSym(sym,symtab1);
    symtab1->addSymbol(sym);
  }
}


bool basicBlocks_stateCase_preMap (Tree **h, void *i)
{
  Tree *t=*h;
  BasicBlockInfo   *bbi=(BasicBlockInfo*)i;
  static State	   *state;
  static StateCase *stateCase;
  static bool	   exitCase=false, contCase=false;

  TreeKind kind=t->getKind();
  switch (kind)
  {
    case TREE_STATE:
      state=(State*)t;
      return true;

    case TREE_STATECASE:
      {
	state=(State*)t->getParent();
	stateCase=(StateCase*)t;
	exitCase = contCase = false;
	list<Stmt*> *stmts=((StateCase*)t)->getStmts();
	StmtGoto *sg=new StmtGoto(NULL,state);
	sg->setParent(t);				// - HACK: modifying
	stmts->append(new StmtGoto(NULL,state));	//   stmt list directly
      }
      return true;

    case TREE_STMT:
      if (exitCase)
	return false;
      else if (contCase)
      {
	bbi->deferStmts->append((Stmt*)t);
	return false;
      }
      else
      {
	switch (((Stmt*)t)->getStmtKind())
	{
          case STMT_GOTO:
	    bbi->saveStmts->append((Stmt*)t);
	    exitCase=true;
	    return false;

          case STMT_BLOCK:
	    mergeSymtabs(bbi->saveDecls,((StmtBlock*)t)->getSymtab());
	    return true;

          case STMT_IF:
	  {
	    Stmt *thenPart=((StmtIf*)t)->getThenPart(),
	         *elsePart=((StmtIf*)t)->getElsePart(),
	         *gotoThen=NULL,
	         *gotoElse=NULL;
	    if (   (thenPart && thenPart->getStmtKind()==STMT_GOTO)
		&& (elsePart && elsePart->getStmtKind()==STMT_GOTO))
	    {
	      // - if-stmt is already canonical ("goto" in either branch),
	      //    so finish state-case here w/o modifying stmt
	      bbi->saveStmts->append((StmtIf*)t);
	      exitCase=true;
	      return false;
	    }
	    else
	    {
	      // - break if-stmt into basic blocks
	      if (thenPart)
	      {
		// - new state for then-part
		list<Stmt*> *thenStmts=new list<Stmt*>;
		thenStmts->append(thenPart);
		thenStmts->append(new StmtGoto(NULL,bbi->deferState));
		StateCase *thenCase=new StateCase(NULL,new list<InputSpec*>,
						  thenStmts);
		set<StateCase*> *thenCases=new set<StateCase*>;
		thenCases->insert(thenCase);
		string thenStateName=state->getName()+"_t";
		// - will be uniquely renamed later, in basicBlocks_Op_map()
		State *thenState=new State(NULL,thenStateName,thenCases);
		bbi->newStates.insert(thenState);
		gotoThen=new StmtGoto(NULL,thenState);
	      }
	      if (elsePart)
	      {
		// - new state for else-part
		list<Stmt*> *elseStmts=new list<Stmt*>;
		elseStmts->append(elsePart);
		elseStmts->append(new StmtGoto(NULL,bbi->deferState));
		StateCase *elseCase=new StateCase(NULL,new list<InputSpec*>,
						  elseStmts);
		set<StateCase*> *elseCases=new set<StateCase*>;
		elseCases->insert(elseCase);
		string elseStateName=state->getName()+"_e";
		// - will be uniquely renamed later, in basicBlocks_Op_map()
		State *elseState=new State(NULL,elseStateName,elseCases);
		bbi->newStates.insert(elseState);
		gotoElse=new StmtGoto(NULL,elseState);
	      }
	      else
	      {
		// - else-less if, force goto continuation state
		gotoElse=new StmtGoto(NULL,bbi->deferState);
	      }
	      StmtIf *newIf=new StmtIf(NULL,((StmtIf*)t)->getCond(),
				       gotoThen,gotoElse);
	      bbi->saveStmts->append(newIf);
	      // *h=newIf;
	      contCase=true;
	      return false;
	    }
	  }

          default:
	    bbi->saveStmts->append((Stmt*)t);	    
	    return true;
	}
      }
      return true;

    /*
    case TREE_EXPR:
      {
	if (((Expr*)t)->getExprKind()==EXPR_LVALUE)
	{
	  // - record all local symbols referenced in stateCase,
	  //   rename to disambiguate overloaded names in new flat scope
	  Symbol *sym=((ExprLValue*)t)->getSymbol();
	  if (sym->isLocal())
	  {
	    Symbol *prevSym=bbi->saveDecls->lookup(sym->getName());
	    if (sym==prevSym)				// already got it
	    {}
	    else if (sym!=prevSym)			// got sym w/same name
	    {
	      uniqueRenameSym(sym,bbi->saveDecls);
	      bbi->saveDecls->addSymbol(sym);
	    }
	    else					// never seen it
	      bbi->saveDecls->addSymbol(sym);
	  }
	}
      }
      return true;
    */

    default:
      return false;
  }
  return false;	// dummy
}


/*
bool basicBlocks_stateCase_postMap (Tree **h, void *i)
{
  Tree *t=*h;
  BasicBlockInfo *bbi=(BasicBlockInfo*)i;

  if (t->getKind()==TREE_STATECASE)
  {
    // - replace stateCase by saved initial stmts
    SymTab      *savedSymtab=new SymTab(*bbi->saveDecls);
    list<Stmt*> *savedStmts =new list<Stmt*>(*bbi->saveStmts);
    StmtBlock   *savedBlock =new StmtBlock(NULL,savedSymtab,savedStmts);
    list<Stmt*> *savedStmtL =new list<Stmt*>;
    savedStmtL->append(savedBlock);
    StateCase   *savedCase  =new StateCase(((StateCase*)t)->getToken(),
					   ((StateCase*)t)->getInputs(),
					   savedStmtL);
    *h=savedCase;
    // warn(string("shrunk StateCase to:\n")+savedCase->toString(),
    //      savedCase->getToken());
    return false;
  }
  else
    return true;
}
*/


bool basicBlocks_liftDecls_map (Tree *t, void *i)
{
  // - find refs to orphaned tmps, lift into regs
  // - i is BehavioralOperator in which to create regs

  OperatorBehavioral *op=(OperatorBehavioral*)i;
  // warn(string("liftDecls on\n")+op->toString());
  TreeKind kind=t->getKind();
  if (   kind==TREE_STATE
      || kind==TREE_STATECASE
      || kind==TREE_STMT
      || kind==TREE_TYPE)
    return true;
  else if (t->getKind()==TREE_EXPR)
  {
    if (((Expr*)t)->getExprKind()==EXPR_LVALUE)
    {
      Symbol *sym=((ExprLValue*)t)->getSymbol();// - sym is orig ref before
						//   linking in new state/case
      // warn("basicBlocks_liftDecls_map: testing "+sym->getName());
      if (sym->isLocal())			// - i.e. if orig ref was local
      {
	SymTab *newScope=t->getScope();		// - scope is in new state/case
	Symbol *symLookup=newScope?newScope->lookup(sym->getName()):NULL;
	if (!symLookup || symLookup!=sym)
	{
	  // warn("  lifting");
	  /*
	  // paranoid debugging
	  Operator *symOp=sym->getOperator(),
		   *tOp  =  t->getOperator();
	  if (symOp!=tOp)
	  {
	    warn(string("symbol %p ",sym)+sym->getName()+" is from operator "+
		 (symOp?symOp->getName():string("<nil>"))+" but its lvalue "+
		 t->toString()+" is from operator "+
		 (tOp?tOp->getName():string("<nil>"))+"\n"+
		 (symOp?sym->getOperator()->toString()
		       :string("<nil symOp>\n"))+
		 (tOp  ?  t->getOperator()->toString()
		       :string("<nil lvalueOp>\n")));
	    assert(0);
	  }
	  if (((SymTab*)sym->getParent())->lookup(sym->getName())!=sym)
	  {
	    warn(string("symbol %p ",sym)+sym->getName()+
		 " is not in its parent\n"+sym->getParent()->toString());
	    if (symLookup)
	      warn(string("looked up %p ",symLookup)+symLookup->getName()+
		   " in parent\n"+symLookup->getParent()->toString());
	    assert(0);
	  }
	  else
	    warn(string("removing symbol %p ",sym)+sym->getName()+
		 " from parent\n"+sym->getParent()->toString());
	  */
	  SymTab *origScope=(SymTab*)sym->getScope();
	  origScope->removeSymbol(sym);
	  // warn(string("removed symbol %p ",sym)+
	  //      " from local symtab:\n"+origScope->toString());
	  uniqueRenameSym(sym,op->getVars());
	  op->getVars()->addSymbol(sym);
	  Expr *init=((SymbolVar*)sym)->getValue();
	  if (init) {
	    // - do not lift initial value assignment-- create stmt for it
	    ((SymbolVar*)sym)->setValue(NULL);
	    StmtAssign *asst=new StmtAssign(NULL,
					    new ExprLValue(NULL,sym),init);
	    StmtBlock *origScopeBlock=(StmtBlock*)origScope->getParent();
	    assert(   origScopeBlock->getKind()==TREE_STMT
		   && origScopeBlock->getStmtKind()==STMT_BLOCK);
	    asst->setParent(origScopeBlock);		// - HACK: modifying
	    origScopeBlock->getStmts()->push(asst);	//   stmt list directly
	    // - WARNING: this modifies order of initial-value assignment
	    //            (but bigger problem is that symtabs do not
	    //             keep any info on order initial-value assignment!)
	  }
	}
      }
      return true;
    }
    else
      return true;
  }
  else
    return false;
}


void liftOrphanedLocals (Tree *t, OperatorBehavioral *op)
{
  // - find "orphaned" locals in *t and promote them to registers in *op.
  // - an orphaned local is a local variable referenced in *t
  //     but not declared in *t's scope (because *t was moved/synthesized)
  // - this is just a wrapper to export basicBlocks_liftDecls_map()

  t->map(basicBlocks_liftDecls_map,(TreeMap)NULL,op);
}


bool basicBlocks_op_map (Tree *t, void *i)
{
  TreeKind kind=t->getKind();
  if (   kind==TREE_OPERATOR
      && ((Operator*)t)->getOpKind()==OP_BEHAVIORAL)
  {
    // - iteratively process all available state-cases in behavioral operators
    OperatorBehavioral *op=(OperatorBehavioral*)t;
    sortseq<string,State*> workingStates, doneStates;	// states keyed by name
    {
      State *s;				// - copy state ptrs into workingStates
      forall (s,*op->getStates())
	// workingStates.insert(s);
	workingStates.insert(s->getName(),s);
    }
    while (!workingStates.empty())
    {
      // State *s=workingStates.choose();		// get elmnt of set
      State *s=workingStates.inf(workingStates.min());	// get elmnt of sortseq
      timestamp(string("basic-blocks working state \"")+s->getName()+"\"");
      set<StateCase*> oldCases, newCases;
      StateCase *c;
      forall (c,*s->getCases())
      {
	timestamp(string("basic-blocks working state \"")+s->getName()+
		  string("\" case %p",c));
	BasicBlockInfo bbi;
	bbi.saveDecls	= new SymTab(SYMTAB_BLOCK);
	bbi.saveStmts	= new list<Stmt*>;
	bbi.deferStmts	= new list<Stmt*>;
	bbi.deferState	= new State(NULL,s->getName(),new set<StateCase*>);
	uniqueRenameState(bbi.deferState,&workingStates);
	c->map2((Tree**)&c,basicBlocks_stateCase_preMap,(TreeMap2)NULL,&bbi);
	//		   basicBlocks_stateCase_postMap,&bbi);
	// - post-map modifies state-case to leave "saved" stmts + decls

	// - replace stateCase by saved initial stmts
	StmtBlock   *saveBlock=new StmtBlock(NULL,
					     bbi.saveDecls,bbi.saveStmts);
	list<Stmt*> *saveStmtL=new list<Stmt*>;
	saveStmtL->append(saveBlock);
	StateCase   *saveCase =new StateCase(c->getToken(),c->getInputs(),
					     saveStmtL);
	oldCases.insert(c);
	newCases.insert(saveCase);
	// warn(string("shrunk StateCase to:\n")+saveCase->toString(),
	//      saveCase->getToken());

	if (bbi.deferStmts->empty())
	{
	  delete bbi.deferStmts;
	  delete bbi.deferState;
	}
	else
	{
	  // - build state for defered stmts
	  StateCase *deferCase=new StateCase(NULL,new list<InputSpec*>,
					     bbi.deferStmts);
	  bbi.deferState->addCase(deferCase);
	  /*
	  set<StateCase*> *deferCases=new set<StateCase*>;
	  deferCases->insert(deferCase);
	  static int stateCnt=1;
	  State *deferState=new State(NULL,
				      s->getName()+string("_%d",stateCnt++),
				      deferCases);
	  */
	  workingStates.insert(bbi.deferState->getName(),bbi.deferState);
	}
	State *newState;
	forall (newState,bbi.newStates)
	{
	  uniqueRenameState(newState,&workingStates);
	  workingStates.insert(newState->getName(),newState);
	}
      }
      forall (c,newCases)
	s->addCase(c);
      forall (c,oldCases)
	s->removeCase(c);
      workingStates.del(s->getName());
      doneStates.insert(s->getName(),s);
    }
    State *s;
    forall (s,*op->getStates())
      op->removeState(s);
    forall (s,doneStates)
      op->addState(s);
    forall (s,doneStates)
      s->map(basicBlocks_liftDecls_map,(TreeMap)NULL,op);	// tmps-->regs
    // warn(string("changed op to:  (pre-link)\n")+op->toString());
    op->link();
    // op->typeCheck();
    return false;
  }
  else if (kind==TREE_SUITE || kind==TREE_OPERATOR)
    return true;
  else
    return false;
}


Tree* basicBlocks (Tree *t)
{
  // - break states into "basic blocks"
  //    (if/goto/stay appears only at end; if branches are goto stmts)
  t->map(basicBlocks_op_map);
  return t;
}


////////////////////////////////////////////////////////////////
//  exline

static
bool isExlinableCall (Tree *t)
{
  // - returns true iff t is an exlinable call/builtin (expr or stmt)
  BuiltinKind b;
  return (   (   t->getKind()==TREE_STMT
	      && (   (   ((Stmt*)t)->getStmtKind()==STMT_CALL )
	          || (   ((Stmt*)t)->getStmtKind()==STMT_BUILTIN
		      && isExlinableCall(((StmtBuiltin*)t)->getBuiltin()))))
	  || (   t->getKind()==TREE_EXPR
	      && (   (   ((Expr*)t)->getExprKind()==EXPR_CALL )
	          || (   ((Expr*)t)->getExprKind()==EXPR_BUILTIN
		      && (   (b=((OperatorBuiltin*)((ExprBuiltin*)t)->getOp())
				->getBuiltinKind()) == BUILTIN_SEGMENT
			  ||  b==BUILTIN_COPY)))));
}


typedef struct {				// Operator info
  list<Stmt*>		*newStmts;		// - stmts of composeOp
  SymTab		*newWires;		// - wires in composeOp
  SymTab		*newFormalsSymtab;	// - formals of fixed behavOp
  list<Symbol*>		*newFormals;		// - formals of fixed behavOp
  list<Expr*>		*newActuals;		// - actuals of fixed behavOp
} ExlineInfo_Op;

typedef struct {				// State info
  SymTab		 *saveDecls;		// - decls saved in this state
  list<Stmt*>		 *saveStmts;		// - stmts saved in this state
  list<Stmt*>		 *deferStmts;		// - stmts moved to cont. state
  State			 *deferState;		// - where deferStmts go
  list<Expr*>		 *exlineActuals;	// - actuals of exlined call
  ExprLValue		**exlineActualRet;	// - return  of exlined call
  list<InputSpec*>	 *exlineReturns;	// - streams cont state waits 4
} ExlineInfo_State;


static
void exline_processCallArg (Symbol *cFormal, Expr *cActual, ExprCall *call,
			    ExlineInfo_Op *ei_op, ExlineInfo_State *ei_s,
			    bool createIO=true, bool doIO=true)
{
  if (cFormal->isStream())
  {
    StreamDir   cFormalDir=((SymbolStream*)cFormal)->getDir(),
	      newFormalDir=(cFormalDir==STREAM_IN?STREAM_OUT:
			    cFormalDir==STREAM_OUT?STREAM_IN:
			    cFormalDir);
    Symbol *newFormal, *newWire;
    if (createIO)
    {
      Operator *calledOp=cFormal->getOperator();
      // - was: calledOp=((ExprCall*)(cActual->getParent()))->getOp();
      // - but this fails when cFormal is return-sym & cActual is asst l-value
      string streamName=calledOp->getName()+"_"+cFormal->getName();
      Type streamType=*cFormal->getType();
      extern Tree* betaRenameInCall(Tree *renameMe, ExprCall *call); // expr.cc
      betaRenameInCall(&streamType,call);
      newFormal=new SymbolStream(NULL,streamName,
				 (Type*)streamType.duplicate(),
				 newFormalDir);
      newWire  =new SymbolVar   (NULL,streamName,
				 (Type*)streamType.duplicate());
      uniqueRenameSym(newFormal,ei_op->newFormalsSymtab);	// may not
      uniqueRenameSym(newWire,  ei_op->newWires);		// match
      ei_op->newWires->addSymbol(newWire);
      ei_op->newFormalsSymtab->addSymbol(newFormal);
      ei_op->newFormals->append(newFormal);
      ei_op->newActuals->append(new ExprLValue(NULL,newWire));
      if (cFormal==calledOp->getRetSym())
	*ei_s->exlineActualRet =   (new ExprLValue(NULL,newWire));
      else
	ei_s->exlineActuals->append(new ExprLValue(NULL,newWire));
      cFormal->setAnnote_(ANNOTE_ARRAY_NEWFORMAL,newFormal);	// for segs
    }  // if (createIO)
    else
    {
      newFormal=(Symbol*)cFormal->getAnnote(ANNOTE_ARRAY_NEWFORMAL);
      newWire  =NULL;
      assert(newFormal);
    }
    if (doIO)
    {
      if (cFormalDir==STREAM_IN)
      {
	/*
	ei_s->saveStmts->append(new StmtAssign(NULL,
					       new ExprLValue(NULL,newFormal),
					       cActual));
	*/
	StmtAssign *stmt=new StmtAssign(NULL,
					new ExprLValue(NULL,newFormal),
					cActual);
	ei_s->saveStmts->append(stmt);
      }
      else  // cFormalDir==STREAM_OUT/_ANY
      {
	ei_s->exlineReturns->append(new InputSpec(NULL,newFormal,NULL,false,false));
	/*
	ei_s->deferStmts->append(new StmtAssign(NULL,
						(ExprLValue*)cActual,
						new ExprLValue(NULL,
							       newFormal)));
	*/
	StmtAssign *stmt=new StmtAssign(NULL,
					(ExprLValue*)cActual,
					new ExprLValue(NULL,
						       newFormal));
	ei_s->deferStmts->append(stmt);

      }
    }  // if (doIO)
  }  // if (cFormal->isStream())
  else if (cFormal->isParam())
  {
    if (createIO)
    {
      assert(cActual);
      ei_s->exlineActuals->append(cActual);
    }
  }
  else
    assert(!"formal is neither stream nor param");
}


void exline_processCall (ExprCall *call,
			 ExlineInfo_Op *ei_op, ExlineInfo_State *ei_s)
{
  // - cFormal/cActual are for call inside orig behav op
  // - newFormal/newWire are for call to fixed op in compositional op
  // - exlineActuals is for lifted call in compositional op
  Operator *calledOp=call->getOp();
  list<Symbol*>		*cFormals=calledOp->getArgs();
  list<Expr*>		*cActuals=call->getArgs();
  list_item formal, actual;
  for (formal=cFormals->first(),
       actual=cActuals->first();
       formal && actual;
       formal=cFormals->succ(formal),
       actual=cActuals->succ(actual))
  {
    // - process all args of call
    Symbol *cFormal=cFormals->inf(formal);
    Expr   *cActual=cActuals->inf(actual);
    exline_processCallArg(cFormal,cActual,call,ei_op,ei_s);
  }  // for (actuals,formals) of call
  Tree *p=call->getParent();
  if (   p
      && p->getKind()==TREE_STMT
      && ((Stmt*)p)->getStmtKind()==STMT_ASSIGN
      && ((StmtAssign*)p)->getRhs()==call)
  {
    // - call is RHS of assignment, so process return stream
    Symbol *cFormal=calledOp->getRetSym();
    Expr   *cActual=((StmtAssign*)p)->getLValue();
    exline_processCallArg(cFormal,cActual,call,ei_op,ei_s);
  }
  // - add goto-continuation stmt
  ei_s->saveStmts->append(new StmtGoto(NULL,ei_s->deferState));
}


void exline_processMemRef (StmtAssign *stmt,
			   ExlineInfo_Op *ei_op, ExlineInfo_State *ei_s)
{
  ExprLValue *lvalue	=(ExprLValue*)stmt->getLValue();
  ExprLValue *rhs	=(ExprLValue*)stmt->getRhs();
  bool        reading	=(rhs->getArrayLoc()!=NULL);
  Symbol     *arraySym	=reading ? (SymbolVar*)rhs->getSymbol()
				 : (SymbolVar*)lvalue->getSymbol();
  Expr	     *arrayLoc	=reading ? rhs->getArrayLoc() : lvalue->getArrayLoc();
  TypeArray  *arrayType	=(TypeArray*)arraySym->getType();
  bool        rwSeg	=(bool)arraySym->getAnnote(ANNOTE_ARRAY_READ)==true &&
			 (bool)arraySym->getAnnote(ANNOTE_ARRAY_WRITTEN)==true;

  OperatorSegment *seg	=(OperatorSegment*)
					arraySym->getAnnote(ANNOTE_ARRAY_SEG);
  bool firstTime=(seg==NULL);
  if (firstTime)
  {
    seg=makeOperatorSegment(NULL,
			    reading ? (rwSeg ? SEGMENT_RW : SEGMENT_R)
				    : (rwSeg ? SEGMENT_RW : SEGMENT_W),
			    arrayType->getElemType());
    arraySym->setAnnote_(ANNOTE_ARRAY_SEG,seg);
  }

  list<Symbol*>	*cFormals=seg->getArgs();
  list_item	  formal;
  Symbol	*cFormal;
  Expr		*cActual;
  //ExprBuiltin	*tmpCall;	// partial list of args for beta-renaming

  // - dwidth
  {
    formal =cFormals->first();
    cFormal=cFormals->inf(formal);
    cActual=arrayType->getElemType()->makeWidthExpr();
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,true);
  }

  // - awidth
  {
    formal =cFormals->succ(formal);
    cFormal=cFormals->inf(formal);
    cActual=constIntExpr(32);		// HACK - using fixed awidth=32
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,true);
  }

  // - nelems
  {
    formal =cFormals->succ(formal);
    cFormal=cFormals->inf(formal);
    cActual=arrayType->makeNelemsExpr();
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,true);
  }

  // - contents
  {
    formal =cFormals->succ(formal);
    cFormal=cFormals->inf(formal);
    cActual=new ExprLValue(NULL,arraySym);
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,true);
  }

  // - addr
  {
    formal =cFormals->succ(formal);
    cFormal=cFormals->inf(formal);
    cActual=arrayLoc;
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,true);
  }

  // - dataR
  if (reading || rwSeg)
  {
    formal =cFormals->succ(formal);
    cFormal=cFormals->inf(formal);
    cActual=reading?lvalue:NULL;
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,
								reading);
  }

  // - dataW
  if (!reading || rwSeg)
  {
    formal =cFormals->succ(formal);
    cFormal=cFormals->inf(formal);
    cActual=(!reading)?rhs:NULL;
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,
								!reading);
  }

  // - write
  if (rwSeg)
  {
    formal =cFormals->succ(formal);
    cFormal=cFormals->inf(formal);
    cActual=new ExprValue(NULL,(Type*)type_bool->duplicate(),!reading, 0);
    ExprBuiltin tmpCall(NULL, new list<Expr*>(*ei_s->exlineActuals), seg);
    exline_processCallArg(cFormal,cActual,&tmpCall,ei_op,ei_s, firstTime,true);
  }

  // - add goto-continuation stmt
  ei_s->saveStmts->append(new StmtGoto(NULL,ei_s->deferState));

  // - move mem to composeOp + create exlined call
  if (firstTime)
  {
    if (   !arraySym->isParam()				// !param of orig bhvOp
	&& !ei_op->newFormalsSymtab
		 ->lookup(arraySym->getName()))		// !param of new cmpzOp
    {
      ((SymTab*)arraySym->getParent())->removeSymbol(arraySym);
      ei_op->newWires->addSymbol(arraySym);
      // - this code lifts local mem arrays from (dup of) original behav op
      //     up into new compositional op.
      // - 1st cond of if checks if arraySym is param of original behav op
      // - 2nd cond of if checks if arraySym is param of new compose op,
      // - if the stmt referencing the arraySym was created during
      //     exlining, it refers to arraySym in the new compose op,
      //     not the orig compose op.
    }
    ExprBuiltin *call=new ExprBuiltin(NULL,ei_s->exlineActuals,seg);
    ei_op->newStmts->append(new StmtBuiltin(NULL,call));
  }
}


bool exline_op_map (Tree **h, void *i)
{
  Tree *t=*h;
  TreeKind kind=t->getKind();
  if (   kind==TREE_OPERATOR
      && ((Operator*)t)->getOpKind()==OP_BEHAVIORAL)
  {
    // - this behav operator gets new states + new streams
    // - this behav operator + all called ops get enclosed in compositional op

    OperatorBehavioral	*origOp=(OperatorBehavioral*)t;	// orig. op
    OperatorBehavioral  *dupOp =(OperatorBehavioral*)origOp->duplicate();
			 dupOp->link();			// duplicate to modify
    OperatorBehavioral	*behavOp=NULL;			// op with new states
    OperatorCompose	*composeOp=NULL;		// new enclosing op
    list<Symbol*>	*newFormals=new list<Symbol*>;	// for behavOp
    list<Expr*>		*newActuals=new list<Expr*>;	// for behavOp in compz
    SymTab		*newFormalsSymtab=new SymTab(SYMTAB_OP);
    SymTab		*newWires	 =new SymTab(SYMTAB_BLOCK);
    list<Stmt*>		*newStmts	 =new list<Stmt*>;
    ExlineInfo_Op ei_op={newStmts,
			 newWires,
			 newFormalsSymtab,
			 newFormals,
			 newActuals};

    // dupOp->getSymtab()->addChild(newWires);		// HACK for renaming
    // - this forces wires in compose op to have names distinct from
    //   original op's i/o's (which become i/o's of compose op)
    // - must undo this hack before linking new behav op & compose op

    {
      Symbol *sym;					// copy behav's formals
      forall (sym,*dupOp->getArgs())
      {
	newActuals->append(new ExprLValue(NULL,sym));
	newFormals->append(sym);
	newFormalsSymtab->addSymbol(sym);
      }
    }
    sortseq<string,State*> workingStates, doneStates;	// states keyed by name
    {
      State *s;				// - copy state ptrs into workingStates
      forall (s,*dupOp->getStates())
	workingStates.insert(s->getName(),s);
    }
    while (!workingStates.empty())
    {
      State *s=workingStates.inf(workingStates.min());	// get elmnt of sortseq
      timestamp(string("exline working state \"")+s->getName()+"\"");
      set<StateCase*> oldCases, newCases;
      StateCase *c;
      forall (c,*s->getCases())
      {
	timestamp(string("exline working state \"")+s->getName()+
		  string("\" case %p",c));
	SymTab		 *saveDecls	 = new SymTab(SYMTAB_BLOCK);
	list<Stmt*>	 *saveStmts	 = new list<Stmt*>;
	list<Stmt*>	 *deferStmts	 = new list<Stmt*>;
	list<Expr*>	 *exlineActuals	 = new list<Expr*>;	// of exld call
	ExprLValue	 *exlineActualRet= NULL;		// of exld call
	list<InputSpec*> *exlineReturns	 = new list<InputSpec*>;// of exld call
	State		 *deferState	 = new State(NULL,s->getName(),
						    new set<StateCase*>);
	uniqueRenameState(deferState,&workingStates);	// add to wrkStts later
	ExlineInfo_State ei_s={saveDecls,saveStmts,deferStmts,deferState,
			       exlineActuals,&exlineActualRet,exlineReturns};
	bool exlineFound=false;		// true if found exline & munged state

	list<Stmt*> pendingStmts(*c->getStmts());
	while (!pendingStmts.empty())
	{
	  // warn(string("pending stmts:\n")+listToString(&pendingStmts));
	  // warn(string("save    stmts:\n")+listToString(saveStmts));
	  // warn(string("defer   stmts:\n")+listToString(deferStmts));
	  // warn(string("new     stmts:\n")+listToString(newStmts));
	  Stmt *stmt=pendingStmts.pop_front();
	  switch (stmt->getStmtKind())
	  {
	    case STMT_BLOCK:
	      mergeSymtabs(saveDecls,((StmtBlock*)stmt)->getSymtab());
	      pushList(&pendingStmts,((StmtBlock*)stmt)->getStmts());
	      goto nextPendingStmt;

	    case STMT_CALL:
	    case STMT_BUILTIN:
	      {
		ExprCall *call=((StmtCall*)stmt)->getCall();
		if (isExlinableCall(call))
		{
		  exline_processCall(call,&ei_op,&ei_s);
		  if (stmt->getStmtKind()==STMT_CALL)
		    newStmts->append(new StmtCall
					   (NULL,
					    new ExprCall(NULL,exlineActuals,
							 call->getOp())));
		  else // STMT_BUILTIN
		    newStmts->append(new StmtBuiltin
					   (NULL,
					    new ExprBuiltin(NULL,exlineActuals,
							    (OperatorBuiltin*)
							    call->getOp())));
		  exlineFound=true;
		  appendList(deferStmts,&pendingStmts);
		  pendingStmts.clear();
		  goto nextPendingStmt;	// none left, so do next state
		}  // if (isExlinableCall(exprCall))
		else
		  saveStmts->append(stmt);
	      }	 // case STMT_CALL, STMT_BUILTIN
	      break;

	    case STMT_ASSIGN:
	      {
		Expr       *rhs   =((StmtAssign*)stmt)->getRhs();
		ExprLValue *lvalue=((StmtAssign*)stmt)->getLValue();
		if (   (   rhs->getExprKind()==EXPR_CALL
			|| rhs->getExprKind()==EXPR_BUILTIN)
		    && (isExlinableCall((ExprCall*)rhs))    )
		{
		  // - assignment RHS is call
		  ExprCall *call=(ExprCall*)rhs;
		  exline_processCall(call,&ei_op,&ei_s);
		  // - note, above call also handles return-val assignment
		  if (call->getExprKind()==EXPR_CALL)
		    newStmts->append(new StmtAssign
					   (NULL,
					    exlineActualRet,
					    new ExprCall(NULL,exlineActuals,
							 call->getOp())));
		  else // EXPR_BUILTIN
		    newStmts->append(new StmtAssign
					   (NULL,
					    exlineActualRet,
					    new ExprBuiltin(NULL,exlineActuals,
							    (OperatorBuiltin*)
							    call->getOp())));
		  exlineFound=true;
		  appendList(deferStmts,&pendingStmts);
		  pendingStmts.clear();
		  goto nextPendingStmt;	// none left, so do next state
		}  // if (isExlinableCall...)
		else if (   (   lvalue->getArrayLoc())
			 || (   rhs->getExprKind()==EXPR_LVALUE
			     && ((ExprLValue*)rhs)->getArrayLoc()))
		{
		  // - assignment LHS is mem-array write,
		  //     or asst. RHS is mem-array read
		  exline_processMemRef((StmtAssign*)stmt,&ei_op,&ei_s);
		  // - writes into newStmts once per array
		  exlineFound=true;
		  appendList(deferStmts,&pendingStmts);
		  pendingStmts.clear();
		  goto nextPendingStmt;	// none left, so do next state
		}  // if (mem ref)
		else
		  saveStmts->append(stmt);
	      }
	      break;

	    default:
	      saveStmts->append(stmt);
	      break;
	  }  // switch (stmt->getStmtKind())
	  nextPendingStmt:
	  {}
	}  // while (!pendingStmts.empty())

	if (exlineFound)
	{
	  // - create state-case for saved stmts
	  StmtBlock *saveBlock=new StmtBlock(NULL,saveDecls,saveStmts);
	  list<Stmt*> *saveStmtL=new list<Stmt*>;
	  saveStmtL->append(saveBlock);
	  StateCase *saveCase=new StateCase(NULL,c->getInputs(),saveStmtL);
	  oldCases.insert(c);
	  newCases.insert(saveCase);

	  if (!deferStmts->empty())
	  {
	    // - create state for defered stmts (continuation state)
	    StmtBlock *deferBlock=new StmtBlock(NULL,new SymTab(SYMTAB_BLOCK),
						deferStmts);
	    list<Stmt*> *deferStmtL=new list<Stmt*>;
	    deferStmtL->append(deferBlock);
	    StateCase *deferCase=new StateCase(NULL,exlineReturns,deferStmtL);
	    /*
	      set<StateCase*> *deferCases=new set<StateCase*>;
	      deferCases->insert(deferCase);
	      State *deferState=new State(NULL,s->getName(),deferCases);
	      uniqueRenameState(deferState,&workingStates);
	    */
	    deferState->addCase(deferCase);
	    workingStates.insert(deferState->getName(),deferState);
	  }  // if (!deferStmts->empty())
	}  // if (!exlineStmt.empty())
      }  // forall (c,*s->getCases())

      // - the rest of this fn. is stolen from basicBlocks_Op_map()

      forall (c,newCases)
	s->addCase(c);
      forall (c,oldCases)
	s->removeCase(c);
      workingStates.del(s->getName());
      doneStates.insert(s->getName(),s);
    }  // while (!workingStates.empty())

    if (!newStmts->empty())
    {
      // dupOp->getSymtab()->removeChild(newWires);	// HACK for renaming

      // - build new behavioral operator
      dictionary<string,State*> *newStateDict=new dictionary<string,State*>;
      State *s;
      forall (s,doneStates)
	newStateDict->insert(s->getName(),s);
      string startStateName=dupOp->getStartState()->getName();
      State *newStartState=newStateDict->access(startStateName);
      assert(newStartState);
      behavOp=new OperatorBehavioral(dupOp->getToken(),
				     dupOp->getName(),		// rename later
				     (Symbol*)dupOp->getRetSym()->duplicate(),
				     newFormals,
				     dupOp->getVars(),
				     newStateDict,
				     newStartState);
      // warn(string("new behav op:  (pre-lift)\n")+behavOp->toString());
      forall (s,doneStates)
	// - do this _after_ adding states into behavOp
	s->map(basicBlocks_liftDecls_map,(TreeMap)NULL,behavOp); // tmps-->regs
      // warn(string("new behav op:  (pre-link)\n")+behavOp->toString());
      behavOp->link();			// ok w/o suite since removed all calls
      behavOp->typeCheck();
      behavOp->setName(dupOp->getName()+"_noinline");	// unique-rename later

      // - insert new behavioral op into origOp's enclosing suite
      // - new compose op is added to suite via "*h=composeOp"
      if (origOp->getParent()->getKind()==TREE_SUITE)
      {
	Suite *suite=(Suite*)origOp->getParent();
	uniqueRenameOperator(behavOp,suite->getSymtab());
	suite->addOperator(behavOp);
      }

      // - build new compositional operator  (steal syms/args from dupOp)
      ExprCall *newBehavCall=new ExprCall(NULL,newActuals,behavOp);
      Symbol *dupOpRetSym=dupOp->getRetSym();
      Stmt     *newBehavStmt=(   dupOpRetSym
			      && dupOpRetSym->getType()->
					      getTypeKind()!=TYPE_NONE)
			    ? (Stmt*)
			      new StmtAssign(NULL,
					     new ExprLValue(NULL,dupOpRetSym),
					     newBehavCall)
			    : (Stmt*)
			      new StmtCall(NULL,newBehavCall);
      newStmts->append(newBehavStmt);
      composeOp=new OperatorCompose(NULL,
				    dupOp->getName(),
				    dupOp->getRetSym(),	// - not a duplicate!
				    dupOp->getArgs(),
				    newWires,
				    newStmts);
      // warn(string("new compose op:  (pre-link)\n")+composeOp->toString());
      *h=composeOp;			// - replace origOp by composeOp
      composeOp->link();		// - presumably done in a suite
      composeOp->typeCheck();

      return false;
    }  // if (!newStmts->empty())
    else
      return false;		// - leave origOp alone
  }  // if (behavioral operator)
  else if (kind==TREE_SUITE || kind==TREE_OPERATOR)
    return true;
  else
    return false;
}


Tree* exline (Tree **h)
{
  // - exline all inlined calls and segment operator references
  // - replaces a behavioral operator by a shell compositional operator
  //     containing modified behavioral operator and instance of called op.
  // - requires relinking any operator which calls this exlined operator
  //     (because it is replaced)

  (*h)->map2(h,exline_op_map);
  if ((*h)->getKind()==TREE_SUITE)
    (*h)->link();			// - relink entire suite, if given
					// - redundant for exlined operators
  return *h;
}


////////////////////////////////////////////////////////////////
//  removeDumbStates


bool bypassDumbStates_map (Tree *t, void *i)
{
  // - Bypass dumb states, i.e. inputless states that contain only "goto"
  // - Do this by modifying "goto dumbstate;" by "goto target;"
  //     where "target" is the target of the "goto" in "dumbstate"
  // - Chase goto's thru multiple dumb states
  // - Also modify start state of behav-op if it is a dumb state
  // - Do NOT bypass infinite state loops (e.g. dumb state with "stay")
  // - Do NOT remove dumb states from operator

  switch (t->getKind())
  {
    case TREE_OPERATOR: if (((Operator*)t)->getOpKind()==OP_BEHAVIORAL) {
			  // - special case for start state of behav op:
			  //     process a fake 'goto startstate' stmt
			  OperatorBehavioral *op=(OperatorBehavioral*)t;
			  State *start=op->getStartState();
			  StmtGoto stmt(NULL,start);
			  bypassDumbStates_map(&stmt,NULL);
			  if (start!=stmt.getState())
			    op->setStartState(stmt.getState());
			}
			return true;

    case TREE_STMT:	if (((Stmt*)t)->getStmtKind()==STMT_GOTO) {
			  State *target=((StmtGoto*)t)->getState();
			  set<StateCase*> *cases=target->getCases();
			  if (cases->size()==1) {
			    StateCase *acase=cases->choose();
			    if (acase->getInputs()->size()==0) {
			      Stmt *stmt=acase->getStmts()->head();
			      while (stmt->getStmtKind()==STMT_BLOCK)
				stmt=(((StmtBlock*)stmt)->getStmts()->head());
			      if (stmt->getStmtKind()==STMT_GOTO) {
				State *target2=((StmtGoto*)stmt)->getState();
				if (target2!=target) {	// - ignore stay
				  // - HACK: modify StmtGoto, token stays
				  ((StmtGoto*)t)->setState(target2);
				  bypassDumbStates_map(t,NULL);
				}
			      }
			    }
			  }
			  return false;
			}
			else
			  return true;

    case TREE_TYPE:
    case TREE_EXPR:	return false;

    default:		return true;
  }
}


bool findReferencedStates_map (Tree *t, void *i)
{
  // - find states referenced as targets of "goto" stmts
  //   and add them to set<State*> *i

  switch (t->getKind())
  {
    case TREE_STMT:	if (((Stmt*)t)->getStmtKind()==STMT_GOTO) {
			  set<State*> *referencedStates=(set<State*>*)i;
			  referencedStates->insert(((StmtGoto*)t)->getState());
			  return false;
			}
			else
			  return true;
    case TREE_TYPE:
    case TREE_EXPR:	return false;
    default:		return true;
  }
}


bool removeUnreferencedStates_map (Tree *t, void *i)
{
  // - remove states that are not referenced by any "goto"  (except start)

  if (t->getKind()==TREE_SUITE)
    return true;
  else if (t->getKind()==TREE_OPERATOR
	   && ((Operator*)t)->getOpKind()==OP_BEHAVIORAL) {
    OperatorBehavioral *op=(OperatorBehavioral*)t;
    set<State*> referencedStates;
    op->map(findReferencedStates_map,(TreeMap)NULL,(void*)&referencedStates);
    dictionary<string,State*> states=*op->getStates();
    State *s;
    forall (s,states)
      if (!referencedStates.member(s) && s!=op->getStartState()) {
	// warn(string("Removing unreferenced state \"")+s->getName()+
	//      string("\" in op \"")+op->getName()+"\"");
	op->removeState(s);
      }
    return false;
  }
  else
    return false;
}


Tree* removeDumbStates (Tree *t)
{
  t->map(bypassDumbStates_map);		  // - bypass dumb "goto" states
  t->map(removeUnreferencedStates_map);	  // - remove unreferenced states
  return t;
}


////////////////////////////////////////////////////////////////
//  infer + generate copy()

class FanoutInfo {		// - fanout info for a stream
public:
  int fanout;			// - # of readers
  ExprLValue *write;		// - ref  in compos op denoting stream write
				//          (may be NULL for input stream)
  list<ExprLValue*> reads;	// - refs in compos op denoting stream reads
				//          (may be empty for output stream)
  FanoutInfo () : fanout(0), write(NULL) {}
};
typedef dictionary<Symbol*,FanoutInfo> SymbolFanoutInfos;

ostream& operator<< (ostream &o, FanoutInfo const&) {return o;} // LEDA 371 bug


bool fillFanoutInfos_map (Tree *t, void *i)
{
  // - discover stream fanout (multiple readers) in a compositional operator

  SymbolFanoutInfos *fanoutInfos = (SymbolFanoutInfos*)i;
  switch (t->getKind()) {
    case TREE_OPERATOR:
    case TREE_STMT:
      return true;
    case TREE_EXPR: {
      if (((Expr*)t)->getExprKind()==EXPR_LVALUE) {
	ExprLValue *lval=(ExprLValue*)t;
	Symbol *s=lval->getSymbol();
	OperatorCompose *op=(OperatorCompose*)lval->getOperator();
	if (   (s->getSymKind()==SYMBOL_STREAM)		// - s is primary IO
	    || (s->getParent()==op->getVars() &&	// - s is local stream
		s->getType()->getTypeKind()!=TYPE_ARRAY)) {
	  // - found a stream reference (either primary IO or local stream)
	  // - HACK:  if should be "if (s->isStream() || s->isLocal())",
	  //          but this code may run before linking,
	  //          when symbol *s has bad ptrs,
	  //          so we use lval to deduce whether *s is stream
	  //          (hmm, this runs after linking, but *s still has bad ptrs)
	  // warn(string("inferCopyOps - fillFanoutInfos_map(\"")+
	  //      s->getName()+"\") is a stream ref");
	  dic_item i=(*fanoutInfos).lookup(s);
	  if (!i)
	    i=(*fanoutInfos).insert(s,FanoutInfo());
	  FanoutInfo &fanoutInfo=(*fanoutInfos)[i];
	  if (isInAsstContext(lval)) {
	    // - found a write
	    // warn(string("inferCopyOps - write \"")+s->getName()+"\"",
	    //      lval->getToken());
	    if (fanoutInfo.write) {
	      warn(string("found multiple writers for stream \"")+
		   lval->getSymbol()->getName()+"\", first write here",
		   fanoutInfo.write->getToken());
	      fatal(1, string("found multiple writers for stream \"")+
		       lval->getSymbol()->getName()+"\"", lval->getToken());
	    }
	    fanoutInfo.write=lval;
	  }
	  else {
	    // - found a read
	    // warn(string("inferCopyOps - read \"")+s->getName()+"\"",
	    //      lval->getToken());
	    fanoutInfo.fanout++;
	    fanoutInfo.reads.append(lval);
	  }
	}
	return false;
      }
      else {	// if (((Expr*)t)->getExprKind()==EXPR_LVALUE)
	return true;
      }
    }
    default:
      return false;
  }
}


void inferCopyOps_composeOp (OperatorCompose *op)
{
  // - infer fanout in a compositional operator
  //     (fanout = stream w/multiple readers)
  //     and convert to use appropriate copy() calls

  // warn (string("inferCopyOps_composeOp(\"")+op->getName()+"\")");

  // - find fanouts:
  SymbolFanoutInfos fanoutInfos;
  op->map(fillFanoutInfos_map,(TreeMap)NULL,(void*)&fanoutInfos);

  // - modify fanouts to use new fanout streams:
  dic_item i;
  forall_items (i,fanoutInfos) {
    Symbol              *s=fanoutInfos.key(i);
    FanoutInfo &fanoutInfo=fanoutInfos[i];
    // warn(string("inferCopyOps: symbol \"")+s->getName()+
    //      string("\" has %d writer(s) and %d reader(s)",
    //             (fanoutInfo.write?1:0),fanoutInfo.fanout));
    bool s_is_primary_io = (s->getParent()==op->getSymtab()); // - is not local
    bool s_is_primary_i  = s_is_primary_io &&
				((SymbolStream*)s)->getDir()==STREAM_IN;
    bool s_is_primary_o  = s_is_primary_io &&
				((SymbolStream*)s)->getDir()==STREAM_OUT;
    bool s_is_used = (fanoutInfo.write!=NULL || fanoutInfo.fanout>0
					     || s_is_primary_io);
    if (fanoutInfo.write==NULL && s_is_used && !s_is_primary_i)
      fatal(1,string("stream \"")+s->getName()+"\" has no writer",
            s->getToken());
    if (fanoutInfo.fanout==0   && s_is_used && !s_is_primary_o)
      warn(string("stream \"")+s->getName()+"\" has no readers",
	   s->getToken());
      // - assume that unread stream is not fatal, can be handled by scheduler
    if (fanoutInfo.fanout>1) {
      // - need to insert a copy operator
      ExprLValue *fanout;
      forall (fanout,fanoutInfo.reads) {
	// - for each fanout (reader), create fanout stream...
	// - note, fanout streams do NOT get orig stream's init value and depth
	SymbolVar *s_fanout=new SymbolVar(NULL,s->getName(),s->getType());
	uniqueRenameSym(s_fanout,op->getVars());
	op->getVars()->addSymbol(s_fanout);
	// - ...and modify reader to use fanout stream
	fanout->setSymbol(s_fanout);
      }
      // - now create copy() call
      list<Expr*> *args=new list<Expr*>;
      args->append(fanoutInfo.write ?(ExprLValue*)fanoutInfo.write->duplicate()
				    :new ExprLValue(NULL,s));
      forall (fanout,fanoutInfo.reads)
	args->append((Expr*)fanout->duplicate());
      ExprBuiltin *eb=new ExprBuiltin(NULL,args,builtin_copy);
      StmtBuiltin *sb=new StmtBuiltin(NULL,eb);
      op->getStmts()->append(sb);	// - HACK: modifying stmt list directly
      sb->setParent(op);
    }
  }
}


bool inferCopyOps_map (Tree *t, void *i)
{
  // - visit each compositional operator
  switch (t->getKind()) {
    case TREE_SUITE:
      return true;
    case TREE_OPERATOR:
      if (((Operator*)t)->getOpKind()==OP_COMPOSE)
	inferCopyOps_composeOp((OperatorCompose*)t);
      return false;
    default:
      return false;
  }
}


Tree* inferCopyOps (Tree *t)
{
  // - infer fanout in all compositional operator subtrees
  //     (fanout = stream w/multiple readers)
  //     and convert to use appropriate copy() calls

  t->map(inferCopyOps_map);
  return t;
}


OperatorBehavioral* makeCopyOperator (Token *token_i,
				      const Type *dataType_i, int fanout)
{
  // - create and return a behavioral operator to implement copy()
  //     for a given fanout and data type
  // - will use an existing op in gSuite, but will not add new op to gSuite
  // - prototypes:
  //     _copy_bN(input boolean i, output boolean o1, ..., output boolean oN);
  //     _copy_uN(param  unsigned[7] w,
  //              input  unsigned[w] i,
  //              output unsigned[w] o1, ..., output unsigned[w] oN);
  //     _copy_sN(param  unsigned[7] w,
  //              input  signed[w] i,
  //              output signed[w] o1, ..., output signed[w] oN);
  //     _copy_fuN(param  unsigned[7] wi, param unsigned[7] wf,
  //               input  unsigned[wi.wf] i,
  //               output unsigned[wi.wf] o1, ...,output unsigned[wi.wf] oN);
  //     _copy_fsN(param  unsigned[7] wi, param unsigned[7] wf,
  //               input  signed[wi.wf] i,
  //               output signed[wi.wf] o1, ...,output signed[wi.wf] oN);

  TypeKind typeKind=dataType_i->getTypeKind();
  assert(typeKind==TYPE_BOOL || typeKind==TYPE_INT || typeKind==TYPE_FIXED || typeKind==TYPE_FLOAT || typeKind==TYPE_DOUBLE);
  assert(fanout>1);
  bool sign=dataType_i->isSigned();

  // - create op name
  string opName("_copy_%s%d",
		(typeKind==TYPE_BOOL  ? ("b")                :
		 typeKind==TYPE_INT   ? (sign ?  "s" :  "u") :
		 typeKind==TYPE_FIXED ? (sign ? "fs" : "fu") : "x"),
		fanout);
  Operator* op=gSuite->lookupOperator(opName);
  if (op) {			// - already have this op, return ref to it
    assert(op->getOpKind()==OP_BEHAVIORAL);
    return (OperatorBehavioral*)op;
  }

  // - create op args (param+streams)
  list<Symbol*> *args=new list<Symbol*>;
  Type *streamType;
  if (typeKind==TYPE_BOOL) {
    streamType=new Type(TYPE_BOOL);
  }
  else if (typeKind==TYPE_FLOAT) {
    streamType=new Type(TYPE_FLOAT);
  }
  else if (typeKind==TYPE_DOUBLE) {
    streamType=new Type(TYPE_DOUBLE);
  }
  else if (typeKind==TYPE_INT) {
    SymbolVar *param_w=new SymbolVar(NULL,"w",
				     new Type(TYPE_INT,7,false));
    streamType=new Type(TYPE_INT,new ExprLValue(NULL,param_w),
			dataType_i->isSigned());
    args->append(param_w);
  }
  else if (typeKind==TYPE_FIXED) {
    SymbolVar *param_wi=new SymbolVar(NULL,"wi",
				      new Type(TYPE_INT,7,false));
    SymbolVar *param_wf=new SymbolVar(NULL,"wi",
				      new Type(TYPE_INT,7,false));
    streamType=new TypeFixed(new ExprLValue(NULL,param_wi),
			     new ExprLValue(NULL,param_wf),
			     dataType_i->isSigned());
    args->append(param_wi);
    args->append(param_wf);
  }
  SymbolStream *stream_i=new SymbolStream(NULL,"i",
					  (Type*)streamType->duplicate(),
					  STREAM_IN);
  args->append(stream_i);
  array<SymbolStream*> stream_o(fanout);
  int n;
  for (n=0; n<fanout; n++) {
    stream_o[n]=new SymbolStream(NULL,string("o%d",n+1),
				 (Type*)streamType->duplicate(),
				 STREAM_OUT);
    args->append(stream_o[n]);
  }

  // - create op FSM
  list<Stmt*> *stmts=new list<Stmt*>;
  for (n=0; n<fanout; n++) {
    StmtAssign *assign=new StmtAssign(NULL,
				      new ExprLValue(NULL,stream_o[n]),
				      new ExprLValue(NULL,stream_i));
    stmts->append(assign);
  }
  InputSpec *ispec=new InputSpec(NULL,stream_i,NULL,false,false); 
  list<InputSpec*> *ispecs=new list<InputSpec*>;
  ispecs->append(ispec);
  StateCase *stateCase=new StateCase(NULL,ispecs,stmts);
  set<StateCase*> *stateCases=new set<StateCase*>;
  stateCases->insert(stateCase);
  State *state=new State(NULL,"only",stateCases);
  stmts->append(new StmtGoto(NULL,state));	// - 'stay' stmt
  dictionary<string,State*> *states=new dictionary<string,State*>;
  states->insert("only",state);

  // - create EOFR escape path (Added by Nachiket onn 1/2/2010 after an awesome debugging session)
  list<Stmt*> *stmts1=new list<Stmt*>;
  for (n=0; n<fanout; n++) {
    // copied from parse_tdf.y
    ExprLValue *el=new ExprLValue(NULL,stream_o[n]);
    list<Expr*> *args=new list<Expr*>;
    args->append(el);
    ExprBuiltin *eb=new ExprBuiltin(NULL,args,builtin_frameclose);
    StmtBuiltin *builtin=new StmtBuiltin(NULL,eb);
    stmts1->append(builtin);
  }
  InputSpec *ispec1=new InputSpec(NULL,stream_i,NULL,false,true); // set as EOFR case
  list<InputSpec*> *ispecs1=new list<InputSpec*>;
  ispecs1->append(ispec1);
  StateCase *stateCase1=new StateCase(NULL,ispecs1,stmts1);
  stateCases->insert(stateCase1);
  State *state1=new State(NULL,"only",stateCases);
  stmts1->append(new StmtGoto(NULL,state1));	// - 'stay' stmt


  OperatorBehavioral *bop
    = new OperatorBehavioral(token_i, opName,
			     (Symbol*)sym_none->duplicate(),
			     args, new SymTab(SYMTAB_BLOCK), states, state);
  return bop;

  // - note, we do not automatically add the op to gSuite
}


bool expandCopyOps_map (Tree **h, void *i)
{
  // - see expandCopyOps()

  switch ((*h)->getKind()) {
    case TREE_SUITE:
      return true;
    case TREE_OPERATOR:
      return (((Operator*)(*h))->getOpKind()==OP_COMPOSE);
    case TREE_STMT:
      if (((Stmt*)(*h))->getStmtKind()==STMT_BUILTIN) {
	ExprBuiltin *eb=((StmtBuiltin*)(*h))->getBuiltin();
	if (((OperatorBuiltin*)eb->getOp())->getBuiltinKind()==BUILTIN_COPY) {
	  // - found copy() call, now generate behav op for it
	  list<Expr*> *args=eb->getArgs();
	  int fanout=args->length()-1;
	  Type *dataType=args->head()->getType();  // - infer type from input
	  OperatorBehavioral *copyOp=makeCopyOperator(NULL,dataType,fanout);
	  if (!gSuite->lookupOperator(copyOp->getName()))
	    gSuite->addOperator(copyOp);
	  // - replace copy() call with call to copyOp (behav op _copy_XX)
	  // - note, we cannibalize the copy() ExprCall
	  switch (dataType->getTypeKind()) {
	    case TYPE_BOOL:
	    case TYPE_FLOAT:
	    case TYPE_DOUBLE:
	      break;
	    case TYPE_INT:
	      args->push(dataType->makeWidthExpr());
	      break;
	    case TYPE_FIXED:
	      args->push(((TypeFixed*)dataType)->makeFracWidthExpr());
	      args->push(((TypeFixed*)dataType)->makeFracWidthExpr());
	      break;
	    default:
	      assert(!"internal inconsistency");
	  }
	  ExprCall *ec=new ExprCall(eb->getToken(),args,copyOp);
	  StmtCall *sc=new StmtCall((*h)->getToken(),ec);
	  *h=sc;	// - replace copy() StmtCall here
        }
      }
      return false;
    default:
      return false;
  }
}


Tree* expandCopyOps (Tree **h)
{
  // - expand calls to copy() builtin (from compositional operators)
  //     into calls to corresponding behavioral operator _copy_XN()
  // - generate, add, and reuse _copy_XN behavioral operators in gSuite

  (*h)->map2(h,expandCopyOps_map);
  return *h;
}


Tree* doCopyOps (Tree **h)
{
  inferCopyOps(*h);		// - infer + create copy() calls for fanout
  expandCopyOps(h);		// - create behavioral code for copy() builtins
  return *h;
}


////////////////////////////////////////////////////////////////
//  checkGotos

bool checkGotos_map (Tree *t, void *i)
{
  // - consistency check on goto stmts:
  //     (1) gotoStmt->getState()->getName() == gotoStmt->getStateName()
  //     (2) gotoStmt->getOp()->lookupState(gotoStmt->getState())
  //     (3) gotoStmt->getState()->getOp()
  //     (4) gotoStmt->getState()->getOp() == gotoStmt()->getOp()
  

  static OperatorBehavioral *op;
  static State *state;
  static Stmt  *stmt;

  switch (t->getKind())
  {
    case TREE_SUITE:	{ return true; }
    case TREE_OPERATOR:	{ if (((Operator*)t)->getOpKind()==OP_BEHAVIORAL) {
			    op=(OperatorBehavioral*)t;
			    return true;
			  }
			  else
			    return false; }
    case TREE_STATE:	{ state=(State*)t;
			  if (state->getCases()->empty())
			    warn("## state "+state->getName()+" has no cases");
			  return true; }
    case TREE_STATECASE:{ return true; }
    case TREE_STMT:	{ stmt=(Stmt*)t;
			  if (stmt->getStmtKind()==STMT_GOTO) {
			    StmtGoto *stmtGoto=(StmtGoto*)t;
			    State    *target=stmtGoto->getState();
			    if (target) {
			      if (target->getName()!=stmtGoto->getStateName())
				warn("## in state "+state->getName()+
				     ", goto "+target->getName()+" (ptr) / "+
				     stmtGoto->getStateName()+" (name)");
			      if (!op->lookupState(target->getName()))
				warn("## in state "+state->getName()+
				     ", goto "+target->getName()+
				     ", target is not in stmt's op");
			      Operator *targetOp=target->getOperator();
			      if (!targetOp)
				warn("## in state "+state->getName()+
				     ", goto "+target->getName()+
				     ", target has no enclosing op");
			      else if (targetOp!=op)
				warn("## in state "+state->getName()+
				     ", goto "+target->getName()+
				     ", target is in op "+targetOp->getName()+
				     " but stmt is in op "+op->getName());
			    }
			  }  // if (stmt->getStmtKind()==STMT_GOTO)
			  return false; }
    default:		{ return false; }
  }
}


void checkGotos (Tree *t)
{
  // - consistency check on goto stmts
  t->map(checkGotos_map);
}


////////////////////////////////////////////////////////////////
//  everything

Tree* canonicalForm (Tree **h)
{
  timestamp(string("before liftInlines()"));
  // warn(string("-- before canonicalForm, operatrs are:\n")+(*h)->toString());
  liftInlines(*h);		// - lift nested calls/mem-access to stmt level
  // warn(string("-- after liftInlines, operators are:\n")+(*h)->toString());

  // markMemoryAccess(*h);	// - discover access modes for each memory
				//     (presently done in liftInlines, all RAM)

  timestamp(string("-- before basicBlocks()"));
  basicBlocks(*h);		// - decompose into basic-block states
  // warn(string("after basicBlocks, operators are:\n")+(*h)->toString());

  removeDumbStates(*h);		// - remove unreachable and "goto" states

  checkGotos(*h);

  timestamp(string("-- before exline()"));
  exline(h);			// - exline calls/mem-access
  // warn(string("-- after exline, operators are:\n")+(*h)->toString());

  // doCopyOps(h);		// - infer + expand copy() calls for fanout
				// - this is now called directly from tdfc.cc

  checkGotos(*h);

  if (gDeleteType)
    rmType(*h);
  if (gEnableGC)
  {
    if (!TreeGC::isRoot(*h))
    {
      TreeGC::addRoot(*h);
      TreeGC::gc();
      TreeGC::removeRoot(*h);
    }
    else
      TreeGC::gc();
  }

  return *h;
}
