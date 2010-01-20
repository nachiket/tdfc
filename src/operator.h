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
// SCORE TDF compiler:  operators
// $Revision: 1.143 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_OPERATOR_
#define _TDF_OPERATOR_

#include "compares.h"
#include <assert.h>
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
//#include <LEDA/core/set.h>
#include <LEDA/core/dictionary.h>
#include "tree.h"
#include "symbol.h"
class Token;
class Type;
class SymTab;
class State;
class Stmt;


enum OperatorKind { OP_BEHAVIORAL, OP_COMPOSE, OP_BUILTIN };


/*string operatorKindToString(OperatorKind kind) {
	if(kind==OP_BEHAVIORAL) {
		return string("OP_BEHAVIORAL");
	} else if(kind==OP_COMPOSE) {
		return string("OP_COMPOSE");
	} else if(kind==OP_BUILTIN) {
		return string("OP_BUILTIN");
	} else {
		return string("what?");
	}
}
*/

class Operator : public Tree
{
protected:
  string 	name;
  OperatorKind	opKind;
  Symbol	*retSym;
  list<Symbol*>	*args;		// input/output arguments
  SymTab	*symtab;	// ret, args, children: vars -> states
  Token		*exception;

  void		duplicateHelper ();

public:
  Operator (Token  *token_i,  const string &name_i,
	    Symbol *retSym_i, list<Symbol*> *args_i,
	    OperatorKind opKind_i, Token *exception_i=NULL);
  virtual ~Operator ()				 { delete args; }

  string		getName		() const { return name; }
  OperatorKind		getOpKind	() const { return opKind; }
  Symbol*		getRetSym	() const { return retSym; }
  list<Symbol*>*	getArgs		() const { return args; }
  SymTab*		getSymtab	() const { return symtab; }
  SymbolOp*		getSymbol	() const;

  virtual bool		setName		(const string &newName);
//void			setRetSym	(Symbol *retSym_i);
//void			setArgs		(list<Symbol*> *args_i);
//void			setSymtab	(SymTab *symtab_i);
//void			setSymbol	(SymbolOp *symbol_i);

  virtual void		setParent	(Tree *p);
  virtual Type*		getType		() const { return retSym->getType(); }
  virtual void		deleteType	()	 {}
  virtual void		clearType	()	 {}
  virtual SymTab*	getScope	() const { return symtab; }
  virtual string	toDOTString	(string offset) const;
  virtual string	declToString	() const;
};


class OperatorBehavioral : public Operator
{
private:
  dictionary<string,State*>	*states;
  State				*startState;
  SymTab			*vars;

public:
  OperatorBehavioral (Token  *token_i,  const string &name_i,
		      Symbol *retSym_i, list<Symbol*> *args_i,
		      SymTab *vars_i,   dictionary<string,State*> *states_i,
		      State  *startState_i, Token *exception_i=NULL);
  virtual ~OperatorBehavioral ()			{ delete states; }

  dictionary<string,State*>*
			getStates	() const	{ return states; }
  State*		getStartState	()		{ return startState; }
  void			setStartState	(State *s)	{ startState=s; }
  State*		lookupState	(const string &name) const;
  void			addState	(State* s);
  void			removeState	(State* s);
  SymTab*		getVars		() const	{ return vars; }

  virtual size_t	getSizeof	() const	{ return sizeof *this;}
  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual BindTime	getBindTime	() const;
  virtual Tree*		duplicate	() const;
  virtual string	toString	() const;
  virtual void		buildDataflowGraph() const;
  virtual string	toDFGString	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};


class OperatorCompose : public Operator
{
private:
  list<Stmt*> *stmts;		// list of StmtCall* + StmtAssign*
  SymTab			*vars;

public:
  OperatorCompose (Token  *token_i,  const string &name_i,
		   Symbol *retSym_i, list<Symbol*> *args_i,
		   SymTab *vars_i,   list<Stmt*> *stmts_i,
		   Token  *exception_i=NULL);
  virtual ~OperatorCompose ()				{ delete stmts; }

  list<Stmt*>*		getStmts	() const	{ return stmts; }
  SymTab*		getVars		() const	{ return vars; }

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


enum BuiltinKind {  BUILTIN_ATTN,
		    BUILTIN_BITSOF,
		    BUILTIN_CAT,
		    BUILTIN_CLOSE,
		    BUILTIN_FRAMECLOSE,
		    BUILTIN_COPY,
		    BUILTIN_DONE,
		  //BUILTIN_EOS,
		    BUILTIN_PRINTF,
		    BUILTIN_SEGMENT,
		    BUILTIN_WIDTHOF,
                    BUILTIN_TERMINATE };

class OperatorBuiltin : public Operator
{
private:
  BuiltinKind	builtinKind;

public:
  OperatorBuiltin (Token  *token_i,  const string &name_i,
		   Symbol *retSym_i, list<Symbol*> *args_i,
		   BuiltinKind builtinKind_i, Token *exception_i=NULL);
  virtual ~OperatorBuiltin () {}

  static void		init		();	// initialize built-in ops

  BuiltinKind		getBuiltinKind	() const { return builtinKind; }
  virtual bool		okInComposeOp	() const;

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


enum SegmentKind { SEGMENT_R,
		   SEGMENT_W,
		   SEGMENT_RW,
		   SEGMENT_SEQ_R,
		   SEGMENT_SEQ_W,
		   SEGMENT_SEQ_RW };

class OperatorSegment : public OperatorBuiltin
{
private:
  SegmentKind	segmentKind;
  Type		*dataType;

public:
  OperatorSegment (Token  *token_i, const string &name_i,
		   SegmentKind segmentKind_i,
		   Type *dataType_i, list<Symbol*> *args_i);
  virtual ~OperatorSegment () {}

  SegmentKind		getSegmentKind	() const { return segmentKind;	}
  Type*			getDataType	() const { return dataType;	}

  virtual void		thread		(Tree *p);
  virtual bool		link		();
  virtual Type*		typeCheck	();
  virtual bool		okInComposeOp	() const { return true;		}
  virtual Tree*		duplicate	() const;
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL);
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false);
};

extern OperatorSegment* makeOperatorSegment (Token *token_i,
					     SegmentKind segmentKind_i,
					     const Type *dataType_i);

////////////////////////////////
//  OperatorSegment prototypes:
//    (for unsigned data type)
//
////////////////////////////////
//  Read-only Segment:
//
//  SegmentR	(param  unsigned[7]        dwidth,	// data width
//		 param  unsigned[7]        awidth,	// addr width 
//		 param  unsigned[awidth+1] nelems,	// no. of words
//		 param  unsigned[nelems][dwidth] contents,
//		 input  unsigned[awidth]   addr,
//		 output unsigned[dwidth]   data);
//
////////////////////////////////
//  Write-only Segment:
//
//  SegmentW	(param  unsigned[7]        dwidth,	// data width
//		 param  unsigned[7]        awidth,	// addr width
//		 param  unsigned[awidth+1] nelems,	// no. of words
//		 param  unsigned[nelems][dwidth] contents,
//		 input  unsigned[awidth]   addr,
//		 input  unsigned[dwidth]   data);
//
////////////////////////////////
//  Read-write Segment:
//
//  SegmentRW	(param  unsigned[7]        dwidth,	// data width
//		 param  unsigned[7]        awidth,	// addr width
//		 param  unsigned[awidth+1] nelems,	// no. of words
//		 param  unsigned[nelems][dwidth] contents,
//		 input  unsigned[awidth]   addr,
//		 output unsigned[dwidth]   dataR,
//		 input  unsigned[dwidth]   dataW,
//		 input  boolean            write);	// r/w mode
//
////////////////////////////////
//  Sequential Read-only Segment:
//
//  SegmentSeqR	(param  unsigned[7]      dwidth,	// data width
//		 param  unsigned[7]      awidth,	// addr width
//		 param  unsigned[awidth] nelems,	// no. of words
//		 param  unsigned[nelems][dwidth] contents,
//		 output unsigned[dwidth] data);
//
////////////////////////////////
//  Sequential Write-only Segment:
//
//  SegmentSeqW	(param  unsigned[7]        dwidth,	// data width
//		 param  unsigned[7]        awidth,	// addr width
//		 param  unsigned[awidth+1] nelems,	// no. of words
//		 param  unsigned[nelems][dwidth] contents,
//		 input  unsigned[dwidth]   data);
//
////////////////////////////////
//  Sequential Read-Write Segment:  (FIFO)
//
//  SegmentSeqRW(param  unsigned[7]        dwidth,	// data width
//		 param  unsigned[7]        awidth,	// addr width
//		 param  unsigned[awidth+1] nelems,	// max words
//		 param  unsigned[nelems][dwidth] contents,
//		 output unsigned[dwidth]   dataR,
//		 input  unsigned[dwidth]   dataW);
//
////////////////////////////////

enum SegmentRFormalPos		{ SEGMENT_R_DWIDTH	 =0,
				  SEGMENT_R_AWIDTH	 =1,
				  SEGMENT_R_NELEMS	 =2,
				  SEGMENT_R_CONTENTS	 =3,
				  SEGMENT_R_ADDR	 =4,
				  SEGMENT_R_DATA	 =5 };

enum SegmentWFormalPos		{ SEGMENT_W_DWIDTH	 =0,
				  SEGMENT_W_AWIDTH	 =1,
				  SEGMENT_W_NELEMS	 =2,
				  SEGMENT_W_CONTENTS	 =3,
				  SEGMENT_W_ADDR	 =4,
				  SEGMENT_W_DATA	 =5 };

enum SegmentRWFormalPos		{ SEGMENT_RW_DWIDTH	 =0,
				  SEGMENT_RW_AWIDTH	 =1,
				  SEGMENT_RW_NELEMS	 =2,
				  SEGMENT_RW_CONTENTS	 =3,
				  SEGMENT_RW_ADDR	 =4,
				  SEGMENT_RW_DATA_R	 =5,
				  SEGMENT_RW_DATA_W	 =6,
				  SEGMENT_RW_WRITE	 =7 };

enum SegmentSeqRFormalPos	{ SEGMENT_SEQ_R_DWIDTH	 =0,
				  SEGMENT_SEQ_R_AWIDTH	 =1,
				  SEGMENT_SEQ_R_NELEMS	 =2,
				  SEGMENT_SEQ_R_CONTENTS =3,
				  SEGMENT_SEQ_R_DATA	 =4 };

enum SegmentSeqWFormalPos	{ SEGMENT_SEQ_W_DWIDTH	 =0,
				  SEGMENT_SEQ_W_AWIDTH	 =1,
				  SEGMENT_SEQ_W_NELEMS	 =2,
				  SEGMENT_SEQ_W_CONTENTS =3,
				  SEGMENT_SEQ_W_DATA	 =4 };

enum SegmentSeqRWFormalPos	{ SEGMENT_SEQ_RW_DWIDTH	 =0,
				  SEGMENT_SEQ_RW_AWIDTH	 =1,
				  SEGMENT_SEQ_RW_NELEMS	 =2,
				  SEGMENT_SEQ_RW_CONTENTS=3,
				  SEGMENT_SEQ_RW_DATA_R  =4,
				  SEGMENT_SEQ_RW_DATA_W  =5 };


////////////////////////////////////////////////////////////////
//  parser support
//   - parts used by parser to build up operators

class OpBody
{
public:
  OperatorKind	opKind;
  OpBody (OperatorKind opKind_i) : opKind(opKind_i) {}
};

class BehavOpBody : public OpBody
{
public:
  SymTab			*vars;
  dictionary<string,State*>	*states;
  State				*startState;
  BehavOpBody(SymTab *vars_i,
	      dictionary<string,State*> *states_i, State *startState_i)
    : OpBody(OP_BEHAVIORAL),
      vars(vars_i),
      states(states_i),
      startState(startState_i) {}
};

class ComposeOpBody : public OpBody
{
public:
  SymTab	*vars;
  list<Stmt*>	*stmts;
  ComposeOpBody(SymTab *vars_i, list<Stmt*> *stmts_i)
    : OpBody(OP_COMPOSE), vars(vars_i), stmts(stmts_i) {}
};

class OpSignature
{
public:
  Token		*token;
  string	name;
  list<Symbol*>	*args;
  Symbol	*retSym;
  OpSignature (Token *token_i, const string &name_i,
	       list<Symbol*> *args_i, Symbol *retSym_i=NULL)
    : token(token_i), name(name_i), args(args_i), retSym(retSym_i) {}
};


////////////////////////////////////////////////////////////////
// pre-defined operators in operator.cc

extern OperatorBuiltin *builtin_attn;		// raise attention
extern OperatorBuiltin *builtin_cat;		// concatenate bits
extern OperatorBuiltin *builtin_close;		// close stream
extern OperatorBuiltin *builtin_frameclose;		// close frame on a stream
extern OperatorBuiltin *builtin_copy;		// copy stream to fanout
// i.e. replicate input sequence to multiple output sequences
extern OperatorBuiltin *builtin_done;		// self-terminate operator
//extern OperatorBuiltin *builtin_eos;		// end-of-stream predicate
extern OperatorBuiltin *builtin_widthof;	// get bit-width of expr
extern OperatorBuiltin *builtin_bitsof;		// get representation of expr
extern OperatorBuiltin *builtin_terminate;	// terminate stream (no cnsumr)
extern OperatorBuiltin *builtin_printf;		// call printf (debug)



#endif  // #ifndef _TDF_OPERATOR_
