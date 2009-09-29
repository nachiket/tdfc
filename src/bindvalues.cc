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
// SCORE TDF compiler:  bind parameter values
// $Revision: 1.150 $
//
//////////////////////////////////////////////////////////////////////////////
#include <iostream> // mostly debug
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/set.h>
#include "misc.h"
#include "symbol.h"
#include "expr.h"
#include "stmt.h"
#include "state.h"
#include "ops.h"
#include "operator.h"
#include "feedback.h"
#include "annotes.h"
#include "gc.h"
// N.B. I need this just to get ccParamCausesInstance 
//    issue here is that I need to know what to bind.
//    If we want this to be generic (not tied to backend)
//    then we'll need to have a way to make this more generic
//    (pass in ccParamCausesInstance as a function pointer?
//      just make this aware of all backends?).
//    In any case, watch this if we start adding backends with different
//    notions of instances.
#include "cctype.h"

using std::cout;
using std::endl;

using leda::list_item;
using std::cerr;
using std::endl;

// #define DEBUG_EVALUATE_EXPR


// things one could consider adding:
//  check expression equivalence
//   foo==foo --> true
//   foo>foo, foo<foo --> false
//   arb?foo:foo --> foo
//   foo+foo-> foo<<1
//   foo-foo -> 0
//   foo^foo -> 0
//  ...etc.


Expr *EvaluateExpr(Expr *orig);		// forward def
Expr *EvaluateExprDeep(Expr *orig);	// forward def


ExprValue *newExprValue_int (Token *token, Type *type, long long val)
{
  // - EC 6/11/03
  // - Allocate and return a new ExprValue*
  //     whose integer value is properly truncated to the type width
  // - Signed values are truncated by forcing sign extension bits
  //     to all 0 or all 1;
  //     truncated values retain same sign even if lose significant bits.
  // - Does not handle types other than TYPE_INT
  // - This function replaces "new ExprValue()" in most of this file

  assert(type->getTypeKind()==TYPE_INT);

  long long newval;
  int width = type->getWidth();

  if (width==-1) {
    Expr *ewidth = type->getWidthExpr();	// - should be eval'ed already
    // - *ewidth should already be evaluated,
    //     since resolve_bound_values evaluates an expr's type before the expr,
    //     but any exprs generated in EvaluateExpr have non-evaluated types,
    //     so we evaluate once here (without descending deep into type)
    if (gDeleteType)
      // - if types are being deleted, then type may not have been folded yet
      ewidth = EvaluateExprDeep(ewidth);
    if (ewidth->getExprKind()==EXPR_VALUE) {
      // - evaluates to const width;  do truncation below
      width = ((ExprValue*)ewidth)->getIntVal();
      assert(width>=0);
    }
    else {
      // - variable width;  NO truncation below;  just do new ExprValue()
      newval = val;
      warn(string("constant folding created value %ld",(long)newval) +
           " with non-constant width type " + type->toString(),
	   (type->getToken()  ? type->getToken() :
	    type->getParent() ? type->getParent()->getToken() : NULL));
    }
  }

  if (width==0 || val==0) {
    newval = 0;
  }
  else if (width>=64) {
    newval = val;	// - do not try to truncate a 64-bit value (or wider),
			//   since we represent it as a 64-bit long long
  }
  else if (width>0) {
    if (type->isSigned()) {
      if (val>0)
	newval = val & ~(-1ll << (width-1));	// - set sign-extensn bits to 0
      else if (val<0)
	newval = val |  (-1ll << (width-1));	// - set sign-extensn bits to 1
    }
    else
	newval = val & ~(-1ll << width);	// - set high-order bits to 0
  }

  if (newval!=val)
    warn(string("constant folding truncated %ld --> %ld to fit %ld bits ",
                (long)val,(long)newval,(long)width)
	     + (type->isSigned() ? "signed" : "unsigned"), token);

  return new ExprValue(token,type,newval);
}


Expr *EvaluateBitsof(Expr *bitsof_expr)
{
  // rewritten by EC 10/19/01
  // AMD's "EvaluateGetBits(Expr*)" was not descending to fold constants,
  //   rather it was always converting "bitsof(x)" into "x"

  assert(bitsof_expr->getExprKind()==EXPR_BUILTIN &&
	 ((OperatorBuiltin*)((ExprBuiltin*)bitsof_expr)->getOp())->
					getBuiltinKind()==BUILTIN_BITSOF);

  Expr *expr=((ExprBuiltin*)bitsof_expr)->getArgs()->head();  // first arg only
  Type *atype=expr->getType();
  switch (atype->getTypeKind())
    {
    case TYPE_BOOL:
      {
	if (expr->getExprKind()==EXPR_VALUE)
	  return newExprValue_int(bitsof_expr->getToken(),
				  new Type(TYPE_INT,1,false),
				  ((ExprValue*)expr)->getIntVal());
	else
	  return bitsof_expr;
	  // it might be better to convert to an int-type cond expr (e?0:1),
	  // but that makes a deeper expr tree
      }
    case TYPE_INT:
      {
	if (expr->getExprKind()==EXPR_VALUE) {
	  if (!atype->isSigned()) {
	    return expr;
	  }
	  else {
	    if (atype->getWidth()>=0)
	      return newExprValue_int(bitsof_expr->getToken(),
				      new Type(TYPE_INT,
					       atype->getWidth(),false),
				      ((ExprValue*)expr)->getIntVal());
	    else
	      return newExprValue_int(bitsof_expr->getToken(),
				      new Type(TYPE_INT,
					       atype->makeWidthExpr(),false),
				      ((ExprValue*)expr)->getIntVal());
	  }
	}
	else if (!atype->isSigned())
	  return expr;
	else
	  return bitsof_expr;
      }
    case TYPE_FIXED:
      {
	// TODO:  this almost certainly isn't right, 
        //      but, we need to work out more on how we're representing
	return(bitsof_expr);
      }
    default:
      error(-1,(string("type unknown in bitsof expression [")+
		expr->toString()+
		string("]")),
	    expr->getToken());
      return(bitsof_expr);
    }

}


Expr *EvaluateGetWidth(Expr *expr)
{

  Type *atype=expr->getType();
  switch (atype->getTypeKind())
    {
    case TYPE_BOOL:
      // - EC 6/12/03:  replace by constIntExpr for correct type of width
      // return(new ExprValue(expr->getToken(),
      //                      expr->getType(), 
      //                      1));
      return constIntExpr(1,expr->getToken());

    // Sporadically being added by Nachiket as and when required..
    case TYPE_FLOAT:
      return constIntExpr(32,expr->getToken());
    case TYPE_DOUBLE:
      return constIntExpr(64,expr->getToken());
    case TYPE_INT:
      {
	int width=atype->getWidth();
	if (width>=0)
	  // - EC 6/12/03:  replace by constIntExpr for correct type of width
	  // return(new ExprValue(expr->getToken(),
	  //                      expr->getType(),
	  //                      width));
	  return constIntExpr(width,expr->getToken());
	else
	  {
	    Expr *esize=atype->getWidthExpr();

	    // DEBUG
	    if (esize==(Expr *)NULL)
	      cerr << "type of expr ["
		   << expr->toString() 
		   << "] has no width expr: " 
		   << atype->toString() 
		   << " and width=" << width
		   << endl;

	    return(esize); 
	  }
      }
    case TYPE_FIXED:
      {
	// - EC 6/12/03:  should use constIntExpr below
	//                for correct type of width, but I am lazy
	TypeFixed *ftype=(TypeFixed *)atype;
	int iwidth=ftype->getIntWidth();
	int fwidth=ftype->getFracWidth();
	Expr *i=(Expr *)NULL;
	Expr *f=(Expr *)NULL;
	if (iwidth>=0)
	  i=new ExprValue(expr->getToken(),
			  new Type(TYPE_INT,6,0),
			  iwidth,0);
	else
	  {
	    Expr *esize=ftype->getIntWidthExpr();
	    i=esize; 
	  }
	if (fwidth>=0)
	  f=new ExprValue(expr->getToken(),
			  new Type(TYPE_INT,6,0),
			  fwidth,0);

	else
	  {
	    Expr *esize=ftype->getFracWidthExpr();
	    f=esize; 
	  }
	return(EvaluateExpr(new ExprBop(expr->getToken(),'+',i,f)));
      }
    default:
      error(-1, string("type unknown in widthof expression [") +
		expr->toString()  + "] type is [" + atype->toString() +
		string("] type kind is %d",(int)atype->getTypeKind()),
	    expr->getToken());
      return(expr);
    }
}


int zeroRetime(Expr *expr)
{
  if (expr==(Expr *)NULL)
    return(1);

  Expr *value=expr;
  if (value->getExprKind()!=EXPR_VALUE)
    return(0);
  else
    {
      int v=((ExprValue *)value)->getIntVal();
      if (v==0)
	return(1);
      else
	return(0);
    }

}


int power_of_two(long long val)
{
  int cnt=0;
  long long p2=1;
  while (p2!=0)
    {
      if (p2==val)
	return(cnt);
      p2=p2<<1;
      cnt++;
    }
  return(-1);
}


/*
Type *EvaluateExprType(Type *orig)
{
  // warn("EvaluateExprType " + orig->toString() + 
  //      (orig->getParent() ?
  //       ("\n     with parent "+orig->getParent()->toString()) : string()));

  int width=orig->getWidth();
  if (width>=0)
    return(orig);
  else
    return(new Type(orig->getTypeKind(),
		    EvaluateExpr(orig->getWidthExpr()),
		    orig->isSigned(),
		    EvaluateExpr(orig->getPredExpr())));
}
*/


Expr* EvaluateExprDeep (Expr *orig)
{
  // - Return constant-folded version of expression *orig,
  //     descending into tree (unlike EvaluateExpr, which does not descend)
  // - Used only to deep-evaluate expressions created within EvaluateExpr()
  extern bool resolve_bound_values_postMap2(Tree**,void*);
  orig->map2((Tree**)&orig,
	     (TreeMap2)NULL,(TreeMap2)resolve_bound_values_postMap2);
  return orig;
}


Expr* warnAndReturn (Expr *orig, Expr *e)
{
  // - EC:  debug, used to show return value of EvaluateExpr
  string change( (orig==e)         ? "[same]   " :
                 (orig->equals(e)) ? "[equiv]  " :
                                     "[new]    "  );
  warn("EvaluateExpr  " + change    + orig->toString()
                        + "  -->  " +    e->toString());
  return (e);
}


Expr *EvaluateExpr(Expr *orig)
{
  // - fold constants in expression, returning ExprValue where possible
  // - this code does NOT descend expr tree,
  //     it only processes the immediate children of orig if they are const
  // - to descend expr tree, call this in a post-order mapping over an expr

  if (orig==(Expr *)NULL)
    return(orig);

  if (orig->getType()->getTypeKind()==TYPE_ANY) {
    assert(!"internal error: TYPE_ANY in EvaluateExpr");
    orig->typeCheck();
    // warn(string("EvaluateExpr: generated type for ")+orig->toString());
    // - paranoid about expr that has never been type-checked  (EC 1/8/02)
    // - if we do not catch it here, bad type will propagate thru folding
    // - a better solution that obviates this step
    //     is to mark un-type-checked trees with type=NULL, not TYPE_ANY
    //     (requires modifying parser and many tree constructors)
  }

  // warn("EvaluateExpr "+orig->toString());
  // Type *type = orig->getType();
  // if (type->getWidth()==-1 && type->getWidthExpr()->getExprKind()!=EXPR_VALUE)
  //   warn("EvaluateExpr " + orig->toString() +
  //        " has non-constant width type " + type->toString());

  // - to debug, uncomment
  // #define return(x) return(warnAndReturn(orig,(x)))

  switch(orig->getExprKind())
    {
    case EXPR_VALUE:
      return(orig); 
    case EXPR_LVALUE:
      {
	ExprLValue *lexpr=(ExprLValue *)orig;
	Symbol *lsym=lexpr->getSymbol();
	if (  lsym->isParam() ||
	    ( lsym->getAnnote(ANNOTE_IS_READ) &&
	     !lsym->getAnnote(ANNOTE_IS_WRITTEN)))
	  {
	    // - constant propagate/fold symbol references
	    //      for constant-valued symbols (params, regs, locals)
	    Expr *value    = NULL;
	    Expr *arrayLoc = lexpr->getArrayLoc();
	    if (arrayLoc) {
	      // - lexpr is subscripted array x[y] --> get element value
	      if (arrayLoc->getExprKind()==EXPR_VALUE) {
		int loc = ((ExprValue*)arrayLoc)->getIntVal();
		ExprArray *arrayValue = (ExprArray*)
					((SymbolVar*)lsym)->getValue();
		if (arrayValue==NULL)
		  return orig;
		list<Expr*> *values = arrayValue->getExprs();
		if (loc<0 || loc>=values->size())
		  fatal(-1, string("array subscript %d out of bounds",loc),
			arrayLoc->getToken());
		value = values->inf(values->get_item(loc));
	      }
	      else
		return orig;
	    }
	    else {
	      // - lexpr is not subscripted array --> get symbol value
	      value = ((SymbolVar*)lsym)->getValue();
	    }
	    if (value==(Expr*)NULL)
	      {
		/*
		  cerr << "lvalue " << lsym->getName() 
		  << " in " << lsym->getOperator()->getName()
		  << " has no value " << endl;
		  */
		return(orig);
	      }
	    else
	      if ((lexpr->usesAllBits())	&& 
		  zeroRetime(lexpr->getRetime()))
		{
		  if (value->getExprKind()==EXPR_VALUE)
		    {
		      // warning: not work for fixed point
		      if (orig->getType()->getTypeKind()==TYPE_INT)
			return(newExprValue_int(orig->getToken(),
						orig->getType(),
						((ExprValue *)value)->
							getIntVal()));
		      else if (orig->getType()->getTypeKind()==TYPE_FLOAT)
			return(new ExprValue   (orig->getToken(),
						orig->getType(),
						((ExprValue *)value)->
							getFloatVal()));
		      else if (orig->getType()->getTypeKind()==TYPE_DOUBLE)
			return(new ExprValue   (orig->getToken(),
						orig->getType(),
						((ExprValue *)value)->
							getDoubleVal()));
		      else
			return(new ExprValue   (orig->getToken(),
						orig->getType(),
						((ExprValue *)value)->
							getIntVal()));
		    }
		  else
		    return(new ExprCast(orig->getToken(),
					orig->getType(),
					value));
		}
	      else
		{
		  Expr *low=lexpr->getPosLow();
		  Expr *high=lexpr->getPosHigh();
		  if ((low->getExprKind()==EXPR_VALUE) &&
		      (high->getExprKind()==EXPR_VALUE) &&
		      (value->getExprKind()==EXPR_VALUE) &&
		      zeroRetime(lexpr->getRetime())
		      )
		    {

		      int v=((ExprValue *)value)->getIntVal();
		      int l=((ExprValue *)low)->getIntVal();
		      int h=((ExprValue *)high)->getIntVal();

		      /* DEBUG
		      cerr << "lsym has value " << value->toString() << endl;
		      cerr << "type is " << orig->getType()->toString() << endl;
		      cerr << "v=" << v 
			   << " l=" << l << " h=" << h 
			   << " v>>l" << (v>>l)
			   << " ((1<<(h-l+1))-1) = " << ((1<<(h-l+1))-1)
			   << endl;
			   */

		      return(newExprValue_int(orig->getToken(),
					      orig->getType(),
					      ((v>>(l)) & ((1<<(h-l+1))-1))));
		    }
		  else
		    return(orig);
		}
	  }
      }
    case EXPR_CALL:
      {
	return(orig);
      }
    case EXPR_BUILTIN:
      {
	ExprBuiltin *bexpr=(ExprBuiltin *)orig;
	OperatorBuiltin *bop =(OperatorBuiltin *)bexpr->getOp();
	switch(bop->getBuiltinKind())
	  {
	  case BUILTIN_CAT:
	  case BUILTIN_PRINTF:
	    {

	      list<Expr *> *args=bexpr->getArgs();
	      Expr *exp;
	      int *widths=new int[args->size()];
	      long long *evals=new long long[args->size()];
	      int allvalues=1;
	      int i=0;
	      forall(exp,*args)
		{
		  Expr *eval_width=EvaluateGetWidth(exp);
		  if ((exp->getExprKind()==EXPR_VALUE) &&
		      (eval_width->getExprKind()==EXPR_VALUE))
		    {
		      widths[i]=((ExprValue *)eval_width)->getIntVal();
		      evals [i]=((ExprValue *)exp)->getIntVal();
		    }
		  else
		    {
		      allvalues=0;

		      // DEBUG
		      /*
		      cerr << "cat arg " << exp->toString() << " with width "
			   << eval_width->toString() << " is not constant "
			   << endl;
			   */

		    }
		  i++;
		}
	      if (bop->getBuiltinKind()==BUILTIN_CAT && allvalues==1)
		{
		  long long res=0;
		  for (i=0;i<args->size();i++)
		    {
		      res=((res<<widths[i])|evals[i]);
		      /* cerr << "evaluating cat i=" << i 
			   << " val=" << evals[i]
			   << " shift=" << widths[i+1]
			   << " res=" << res << endl; */
		    }
		  return(newExprValue_int(orig->getToken(),
					  orig->getType(),
					  res));
		}
	      else
		return(orig);
	      break;
	    }
	  case BUILTIN_WIDTHOF:
	    {
	      Expr *first=bexpr->getArgs()->head(); 
	      return(EvaluateGetWidth(first));
	    }
	  case BUILTIN_BITSOF:
	    {
	      // Expr *first=bexpr->getArgs()->head();	// modified by EC
	      // return(EvaluateGetBits(first));
	      return EvaluateBitsof(bexpr);
	    }
	  case BUILTIN_CLOSE:
	  case BUILTIN_DONE:
	  case BUILTIN_SEGMENT:
	    {
	      // leave them alone
	      return(orig);
	    }
	  default:
	    {
	      fatal(-1,
		    string("EvaluateExpr(bindvalues):unknown expr builtin[") +
		    orig->toString() + "]",
		    orig->getToken());
	      break;
	    }
	  }
      }
    case EXPR_COND:
      {
	ExprCond * cexpr=(ExprCond *)orig;
	Expr *cpart=cexpr->getCond();
	Expr *tpart=cexpr->getThenPart();
	Expr *epart=cexpr->getElsePart();
	if (cpart->getExprKind()==EXPR_VALUE)
	  {
	    int c=((ExprValue *)cpart)->getIntVal();
	    if (c==0)
	      return(epart);
	    else
	      return(tpart);
	  }
	else if ((tpart->getExprKind()==EXPR_VALUE)
		 && (epart->getExprKind()==EXPR_VALUE))
	  {
	    int t=((ExprValue *)tpart)->getIntVal();
	    int e=((ExprValue *)epart)->getIntVal();
	    if (t==e)
	      return(tpart);
	    else 
	      return(orig);
	  }
	else
	  return(orig);
      }
    case EXPR_BOP:
      {
	ExprBop *bexpr=(ExprBop *)orig;
	Expr* value1=bexpr->getExpr1();
	Expr* value2=bexpr->getExpr2();
	if ((value1->getExprKind()==EXPR_VALUE) &&
	    (value2->getExprKind()==EXPR_VALUE))
	  {
	    // NOTE: not work for fixed, need to fix
	    long long v1=((ExprValue *)value1)->getIntVal();
	    long long v2=((ExprValue *)value2)->getIntVal();
	    float v1f=((ExprValue *)value1)->getFloatVal();
	    float v2f=((ExprValue *)value2)->getFloatVal();
	    double v1d=((ExprValue *)value1)->getDoubleVal();
	    double v2d=((ExprValue *)value2)->getDoubleVal();
	    switch (bexpr->getOp())
	      {
	      case (RIGHT_SHIFT): return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1>>v2)));
	      case (LEFT_SHIFT):  return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1<<v2)));
	      case (EQUALS):      return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1==v2),0));
	      case (NOT_EQUALS):  return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1!=v2),0));
	      case (LOGIC_AND):   return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1&&v2),0));
	      case (LOGIC_OR):    return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1||v2),0));
	      case (GTE):         return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1>=v2),0));
	      case (LTE):         return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1<=v2),0));
	      case('<'):          return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1<v2),0));
	      case('>'):          return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  (v1>v2),0));
	      case('+'):   	  if((orig->getType())->getTypeKind()==TYPE_FLOAT) {
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (float)(v1f+v2f)));
			          } else if ((orig->getType())->getTypeKind()==TYPE_DOUBLE) {
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (double)(v1d+v2d)));
				  } else {
	      			     return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1+v2)));
				  }
	      case('-'):   	  if((orig->getType())->getTypeKind()==TYPE_FLOAT) {
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (float)(v1f-v2f)));
			          } else if ((orig->getType())->getTypeKind()==TYPE_DOUBLE) {
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (double)(v1d-v2d)));
				  } else {
	      			     return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1-v2)));
				  }
	      case('*'):   	  if((orig->getType())->getTypeKind()==TYPE_FLOAT) {
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (float)(v1f*v2f)));
			          } else if ((orig->getType())->getTypeKind()==TYPE_DOUBLE) {
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (double)(v1d*v2d)));
				  } else {
	      			     return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1*v2)));
				  }
	      case('/'):   	  if((orig->getType())->getTypeKind()==TYPE_FLOAT) {
					if (v2f==0)
					  {
					    cerr << "Evaluating: " << orig->toString() << endl;
					    cerr << "     found: " << v1 << "/" << v2 << endl;
					    error(-1,"divide by zero during bound instance constant folding",
						  orig->getToken());
					    return(new ExprValue(orig->getToken(),
								 orig->getType(),
								 0,0));
					}
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (float)(v1f/v2f)));
			          } else if ((orig->getType())->getTypeKind()==TYPE_DOUBLE) {
					if (v2d==0)
					  {
					    cerr << "Evaluating: " << orig->toString() << endl;
					    cerr << "     found: " << v1 << "/" << v2 << endl;
					    error(-1,"divide by zero during bound instance constant folding",
						  orig->getToken());
					    return(new ExprValue(orig->getToken(),
								 orig->getType(),
								 0,0));
					}
	      			     return(new ExprValue(orig->getToken(),
							  (orig->getType()),
							  (double)(v1d/v2d)));
				  } else {
					if (v2==0)
					  {
					    cerr << "Evaluating: " << orig->toString() << endl;
					    cerr << "     found: " << v1 << "/" << v2 << endl;
					    error(-1,"divide by zero during bound instance constant folding",
						  orig->getToken());
					    return(new ExprValue(orig->getToken(),
								 orig->getType(),
								 0,0));
					  }
	      			     return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1/v2)));
			  	  }
	      case('%'):          return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1%v2)));
	      case('|'):          return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1|v2)));
	      case('^'):          return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1^v2)));
	      case('&'):          return(newExprValue_int(orig->getToken(),
							  (orig->getType()),
							  (v1&v2)));
	      case('.'):          return(new ExprValue   (orig->getToken(),
							  (orig->getType()),
							  v1,v2));
	      default:
		error(-1,
		      string("Unknown op in EvaluateExpr [%s] in expression [%s]",
			     (const char *)opToString(bexpr->getOp()),
			     (const char *)bexpr->toString()
			     ),
		      orig->getToken());
		return(orig);
	      }
	  }
	else if ((value1->getExprKind()==EXPR_VALUE) ||
		 (value2->getExprKind()==EXPR_VALUE))
	  {


	    Expr *value_expr;
	    long long v;
	    float vf;
	    double vd;
	    if (value1->getExprKind()==EXPR_VALUE) 
	      {

		long long v1=((ExprValue *)value1)->getIntVal();
		float v1f=((ExprValue *)value1)->getFloatVal();
		double v1d=((ExprValue *)value1)->getDoubleVal();
		switch (bexpr->getOp())
		  {
		  case('-'):
		    if (
		    	((((ExprValue *)value1)->getType())->getTypeKind()==TYPE_FLOAT && v1f==0)  ||
		    	((((ExprValue *)value1)->getType())->getTypeKind()==TYPE_DOUBLE && v1d==0) ||
		    	((((ExprValue *)value1)->getType())->getTypeKind()==TYPE_INT && v1==0))
		      return(new ExprCast(orig->getToken(),
					  orig->getType(),
					  new ExprUop(orig->getToken(),
						      '-',
						      value2)));
		    else break;
		  }
		v=v1;
		vf=v1f;
		vd=v1d;
		value_expr=value2;
	      }
	    else
	      {
		long long v2=((ExprValue *)value2)->getIntVal();
		float v2f=((ExprValue *)value2)->getFloatVal();
		double v2d=((ExprValue *)value2)->getDoubleVal();
		switch (bexpr->getOp())
		  {
		  case('-'):
		    if (
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_FLOAT && v2f==0)  ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_DOUBLE && v2d==0) ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_INT && v2==0))
		      return(new ExprCast(orig->getToken(),
					  orig->getType(),
					  value1));
		    else break;
		  case (RIGHT_SHIFT): 
		  case (LEFT_SHIFT):  
		    if (
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_FLOAT && v2f==0)  ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_DOUBLE && v2d==0) ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_INT && v2==0))
		      return(new ExprCast(orig->getToken(),
					  (orig->getType()),
					  value1));
		    else break;
		  case('/'): 
		    if (
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_FLOAT && v2f==0)  ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_DOUBLE && v2d==0) ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_INT && v2==0))
		      {
			cerr << "Evaluating: " << orig->toString() << endl;
			cerr << "     found: " << value1->toString() 
			     << "/" << v2 << endl;
			error(-1,"divide by zero during bound instance constant folding",
			      orig->getToken());
			return(newExprValue_int(orig->getToken(),
						orig->getType(),
						0));
		      }
		    else
		      if (
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_FLOAT && v2f==1)  ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_DOUBLE && v2d==1) ||
		    	((((ExprValue *)value2)->getType())->getTypeKind()==TYPE_INT && v2==1))
			return(new ExprCast(orig->getToken(),
					    (orig->getType()),
					    value1));
		      else if(((((ExprValue *)value1)->getType())->getTypeKind()!=TYPE_FLOAT)  &&
				      ((((ExprValue *)value1)->getType())->getTypeKind()!=TYPE_DOUBLE)) {
			      int p2=power_of_two(v2);
			      if (p2>0)
				      return(new ExprCast(orig->getToken(),
							      orig->getType(),
							      new ExprBop(orig->getToken(),
								      RIGHT_SHIFT,
								      value1,
								      newExprValue_int(value2->getToken(),
									      value2->getType(),
									      p2))));

		      }
		      else break;
		  case('%'): 
		    if (power_of_two(v2)>0) // turn into mask
		      return(new ExprCast(orig->getToken(),
					  orig->getType(),
					  new ExprBop(orig->getToken(),
						      '&',
						      value1,
						      newExprValue_int(value2->getToken(),
								       value2->getType(),
								       (v2-1)))));
		    else break;
		  }
		v=v2;
		vf=v2f;
		vd=v2d;
		value_expr=value1;
	      }

	    // handle cases where order not important

	    // NOTE: not work for fixed, need to fix
	    switch (bexpr->getOp())
	      {
	      case (LOGIC_AND): 
		if (v==0)
		  return(new ExprValue(orig->getToken(),
				       orig->getType(),
				       0,0));
		else if (v==1)
		  return(new ExprCast(orig->getToken(),
				      orig->getType(),
				      value_expr));
		else
		  break;
	      case (LOGIC_OR): 
		if (v==0)
		  return(new ExprCast(orig->getToken(),
				      orig->getType(),
				      value_expr));
		else if (v==1)
		  return(new ExprValue(orig->getToken(),
				       orig->getType(),
				       1,0));
		else
		  break;
	      case('+'):   	  if(
	      				((orig->getType())->getTypeKind()==TYPE_FLOAT && vf==0) ||
	      				((orig->getType())->getTypeKind()==TYPE_DOUBLE && vd==0) ||
	      				((orig->getType())->getTypeKind()==TYPE_INT && v==0))
					  {
					    cerr << "folding out +0 on add [" << orig->toString() 
						 << "]" << endl;
					    return(new ExprCast(orig->getToken(),
								(orig->getType()),
								value_expr));
					  }
				  else break;
	      case('*'):   	  if(
	      				((orig->getType())->getTypeKind()==TYPE_FLOAT && vf==0) || 
	      				((orig->getType())->getTypeKind()==TYPE_DOUBLE && vd==0) || 
	      				((orig->getType())->getTypeKind()==TYPE_INT && v==0))
					  {
					    cerr << "folding out *1 on mult [" << orig->toString() 
						 << "]" << endl;
					    return(new ExprCast(orig->getToken(),
								(orig->getType()),
								value_expr));
					  }
	      			  else if(
				  	((orig->getType())->getTypeKind()==TYPE_FLOAT && vf==1) || 
	      				((orig->getType())->getTypeKind()==TYPE_DOUBLE && vd==1) || 
	      				((orig->getType())->getTypeKind()==TYPE_INT && v==1)) 
					  {
					    cerr << "folding out *1 on mult [" << orig->toString() 
						 << "]" << endl;
					    return(new ExprCast(orig->getToken(),
								(orig->getType()),
								0));
					  }
				  else break;
	      case('|'): 
		if (v==0)
		  return(new ExprCast(orig->getToken(),
				      orig->getType(),
				      value_expr));
		else break;
	      case('^'): 
		if (v==0)
		  return(new ExprCast(orig->getToken(),
				      orig->getType(),
				      value_expr));
		else break;
	      case('&'): 
		if (v==0)
		  return(new ExprValue(orig->getToken(),
				       orig->getType(),
				       0,0));
		else break;
	      }

	    /*
	    // - Try regrouping to cluster constants:  (x+K1)+K2 = x+(K1+K2)
	    //     (apply associative property, possibly w/commutation)
	    if (value_expr->getExprKind()==EXPR_BOP) {
	      ExprBop *value_bexpr = (ExprBop*)value_expr;
	      Expr *child1     = value_bexpr->getExpr1();
	      Expr *child2     = value_bexpr->getExpr2();
	      Expr *child_value= (child1->getExprKind()==EXPR_VALUE) ? child1 :
				 (child2->getExprKind()==EXPR_VALUE) ? child2 :
				 (NULL);
	      Expr *child_expr = (child1->getExprKind()==EXPR_VALUE) ? child2 :
				 (child2->getExprKind()==EXPR_VALUE) ? child1 :
				 (NULL);
	      if (child_expr) {
		Expr *sibling = (value_expr==value1) ? value2 : value1;
		switch (bexpr->getOp())
		{
		  case (LOGIC_AND):
		  case (LOGIC_OR):
		  case ('&'):
		  case ('|'):
		  case ('^'):
		  case ('+'):
		  case ('*'):	if (bexpr->getOp()==value_bexpr->getOp()) {
				  ExprBop *fold = new ExprBop(NULL,
							      bexpr->getOp(),
							      child_value,
							      sibling);
				  Expr *folded = EvaluateExpr(fold);
				  return new ExprCast(orig->getToken(),
						      orig->getType(),
						      new ExprBop(orig->getToken(),
								  bexpr->getOp(),
								  child_expr,
								  folded));
				}
				break;
		  case ('-'):
		  case ('/'):
		  case (LEFT_SHIFT):
		  case (RIGHT_SHIFT):
				if (bexpr->getOp()==value_bexpr->getOp() &&
				    value_bexpr==bexpr->getExpr1() &&
				    child_expr==value_bexpr->getExpr1()) {
				  // - depends on order, non-const must be 1st
				  //     (x-K1) - K2 = x- (K1+K2)
				  //     (x/K1) / K2 = x/ (K1*K2)
				  //     (x>>K1)>>K2 = x>>(K1+K2)
				  //     (x>>K1)<<K2 = x<<(K1+K2)
				  int bop = bexpr->getOp();
				  int foldbop = (bop=='-')         ? '+' :
						(bop=='/')         ? '*' :
						(bop==LEFT_SHIFT)  ? '+' :
						(bop==RIGHT_SHIFT) ? '+' : 'X';
				  ExprBop *fold = new ExprBop(NULL, foldbop,
							      child_value,
							      sibling);
				  Expr *folded = EvaluateExpr(fold);
				  return new ExprCast(orig->getToken(),
						      orig->getType(),
						      new ExprBop(orig->getToken(),
								  bop,
								  child_expr,
								  folded));
				}
				break;
				// - additional regroupings for +, -
				//     (x+Y)-Z     =     x+(Y-Z)
				//     (X+y)-Z     =     y+(X-Z)
				//         X-(y+Z) = (X-Z)-y
				//         X-(Y+z) = (X-Y)+z
				//     (x-Y)+Z     =     x-(Y+Z)
				//     (X-y)+Z     = (X+Z)-y
				//     X+(y-Z)     =     y+(X-Z)
				//     X+(Y-z)     = (X+Y)-z
		}
	      }
	    }
	    */

	    return(orig); // none of these cases applied
	  }

	else {
	  // - Algebraic transforms go here.  Test: expr1->equals(expr2)
	  //   x-x     ==  0
	  //   x+x     ==  cat(x,0)
	  //   x/x     ==  1		// - assuming x!=0
	  //   x&x     ==  x
	  //   x|x     ==  x
	  //   x^x     ==  0
	  //   x==x    ==  true
	  //   x!=x    ==  false
	  //   x<x     ==  false
	  //   x>x     ==  false
	  //   x<=x    ==  true
	  //   x>=x    ==  true
	  //   x?y:y   ==  y
	  //   (x+y)-x ==  y		// - width of shifted bit sub-ranges
	  //   (x-y)-x == -y		// - e.g. z[x+2:x] --> 3 bits
	  //   (y+x)-x ==  y
	  //   (y-x)+x ==  y
	  //   x-(x+y) == -y
	  //   x-(y+x) == -y
	  //   x-(x-y) ==  y
	  //   x+(y-x) ==  y

	  return(orig);
	}

	cerr << " SHOULDN'T BE GETTING HERE !" << endl;
	cerr << "expr is: " << orig->toString() << endl;

	break;

      }
    case EXPR_UOP:
      {
	ExprUop *uexpr=(ExprUop *)orig;
	Expr *ivalue=uexpr->getExpr();
	if (ivalue->getExprKind()==EXPR_VALUE)
	  {
	    long long v=((ExprValue *)ivalue)->getIntVal();
	    float vf=((ExprValue *)ivalue)->getFloatVal();
	    double vd=((ExprValue *)ivalue)->getDoubleVal();
	    switch(uexpr->getOp())
	      {
	      case('-'): 
	      case('~'):
	       	if((orig->getType())->getTypeKind()==TYPE_FLOAT) {
	      		return(new ExprValue(orig->getToken(),
						 (orig->getType()),
						 (-vf)));
		} else if((orig->getType())->getTypeKind()==TYPE_DOUBLE) {
	      		return(new ExprValue(orig->getToken(),
						 (orig->getType()),
						 (-vd)));
		} else { 
		//cout << typekindToString(orig->getType()->getTypeKind()) << endl;
			return(new ExprValue (orig->getToken(),
						 (orig->getType()),
						 (-v)));
		}
	      case('+'):
	       	if((orig->getType())->getTypeKind()==TYPE_FLOAT) {
	      		return(new ExprValue(orig->getToken(),
						 (orig->getType()),
						 (vf)));
		} else if((orig->getType())->getTypeKind()==TYPE_DOUBLE) {
	      		return(new ExprValue(orig->getToken(),
						 (orig->getType()),
						 (vd)));
		} else { 
			return(newExprValue_int(orig->getToken(),
						 (orig->getType()),
						 (v)));
		}
	      case('!'): return(new ExprValue   (orig->getToken(),
						 (orig->getType()),
						 (!v),0));
	      default:
		error(-1,string("Unknown unary op in Evaluate Expr [%s]",
				opToString(uexpr->getOp())),
		      orig->getToken());
		return(orig);
	      }
	  }
	else
	  return(orig);
      }
    case EXPR_CAST:
      {
	
	ExprCast *cexpr=(ExprCast *)orig;
//	Expr *real_value=EvaluateExpr(cexpr->getExpr());
	Expr *real_value=(cexpr->getExpr());
	if (real_value->getExprKind()==EXPR_VALUE)
	  {
	    return(newExprValue_int(orig->getToken(),
				    (orig->getType()),
				    ((ExprValue *)real_value)->getIntVal()));
	  }
	else
	  return(orig);
      }
    case EXPR_ARRAY:
      {
	// basic idea is that we just want to evaluate each of the 
	//  subsidiary exprs....which will be done by the map call, 
	//  so there's nothing to do at this node.
	return(orig); 
      }
    default:
      fatal(-1,(string("unknown Expression type [")+orig->toString()+
		string("]")),
	    orig->getToken());
      return(orig);
    }

  // - to debug, uncomment
  // #undef return
    
}


/* NOT USED  [ replaced by findReadWrittenSyms() ]
 *
bool findUnassignedVars_preMap (Tree *t, void *i)
{
  // - scan behavioral operator *t for symbols
  //     that are dynamically assigned in assignment or inline call
  // - pre-map adds each dynamically assigned symbol to set<Symbol*> *i

  set<Symbol*> *assignedSyms = (set<Symbol*>*)i;

  Tree *p = t->getParent();

  switch (t->getKind())
  {
    case TREE_OPERATOR:
    case TREE_STATE:
    case TREE_STATECASE:	{ return true; }

    case TREE_STMT:	{
			  if (((Stmt*)t)->getStmtKind()==STMT_ASSIGN) {
			    ExprLValue *lvalue=((StmtAssign*)t)->getLValue();
			    Symbol *sym=lvalue->getSymbol();
			    if (sym->isReg() || sym->isLocal()) {
			      assignedSyms->insert(sym);
			      // warn("ASSIGNED   SYM: "+sym->getName());
			    }
			  }
			  return true;
			}

    case TREE_EXPR:	{
			  if (((Expr*)t)->getExprKind()==EXPR_LVALUE &&
			      p && p->getKind()==TREE_EXPR &&
			      ((Expr*)p)->getExprKind()==EXPR_CALL) {
			    ExprLValue *lvalue = (ExprLValue*)t;
			    Symbol     *actual = lvalue->getSymbol();
			    if (actual->isReg() || actual->isLocal()) {
			      ExprCall   *call     = (ExprCall*)t->getParent();
			      Operator   *calledOp = call->getOp();
			      int pos = call->getArgs()->rank(lvalue);
			      Symbol *formal =
				calledOp->getArgs()->inf(
				calledOp->getArgs()->get_item(pos-1));
			      if (formal->getSymKind()==SYMBOL_STREAM &&
				  ((SymbolStream*)formal)->getDir()==STREAM_OUT) {
				assignedSyms->insert(actual);
				// warn("ASSIGNED   SYM: "+actual->getName());
			      }
			    }
			  }
			  return true;
			}

    default:		{ return false; }
  }

  return false;		// - dummy
}
 *
 */


/* NOT USED  [ replaced by findReadWrittenSyms() ]
 *
bool findUnassignedVars_postMap (Tree *t, void *i)
{
  // - scan behavioral operator *t for symbols
  //     that are dynamically assigned in assignment or inline call
  // - post-map sets annote CC_IS_UNASSIGNED_VAR
  //     for each reg/local variable (Symbol) that is not dynamically assigned

  set<Symbol*> *assignedSyms = (set<Symbol*>*)i;

  switch (t->getKind())
  {
    case TREE_OPERATOR:	{
			  OperatorBehavioral *op = (OperatorBehavioral*)t;
			  Symbol *sym;
			  forall (sym, *op->getVars()->getSymbols())
			    if (!assignedSyms->member(sym)) {
			      sym->setAnnote_(CC_IS_UNASSIGNED_VAR,1);
			      // warn("UNASSIGNED VAR: "+sym->getName());
			    }
			  return true;
			}

    case TREE_STATE:
    case TREE_STATECASE:	{ return true; }

    case TREE_STMT:	{
			  if (((Stmt*)t)->getStmtKind()==STMT_BLOCK) {
			    StmtBlock *block = (StmtBlock*)t;
			    Symbol *sym;
			    forall (sym, *block->getSymtab()->getSymbols())
			      if (!assignedSyms->member(sym)) {
				sym->setAnnote_(CC_IS_UNASSIGNED_VAR,1);
				// warn("UNASSIGNED VAR: "+sym->getName());
			      }
			  }
			  return true;
			}

    default:		{ return false; }
  }

  return false;		// - dummy
}
 *
 */


/* NOT USED  [ replaced by findReadWrittenSyms() ]
 *
void findUnassignedVars (OperatorBehavioral *op)
{
  // - Scan behavioral operator *op for reg/local variables that are
  //     not assigned dynamically, and set their annote CC_IS_UNASSIGNED_VAR
  // - we use this for a very simple case of constant propagation,
  //     namely converting non-assigned vars into their initialization value

  set<Symbol*> assignedSyms;
  op->map(findUnassignedVars_preMap,
	  findUnassignedVars_postMap,
	  (void*)&assignedSyms); 
}
 *
 */


bool isInAssignContext (Expr *e)
{
  // - return true if Expr *e is in an assignment context, namely
  //     (1) e is right hand side of assignment stmt
  //     (2) e is argument of inline call/builtin, formal is an output stream
  //     (3) e is argument of inline call/builtin, formal is an array parameter
  //           (conservatively assume that array is written in call)

  Tree *p = e->getParent();

  if (p && p->getKind()==TREE_STMT &&
      ((Stmt*)p)->getStmtKind()==STMT_ASSIGN &&
      ((StmtAssign*)p)->getLValue()==e)
    return true;

  if (p && p->getKind()==TREE_EXPR &&
      (((Expr*)p)->getExprKind()==EXPR_CALL ||
       ((Expr*)p)->getExprKind()==EXPR_BUILTIN)) {
    ExprCall      *call     = (ExprCall*)p;	// - builtin is call too
    Operator      *calledOp = call->getOp();
    list<Expr*>   *actuals  = call->getArgs();
    list<Symbol*> *formals  = calledOp->getArgs();
    int            pos0     = actuals->rank(e)-1;
    if (pos0>=formals->size())
      return false;
      // - if argument has no declared formal (formal '...' as in cat())
      //   then assume argument is read, not assigned
    Symbol        *formal   = formals->inf(formals->get_item(pos0));
    if (formal->getSymKind()==SYMBOL_STREAM &&
	((SymbolStream*)formal)->getDir()==STREAM_OUT)
      return true;
    if (formal->getSymKind()==SYMBOL_VAR && formal->isArray()) {
      return true;	// - NOTE: misses arrays passed in compositional ops
      /*
      if (e->getExprKind()==EXPR_LVALUE &&
	  ((ExprLValue*)e)->getSymbol()->getSymKind()==SYMBOL_VAR &&
	  ((SymbolVar*)((ExprLValue*)e)->getSymbol())->getValue() &&
	  ((SymbolVar*)((ExprLValue*)e)->getSymbol())->getValue()
				       ->getExprKind()==EXPR_ARRAY  )
	// && optionTarget=TARGET_VERILOG
	return false;
      else
	return true;
      */
    }
  }

  return false;
}


bool findReadWrittenSyms_map (Tree *t, void *i)
{
  // - Scan behavioral operator *t for symbols that are
  //     dynamically read / writen and set their annotes
  //     ANNOTE_IS_READ, ANNOTE_IS_WRITTEN, respectively

  Tree *p = t->getParent();

  switch (t->getKind())
  {
    case TREE_OPERATOR:
    case TREE_SYMTAB:
    case TREE_SYMBOL:
    case TREE_STATE:
    case TREE_STATECASE:
    case TREE_STMT:	{ return true; }

    case TREE_EXPR:	{
			  Expr *e = (Expr*)t;
			  if (e->getExprKind()==EXPR_LVALUE) {
			    Symbol *sym = ((ExprLValue*)e)->getSymbol();
			    if (isInAssignContext(e)) {
			      sym->setAnnote_(ANNOTE_IS_WRITTEN,1);
			      // warn ("WRITTEN: "+sym->getName(),e->getToken());
			    }
			    else {
			      sym->setAnnote_(ANNOTE_IS_READ,1);
			      // warn ("READ:    "+sym->getName(),e->getToken());
			    }
			    if (p && p->getKind()==TREE_EXPR &&
				(((Expr*)p)->getExprKind()==EXPR_CALL ||
				 ((Expr*)p)->getExprKind()==EXPR_BUILTIN) &&
				e->getType()->getTypeKind()==TYPE_ARRAY) {
			      // - assume array param for inlined call/builtin
			      //     is read in the called op
			      // - isInAssignContext() above already assumes
			      //     it is written in the called op
			      sym->setAnnote_(ANNOTE_IS_READ,1);
			      // warn ("READ:    "+sym->getName(),e->getToken());
			    }
			  }
			  return true;
			}

    default:		{ return false; }
  }

  return false;		// - dummy
}


void findReadWrittenSyms (OperatorBehavioral *op)
{
  // - Scan behavioral operator *op for symbols that are
  //     dynamically read / writen and set their annotes
  //     ANNOTE_IS_READ, ANNOTE_IS_WRITTEN, respectively
  // - we use this for a very simple case of constant propagation,
  //     namely converting read-only vars into their initialization value

  op->map(findReadWrittenSyms_map);
}


bool resolve_bound_values_postMap2(Tree **t, void *aux)
{

  if ((*t)->getKind()==TREE_EXPR)
    {
      Expr **e=(Expr **)t;
#ifdef DEBUG_EVALUATE_EXPR
      Expr *orig=*e;
#endif

      *e=EvaluateExpr(*e);


#ifdef DEBUG_EVALUATE_EXPR
      if (*e!=orig)
	if (*e==(Expr *)NULL)
	  cerr << "EvaluateExpr(" << orig->toString() << ")-->NULL" << endl;
      else
	cerr << "EvaluateExpr(" << orig->toString() << ")-->" 
	     << (*e)->toString() << endl;
#endif
    }

  else
    if ((*t)->getKind()==TREE_STMT)
      if (gDeleteType)
	{
	  rmType(*t);
	}


  return(1);
}


void resolve_bound_values (Operator **op)
{
  // - first identify read-only symbols
  //     for constant-propagating initial values in EvaluateExpr()
  if ((*op)->getOpKind()==OP_BEHAVIORAL)
    findReadWrittenSyms((OperatorBehavioral*)(*op));

  // - if types are being deleted, then do constant-folding w/fresh types
  //     (remove all types now, so the map below does not fold types;
  //      then EvaluateExpr() will generate fresh types as needed,
  //      in post-fix order, i.e. fresh types will be made from folded exprs)
  if (gDeleteType)
    rmType(*op);

  // - then recurse to fold constants
  (*op)->map2((Tree **)op,
	      (TreeMap2)NULL,
	      (TreeMap2)resolve_bound_values_postMap2);
}


bool set_values_preMap (Tree *t, void *aux)
{ 
  // - optionally pass non-NULL *aux to also bind values into
  //     called segment ops (for -everilog), otherwise ignore seg ops

  if (t->getKind()==TREE_EXPR)
    {
      Expr *expr = (Expr *)t;
      if ( expr->getExprKind()==EXPR_CALL ||
	  (expr->getExprKind()==EXPR_BUILTIN &&
	   ((OperatorBuiltin*)((ExprBuiltin*)expr)->getOp())->
				getBuiltinKind()==BUILTIN_SEGMENT) && aux)
	{
	  ExprCall *ecall=(ExprCall *)expr;
	  list <Expr*> *actuals=ecall->getArgs();
	  list <Symbol *> *formals=(ecall->getOp())->getArgs();
	  Symbol *sym;
	  list_item act=actuals->first();
	  // cerr << "call to " << ecall->getOp()->getName() << endl;
	  forall(sym,*formals)
	    {
	      Expr *actual=actuals->inf(act); 
	      act=actuals->succ(act);  // is this like a cdr, 
	      // or does it imply a search?
	      // if (ccParamCausesInstance(sym)) // - ignore arrays, amd
	      if (sym->isParam())		 // - handle arrays, EC 8/16/04
		{
		  // warn("BINDING "+actual->toString()+" into "+
		  //      sym->getName()+" when calling "+
		  //      ecall->getOp()->getName(), actual->getToken());
		  if (sym->getType()->getTypeKind()==TYPE_ARRAY)
		    // - Eylon's handling of array parameter values
		    // - Arrays (segments) are passed by value-return,
		    //     not by value;  furthermore, the caller cannot know
		    //     if callee modifies array (no syntax, no analysis);
		    //     so we CANNOT bind an array value into a call
		    // - Instead, we leave a pointer to the call actual,
		    //     so we can later chase a read-only array parameter
		    //     from the callee (who know it does not modify array)
		    //     up the call stack to find the original passed value
		    //     (e.g. -everilog turns inline arrays into ROMs)
		    sym->setAnnote_(ANNOTE_VALUE_PASSED,actual);
		  else
		    // - Andre's binding
		    // - Note, if we blindly use this on array parameters,
		    //     then passing an array variable leaves an
		    //     unlinked symbol in the callee, error in flatten()
		    ((SymbolVar *)sym)->setValue(actual);

		  // cerr << "  bound formal " << sym->getName() << " to "
		  //      << ((SymbolVar *)sym)->getValue()->toString() << endl;
		}
	    }
	  // now recusrively call bindvalues on the called operator
	  Operator *op=ecall->getOp();
	  resolve_bound_values(&op);
	}
      else
	return(1);
    }
  if (t->getKind()==TREE_SYMBOL)
    return(0);
  else
    return(1);
}


void set_values (ExprCall *call, bool bindSegOps=false)
{
  // - bind call actuals into called operator's formals + propagate values
  // - optionally pass bindSegOps to also bind values into
  //     called segment ops (for -everilog), otherwise ignore seg ops

  call->map(set_values_preMap, (TreeMap)NULL, (void*)bindSegOps);
}


void set_values (Operator *op, bool bindSegOps=false)
{
  // - bind call actuals into called operator's formals + propagate values
  // - optionally pass bindSegOps to also bind values into
  //     called segment ops (for -everilog), otherwise ignore seg ops

  op->map(set_values_preMap, (TreeMap)NULL, (void*)bindSegOps);
}


void bindvalues(Operator *op, FeedbackRecord *rec)
{

  timestamp(("begin binding values ") + op->getName());

  // (1) bind up args from record
  int i=0;
  Symbol *sym;
  forall(sym,*(op->getArgs()))
    {
      if (rec->isParam(i))
	if (ccParamCausesInstance(sym))
	  {
	    SymbolVar *svar=(SymbolVar *)sym;

	    TypeKind t =  (*(*svar).getType()).getTypeKind();
	    if(t==TYPE_DOUBLE) {
		    svar->setValue(new ExprValue(sym->getToken(),
					 sym->getType(),
					 (double)rec->getParam(i)));
	    } else if(t==TYPE_FLOAT) {
		    svar->setValue(new ExprValue(sym->getToken(),
					 sym->getType(),
					 (float)rec->getParam(i)));
	    } else {
		    // TODO/note: not handling fixed point case
		    svar->setValue(new ExprValue(sym->getToken(),
					 sym->getType(),
					 rec->getParam(i), 0));
	    }

	    
	    /* DEBUG - Nachiket recognizes how useful this was with a comment
	       cerr << " set initial value [" << svar->getName() << "[" << (*(*svar).getType()).toString()  << "]"
	       << " in " << svar->getOperator()->getName() << "]" 
	       << "<-" << rec->getParam(i) << endl;
	       cerr << " readback initial value " << svar->getValue()->toString()
	       << endl;
	    */
					 
	  }
	else
	  {

	    if (!(sym->isParam()))
	      error(-1,(string("parameter %d (",i)+sym->getName()+
			string(") is not a parameter but was given a value [0x%x]",
			       rec->getParam(i)
			       )),sym->getToken());
	  }
      i++;
	
    }
  // (3)  propogate constants and fold
  //   --- map over all exprs: if expr is lvalue and symbol is param
  //                                              or symbol is never assigned,
  //                           and if expr has value,
  //                           replace exprlvalue with exprvalue
  resolve_bound_values(&op);
  set_values(op);
  timestamp(("end binding values ") + op->getName());

  // THIS IS PROBABLY BUGGY -- amd 9/1 
  // plan: first let eylon solve his other problem,
  //   then need to understand what this is deleting which 
  //   it shouldn't be.  Thought we got the operator into the root set
  //   OHH, as I write this, I think we've discovered (hypothesized)
  //   that the problem is that the child operators ....
  if (gEnableGC)
    {
      TreeGC::gc();	
      timestamp(("end gc after binding values ") + op->getName());
    }

}


////////////////////////////////////////////////////////////////


ExprArray* chaseArrayValue (Symbol *sym)
{
  // - Return bound value of array *sym,  [ AFTER bindvalues() ]
  //     chasing array param up call stack if necessary
  // - Special treatment is necessary for array parameters,
  //     because bindvalues() does not propagate their values into calls
  // - Array parameters are passed by value-return, not by value,
  //     and the caller does not know if the callee modifies the array
  //     (so binding array param value into call would be wrong)
  // - Hence we chase an array parameter from the callee,
  //     who knows it is read-only, up the call stack, to get its value
  //     (chase via ANNOTE_VALUE_PASSED expr ptrs)
  // - With flatten(), the call stack is only one level deep (top composition)

  assert(sym->getType()->getTypeKind()==TYPE_ARRAY);

  ExprArray *arrayValue = NULL;		// - array value to recover
  bool       chase      = true;		// - loop condition
  Symbol    *chaseSym   = sym;		// - loop var, chase up the call stack
  while (chase) {
    assert(chaseSym->getSymKind()==SYMBOL_VAR);
    Expr *chaseSymValue = ((SymbolVar*)chaseSym)->getValue();
			  // - for locally defined array
    if  (!chaseSymValue)
          chaseSymValue = (Expr*)sym->getAnnote(ANNOTE_VALUE_PASSED);
			  // - for parameter array, passed from caller
    if  (!chaseSymValue) {
      // - no value found, done  (probably an error)
      // warn("array " + sym->getName() + " has no value, "
      //      "so -everilog will generate don't-cares instead of ROM",
      //      sym->getToken());
      arrayValue = NULL;
      chase = false;
    }
    else if (chaseSymValue->getExprKind()==EXPR_ARRAY) {
      // - array value found, done
      // warn("FOUND array parameter "+sym->getName()+" binding to value "+
      //      chaseSymValue->toString(), sym->getToken());
      arrayValue = (ExprArray*)chaseSymValue;
      chase = false;
    }
    else {
      // - chase symbol to caller
      assert(chaseSymValue->getExprKind()==EXPR_LVALUE);
      chaseSym = ((ExprLValue*)chaseSymValue)->getSymbol();
      // warn("CHASING array parameter "+sym->getName()+" binding to "+
      //      chaseSym->toString(), sym->getToken());
    }
  }

  return arrayValue;
}

