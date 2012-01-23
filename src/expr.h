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
// SCORE TDF compiler:  expressions
// $Revision: 1.120 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_EXPR_
#define _TDF_EXPR_

#include "expr.h"
#include <sys/types.h>
#include <assert.h>
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/dictionary.h>
#include "tree.h"
#include "type.h"
#include "misc.h"
class Operator;
class OperatorBuiltin;
class Symbol;
class Token;

using std::cout;
using std::endl;


enum ExprKind { EXPR_VALUE, EXPR_LVALUE, EXPR_CALL, EXPR_BUILTIN,
		EXPR_COND,  EXPR_BOP,    EXPR_UOP,  EXPR_CAST,    EXPR_ARRAY,
		EXPR_BITSEL	// - used only in blockdfg.cc
		};

class Expr : public Tree
{
protected:
  ExprKind	exprKind;
  Type		*type;

public:
  Expr (Token *token_i, ExprKind exprKind_i, Type *type_i);
  virtual ~Expr () {}

  static void		init		();

//  ExprKind	 	getExprKind	() const	{ if(exprKind!=(ExprKind)NULL) {return exprKind;} else { cout << "exprkind is null" << endl; exit(1);} }
  ExprKind	 	getExprKind	() const	{ return exprKind; }
  void		        setType		(Type *ntype) 	{ type=ntype;        }

  virtual bool		isLValue	() const;	// - is assignable
  virtual bool		isRValue	() const;	// - is valued
  virtual Type*		getType		() const;
  virtual void		deleteType	()		{}
  virtual void		clearType	()		{}

  virtual bool		equals		(const Expr *x) const = 0;
  virtual bool		okInComposeOp	() const = 0;
};


class ExprValue : public Expr
{
private:
  long long intVal, fracVal;
  float floatVal;
  double doubleVal;
  string stateVal;

public:
  ExprValue (Token *token_i, Type *type_i,
	     long long intVal_i, long long fracVal_i=0);
  ExprValue (Token *token_i, Type *type_i, float floatVal_i);
  ExprValue (Token *token_i, Type *type_i, double doubleVal_i);
  ExprValue (Token *token_i, string state_i);
  virtual ~ExprValue () {}

  long long		getIntVal	() const	{ return intVal;  }
  long long		getFracVal	() const	{ return fracVal; }
  float			getFloatVal	() const	{ return floatVal;}
  double		getDoubleVal	() const	{ return doubleVal;}
  void			setIntVal	(long long v)	{ intVal=v;	  }
  void			setFracVal	(long long v)	{ fracVal=v;	  }
  void			setFloatVal	(float v)	{ floatVal=v;}
  void			setDoubleVal	(double v)	{ doubleVal=v;}

  virtual bool		equals		(const Expr *x) const;
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


class ExprLValue : public Expr
{
private:
  Symbol	*sym;
  Expr		*posLow, *posHigh;	// bit position/range
  Expr		*retime;		// token time for stream rvalues
					//   (0=current, 1...=past, NULL=N/A)
  Expr		*arrayLoc;		// location in mem array   (NULL=N/A)
  bool		allBits;		// true if refrnces all bits implicitly
					//   (constructed w/NULL posLow/High)

public:
  ExprLValue (Token *token_i, Symbol *sym_i,
	      Expr  *posLow_i=NULL, Expr *posHigh_i=NULL,
	      Expr  *retime_i=NULL, Expr *arrayLoc_i=NULL);
  virtual ~ExprLValue () {}

  Symbol*		getSymbol	() const	{ return sym;      }
  void		        setSymbol	(Symbol *nsym) 	{ sym=nsym;        }
  // amd again 7/5/99 :-|
  Expr*			getPosLow	() const	{ return posLow;   }
  Expr*			getPosHigh	() const	{ return posHigh;  }
  Expr*			getRetime	() const	{ return retime;   }
  Expr*			getArrayLoc	() const	{ return arrayLoc; }
  bool			usesAllBits	() const	{ return allBits;  }

  virtual bool		equals		(const Expr *x) const;
  virtual bool		okInComposeOp	() const;

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	();
  virtual void		clearType	()		{ type=NULL; }
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class ExprCall : public Expr
{
protected:
  Operator	*op;
  list<Expr*>	*args;

public:
  ExprCall (Token *token_i, list<Expr*> *args_i, Operator *op_i=NULL);
  virtual ~ExprCall ()					 { delete args; }

  Operator*		getOp		() const	 { return op;   }
  list<Expr*>*		getArgs		() const	 { return args; }
  list<Expr*>*		getMutableArgs	()      	 { return args; }
  void			setOp		(Operator *op_i) { assert(op=op_i); }

  virtual bool		equals		(const Expr *x) const;
  virtual bool		okInComposeOp	() const;

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	();
  virtual void		clearType	()		{ type=NULL; }
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class ExprBuiltin : public ExprCall	// methods commented-out are ExprCall
{
private:
  Symbol* sym;
public:
  ExprBuiltin (Token *token_i, list<Expr*> *args_i,
	       OperatorBuiltin *builtin_i);
  virtual ~ExprBuiltin () {}

//--------------------------------------------------------------------------
  Symbol*		getSymbol	() const	{ return sym;      }
  void		        setSymbol	(Symbol *nsym) 	{ sym=nsym;        }
//--------------------------------------------------------------------------
//virtual bool		equals		(const Expr *x) const;
  virtual bool		okInComposeOp	() const;

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
//virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	();
  virtual void		clearType	()		{ type=NULL; }
  virtual BindTime	getBindTime	() const;
//virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
//virtual void		map		(TreeMap pre,
//					 TreeMap post=NULL, void *i=NULL);
//virtual void		map2		(Tree **h,
//					 TreeMap2 pre, TreeMap2 post=NULL,
//					 void *i=NULL, bool skipPre=false);
};


class ExprCond : public Expr
{
private:
  Expr *cond;
  Expr *thenPart, *elsePart;

public:
  ExprCond (Token *token_i,    Expr *cond_i,
	    Expr  *thenPart_i, Expr *elsePart_i);
  ExprCond (Token *token_i,    Expr *cond_i, Type *type_i);
  virtual ~ExprCond () {}

  Expr*			getCond		() const	{ return cond; }
  Expr*			getThenPart	() const	{ return thenPart; }
  Expr*			getElsePart	() const	{ return elsePart; }

  virtual bool		equals		(const Expr *x) const;
  virtual bool		okInComposeOp	() const;

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	();
  virtual void		clearType	()		{ type=NULL; }
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class ExprBop : public Expr
{
private:
  int  bop;
  Expr *e1, *e2;

public:
  ExprBop (Token *token_i, int bop_i, Expr *e1_i, Expr *e2_i);
  virtual ~ExprBop () {}

  int			getOp		() const	{ return bop; }
  Expr			*getExpr1	() const	{ return e1; }
  Expr			*getExpr2	() const	{ return e2; }

  virtual bool		equals		(const Expr *x) const;
  virtual bool		okInComposeOp	() const;

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	();
  virtual void		clearType	()		{ type=NULL; }
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class ExprUop : public Expr
{
private:
  int  uop;
  Expr *e;

public:
  ExprUop (Token *token_i, int Uop_i, Expr *e_i);
  virtual ~ExprUop () {}

  int			getOp		() const	{ return uop; }
  Expr			*getExpr	() const	{ return e; }

  virtual bool		equals		(const Expr *x) const;
  virtual bool		okInComposeOp	() const;

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	();
  virtual void		clearType	()		{ type=NULL; }
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class ExprCast : public Expr
{
private:
  Expr *e;

public:
  ExprCast (Token *token_i, Type *type_i, Expr *e_i);
  ExprCast (Token *token_i, bool  sign_i, Expr *e_i);
  virtual ~ExprCast () {}

  Expr*			getExpr		() const	{ return e; } 

  virtual bool		equals		(const Expr *x) const;
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


class ExprArray : public Expr
{
private:
  list<Expr*> *exprs;

public:
  ExprArray (Token *token_i, TypeArray *type_i, list<Expr*> *exprs_i=NULL);
  ~ExprArray ()						{ delete exprs; }

  list<Expr*>*		getExprs	() const	{ return exprs; }
  void			addExpr		(Expr *e);

  virtual bool		equals		(const Expr *x) const;
  virtual bool		okInComposeOp	() const	{ return true; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	();
  virtual void		clearType	()		{ type=NULL; }
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class ExprBitSel : public Expr
{
  // - this class is a partial implementation of general bit selection
  // - used only in blockdfg.cc for DFG of a page state
  // - mostly a HACK  (no argument being bit-selected; type always NULL)
  // - posHigh==-1 --> [posLow]         is single-bit selection
  // - posHigh!=-1 --> [posHigh,posLow] is bit-field  selection

private:
  int posLow, posHigh;			// bit position / range

public:
  ExprBitSel (Token *token_i, int posLow_i=-1, int posHigh_i=-1)
    : Expr(token_i,EXPR_BITSEL,NULL), posLow(posLow_i), posHigh(posHigh_i) {}
  virtual ~ExprBitSel () {}

  int			getPosLow	() const	{ return posLow;   }
  int			getPosHigh	() const	{ return posHigh;  }

  virtual bool		equals		(const Expr *x) const { return false; }
  virtual bool		okInComposeOp	() const	      { return false; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p)	{ parent=p;    }
  virtual bool		link		()		{ return true; }
  virtual Type*		getType		()		{ return NULL; }
  virtual Type*		typeCheck	()		{ return NULL; }
  virtual void		deleteType	()		{ type=NULL;   }
  virtual void		clearType	()		{ type=NULL;   }
  virtual BindTime	getBindTime	() const	{ return BindTime
							    (BIND_UNKNOWN); }
  virtual Tree*		duplicate	() const	{ return new
							  ExprBitSel(*this); }
  virtual string	toString	() const  { return posLow==-1 ?
						      string("[%d]",posHigh) :
						      string("[%d:%d]",
							     posHigh,posLow); }
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL) {}
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false) {}
};


////////////////////////////////////////////////////////////////
//  pre-defined expressions in expr.cc

extern const ExprValue *expr_0;		// the constant "0"
extern const ExprValue *expr_1;		// the constant "1"


////////////////////////////////////////////////////////////////
//  expression generators

// - create new constant-value expression of minimum-width unsigned type:
static inline Expr* constIntExpr (long long val, Token *t=NULL) {
  return (Expr*)new ExprValue(t,new Type(TYPE_INT,countBits(val),false),val);
}

static inline Expr* constIntExprWithWidth (int width, long long val, Token *t=NULL) {
  // get max of countBits and specified width.. for fractions it should default to width, for other constants it should take countBits result..
  return (Expr*)new ExprValue(t,new Type(TYPE_INT,countBits(val),width,false),val);
}

// - create new constant-value expression of minimum-width signed type:
static inline Expr* constIntExprS (long long val, Token *t=NULL) {
  return (Expr*)new ExprValue(t,new Type(TYPE_INT,countBits(val),true),(val));
}

/*
// - create new constant-value expression of unknown-width unsigned type:
#define constIntExprBase(val) \
  ((Expr*)(new ExprValue(NULL,(Type*)type_uintx->duplicate(),(val))))

// - create new constant-value expression of unknown-width unsigned type:
#define constIntExprBaseS(val) \
  ((Expr*)(new ExprValue(NULL,(Type*)type_sintx->duplicate(),(val))))
*/

// - create "max(a,b)" expression-tree as "(a>b)?a:b" (a,b must be unique dups)
inline
Expr* newMaxExpr (Expr *e1, Expr *e2)
{
  return new ExprCond(NULL,new ExprBop(NULL,'>',e1,e2),
		      (Expr*)e1->duplicate(),
		      (Expr*)e2->duplicate());
}

extern string exprkindToString(ExprKind e);


#endif  // #ifndef _TDF_EXPR_
