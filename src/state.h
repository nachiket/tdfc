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
// SCORE TDF compiler:  states
// $Revision: 1.140 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_STATE_
#define _TDF_STATE_

#include "compares.h"
#include "symbol.h"
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/dictionary.h>
#include <LEDA/core/set.h>
#include "type.h"
#include "tree.h"
#include "blockdfg.h" // Added by Nachiket on 11/29/2009

class StateCase;
class InputSpec;
class Token;
class Operator;
class Stmt;
class Expr;
class Symbol;

using leda::set;
using leda::string;

class State : public Tree
{
private:
  Token			*token;
  leda::string		name;
  set<StateCase*>	*cases;

public:
  State (Token *token_i, const string &name_i,
	 set<StateCase*> *cases_i=NULL);
  virtual ~State ()					{ delete cases; }

  leda::string		getName		() const	{ return name; }
  set<StateCase*>*	getCases	() const	{ return cases; }
  void			addCase		(StateCase *case_i);
  void			removeCase	(StateCase *case_i);
  bool			setName		(const string &newName);

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		setParent	(Tree *p);
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual void		deleteType	()		{}
  virtual void		clearType	()		{}
  virtual Type*		getType		() const	{ return type_none; }
  virtual BindTime	getBindTime	() const;
  virtual SymTab*	getScope	() const;	// nearest symtab
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class StateCase : public Tree
{
private:
  list<InputSpec*>	*inputs;
  list<Stmt*>		*stmts;
  BlockDFG			dfg; // Added by Nachiket on 11/29/2009 to store the dataflow graph for each case..

public:
  StateCase (Token *token_i, list<InputSpec*> *inputs_i,
	     list<Stmt*> *stmts_i);
  virtual ~StateCase ()					{ delete inputs;
							  delete stmts;  }

  list<InputSpec*>*	getInputs	() const	{ return inputs; }
  list<Stmt*>*		getStmts	() const	{ return stmts; }
  void			addDataflowGraph(BlockDFG dfg_i);
  BlockDFG		getDataflowGraph() const 	{ return dfg;}
  void			addInput	(InputSpec *input_i);

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		setParent	(Tree *p);
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual Type*		getType		() const	{ return type_none; }
  virtual void		deleteType	()		{}
  virtual void		clearType	()		{}
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class InputSpec : public Tree {
private:
  Symbol	*stream;
  Expr		*numTokens;
  bool		eos;
  bool		eofr; // added an end-of-frame token on October 5th 2009

public:
  InputSpec (Token *token_i, Symbol *stream_i, Expr *numTokens_i=NULL, bool eos_i=false, bool eofr_i=false);

  Symbol*		getStream	() const	{ return stream; }
  Expr*			getNumTokens	() const	{ return numTokens; }
  bool			isEosCase	() const	{ return eos; }
  bool			isEofrCase	() const	{ return eofr; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		setParent	(Tree *p);
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual Type*		getType		() const { return stream->getType(); }
  virtual void		deleteType	()		{}
  virtual void		clearType	()		{}
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


extern string printStateCaseHead (const StateCase *sc);
#endif  // #ifndef _TDF_STATE_
