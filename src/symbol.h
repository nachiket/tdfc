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
// $Revision: 1.120 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_SYMBOL_
#define _TDF_SYMBOL_

#include "compares.h"
#include "tree.h"
#include "type.h"
#include "bindTime.h"
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/set.h>
#include <LEDA/core/dictionary.h>
using leda::dictionary;
using leda::list;
//using leda::set;

using std::cout;
using std::endl;

class Operator;
class Expr;
class BindTime;
class Symbol;

enum SymTabKind { SYMTAB_SUITE, SYMTAB_OP, SYMTAB_BLOCK };

class SymTab : public Tree
{
private:
  SymTabKind			symtabKind;
  dictionary<string,Symbol*>	*symbols;
  list<Symbol*>                 *symbolOrder;
  leda::set<SymTab*>			*children;
  SymTab			*scope;

public:
  SymTab (SymTabKind symtabKind_i,
	  dictionary<string,Symbol*> *symbols_i=NULL,
	  list<Symbol*> *symbolOrder_i=NULL,
	  leda::set<SymTab*> *children_i=NULL);
  ~SymTab ()						 { delete symbols;
							   delete children; }

  leda::set<SymTab*>*			getChildren	() const { return children; }
  SymTabKind			getSymtabKind	() const { return symtabKind; }
  dictionary<string,Symbol*>*	getSymbols	() const { return symbols; }
  list<Symbol*>*                getSymbolOrder  () const { return symbolOrder;}
  void				addChild	(SymTab *child_i);
  void				removeChild	(SymTab *child_i);
  void				addSymbol	(Symbol *sym_i);
  void				removeSymbol	(Symbol *sym_i);
  Symbol*			lookup		(const string &name_i) const;
  void				foreach_sym	(void (*f)(Symbol *s, void *i),
						 void *i, bool preorder=true,
						 bool descend=true);

  virtual void		setScope	(SymTab *s)	{ scope=s; };

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual Type*		getType		() const	{ return type_none; }
  virtual void		deleteType	()		{}
  virtual void		clearType	()		{}
  virtual BindTime	getBindTime	() const;
  virtual SymTab*	getScope	() const	{ return scope; }
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


enum SymbolKind { SYMBOL_STREAM, SYMBOL_STREAM_VALID, SYMBOL_VAR, SYMBOL_OP };

class Symbol : public Tree
{
protected:
  SymbolKind	symKind;
  string	name;
  Type		*type;	    // element type for streams, return type for ops

public:
  Symbol (Token *token_i, SymbolKind kind_i,
	  const string &name_i, Type *type_i);
  virtual ~Symbol () {}

  bool			isStream	() const { return symKind==
								SYMBOL_STREAM;}
  bool			isStreamValid	() const { return symKind==
								SYMBOL_STREAM_VALID;}
  void			setStreamValid	();
  bool			isOp		() const { return symKind==SYMBOL_OP; }
  bool			isReg		() const;	// kinds of SYMBOL_VAR
  bool			isLocal		() const;
  bool			isParam		() const;
  bool			isArray		() const;

  static void		init		();	 // initialize pre-defined syms
  SymbolKind		getSymKind	() const { return symKind; }
  string		getName		() const { return name; }

  virtual bool		setName		(const string &newName);
  virtual bool		equals		(const Symbol *s) const;

  virtual void		setParent	(Tree *p);
  virtual Type*		getType		() const { return type; }
  virtual void		deleteType	()	 {}
  virtual void		clearType	()	 {}
  virtual SymTab*	getScope	() const { return (SymTab*)parent; }
  virtual string	toString	() const { return name; }
  virtual string	declToString	() const = 0;
};


enum StreamDir { STREAM_IN, STREAM_OUT, STREAM_ANY };

class SymbolStream : public Symbol
{
private:
  StreamDir dir;
  string range;
  string range_left;
  string range_right;

public:
  SymbolStream (Token *token_i, const string &name_i,
		Type *type_i,   StreamDir dir_i, string low = "", string high = "");
  virtual ~SymbolStream () {}

  StreamDir		getDir		() const	{ return dir; }
  string		getRange	() const	{ return range; }
  string		getMatlabRange	(int samples) const	{ return range_left+":"+samples+":"+range_right ; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
//virtual string	toString	() const;
  virtual string	declToString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


// enum VarKind { VAR_PARAM, VAR_REG, VAR_LOCAL };

class SymbolVar : public Symbol
{
private:
  // VarKind	varKind;	// redundant with scope (which SymTab)
  Expr		*value;		// initial value(s)
  Expr		*depth;		// minimum-depth hint for local streams
				//   inside compos. op (NULL for reg. vars)
  string    number;

public:
  SymbolVar (Token *token_i, const string &name_i,
	     Type *type_i,   Expr *value_i=NULL,   Expr *depth_i=NULL);
  SymbolVar (Token *token_i, const string &name_i, Type *type_i,  string num);
  virtual ~SymbolVar () {}

  Expr*			getValue	() const	{ return value; }
  Expr*			getDepth	() const	{ return depth; }
  void                  setValue	(Expr *nvalue)	{ value=nvalue; }
  void                  setSetDepthHint	(Expr *ndepth)	{ depth=ndepth; }
  
  string		getNumber	() const	{ return number; }

//VarKind		getVarKind	() const;

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
//virtual string	toString	() const;
  virtual string	declToString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class SymbolOp : public Symbol
{
private:
  Operator *op;

public:
  SymbolOp (Token *token_i, Operator *op_i);
  virtual ~SymbolOp () {}

  Operator*		getOp		() const	 { return op; }

  virtual bool		setName		(const string &newName);
  
  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
//virtual string	toString	() const;
  virtual string	declToString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


////////////////////////////////////////////////////////////////
//  pre-defined symbols

extern const SymbolVar *sym_none;	// for unlinked syms & non-valued trees



#endif  // #ifndef _TDF_SYMBOL_
