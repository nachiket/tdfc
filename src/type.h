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
// $Revision: 1.80 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_TYPE_
#define _TDF_TYPE_

#include <LEDA/core/string.h>
#include "tree.h"
#include "bindTime.h"
class Expr;


enum TypeKind { TYPE_BOOL,		// boolean
		TYPE_INT,		// integer
		TYPE_FIXED,		// fixed point
		TYPE_ARRAY,		// array (memory segment, LUT, etc.)
		TYPE_ANY,		// wildcard
		// floating-point types added by Nachiket
		TYPE_FLOAT,		// single-precision float		
		TYPE_DOUBLE,		// double-precision float
		TYPE_NONE };		// for non-typed tree nodes

class Type : public Tree
{
protected:
  TypeKind	typeKind;
  int		width;			// -1 if not static const
  Expr		*widthExpr;		// NULL if static const or unknown
  bool		signd;			// true:signed, false:unsigned
  Expr		*predExpr;		// predicate for type validity

  void		duplicateHelper ();

public:
  Type (TypeKind typeKind_i, Expr *predExpr_i=NULL);	// TYPE_BOOL, ANY, NONE
  Type (TypeKind typeKind_i, int   width_i,
	bool signd_i, Expr *predExpr_i=NULL);		// TYPE_INT
  Type (TypeKind typeKind_i, Expr *widthExpr_i,
	bool signd_i, Expr *predExpr_i=NULL);		// TYPE_INT
  // virtual ~Type () { }
  // DEBUG
  virtual ~Type();
  static void	init		();		// initialize pre-defined types

  TypeKind	getTypeKind	() const	{ return typeKind;  }
  int		getWidth	() const	{ return width;     }
  Expr*		getWidthExpr	() const	{ return widthExpr; }
  Expr*		makeWidthExpr	() const;
  Expr*		getPredExpr	() const	{ return predExpr;  }
  bool		isSigned	() const	{ return signd; }

  virtual bool	equals		(const Type *t) const;	// same in widths
  virtual bool	upgradable	(const Type *t) const;	// this upgradbl to t
  virtual Type*	upgrade		(const Type *t) const;	// new upgraded /NULL
  virtual Type*	merge		(const Type *t) const;	// new merged   /NULL
  virtual Type*	upgradeSign	(bool sign)     const;	// new upgraded /NULL
//virtual bool	compat		(Type *t);		// compatible types
//virtual Type*	simplify	();

  virtual size_t	getSizeof	() const { return sizeof(*this); }
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual Type*		getType		() const { return (Type*)this; }
  virtual void		deleteType	()	 {}
  virtual void		clearType	()	 {}
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class TypeFixed : public Type
{
private:
  int   intWidth,      fracWidth;	// -1 if not static const
  Expr *intWidthExpr, *fracWidthExpr;	// NULL if static const or unknown

public:
  TypeFixed (int intWidth_i, int fracWidth_i,
	     bool signd_i,   Expr *predExpr_i=NULL);
  TypeFixed (Expr *intWidthExpr_i, Expr *fracWidthExpr_i,
	     bool signd_i,         Expr *predExpr_i=NULL);
  virtual ~TypeFixed () {}

  int		getIntWidth		() const { return intWidth; }
  int		getFracWidth		() const { return fracWidth; }
  Expr*		getIntWidthExpr		() const { return intWidthExpr; }
  Expr*		getFracWidthExpr	() const { return fracWidthExpr; }
  Expr*		makeIntWidthExpr	() const;
  Expr*		makeFracWidthExpr	() const;

  virtual bool	equals		(const Type *t) const;	// same in widths
  virtual Type*	upgradeSign	(bool sign)     const;	// new upgraded /NULL
//virtual bool	compat		(Type *t);		// compatible types
//virtual Type*	simplify	();

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


class TypeArray : public Type
{
private:
  Type		*elemType;		// type of array elements
  long long	 nelems;		// -1 if not static const
  Expr		*nelemsExpr;		// NULL if static const or unknown

public:
  TypeArray (Type *elemType_i, int   nelems_i,     Expr *predExpr_i=NULL);
  TypeArray (Type *elemType_i, Expr *nelemsExpr_i, Expr *predExpr_i=NULL);
  virtual ~TypeArray () {}

  long long	getNelems	() const { return nelems; }
  Expr*		getNelemsExpr	() const { return nelemsExpr; }
  Expr*		makeNelemsExpr	() const;
  Type*		getElemType	() const { return elemType; }

  virtual bool	equals		(const Type *t) const;	// same in widths
  virtual Type*	upgradeSign	(bool sign)     const;	// new upgraded /NULL
//virtual bool	compat		(Type *t);		// compatible types
//virtual Type*	simplify	();

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


inline
Type* typeMerge (Type *a, Type *b)	{ return (a&&b) ? a->merge(b) : NULL; }


////////////////////////////////////////////////////////////////
//  predefined types in type.cc
//   - use duplicates of these  (except type_none, type_any, which are shared)

extern /*const*/ Type	*type_none;	// for non-typed trees
extern /*const*/ Type	*type_any;	// wildcard type
extern /*const*/ Type	*type_bool;	// boolean (t=1; f=0)
extern /*const*/ Type	*type_uintx;	// unknown width
extern /*const*/ Type	*type_uint0;	// for const 0
extern /*const*/ Type	*type_uint1;	// 0;1
extern /*const*/ Type	*type_uint8;
extern /*const*/ Type	*type_uint16;
extern /*const*/ Type	*type_uint32;
extern /*const*/ Type	*type_uint64;
extern /*const*/ Type	*type_sintx;	// unknown width
extern /*const*/ Type	*type_sint1;	// 0,-1
extern /*const*/ Type	*type_sint8;
extern /*const*/ Type	*type_sint16;
extern /*const*/ Type	*type_sint32;
extern /*const*/ Type	*type_sint64;


void rmType(Tree *t);

extern string typekindToString(TypeKind t);
extern string typekindToCplusplus(TypeKind t);

#endif  // #ifndef _TDF_TYPE_
