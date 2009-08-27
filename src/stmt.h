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
// $Revision: 1.130 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_STMT_
#define _TDF_STMT_

#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/set.h>
#include <LEDA/core/dictionary.h>
#include "tree.h"
#include "type.h"
#include "expr.h"
#include "state.h"
class State;
class SymTab;


enum StmtKind { STMT_IF,     STMT_GOTO,    /*STMT_STAY,*/
		STMT_CALL,   STMT_BUILTIN,
		STMT_ASSIGN, STMT_BLOCK };

class Stmt : public Tree
{
protected:
  StmtKind	stmtKind;

public:
  Stmt (Token *token_i, StmtKind kind_i);
  virtual ~Stmt () {}

  StmtKind		getStmtKind	() const	{ return stmtKind; }

  virtual bool		okInComposeOp	() const = 0;

  virtual void		setParent	(Tree  *p);
  virtual Type*		getType		() const	{ return type_none; }
  virtual void		deleteType	()		{}
  virtual void		clearType	()		{}
  virtual SymTab*	getScope	() const;
};


class StmtIf : public Stmt
{
private:
  Expr *cond;
  Stmt *thenPart, *elsePart;

public:
  StmtIf (Token *token_i,   Expr *cond_i,
	  Stmt *thenPart_i, Stmt *elsePart_i=NULL);
  virtual ~StmtIf () {}

  Expr*			getCond		() const	{ return cond; }
  Stmt*			getThenPart	() const	{ return thenPart; }
  Stmt*			getElsePart	() const	{ return elsePart; }

  virtual bool		okInComposeOp	() const	{ return false; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class StmtGoto : public Stmt
{
private:
  string  stateName;
  State  *state;

public:
  StmtGoto (Token *token_i, const string &stateName_i);
  StmtGoto (Token *token_i, State *state_i=NULL);
  virtual ~StmtGoto () {};

  string		getStateName	() const    { return stateName; }
  State*		getState	() const    { return state; }
  void			setState	(State *s)  { state=s;
						      stateName=s->getName(); }

  virtual bool		okInComposeOp	() const    { return false; }

  virtual size_t	getSizeof	() const    { return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class StmtCall : public Stmt
{
protected:
  ExprCall *call;

public:
  StmtCall (Token *token_i, ExprCall *call_i);
  virtual ~StmtCall () {}

  ExprCall*		getCall		() const	{ return call; }

  virtual bool		okInComposeOp	() const	{ return true; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class StmtBuiltin : public Stmt
{
private:
  ExprBuiltin *builtin;

public:
  StmtBuiltin (Token *token_i, ExprBuiltin *builtin_i);
  virtual ~StmtBuiltin () {}

  ExprBuiltin*		getBuiltin	() const	{ return builtin; }

  virtual bool		okInComposeOp	() const	{ return builtin->
							    okInComposeOp(); }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class StmtAssign : public Stmt
{
private:
  ExprLValue	*lvalue;
  Expr		*rhs;

public:
  StmtAssign (Token *token_i, ExprLValue *lvalue_i, Expr *rhs_i);
  virtual ~StmtAssign () {}

  ExprLValue*		getLValue	() const	{ return lvalue; }
  Expr*			getRhs		() const	{ return rhs; }

  virtual bool		okInComposeOp	() const	{ return true; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class StmtBlock : public Stmt
{
private:
  SymTab	*symtab;	// locals
  list<Stmt*>	*stmts;

public:
  StmtBlock (Token *token_i, SymTab *symtab_i, list<Stmt*> *stmts_i);
  virtual ~StmtBlock ()					{ delete stmts; }

  SymTab*		getSymtab	() const	{ return symtab; }
  list<Stmt*>*		getStmts	() const	{ return stmts; }

  virtual bool		okInComposeOp	() const	{ return false; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual SymTab*	getScope	() const	{ return symtab; }
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


#endif  // #ifndef _TDF_STMT_
