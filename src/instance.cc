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
// SCORE TDF compiler:  generate instance operators (generic)
// $Revision: 1.126 $
//
//////////////////////////////////////////////////////////////////////////////
#include <LEDA/core/string.h>
#include <LEDA/core/array.h>
#include <LEDA/core/list.h>
#include <LEDA/core/stack.h>
#include "misc.h"
#include "symbol.h"
#include "expr.h"
#include "stmt.h"
#include "operator.h"
#include "annotes.h"
#include "feedback.h"
#include "rebind.h"
#include "bindvalues.h"
#include "ccannote.h"
#include "ccheader.h"

using leda::list_item;
using leda::stack;

//#define VERBOSE_DEBUG_INSTANCE
//#define DEBUG_RESOLVE_INSTANCE  

// forward definition
Operator *flatten (Operator *op, SymTab *env, int top);

//
////////////////////////////////////////////////////////////////////////
// type for var+stmt return values

class VarCodePair
{
public:
  VarCodePair(list<Symbol*>*nsyms, list<Stmt*>*nstmts):
    syms(nsyms), stmts(nstmts), rexpr((Expr *)NULL) {}
  VarCodePair(list<Symbol*>*nsyms, list<Stmt*>*nstmts,
	      Expr *nrexpr):
    syms(nsyms), stmts(nstmts), rexpr(nrexpr) {}
  list<Symbol*> *getVars() {return(syms);}
  list<Stmt*> *getStmts() {return(stmts);}
  Expr* getExpr() {return(rexpr);}
private:
  list<Symbol*> *syms;
  list<Stmt*> *stmts;
  Expr *rexpr;
};

////////////////////////////////////////////////////////////////////////
// unique naming

static int ucnt=0;
string unique_name()
{
  string res=string("ScOrEtMp%d",ucnt);
  ucnt++;
  return(res);
}

////////////////////////////////////////////////////////////////////////
// Add environment annotation
//
//  - this is done for "destream"ed streams to retain environment (closure)
//     from call site, primarily for evaluating params
//  - was: fixType(origType,newenv)

Tree *fixEnvAnnote (Tree *orig, SymTab *newenv)
{
  if (orig->getAnnote(CC_ENVIRONMENT)==NULL)
    orig->setAnnote(CC_ENVIRONMENT,newenv);
  return orig;
}

////////////////////////////////////////////////////////////////////////
// stream hacks
//
//   - convert a stream into a SymbolVar
//      (SymbolVar represents a local stream declaration in compositional op)

SymbolVar *destream(Symbol *sym, SymTab *env)
{
  Type *oldtype = (Type *)sym->getType()->duplicate();
  Type *newtype = (Type *)fixEnvAnnote(oldtype,env);
  Expr *olddepth, *newdepth, *depth;
  if (sym->getSymKind()==SYMBOL_VAR &&
      (depth=((SymbolVar*)sym)->getDepth())) {
    olddepth = (Expr*)depth->duplicate();
    newdepth = (Expr*)fixEnvAnnote(olddepth,env);
  }
  else
    newdepth = olddepth = NULL;
  return(new SymbolVar(sym->getToken(),sym->getName(),newtype,NULL,newdepth));
}

////////////////////////////////////////////////////////////////////////
// debug

void printenv(SymTab *env)
{
  if (env!=(SymTab *)NULL)
    {
      SymTab *tenv=env;
      cerr << "environment is: " << tenv << endl;
      while (tenv!=(SymTab *)NULL)
	{
	  Symbol *tsym;
	  forall(tsym,*(tenv->getSymbolOrder()))
	    {
	      cerr << "  " << tsym->getName() ;
	      if (tsym->getSymKind()==SYMBOL_VAR)
		{
		  Expr* texpr=((SymbolVar *)tsym)->getValue();
		  if (texpr!=(Expr *)NULL)
		    cerr << "=" << texpr->toString(); 
		}
	      cerr << endl;
	    }
	  tenv=tenv->getScope();
	  cerr << "--- " << tenv << endl;
	}
    }
  else
    cerr << "<NULL environment>" << endl;
}

//
////////////////////////////////////////////////////////////////////////
// flatten expr

VarCodePair *flatten(Expr *expr, SymTab *env)
{
  list<Symbol*>* syms=new list<Symbol*>();
  list<Stmt*>* stmts=new list<Stmt*>();
  expr->setAnnote(CC_ENVIRONMENT,(void *)env);
#ifdef DEBUG_RESOLVE_INSTANCE
  cerr << "tagging expression " << expr->toString() << " with:" << endl;
  printenv(env);
#endif  
  if (expr->getExprKind()==EXPR_LVALUE)
    {
      return(new VarCodePair(syms,stmts,expr));
    }
  else if ( (expr->getExprKind()==EXPR_CALL) ||
	   ((expr->getExprKind()==EXPR_BUILTIN) &&
	    ((((OperatorBuiltin *)((ExprBuiltin *)expr)->getOp())->
				getBuiltinKind())==BUILTIN_SEGMENT)))
    {
      SymTab* newenv=new SymTab(SYMTAB_BLOCK); 
      newenv->setScope(env);

      ExprCall *ecall=(ExprCall *)expr;
      Operator *call_op;
      if (expr->getExprKind()==EXPR_BUILTIN)
	call_op=ecall->getOp();     // - EC:  do not flatten called segment ops
      else
	call_op=flatten(ecall->getOp(),newenv,0);
      list<Expr*> *cargs=ecall->getArgs();
      Expr *aexp;
      list<Expr*> *newargs=new list<Expr*>();
      list<Symbol*> *formals=call_op->getArgs();
      list_item form=formals->first();
      
      forall(aexp, *cargs)
	{
	  Symbol *formal=formals->inf(form); 
	  form=formals->succ(form);    //  this works, but is it efficient?
	  //  that is, does it have to do a search, or is this just like
	  //  a cdr?

	  // was: VarCodePair *pair=flatten(aexp,newenv);
	  // 7/28 -- this was tricky for me -- amd
	  // point is that arguments to a function should not 
	  //   be evaluated in that functions scope, but in the 
	  //   enclosing scope!
	  VarCodePair *pair=flatten(aexp,newenv->getScope());
	  SymbolVar *fsym=destream(formal,newenv);
	  Symbol *sym;
	  Stmt *stmt;
	  forall(sym,*(pair->getVars()))
	    syms->append(sym);
	  forall(stmt,*(pair->getStmts()))
	    stmts->append(stmt);
	  Expr *newaexp=pair->getExpr();
	  if (newaexp->getExprKind()==EXPR_CALL)
	    {
	      // if it's a call, need to resolve call and
	      //  assign result to a new symbol

	      Symbol *rsym;
	      // here, we're using inside a procedure call, so need as var ?
	      Type *rsym_type=(Type *)aexp->getType()->duplicate();
	      rsym=new SymbolVar(aexp->getToken(),
				 unique_name(),
				 rsym_type);
	      // 8/2 amd -- note use of newenv->getScope() here 
	      //   same case as above (comment 7/28 on flatten call)
	      //  ...very tricky, indeed...
	      (rsym_type)->setAnnote(CC_ENVIRONMENT,
					   (void *)newenv->getScope());
	      ExprLValue *newexpr=new ExprLValue(aexp->getToken(),rsym);
	      if ((SymTab *)((newexpr->getType())->getAnnote(CC_ENVIRONMENT))
		  ==(SymTab *)NULL)
		(newexpr->getType())->setAnnote(CC_ENVIRONMENT,
						(void *)newenv->getScope());
	      Stmt* assign_arg=new StmtAssign(aexp->getToken(),
					      newexpr,newaexp);
	      stmts->append(assign_arg);
	      syms->append(rsym);
	      newargs->append((Expr *)newexpr->duplicate());
	      // shouldn't have to do...and why using pair->getExpr() here?
	      // pair->getExpr()->setAnnote(CC_ENVIRONMENT,(void *)newenv->getScope());
	      // note: rsym is not a formal, so should not be 
	      //   replacing it...
	      fsym->setValue(newexpr);
	    }
	  else 
	    {
	      // otherwise, just keep track of flattened arg
	      newargs->append(newaexp);
	      // shouldn't this have been annotated during recursive call?
	      //  (maybe only had to have this in while was screwed up
              //   on evaluating actuals in this environment...so
              //   hopefully, this is obsolesed by above correction)
	      // newaexp->setAnnote(CC_ENVIRONMENT,(void *)newenv->getScope());
	      fsym->setValue(newaexp);
	    }
	  newenv->addSymbol(fsym);

#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << "defining formal " << fsym->getName() << " in " << newenv 
	       << "   gets " << newaexp->toString() << " in " 
	       << newenv->getScope() << endl;
#endif

	  // need this to make sure we only look at replacment on formals
	  fsym->setAnnote(CC_FORMAL,(void *)1);
	  // need this to make sure once replaced we end up resolving in
	  //  correct environment
	  // Note: type of this value is resolved in *this* environment
	  //   (set in destream into fsym)
	  // CHECK 7/21
	  fsym->setAnnote(CC_ENVIRONMENT,(void *)newenv);

	}
      if (call_op->getOpKind()==OP_COMPOSE)
	{
	  Symbol *rsym=call_op->getRetSym();
	  if (!noReturnValue(rsym))
	    {
	      // need to mark this guy as a formal, too
	      SymbolVar *fsym=destream(rsym,newenv);
	      newenv->addSymbol(fsym);
	      fsym->setAnnote(CC_FORMAL,(void *)1);
	    }

	  SymTab *symtab=((OperatorCompose *)call_op)->getVars();
	  list<Symbol*>* lsyms=symtab->getSymbolOrder();
	  list_item item;
	  // need new environment to mimic any bound parameter shadowing
	  //   which may occur
	  SymTab *newenv2=new SymTab(SYMTAB_BLOCK);
	  list<Symbol *> *vsyms=new list<Symbol *>();
	  forall_items(item,*lsyms)
	    {
	      Symbol *asym=lsyms->inf(item);
	      vsyms->append(asym);
	    }

	  Symbol *sym;
	  forall(sym,*vsyms)
	    {
	      Symbol *dsym=destream(sym,newenv2); 
	      // necessary because using a block symbol table here
	      syms->append(dsym);
	      newenv2->addSymbol(dsym); 
	    }
	  newenv2->setScope(newenv);
	  // copy over stmts
	  Stmt *stmt;
	  forall(stmt,*(((OperatorCompose *)call_op)->getStmts()))
	    {
	      if ((SymTab *)(stmt->getAnnote(CC_ENVIRONMENT))
		  ==(SymTab *)NULL)
		{
		  stmt->setAnnote(CC_ENVIRONMENT,newenv2);
		}
	      // do environment binding/resolution at end
	      // 
	      // note assignments to formals taken care of in flatten
              //   (plus final resolve/bind)
              // ...I think I'm OK, as long as final map also
              // replaces formals in lhs side of assignment
              // with bound values!

	      // if any formal is assigned to a call, then
	      //  ...will want to declare? otherwise this is true...
	      // NOTE: return syms are marked as formals, so 
	      //  this *is* keeping assignment to return syms.
	      // Blah, if there is fanout, we want that.  If there isn't
	      //  we might not.  
	      // May need to think about sorting these cases out...
	      //
	      if (stmt->getStmtKind()==STMT_ASSIGN)
		{
		  Expr* rval=((StmtAssign *)stmt)->getRhs();
		  if ((rval->getExprKind())==EXPR_CALL)
		    {
		      ExprLValue *lval=((StmtAssign *)stmt)->getLValue();
		      Symbol *sym=lval->getSymbol();
		      
		      /*
		      cerr << "...considering assignment of " 
			   << sym->getName() << " to " 
			   << rval->toString() << endl;
			   */

		      Symbol *envsym=newenv->lookup(sym->getName());
		      if (envsym!=(Symbol *)NULL)
			if (envsym->getSymKind()==SYMBOL_VAR)
			  if ((long)(envsym->getAnnote(CC_FORMAL))==1)
			    syms->append(envsym);
		      // debug
		      //else  cerr << "not formal " << endl;
		      //    else  cerr << "not var " << endl;
		      //      else cerr << " not found in environment" << endl;
		      stmts->append(stmt);
		    }
		  else
		    {
		      ExprLValue *lval=((StmtAssign *)stmt)->getLValue();
		      Symbol *sym=lval->getSymbol();
		      Symbol *envsym=newenv->lookup(sym->getName());
		      if (envsym!=(Symbol *)NULL)
			if (envsym->getSymKind()==SYMBOL_VAR)
			  if ((long)(envsym->getAnnote(CC_FORMAL))==1)
			    {
			      if (rval->getAnnote(CC_ENVIRONMENT)==
				  (SymTab *)NULL)
				rval->setAnnote(CC_ENVIRONMENT,(void *)newenv2);
			      ((SymbolVar *)envsym)->setValue(rval);
//			      cout << "int->long... symbol=" << sym->toString() << endl;
			    }
					   
		      // n.b. drop statement in this case
			
		    }
		}
	      else
		stmts->append(stmt);
	    }
	  if (noReturnValue(rsym))
	    return(new VarCodePair(syms,stmts));
	  else
	    {


	      // note: this return value should be replaced by whatever
	      //   it was assigned to
	      //  ...mark the environment here and should collapse
	      //  to bound value on later pass
	      ExprLValue *res=new ExprLValue(expr->getToken(),
					     rsym);
	      res->setAnnote(CC_ENVIRONMENT,(void *)newenv2);


	      return(new VarCodePair(syms,stmts,res));
	    }
	}
      else // called operator is a leaf, call (not inline)
	{

	  ExprCall *newcall;
	  // memory segments
	  if (call_op->getOpKind()==OP_BUILTIN)
	    newcall=new ExprBuiltin(call_op->getToken(),
				    newargs,(OperatorBuiltin *)call_op);
	  else
	    newcall=new ExprCall(call_op->getToken(),
				 newargs,call_op);

	  // 8/2 ... see above, looks like this, too, should 
	  //   be the enclosing scope, not the new one
	  newcall->setAnnote(CC_ENVIRONMENT,(void *)newenv->getScope());
	  // now, the type may need to be in the new environment
	  (newcall->getType())->setAnnote(CC_ENVIRONMENT,
					  (void *)newenv);
#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << "call is now:" << newcall->toString() << endl;
	  printenv(newenv->getScope());
#endif
	  return(new VarCodePair(syms,stmts,newcall));
	}

    }
  else if (expr->getExprKind()==EXPR_BUILTIN)
    {
      ExprBuiltin *bexpr=(ExprBuiltin *)expr;
      OperatorBuiltin *bop =(OperatorBuiltin *)bexpr->getOp();
      if ((bop->getBuiltinKind()==BUILTIN_CAT)
	  |(bop->getBuiltinKind()==BUILTIN_WIDTHOF)
	  |(bop->getBuiltinKind()==BUILTIN_BITSOF))
	{
	  // should be scalar only inside (as else case below), 
	  //   so just keep it.
	  return(new VarCodePair(syms,stmts,expr));
	}
      else
	// 8/2/99 -- note tht segments are handled above w/ calls
	warn("instance not handling compose builtin (maybe should happen earlier?)",
	     expr->getToken());
    }
  else
    {
      // blah, any other kind of expr should not have a call contained w/in
      // in particular it should only be an expression that reduces to
      // a scalar constant

      // also saves resolution for later
      expr->setAnnote(CC_ENVIRONMENT,(void *)env);
      return(new VarCodePair(syms,stmts,expr));
    }

  // default case (errors) return empty pair
  return(new VarCodePair(syms,stmts));
  
}

//
////////////////////////////////////////////////////////////////////////
// flatten statement

VarCodePair *flatten(Stmt *stmt, SymTab *env)
{
  //cerr << "enter flatten stmt" << endl;
  if (stmt->getStmtKind()==STMT_CALL)
    {
      VarCodePair *pair=flatten(((StmtCall *)stmt)->getCall(),env);
      // in a case like this, w/ the current flatten(expr) returnning exprs,
      //  we need to turn the expr back into a statement so it doesn't get 
      //  lost  -- amd 7/26/99
      Expr *res=pair->getExpr();
      if (res!=(Expr *)NULL)
	if (res->getExprKind()==EXPR_CALL)
	  {
	    list<Symbol *> *syms=pair->getVars();
	    list<Stmt *> *stmts=pair->getStmts();
	    Stmt *ncall=new StmtCall(res->getToken(),(ExprCall *)res);
	    stmts->append(ncall);
	    return (new VarCodePair(syms,stmts));
	  }
      return(pair);
    }
  else if (stmt->getStmtKind()==STMT_ASSIGN)
    {
      ExprLValue *lval=((StmtAssign *)stmt)->getLValue();
      Expr* rval=((StmtAssign *)stmt)->getRhs();
      VarCodePair *epair=flatten(rval,env);
      Symbol *sym=lval->getSymbol();
      if (env!=(SymTab *)NULL)
	{
	  Symbol *envsym=env->lookup(sym->getName());
	  if (envsym!=(Symbol *)NULL)
	    if (sym->getSymKind()==SYMBOL_VAR)
	      if ((long)(sym->getAnnote(CC_FORMAL))==1)
		{
		  // don't think this guy should be setting environment
		  //(epair->getExpr())->setAnnote(CC_ENVIRONMENT,(void *)env);
		  ((SymbolVar *) envsym)->setValue(epair->getExpr());
//			      cout << "int->long... symbol=" << ((SymbolVar*)envsym)->toString() << endl;
		  if ((SymTab *)(envsym->getAnnote(CC_ENVIRONMENT))==env)
		    {
		      cerr << "WARNING: assigning value in same environment as formal" << endl;
		      cerr << envsym->getName() << " in " 
			   << (SymTab *)envsym->getAnnote(CC_ENVIRONMENT)
			   << endl;
		      cerr << epair->getExpr() << " in "
			   << env << endl;
		    }
		  // in this case, lose assignment since
                  //  will be made by substitution during resolution
		  return(new VarCodePair(epair->getVars(),epair->getStmts()));
		}
	}

      // BLAH, Note that this goes ahead and inserts assignments
      //   to return value/outputs, which is *not* the right thing.
      // I'm going to try patching that up at a subsequent pass.
      //   (see below) -- amd 8/26/99
      // DEBUG 8/26
      
      list<Stmt*>*stmts=epair->getStmts();
      StmtAssign *newstmt=new StmtAssign(stmt->getToken(),
					 lval,epair->getExpr());

      stmts->append(newstmt);
      return(new VarCodePair(epair->getVars(),stmts));
      
    }
  else if (stmt->getStmtKind()==STMT_BUILTIN)
    {
      // primarily (only?) to deal with memory builtins
      ExprBuiltin *bexpr=((StmtBuiltin *)stmt)->getBuiltin();
      // handle like call
      VarCodePair *pair=flatten(bexpr,env);
      // like call case above, back int stmt
      Expr *res=pair->getExpr();
      if (res!=(Expr *)NULL)
	if (res->getExprKind()==EXPR_BUILTIN)
	  {
	    list<Symbol *> *syms=pair->getVars();
	    list<Stmt *> *stmts=pair->getStmts();
	    Stmt *ncall=new StmtBuiltin(res->getToken(),(ExprBuiltin *)res);
	    stmts->append(ncall);
	    return (new VarCodePair(syms,stmts));
	  }
      return(new VarCodePair(new list<Symbol *>(),
			     new list<Stmt *>()));
    }
  else
    {
      fatal(-1,string("flatten statement not recognize statement type %d",
		      (int)stmt->getStmtKind()),stmt->getToken());
      return(new VarCodePair(new list<Symbol *>(),
			     new list<Stmt *>()));
    }
}


//
////////////////////////////////////////////////////////////////////////
// maps for operator flatten

bool instance_treemap_false(Tree *t, void *aux)
{ return (0); }


bool resolve_pop_environments(Tree **t, void *aux)
{
  stack<SymTab *> *envs=(stack<SymTab *> *)aux;

  // cerr << "<--on entry depth=" << envs->size() << endl;

  long null_environment=(long)((*t)->getAnnote(CC_NULL_ENVIRONMENT));
  
  if (null_environment==1)
    {
      if (envs->size()==0)
	{
	  cerr << "----------ENVIRONMENT STACKING ERROR-------------" << endl;
	  cerr << "  should be popping null environment at node " 
	       << (long) (*t) << " " << (long)t << endl; 
	  cerr << "  but there are no environmnets to pop!" << endl;
	  cerr << "-------------------------------------------------" << endl;
	  if ((*t)->getKind()==TREE_EXPR)
	    cerr << "  expression is: " << ((Expr *)(*t))->toString() << endl;
	}
      else
	{
	  SymTab *oldenv=envs->pop();

#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << "POP  env=" << oldenv << " " << *t << " " << t 
	       << " depth now=" 
	       << envs->size()
	       << endl;
	  if ((*t)->getKind()==TREE_EXPR)
	    cerr << "  expression is: " << ((Expr *)(*t))->toString() << endl;
#endif

	  if (oldenv!=(SymTab *)NULL)
	    {
	      cerr << "----------ENVIRONMENT STACKING ERROR-------------" << endl;
	      cerr << "  should be popping null environment " << endl;
	      cerr << "  but poped " << oldenv << endl;
	      cerr << "-------------------------------------------------" << endl;
	    }
	}
      return(0);
    }

  SymTab *newenv=(SymTab*)((*t)->getAnnote(CC_ENVIRONMENT));
  if (newenv!=(SymTab *)NULL)
    {
      /* DEBUG */
      if (envs->size()==0)
	{
	  cerr << "----------ENVIRONMENT STACKING ERROR-------------" << endl;
	  cerr << "  should be popping " << newenv << endl;
	  cerr << "  but there are no environmnets to pop!" << endl;
	  cerr << "-------------------------------------------------" << endl;
	}
      else
	{

	  SymTab *oldenv=envs->pop();

#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << "POP  env=" << oldenv << " " << *t << " " << t 
	       << " depth now=" 
	       << envs->size()
	       << endl;
	  if ((*t)->getKind()==TREE_EXPR)
	    cerr << "  expression is: " << ((Expr *)(*t))->toString() << endl;
#endif

	  /* DEBUG */
	  if (oldenv!=newenv)
	    {
	      cerr << "----------ENVIRONMENT STACKING ERROR-------------" << endl;
	      cerr << "  should be popping " << newenv << endl;
	      cerr << "  but  popped " << oldenv << endl;
	      cerr << "-------------------------------------------------" << endl;
	    }
	}
	
    }
  return(0);
}

bool resolve_bound_formals(Tree **t, void *aux)
{
  
  stack<SymTab *> *envs=(stack<SymTab *> *)aux;

  // cerr << "-->on entry depth=" << envs->size() << endl;

  SymTab *newenv=(SymTab*)((*t)->getAnnote(CC_ENVIRONMENT));
  SymTab *env;

  if (newenv!=(SymTab *)NULL)
    {
      env=newenv;
      envs->push(env);

#ifdef DEBUG_RESOLVE_INSTANCE
      cerr << "PUSH " << "env=" << env << " " << *t << " " << t 
	   << " depth now=" 
	   << envs->size()
	   << endl;
      printenv(env);
#endif

    }

  // cerr << "...entering " << (*t)->toString() << endl;

  if (envs->size()>0)
    {
      env=envs->top(); 
      int replaced_env=0;
      if ((*t)->getKind()==TREE_EXPR)
	{
	  Expr * bexpr=(Expr *)*t;

	  // eylon suggests punting out now...
	  if (bexpr->getExprKind()==EXPR_VALUE)
	    return(0);

	  Expr *lastexpr=bexpr;
#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << " expr is " << bexpr << " " << bexpr->toString() ;
#endif
	  while ((bexpr!=(Expr *)NULL) & (env!=(SymTab *)NULL))
	    {
	      // lastexpr=bexpr; n.b. moved into bexpr!=NULL case
	      //  problem was cases wher env became null first
	      //   and not getting real last expr...
	      bexpr=(Expr *)NULL;
	      if (lastexpr->getExprKind()==EXPR_LVALUE)
		{
		  Symbol *sym=((ExprLValue *)(lastexpr))->getSymbol();
		  Symbol *envsym=env->lookup(sym->getName());
		  if (envsym!=(Symbol *)NULL)
		    if (envsym->getSymKind()==SYMBOL_VAR)
		      if ((long)(envsym->getAnnote(CC_FORMAL))==1)
			{
//				cout << "Processing symbol=" << envsym->toString() << endl;
			  bexpr=((SymbolVar *)envsym)->getValue();
			  // note: the looked up symbol may have
			  //   a new environment!
			  if (bexpr!=(Expr *)NULL)
			    {
			      
			      lastexpr=bexpr;
			      env=(SymTab*)((bexpr)->getAnnote(CC_ENVIRONMENT));
			      // this should always be correct at this point
			      //  if it's null that means the top level environment
			      // but I do need to have some way to distinguish
			      //  that from a null annotation (below)
			      replaced_env=1;
#ifdef DEBUG_RESOLVE_INSTANCE
			      cerr << "-->" << bexpr->toString() 
				   << "[" << env << "]";
#endif
			    }
			}
		}
	    }
#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << endl;
#endif
	  // bottomed out chasing lvalues, keep last non-lvalue expr

	  if (lastexpr!=(Expr *)*t)
	    {
#ifdef DEBUG_RESOLVE_INSTANCE
	      cerr << "RPLC " << *t << " with " 
		   << lastexpr << endl;
	      cerr << "  using formal " << ((Expr *)*t)->toString() << " in " 
		   << envs->top() << " becomes " << lastexpr->toString() << " in " << env << endl;
#endif
	      *t=lastexpr->duplicate(); // eylon said would copy annotes now
	      if (env==(SymTab *)NULL)
		(*t)->setAnnote(CC_NULL_ENVIRONMENT,(void *)1);
	      // (a) duplication preserves annotations 
              //    (but they annotation contents is *not* duplicated
              //   so pointing to same thing.
	      if (replaced_env==1) // this means we may have swapped environments during resolution
		if (newenv==(SymTab *)NULL) 
		  // this means it's a new environment at this level
		  //   we need to save it
		  {
		    envs->push(env);
#ifdef DEBUG_RESOLVE_INSTANCE
		    cerr << "PUSH " << "env=" << env << " "
			 << *t << " " << t 
			 << " (from resolved value) depth now=" 
			 << envs->size()
			 << endl;
		    printenv(env);
#endif
		  }
		else // newenv was not null
		  // this means it could be a replacement environment at this level
		  {
		    envs->pop(); // get rid of newenv
		    envs->push(env); // put in the replacement environment instead
#ifdef DEBUG_RESOLVE_INSTANCE
		    cerr << "POP/PUSH " << "env=" << env << " " 
			 << *t << " " << t 
			 << " (replacing from resolved value) depth now=" 
			 << envs->size()
			 << endl;
		    printenv(env);
#endif
		  }
	      else // didn't swap, so replacement didn't change environment
		{
		  // this means we need to discard the environment which 
		  //  the old node stacked
		  if (newenv!=(SymTab *)NULL)
		    {
#ifdef DEBUG_RESOLVE_INSTANCE
		      cerr << "POP  " << *t << " " << t 
			   << " (from resolved value) depth now=" 
			   << envs->size()
			   << endl;
#endif
		      envs->pop();
		    }
		  else 
		    // we didn't push an environment here, so no change necessary 
		    {
		    }
		}
	    }
	}
    }
  return(1); // probably can shortcut some things, 
  // but have to be careful with what
}

bool all_symbols(Tree *t, void *aux)
{
  if (t->getKind()==TREE_SYMBOL)
    {
      Symbol *sym=(Symbol *)t;
      cerr << "     symbol " << sym->getName() << " in operator " 
	   << sym->getOperator()->getName() << endl;
      return(0);
    }
  else if (t->getKind()==TREE_STMT)
    cerr << "  stmt:" << ((Stmt *)t)->toString() << endl;
  else if (t->getKind()==TREE_EXPR)
    if ((((Expr *)t)->getExprKind())==EXPR_LVALUE)
      {
	cerr << "     lvalue with symbol " 
	     << ((ExprLValue *)t)->getSymbol()->getName() << " in operator " 
	     << ((ExprLValue *)t)->getSymbol()->getOperator()->getName() 
	     << " w/type " 
	     << ((ExprLValue *)t)->getType()->toString() 
	     << " width=" 
	     << ((ExprLValue *)t)->getType()->getWidth() 
	     << endl;
      }
  /*
    else
      cerr << "      descending " << ((Expr *)t)->toString() << endl;
      */
  
  return(1);
}


//
////////////////////////////////////////////////////////////////////////
// flatten operator

Operator *flatten (Operator *orig_op, SymTab *env, int top)
{

  timestamp(string("enter flatten ")+ orig_op->getName() +
            string(": begin duplicate"));

#ifdef VERBOSE_DEBUG_INSTANCE
  cerr << "********original operator before duplicate/relink**********" 
       << endl;
  orig_op->map(all_symbols,instance_treemap_false,(void *)NULL);
#endif

  Operator *op=(Operator *)orig_op->duplicate();

  timestamp(string("flatten ")+ orig_op->getName() +
            string(": end duplicate begin extra relink"),op);
  op->link();
  timestamp(string("flatten ")+ orig_op->getName() +
            string(": after extra (early) relink"),op);

  // debug
  op->setName(orig_op->getName()+string("_d1_")+unique_name());

#ifdef VERBOSE_DEBUG_INSTANCE
  cerr << "**********original operator after relink************" << endl;
  orig_op->map(all_symbols,instance_treemap_false,(void *)NULL);
  cerr << "**********duplicate operator after relink***********" << endl;
  op->map(all_symbols,instance_treemap_false,(void *)NULL);
  cerr << "****************************************************" << endl;
#endif


  Operator *res=(Operator *)NULL;

      //  to need to hang on to this until we do the rebind
      /* ...but, w/ above relink maybe this can go away? 
      Symbol *rsym=op->getRetSym();
      if (!noReturnValue(rsym))
	rsym->setName(orig_op->getName()); 
	*/

  // will trash op, but op is a duplicate created just for this purpose!
  if (op->getOpKind()!=OP_COMPOSE)
      res=op;
  else 
    {
      OperatorCompose *cop=(OperatorCompose *)op;
      list<Stmt*>* stmts=cop->getStmts(); 
      SymTab* vars=cop->getVars();        

      // unique renaming here before this guy gets inlined into someone else
      list<Symbol*>* lsyms=vars->getSymbolOrder();
      list_item item;
      list<Symbol *> *vsyms=new list<Symbol *>();
      forall_items(item,*lsyms)
	{
	  Symbol *asym=lsyms->inf(item);
	  vsyms->append(asym);
	}
      // rename for uniqueness
      Symbol *sym;
      forall(sym,*vsyms)
	{
	  sym->setName(unique_name());
	}

      list<Stmt*>* new_stmts=new list<Stmt*>();
      Stmt *stmt;

      forall(stmt,*stmts)
	{
	  VarCodePair *pair=flatten(stmt,env);
	  if (gDeleteType)
	    rmType(stmt); // get rid of any types used during stmt processing
	  Symbol *sym;
	  forall(sym,*(pair->getVars()))
	    vars->addSymbol(sym); 
	  Stmt *cstmt;
	  forall(cstmt,*(pair->getStmts()))
	    {
	      if (cstmt->getStmtKind()==STMT_ASSIGN)
		{
		  ExprLValue *lhs = ((StmtAssign *)cstmt)->getLValue();
		  if ((op->getSymtab()->lookup(lhs->getSymbol()->getName()))
		      !=(Symbol *)NULL) // i.e. this is an arg or the return value
		    {

		      cerr << "FOUND ASSIGNMENT TO RETURN VALUE OR OUTPUT" 
			   << cstmt << endl;
		      Expr *rhs=((StmtAssign *)cstmt)->getRhs();
		      if (rhs->getExprKind()!=EXPR_LVALUE)
			{
			  error(-1,"Found non L-value assigned to an output stream",
				cstmt->getToken());
			  new_stmts->append(cstmt);
			}
		      else
			{
			  // don't copy it over, instead rename...
			  // WORK HERE			  
			  // ...when can I not rename? and what to do then!
			}
		    }
		  else
		    new_stmts->append(cstmt);
		}
	      else
		new_stmts->append(cstmt);
	    }
	}

      timestamp("before create frankenstein operator");

      res=new OperatorCompose(cop->getToken(),
			      cop->getName(),
			      cop->getRetSym(),
 			      new list<Symbol*>(*cop->getArgs()),  // dupl LEDA
			      //cop->getArgs(),			   //   for gc
			      vars,
			      new_stmts,
			      (Token *)NULL // don't know how to get exception
			      );

      // Here is where everything has been copied over
      //   now make pass to resolve/replace all bound values

      res->thread((Tree *)NULL); // ??? dangerous?
      // maybe eventually link into iSuite...
      // this guy is not visible to any global thing now
      //  so no one else will find it

      if (top) // only want to resolve at top level
	{
#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << "++before resolve++" << endl;
	  cerr << res->toString();
#endif
	  timestamp(string("resolve begin"));
	  /*
	    Stmt *stmt;
	    forall(stmt,*new_stmts)
	    {
	      printenv((SymTab *)(stmt->getAnnote(CC_ENVIRONMENT)));
	      cerr << "(for stmt):" << stmt->toString() << endl;
		
	    }
	    */
#ifdef DEBUG_RESOLVE_INSTNACE
	  cerr << "++start resolve bound++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
#endif
	  stack<SymTab *>* estack=new stack<SymTab *>();
	  res->map2((Tree **)(&res),resolve_bound_formals,
		    resolve_pop_environments,estack);
#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << "++finish resolve bound++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
#endif
	  timestamp(string("resolve end"));
#ifdef DEBUG_RESOLVE_INSTANCE
	  cerr << "++after resolve++" << endl;
	  cerr << res->toString();
	  cerr << "++++" << endl;
#endif
	}

    }

  if (top) // only do final link at top level
    {
      // ok, this should DTRT...
#ifdef VERBOSE_DEBUG_INSTANCE
      cerr << "before relink------ " << endl;
      cerr << res->toString();
       res->map(all_symbols,instance_treemap_false,(void *)NULL);
#endif
      timestamp(string("begin relink ")+ op->getName());
      res->link();
#ifdef VERBOSE_DEBUG_INSTANCE
      res->map(all_symbols,instance_treemap_false,(void *)NULL);
#endif
      timestamp(string("end relink ")+ op->getName(),res);

#ifdef VERBOSE_DEBUG_INSTANCE
      cerr << res->toString();
#endif

    }
  return(res);


}

//
////////////////////////////////////////////////////////////////////////
// actual exported call

Operator *flatten(Operator *op)
{
  Operator *rop=flatten(op,new SymTab(SYMTAB_BLOCK),1); 
  return(rop);
}

Operator *generate_instance(Operator *orig_op, FeedbackRecord *rec)
{
  Operator *rop=(Operator *)orig_op->duplicate();
  timestamp(string("generate_instance ")+ orig_op->getName() +
            string(": end duplicate begin relink"),rop);
  rop->link();
  timestamp(string("generate_instance ")+ orig_op->getName() +
            string(": after relink"),rop);

  bindvalues(rop,rec);
  return(rop);
}




