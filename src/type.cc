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
// SCORE TDF compiler:  TDF data types
// $Revision: 1.145 $
//
//////////////////////////////////////////////////////////////////////////////


#include <assert.h>
#include "type.h"
#include "expr.h"
#include "gc.h"

#define safeEquals(a,b) ((a) ? ((b)?(a)->equals(b):false) : ((b)?false:true))

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif


////////////////////////////////////////////////////////////////
//  pre-defined types
//   - use duplicates of these  (except type_none, type_any, which are shared)

/*const*/ Type	*type_none	= NULL;		// for non-typed trees
/*const*/ Type	*type_any	= NULL;		// wildcard type
/*const*/ Type	*type_bool	= NULL;		// boolean (t=1, f=0)
/*const*/ Type	*type_uintx	= NULL;		// unknown width
/*const*/ Type	*type_uint0	= NULL;		// for const 0
/*const*/ Type	*type_uint1	= NULL;		// 0,1
/*const*/ Type	*type_uint8	= NULL;
/*const*/ Type	*type_uint16	= NULL;
/*const*/ Type	*type_uint32	= NULL;
/*const*/ Type	*type_uint64	= NULL;
/*const*/ Type	*type_sintx	= NULL;		// unknown width
/*const*/ Type	*type_sint1	= NULL;		// 0,-1
/*const*/ Type	*type_sint8	= NULL;
/*const*/ Type	*type_sint16	= NULL;
/*const*/ Type	*type_sint32	= NULL;
/*const*/ Type	*type_sint64	= NULL;


////////////////////////////////////////////////////////////////
//  init

void Type::init ()
{
  type_uintx	= new Type(TYPE_INT, (Expr*)NULL, false);   // unknown width
  type_uint0	= new Type(TYPE_INT,           0, false);   // for const 0
  type_uint1	= new Type(TYPE_INT,           1, false);   // 0,1
  type_uint8	= new Type(TYPE_INT,           8, false);
  type_uint16	= new Type(TYPE_INT,          16, false);
  type_uint32	= new Type(TYPE_INT,          32, false);
  type_sintx	= new Type(TYPE_INT, (Expr*)NULL, true);    // unknown width
  type_sint1	= new Type(TYPE_INT,           1, true);    // 0,-1
  type_sint8	= new Type(TYPE_INT,           8, true);
  type_sint16	= new Type(TYPE_INT,          16, true);
  type_sint32	= new Type(TYPE_INT,          32, true);

  Expr::init();	// must init pre-defined expr_0, expr_1

  type_none	= new Type(TYPE_NONE);			// for non-typed trees
  type_any	= new Type(TYPE_ANY);			// wildcard type
  type_bool	= new Type(TYPE_BOOL);			// boolean (t=1, f=0)

  TreeGC::addRoot(type_uintx);
  TreeGC::addRoot(type_uint0);
  TreeGC::addRoot(type_uint1);
  TreeGC::addRoot(type_uint8);
  TreeGC::addRoot(type_uint16);
  TreeGC::addRoot(type_uint32);
  TreeGC::addRoot(type_sintx);
  TreeGC::addRoot(type_sint1);
  TreeGC::addRoot(type_sint8);
  TreeGC::addRoot(type_sint16);
  TreeGC::addRoot(type_sint32);
  TreeGC::addRoot(type_none);
  TreeGC::addRoot(type_any);
  TreeGC::addRoot(type_bool);
}


////////////////////////////////////////////////////////////////
//  constructors

Type::Type (TypeKind typeKind_i, Expr *predExpr_i)	// TYPE_BOOL, ANY, NONE
							// (also TYPE_ARRAY)
  : Tree(NULL,TREE_TYPE),
    typeKind(typeKind_i),
    width(    (typeKind==TYPE_BOOL) ? 1 : 0),
    widthExpr(NULL),
    signd(false),
    predExpr(predExpr_i)
    // linked(true),
    // bindTime(predExpr ? predExpr->getBindTime() : BindTime(BIND_COMPILE))
{
  assert(   typeKind==TYPE_BOOL
	 || typeKind==TYPE_ANY
	 || typeKind==TYPE_NONE
	 || typeKind==TYPE_ARRAY);
  if (widthExpr)
    widthExpr->setParent(this);
  if (predExpr)
    predExpr->setParent(this);
}


Type::Type (TypeKind typeKind_i, int width_i,
	    bool signd_i, Expr *predExpr_i)	// TYPE_INT
							// (also TYPE_FIXED)
  : Tree(NULL,TREE_TYPE),
    typeKind(typeKind_i),
    width(width_i),
    widthExpr(NULL),
    signd(signd_i),
    predExpr(predExpr_i)
    // linked(true),
    // bindTime(predExpr ? predExpr->getBindTime() : BindTime(BIND_COMPILE))
{
  assert(typeKind==TYPE_INT || typeKind==TYPE_FIXED);
  assert(width>=0);			// width 0 ok for const value 0
  if (widthExpr)
    widthExpr->setParent(this);
  if (predExpr)
    predExpr->setParent(this);
}

Type::Type (TypeKind typeKind_i, Expr *widthExpr_i,
	    bool signd_i, Expr *predExpr_i)	// TYPE_INT
							// (also TYPE_FIXED)
  : Tree(NULL,TREE_TYPE),
    typeKind(typeKind_i),
    width(-1),
    widthExpr(widthExpr_i),
    signd(signd_i),
    predExpr(predExpr_i)
{
  assert(typeKind==TYPE_INT || typeKind==TYPE_FIXED);
  // NULL widthExpr ok for unknown widths

  // - recover constant width in widthExpr:
  if (widthExpr)
  {
    Type  *widthType=widthExpr->getType();
    if (   widthExpr->getExprKind()==EXPR_VALUE
	   && widthType
	   && widthType->getTypeKind()==TYPE_INT
	   && widthType->isSigned()   ==false
	   && widthType->getPredExpr()==NULL)
    {
      width = ((ExprValue*)widthExpr)->getIntVal();
      widthExpr=NULL;
    }
    else
      widthExpr->setParent(this);
  }
  if (predExpr)
    predExpr->setParent(this);

}


TypeFixed::TypeFixed (int intWidth_i, int fracWidth_i,
		      bool signd_i, Expr *predExpr_i)
  : Type(TYPE_FIXED,
	 constIntExpr(intWidth_i+fracWidth_i),
	 signd_i,
	 predExpr_i),
    intWidth(intWidth_i),
    fracWidth(fracWidth_i),
    intWidthExpr (NULL),
    fracWidthExpr(NULL)
{
  assert(intWidth_i>=0);
  assert(fracWidth_i>=0);
  // 0 width ok for const value 0.0
  if (intWidthExpr)
    intWidthExpr ->setParent(this);
  if (fracWidthExpr)
    fracWidthExpr->setParent(this);
  if (predExpr)
    predExpr->setParent(this);
}


TypeFixed::TypeFixed (Expr *intWidthExpr_i, Expr *fracWidthExpr_i,
		      bool signd_i,         Expr *predExpr_i)
  : Type(TYPE_FIXED,
	 new ExprBop(NULL,'+',intWidthExpr_i,fracWidthExpr_i),
	 signd_i,
	 predExpr_i),
    intWidth(-1),
    fracWidth(-1),
    intWidthExpr (intWidthExpr_i),
    fracWidthExpr(fracWidthExpr_i)
{
  // NULL intWidthExpr, fracWidthExpr ok for unknown widths
  // - should check for constant widths in intWidthExpr, fracWidthExpr
  if (intWidthExpr)	intWidthExpr ->setParent(this);
  if (fracWidthExpr)	fracWidthExpr->setParent(this);
  if (predExpr)		predExpr     ->setParent(this);
}


TypeArray::TypeArray (Type *elemType_i, int nelems_i,
		      Expr *predExpr_i)
  : Type(TYPE_ARRAY,predExpr_i),
    elemType(elemType_i),
    nelems(nelems_i),
    nelemsExpr(NULL)
{
  // - TypeArray has NULL widthExpr, real width in elemType
  assert(elemType->getTypeKind()!=TYPE_ARRAY);
  assert(nelems_i>=0);
  if (nelemsExpr)
    nelemsExpr->setParent(this);
  if (predExpr)
    predExpr->setParent(this);
}


TypeArray::TypeArray (Type *elemType_i, Expr *nelemsExpr_i,
		      Expr *predExpr_i)
  : Type(TYPE_ARRAY,predExpr_i),
    elemType(elemType_i),
    nelems(-1),
    nelemsExpr(nelemsExpr_i)
{
  // - TypeArray has NULL widthExpr, real width in elemType
  assert(elemType->getTypeKind()!=TYPE_ARRAY);
  if (nelemsExpr)
    nelemsExpr->setParent(this);
  if (predExpr)
    predExpr->setParent(this);
}

////////////////////////////////////////////////////////////////
//  destructor -- DEBUG
Type::~Type () { if ((this==type_any)||(this==type_none)) abort(); }

////////////////////////////////////////////////////////////////
//  thread

void Type::thread (Tree *p)
{
  setParent(p);
  if (widthExpr)	widthExpr->thread(this);
  if (predExpr)		predExpr->thread(this);
}


void TypeFixed::thread (Tree *p)
{
  setParent(p);
  if (intWidthExpr)	intWidthExpr->thread(this);
  if (fracWidthExpr)	fracWidthExpr->thread(this);
  if (predExpr)		predExpr->thread(this);
}


void TypeArray::thread (Tree *p)
{
  setParent(p);
  elemType->thread(this);
  if (nelemsExpr)	nelemsExpr->thread(this);
  if (predExpr)		predExpr->thread(this);
}


////////////////////////////////////////////////////////////////
//  link

bool Type::link ()
{
  bool linked=true;
  if (widthExpr)
    linked = linked && widthExpr->link();
  if (predExpr)
    linked = linked && predExpr->link();
  return linked;
}


bool TypeFixed::link ()
{
  bool linked=true;
  if (intWidthExpr)
    linked = linked && intWidthExpr->link();
  if (fracWidthExpr)
    linked = linked && fracWidthExpr->link();
  if (predExpr)
    linked = linked && predExpr->link();
  return linked;
}


bool TypeArray::link ()
{
  bool linked = elemType->link();
  if (nelemsExpr)
    linked = linked && nelemsExpr->link();
  return linked;
}


////////////////////////////////////////////////////////////////
//  typeCheck
//   - returns valid, complete type (not ANY, NONE, or unknown width) or NULL
//   - evaluates simplified predicates (boolean const)
//     but ignores predicate expressions

Type* Type::typeCheck ()
{
  if (   typeKind==TYPE_NONE
      || typeKind==TYPE_ANY )
    return NULL;
  if (widthExpr)
    widthExpr->typeCheck();
  if (predExpr)
  {
    predExpr->typeCheck();
    if (predExpr->getExprKind()==EXPR_VALUE)		// simplified predicate
    {
      if (predExpr->getType()->getTypeKind()==TYPE_BOOL)
      {
	if (((ExprValue*)predExpr)->getIntVal()==0)	// - false predicate
	  return type_none;
	else						// - true predicate
	  predExpr=NULL;
      }
      else
	assert(!"internal inconsistency");
    }
  }
  return this;
  // warning: should check that width expr contains only param symbols & consts
}


Type* TypeFixed::typeCheck ()
{
  Type::typeCheck();
  if (intWidthExpr)
    intWidthExpr->typeCheck();
  if (fracWidthExpr)
    fracWidthExpr->typeCheck();
  return this;
  // warning: should check that width exprs contain only param symbols & consts
}


Type* TypeArray::typeCheck()
{
  Type::typeCheck();
  elemType->typeCheck();
  if (nelemsExpr)
    nelemsExpr->typeCheck();
  return this;
  // warning: should check that nelems expr contains only param symbols &consts
}


////////////////////////////////////////////////////////////////
//  upgradable
//   - returns true iff type (this) is upgradable to type (t)
//   - also assumes true for like-types of non-static widths
//   - ignores predicate expressions
//   - "a is upgradable to b" == (a>b) as per type lattice,
//     except TYPE_NONE cannot upgrade (!(a>TYPE_NONE) for all a)

bool Type::upgradable (const Type *t) const
{
  if (typeKind==TYPE_ANY || t->typeKind==TYPE_ANY)
    return true;
  if (typeKind==TYPE_NONE && t->typeKind==TYPE_NONE)
    return true;
  if (typeKind!=t->typeKind)
    return false;
  // now know typeKind==t->typeKind, and neither is TYPE_ANY or TYPE_NONE

  if (typeKind==TYPE_BOOL)
    return true;
  else if (typeKind==TYPE_INT)
  {
    if (signd && !t->signd)			    // no signed-->unsigned
      return false;
    else if (width>=0 && t->width>=0)		    // compare known widths
      return (width<=t->width);
    else if (   (   width<0 &&    widthExpr==NULL)
	     || (t->width<0 && t->widthExpr==NULL)) // false if no width
      return false;
      /*
      // - NO, assume upgradable (warning: requires predicates)
      return true;
      */
    else					    // true for dynamic widths
      return true;
  }
  else if (typeKind==TYPE_FIXED)
  {
    TypeFixed *thisF=(TypeFixed*)this, *tF=(TypeFixed*)t;
    if (signd && !t->signd)			    // no signed-->unsigned
      return false;
    else if ( thisF->getIntWidth() >=0 &&
	      tF   ->getIntWidth() >=0 &&
	      thisF->getFracWidth()>=0 &&
	      tF   ->getFracWidth()>=0    )	    // compare known widths
      return ( thisF->getIntWidth()  <= tF->getIntWidth()  &&
	       thisF->getFracWidth() <= tF->getFracWidth()    );
    else if (   (thisF->getIntWidth() <0 && thisF->getIntWidthExpr ()==NULL)
	     || (   tF->getIntWidth() <0 &&    tF->getIntWidthExpr ()==NULL)
	     || (thisF->getFracWidth()<0 && thisF->getFracWidthExpr()==NULL)
	     || (   tF->getFracWidth()<0 &&    tF->getFracWidthExpr()==NULL))
						    // false if no width
      return false;
    else
      return true;				    // true for dynamic widths
  }
  else if (typeKind==TYPE_ARRAY)
  {
    TypeArray *thisA=(TypeArray*)this,
	      *tA   =(TypeArray*)t;
    if (!thisA->getElemType()->upgradable(tA->getElemType()))
      return false;
    if (thisA->getNelems()>=0 && tA->getNelems()>=0) // true for equal sizes
      return (thisA->getNelems()==tA->getNelems());
    else if (   (thisA->getNelems()<0 && thisA->getNelemsExpr()==NULL)
	     || (   tA->getNelems()<0 &&    tA->getNelemsExpr()==NULL))
						    // false if no size
      return false;
    else					    // assume true 4 dynmc size
      return true;
  }
  else
  {
    assert(!"internal inconsistency");
    return false;	// dummy
  }
}


////////////////////////////////////////////////////////////////
//  upgrade
//   - returns upgraded type iff type (this) is upgradable to type (t),
//     else returns NULL
//   - creates predicate expr for dynamic widths
//   - does not propagate predicates from original types
//   - same logic as upgradable() but actually creates+returns upgraded type

Type* Type::upgrade (const Type *t) const
{
  if (typeKind==TYPE_ANY || t->typeKind==TYPE_ANY)
    return type_any;
  if (typeKind==TYPE_NONE && t->typeKind==TYPE_NONE)
    return type_none;
  if (typeKind!=t->typeKind)
    return NULL;
  // now know typeKind==t->typeKind, and neither is TYPE_ANY or TYPE_NONE

  /*
  Expr *newPredExpr = (predExpr && t->predExpr) ?
			    new ExprBop(NULL,LOGIC_AND,
					(Expr*)predExpr->duplicate(),
					(Expr*)t->predExpr->duplicate()) :
		      (predExpr)    ? (Expr*)   predExpr->duplicate() :
		      (t->predExpr) ? (Expr*)t->predExpr->duplicate() : NULL;
  */
  Expr *newPredExpr = NULL;

  if (typeKind==TYPE_BOOL)
  {
    Type *ret=(new Type(TYPE_BOOL,newPredExpr)) -> typeCheck();
    return ret;
  }
  else if (typeKind==TYPE_INT)
  {
    if (signd && !t->signd)			    // no signed-->unsigned
      return NULL;
    else if (width>=0 && t->width>=0)		    // compare known widths
    {
      if ((signd==t->signd && width<=t->width) || (width<t->width))
      {
	Type *ret=(new Type(TYPE_INT,t->width,t->signd,newPredExpr))
							-> typeCheck();
	return ret;
      }
      else
	return NULL;
    }
    else if (   (   width<0 &&    widthExpr==NULL)
	     || (t->width<0 && t->widthExpr==NULL)) // false if no width
    {
      return NULL;
      /*
      // - NO, assume upgradable (warning: requires predicates)
      Type *ret=(widthExpr==NULL)
		  ? new Type(TYPE_INT,width,signd,newPredExpr)
		  : new Type(TYPE_INT,t->width,t->signd,newPredExpr);
      ret->typeCheck();
      return ret;
      */
    }
    else					    // pred for dynamic widths
    {
      Expr *widthPred	= new ExprBop(NULL,(signd==t->signd)?LTE:'<',
				      makeWidthExpr(),t->makeWidthExpr());
			// (must widen by 1 bit for unsigned-->signed)
      Type *ret=(new Type(TYPE_INT, t->makeWidthExpr(), t->signd,
			  newPredExpr
			    ? new ExprBop(NULL,LOGIC_AND,newPredExpr,widthPred)
			    : widthPred)) -> typeCheck();
      return ret;
    }
  }
  else if (typeKind==TYPE_FIXED)
  {
    TypeFixed *thisF=(TypeFixed*)this, *tF=(TypeFixed*)t;
    if (signd && !t->signd)			    // no signed-->unsigned
      return NULL;
    else if ( thisF->getIntWidth() >=0 &&
	      tF   ->getIntWidth() >=0 &&
	      thisF->getFracWidth()>=0 &&
	      tF   ->getFracWidth()>=0    )	    // compare known widths
    {
      if ( thisF->getIntWidth()  <= tF->getIntWidth()  &&
	   thisF->getFracWidth() <= tF->getFracWidth()    )
      {
	Type *ret=(new TypeFixed(tF->getIntWidth(),
				 tF->getFracWidth(),
				 t->signd,newPredExpr)) -> typeCheck();
	return ret;
      }
      else
	return NULL;
    }
    else if (   (thisF->getIntWidth() <0 && thisF->getIntWidthExpr ()==NULL)
	     || (   tF->getIntWidth() <0 &&    tF->getIntWidthExpr ()==NULL)
	     || (thisF->getFracWidth()<0 && thisF->getFracWidthExpr()==NULL)
	     || (   tF->getFracWidth()<0 &&    tF->getFracWidthExpr()==NULL))
						    // false if no width
      return NULL;
    else					    // pred for dynamic widths
    {
      Expr *tIntWider	= new ExprBop(NULL, (signd==t->signd)?LTE:'<',
				      thisF->makeIntWidthExpr(),
				         tF->makeIntWidthExpr());
      Expr *tFracWider	= new ExprBop(NULL,LTE,
				      thisF->makeFracWidthExpr(),
				         tF->makeFracWidthExpr());
      Expr *tWider	= new ExprBop(NULL,LOGIC_AND,
				      tIntWider,tFracWider);
      Expr *widthPred	= tWider;
      Type *ret=(new TypeFixed(tF->makeIntWidthExpr(),
			       tF->makeFracWidthExpr(), t->signd,
			       newPredExpr
			         ? new ExprBop(NULL,LOGIC_AND,
					       newPredExpr,widthPred)
			         : widthPred)) -> typeCheck();
      return ret;
    }
  }
  else if (typeKind==TYPE_ARRAY)
  {
    TypeArray *thisA =(TypeArray*)this,
	      *tA    =(TypeArray*)t;
    Type *elemUpgrade=thisA->getElemType()->upgrade(tA->getElemType());
    if (elemUpgrade==NULL)
      return NULL;
    if (thisA->getNelems()>=0 && tA->getNelems()>=0)	// compare known sizes
    {
      if (thisA->getNelems()==tA->getNelems())
      {
	Type *ret=(new TypeArray(elemUpgrade,thisA->getNelems(),
				 newPredExpr)) -> typeCheck();
	return ret;
      }
      else
	return NULL;
    }
    else if (   (thisA->getNelems()<0 && thisA->getNelemsExpr()==NULL)
	     || (   tA->getNelems()<0 &&    tA->getNelemsExpr()==NULL))
							// false if no size
      return NULL;
    else						// pred for dynamc size
    {
      Expr *sizeEq=new ExprBop(NULL,EQUALS,
			       thisA->makeNelemsExpr(),tA->makeNelemsExpr());
      Type *ret   =(new TypeArray(elemUpgrade,thisA->makeNelemsExpr(),
				  newPredExpr ? new ExprBop(NULL,LOGIC_AND,
							    newPredExpr,sizeEq)
					      : sizeEq)) -> typeCheck();
      return ret;
    }
  }
  else
  {
    assert(!"internal inconsistency");
    return NULL;	// dummy;
  }
}


////////////////////////////////////////////////////////////////
//  merge
//   - returns merged type iff type (this) is upgradable to type (t)
//     or vice versa, else returns NULL
//   - creates predicate expr for dynamic widths
//   - does not propagate predicates from original types
//   - logically equivalent to:
//       ( this->upgradable(t)    ? this->upgrade(t)    :
//            t->upgradable(this) ?    t->upgrade(this) : NULL )

Type* Type::merge (const Type *t) const
{
  if (typeKind==TYPE_ANY || t->typeKind==TYPE_ANY)
    return type_any;
  if (typeKind==TYPE_NONE && t->typeKind==TYPE_NONE)
    return type_none;
  if (typeKind!=t->typeKind)
    return NULL;
  // now know typeKind==t->typeKind, and neither is TYPE_ANY or TYPE_NONE

  /*
  Expr *newPredExpr = (predExpr && t->predExpr) ?
			    new ExprBop(NULL,LOGIC_AND,
					(Expr*)predExpr->duplicate(),
					(Expr*)t->predExpr->duplicate()) :
		      (predExpr)    ? (Expr*)   predExpr->duplicate() :
		      (t->predExpr) ? (Expr*)t->predExpr->duplicate() : NULL;
  */
  Expr *newPredExpr = NULL;

  if (typeKind==TYPE_BOOL)
  {
    Type *ret=new Type(TYPE_BOOL,newPredExpr);
    return ret;
  }
  else if (typeKind==TYPE_INT)
  {
    bool newSignd = signd || t->signd;
    if (width>=0 && t->width>=0)		    // compare known widths
    {
      int newThisWidth =    width + ((newSignd==   signd)?0:1);
      int newTWidth    = t->width + ((newSignd==t->signd)?0:1);
      if (newThisWidth>newTWidth)
      {
	Type *ret=(new Type(TYPE_INT,newThisWidth,
			    newSignd,newPredExpr)) -> typeCheck();
	return ret;
      }
      else
      {
	Type *ret=(new Type(TYPE_INT,newTWidth,
			    newSignd,newPredExpr)) -> typeCheck();
	return ret;
      }
    }
    else if (   (   width<0 &&    widthExpr==NULL)
	     || (t->width<0 && t->widthExpr==NULL))  // false if no width
    {
      return NULL;
      /*
      // - NO, assume upgradable (requires predicates)
      Type *ret=(widthExpr==NULL)
		  ? new Type(TYPE_INT,width,signd,newPredExpr)
		  : new Type(TYPE_INT,t->width,t->signd,newPredExpr);
      return ret;
      */
    }
    else					    // pred for dynamic widths
    {
      Expr *newThisWidthE = (newSignd==signd)
			      ? makeWidthExpr()
			      : new ExprBop(NULL,'+',makeWidthExpr(),
					    (Expr*)expr_1->duplicate());
      Expr *newTWidthE	  = (newSignd==t->signd)
			      ? t->makeWidthExpr()
			      : new ExprBop(NULL,'+',t->makeWidthExpr(),
					    (Expr*)expr_1->duplicate());
      Expr *newThisWider  = new ExprBop(NULL,'>',newThisWidthE,newTWidthE);
      Expr *newWidthExpr  = new ExprCond(NULL,newThisWider,
					 (Expr*)newThisWidthE->duplicate(),
					 (Expr*)newTWidthE->duplicate());
      Type *ret=(new Type(TYPE_INT,newWidthExpr,newSignd)) -> typeCheck();
      return ret;
    }
  }
  else if (typeKind==TYPE_FIXED)
  {
    TypeFixed *thisF=(TypeFixed*)this, *tF=(TypeFixed*)t;
    bool newSignd = signd || t->signd;
    if ( thisF->getIntWidth() >=0 &&
	 tF   ->getIntWidth() >=0 &&
	 thisF->getFracWidth()>=0 &&
	 tF   ->getFracWidth()>=0    )		    // compare known widths
    {
      int newThisIntWidth = thisF->getIntWidth() + ((newSignd==   signd)?0:1);
      int newTIntWidth    =    tF->getIntWidth() + ((newSignd==t->signd)?0:1);
      int newIntWidth	  = max(newThisIntWidth,newTIntWidth);
      int newFracWidth	  = max(thisF->getFracWidth(),tF->getFracWidth());
      Type *ret=(new TypeFixed(newIntWidth,newFracWidth,
			       newSignd,newPredExpr)) -> typeCheck();
      return ret;
    }
    else if (   (thisF->getIntWidth() <0 && thisF->getIntWidthExpr ()==NULL)
	     || (   tF->getIntWidth() <0 &&    tF->getIntWidthExpr ()==NULL)
	     || (thisF->getFracWidth()<0 && thisF->getFracWidthExpr()==NULL)
	     || (   tF->getFracWidth()<0 &&    tF->getFracWidthExpr()==NULL))
						    // false if no width
      return NULL;
    else					    // pred for dynamic widths
    {
      return NULL;
      // warning: fixed types not supported! must check type widening from sign

      Expr *newThisIntWidthE  = (newSignd==signd)
				  ? thisF->makeIntWidthExpr()
				  : new ExprBop(NULL,'+',
						thisF->makeIntWidthExpr(),
						(Expr*)expr_1->duplicate());  
      Expr *newTIntWidthE     = (newSignd==signd)
				  ? tF->makeIntWidthExpr()
				  : new ExprBop(NULL,'+',
						tF->makeIntWidthExpr(),
						(Expr*)expr_1->duplicate());  
      Expr *newIntWidthE      = newMaxExpr(newThisIntWidthE,newTIntWidthE);
      Expr *newFracWidthE     = newMaxExpr(thisF->makeFracWidthExpr(),
					      tF->makeFracWidthExpr());
      Type *ret=(new TypeFixed(newIntWidthE,newFracWidthE,
			       newSignd,newPredExpr)) -> typeCheck();
      return ret;
    }
  }
  else if (typeKind==TYPE_ARRAY)
  {
    TypeArray *thisA=(TypeArray*)this,
	      *tA   =(TypeArray*)t;
    Type      *elemMerge=thisA->getElemType()->merge(tA->getElemType());
    if (elemMerge==NULL)
      return NULL;
    if (thisA->getNelems()>=0 && tA->getNelems()>=0)	// compare known sizes
    {
      if (thisA->getNelems()==tA->getNelems())
      {
	Type *ret=new TypeArray(elemMerge,thisA->getNelems(),newPredExpr);
	return ret;
      }
      else
	return NULL;
    }
    else if (   (thisA->getNelems()<0 && thisA->getNelemsExpr()==NULL)
	     || (   tA->getNelems()<0 &&    tA->getNelemsExpr()==NULL))
							// false if no size
      return NULL;
    else						// pred for dynamc size
    {
      Expr *sizeEq=new ExprBop(NULL,EQUALS,
			       thisA->makeNelemsExpr(),tA->makeNelemsExpr());
      Type *ret   =(new TypeArray(elemMerge,
				  thisA->makeNelemsExpr(),
				  newPredExpr ? new ExprBop(NULL,LOGIC_AND,
							    newPredExpr,sizeEq)
					      : sizeEq)) -> typeCheck();
      return ret;
    }
  }
  else
  {
    assert(!"internal inconsistency");
    return NULL;	// dummy
  }
}


////////////////////////////////////////////////////////////////
//  upgradeSign()
//   - return sign-upgraded version of this, NULL if not upgradable
//   - does not propagate predicate from original type

Type* Type::upgradeSign (bool sign) const
{
  if (typeKind==TYPE_ANY)			// wildcard type is invariant
    return (Type*)duplicate();
  if (typeKind==TYPE_BOOL || typeKind==TYPE_NONE) // reject incompatible types
    return NULL;
  else if (signd==sign)				// now know typeKind==TYPE_INT
    return (Type*)duplicate();
  else if (signd==true && sign==false)		// disallow signed->unsigned
    return NULL;
  else // (signd==false && sign==true)		// widen for unsigned->signed
  {
    if (width>=0)				// - warning: signed 0 is 1-bit
      return new Type(TYPE_INT,width+1,sign);
    else
      return new Type(TYPE_INT,
		      new ExprBop(NULL,'+',makeWidthExpr(),
				  (Expr*)expr_1->duplicate()), sign);
  }
}


Type* TypeFixed::upgradeSign (bool sign) const
{
  if (signd==sign)
    return (Type*)duplicate();
  else if (signd==true && sign==false)		// disallow signed->unsigned
    return NULL;
  else // (signd==false && sign==true)		// widen for unsigned->signed
  {
    if (intWidth>=0 && fracWidth>=0)		// - warning: signed 0 is 1-bit
    {
      Type *ret=(new TypeFixed(intWidth+1,fracWidth,sign)) -> typeCheck();
      return ret;
    }
    else
    {
      Type *ret=(new TypeFixed(new ExprBop(NULL,'+',makeIntWidthExpr(),
					   (Expr*)expr_1->duplicate()),
			       makeFracWidthExpr(), sign)) -> typeCheck();
      return ret;
    }
  }
}


Type* TypeArray::upgradeSign (bool sign) const
{
  return NULL;		// not allowed
}


////////////////////////////////////////////////////////////////
//  equals

bool Type::equals (const Type *t) const
{
  if (getTypeKind() != t->getTypeKind())
    return false;
  Type *tt=(Type*)t;
  return (   (signd==tt->signd)
	  && ((width==tt->width && width>=0)
	      || safeEquals(widthExpr,tt->widthExpr))
	  && (safeEquals(predExpr,tt->predExpr)));
}


bool TypeFixed::equals (const Type *t) const
{
  if (getTypeKind() != t->getTypeKind())
    return false;
  TypeFixed *tt=(TypeFixed*)t;
  return (   (signd==tt->signd)
	  && ((intWidth==tt->intWidth && intWidth>=0)
	      || safeEquals(intWidthExpr,tt->intWidthExpr))
	  && ((fracWidth==tt->fracWidth && fracWidth>=0)
	      || safeEquals(fracWidthExpr,tt->fracWidthExpr))
	  && (safeEquals(predExpr,tt->predExpr)));
}


bool TypeArray::equals (const Type *t) const
{
  if (getTypeKind() != t->getTypeKind())
    return false;
  TypeArray *tt=(TypeArray*)t;
  return (   elemType->equals(tt->elemType)
	  && ((nelems==tt->nelems && nelems>=0)
	      || safeEquals(nelemsExpr,tt->nelemsExpr))
	  && (safeEquals(predExpr,tt->predExpr)));
}


////////////////////////////////////////////////////////////////
//  getBindTime  (stubs for now)

BindTime Type::getBindTime () const	    { return BindTime(BIND_UNKNOWN); }
BindTime TypeFixed::getBindTime () const    { return BindTime(BIND_UNKNOWN); }
BindTime TypeArray::getBindTime () const    { return BindTime(BIND_UNKNOWN); }


////////////////////////////////////////////////////////////////
//  makeWidthExpr
//   - creates copy of width expressions, or NULL if no width

Expr* Type::makeWidthExpr () const
{
  return width>=0  ? constIntExpr(width) :
	 widthExpr ? (Expr*)widthExpr->duplicate() : NULL;
}


Expr* TypeFixed::makeIntWidthExpr () const
{
  return intWidth>=0  ? constIntExpr(intWidth) :
	 intWidthExpr ? (Expr*)intWidthExpr->duplicate() : NULL;
}


Expr* TypeFixed::makeFracWidthExpr () const
{
  return fracWidth>=0  ? constIntExpr(fracWidth) :
	 fracWidthExpr ? (Expr*)fracWidthExpr->duplicate() : NULL;
}


Expr* TypeArray::makeNelemsExpr () const
{
  return nelems>=0  ? constIntExpr(nelems) :
	 nelemsExpr ? (Expr*)nelemsExpr->duplicate() : NULL;
}


////////////////////////////////////////////////////////////////
//  duplicate()

void Type::duplicateHelper ()
{
  if (widthExpr)
  {
    widthExpr = (Expr*)widthExpr->duplicate();
    widthExpr ->setParent(this);
  }
  if (predExpr)
  {
    predExpr  = (Expr*)predExpr ->duplicate();
    predExpr  ->setParent(this);
  }
}


Tree* Type::duplicate () const
{
  Type *t=new Type(*this);
  t->setParent(NULL);
  t->duplicateHelper();
  return t;
}


Tree* TypeFixed::duplicate () const
{
  TypeFixed *t=new TypeFixed(*this);
  t->setParent(NULL);
  t->duplicateHelper();
  if (intWidthExpr)
  {
    t->intWidthExpr = (Expr*)t->intWidthExpr->duplicate();
    t->intWidthExpr->setParent(t);
  }
  if (fracWidthExpr)
  {
    t->fracWidthExpr = (Expr*)t->fracWidthExpr->duplicate();
    t->fracWidthExpr->setParent(t);
  }
  return t;
}


Tree* TypeArray::duplicate () const
{
  TypeArray *t=new TypeArray(*this);
  t->setParent(NULL);
  t->duplicateHelper();
  t->elemType = (Type*)t->elemType->duplicate();
  t->elemType->setParent(t);
  if (nelemsExpr)
  {
    t->nelemsExpr = (Expr*)t->nelemsExpr->duplicate();
    t->nelemsExpr->setParent(t);
  }
  return t;
}


////////////////////////////////////////////////////////////////
//  toString

string Type::toString () const
{
  switch (typeKind)
  {
    case TYPE_NONE:
      return "<no type>";

    case TYPE_ANY:
      return "<any type>";

    case TYPE_BOOL:
      return "boolean";

    case TYPE_INT:
    {
      string signStr(signd?"signed[":"unsigned[");
      string widthStr = (width>=0)  ? string("%d",width)    :
			(widthExpr) ? widthExpr->toString() : string("$");
      string predStr  = (predExpr)  ? (string(" /*")+predExpr->toString()+"*/")
							    : string();
      return signStr + widthStr + "]" + predStr;
    }

    default:
      assert(!"internal inconsistency");
      return string();
  }
}


string TypeFixed::toString () const
{
  string signStr(signd?"signed[":"unsigned[");

  string intWidthStr  = (intWidth>=0)   ? string("%d",intWidth)     :
		        (intWidthExpr)  ? intWidthExpr->toString()  :
					  string("$");
  string fracWidthStr = (fracWidth>=0)  ? string("%d",fracWidth)    :
		        (fracWidthExpr) ? fracWidthExpr->toString() :
					  string("$");
  string predStr      = (predExpr)      ? (string("(")+predExpr->toString()+
							       ")") : string();
  return signStr + intWidthStr + "." + fracWidthStr + "]" + predStr;
}


string TypeArray::toString () const
{
  string elemTypeStr = elemType->toString();
  string nelemsStr   = string("[") + 
		       ((nelems>=0)  ? string("%ld",(long)nelems) :
		        (nelemsExpr) ? nelemsExpr->toString()     :
				       string("$")		   ) +
		       string("]");
  int    bracketPos  = elemTypeStr.pos("[");
  return (bracketPos>=0) ? (elemTypeStr.insert(nelemsStr,bracketPos))
			 : (elemTypeStr + nelemsStr);
}


////////////////////////////////////////////////////////////////
//  map


void Type::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (widthExpr)
      widthExpr->map(pre,post,i);
    if (predExpr)
      predExpr->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void Type::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    if (widthExpr)
      widthExpr->map2((Tree**)&widthExpr,pre,post,i);
    if (predExpr)
      predExpr ->map2((Tree**)&predExpr, pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void TypeFixed::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    if (widthExpr)
      widthExpr->map(pre,post,i);
    // - the following are redundant, since width=intWidth+fracWidth:
    // if (intWidthExpr)
    //   intWidthExpr->map(pre,post,i);
    // if (fracWidthExpr)
    //   fracWidthExpr->map(pre,post,i);
    if (predExpr)
      predExpr->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void TypeFixed::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    if (widthExpr)
      widthExpr->map2((Tree**)&widthExpr,pre,post,i);
    if (predExpr)
      predExpr ->map2((Tree**)&predExpr, pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


void TypeArray::map (TreeMap pre, TreeMap post, void *i)
{
  bool descend=true;
  if (pre)
    descend=pre(this,i);
  if (descend)
  {
    elemType->map(pre,post,i);
    if (nelemsExpr)
      nelemsExpr->map(pre,post,i);
    if (predExpr)
      predExpr->map(pre,post,i);
  }
  if (post)
    post(this,i);
}


void TypeArray::map2 (Tree **h, TreeMap2 pre, TreeMap2 post,
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
    elemType->map2((Tree**)&elemType,pre,post,i);
    if (nelemsExpr)
      nelemsExpr->map2((Tree**)&nelemsExpr,pre,post,i);
    if (predExpr)
      predExpr  ->map2((Tree**)&predExpr,  pre,post,i);
  }
  if (post)
  {
    post(h,i);
    if (this!=*h)
      (*h)->setParent(p);
  }
}


static
bool typeDeleteMap (Tree *t, void *i)
{
  t->deleteType();
  return(true);
}

void rmType(Tree *t)
{
  t->map(typeDeleteMap,(TreeMap)NULL,(void *)NULL);
}

string typekindToString(TypeKind t) {
	switch(t) {
		case TYPE_BOOL: return string("boolean");
		case TYPE_INT: return string("integer");
		case TYPE_FIXED: return string("fixed");
		case TYPE_ARRAY: return string("array");
		case TYPE_ANY: return string("any");
		case TYPE_FLOAT: return string("float");
		case TYPE_DOUBLE: return string("double");
		case TYPE_NONE: return string("none");
		default: return string("bs");
	}
}
