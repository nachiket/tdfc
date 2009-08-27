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

#ifndef _TDF_SUITE_
#define _TDF_SUITE_

#include "compares.h"
#include <LEDA/core/string.h>
#include <LEDA/core/set.h>
#include <assert.h>
#include "tree.h"
#include "symbol.h"

using leda::set;

class Operator;


class Suite : public Tree
{
private:
  SymTab		*symtab;	// operator symbols
  set<Operator*>	*operators;

public:
  Suite (set<Operator*> *operators_i=NULL);
  Suite (SymTab *symtab_i);
  virtual ~Suite ()					{ delete operators; }

  SymTab*		getSymtab	() const	{ return symtab; }
  set<Operator*>*	getOperators	() const	{ return operators; }
  Operator*		lookupOperator	(const string &name) const;
  void			addOperator	(Operator *op);
  void			removeOperator	(Operator *op);

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual SymTab*	getScope	() const	{ return symtab;   }
  virtual void		setParent	(Tree *p)	{ assert(p==NULL); }
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual Type*		getType		() const	{ return type_none; }
  virtual void		deleteType	()		{}
  virtual void		clearType	()		{}
  virtual BindTime	getBindTime	() const	{ return symtab->
							      getBindTime(); }
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};



#endif	// #ifndef _TDF_SUITE_
