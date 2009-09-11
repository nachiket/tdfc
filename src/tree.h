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
// SCORE TDF compiler:  abstract syntax tree
// $Revision: 1.83 $
//
//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
//
//  Abstract syntax tree (AST)
//
////////////////////////////////////////////////////////////////
//
//  Parentage:   ("a node may contain...")
//
//	Suite -+-> Operator -+-> State --> StateCase -+-> Stmt ------+-> Expr <
//	       |             |                        |              |
//	       +-> SymTab    +-> Stmt                 +-> InputSpec  +-> Stmt
//	                     |                        |              |
//	                     +-> SymTab               +-> SymTab     +-> Symtab
//
//	SymTab -+-> Symbol --> Type --> Expr --> Type <
//		|
//	        +-> SymTab
//
////////////////////////////////////////////////////////////////
//
//  Class hierarchy:
//
//	Tree
//	|
//	|_______________________________________________________.
//	|	|		| | |		|		|
//	Suite	Operator	| | InputSpec	Stmt		Expr
//		|		| StateCase	|		|
//		|		State		|		|
//		|				|		|
//		OperatorBehavioral		StmtIf		ExprValue
//		OperatorCompose			StmtGoto	ExprLValue
//		OperatorBuiltin			StmtCall	ExprCall
//		OperatorSegment			StmtBuiltin	ExprBuiltin
//						StmtAssign	ExprCond
//						StmtBlock	ExprBop
//								ExprUop
//								ExprCast
//								ExprArray
//	Symbol			Type
//	|			|
//	SymbolStream		TypeFixed
//	SymbolVar		TypeArray
//	SymbolOp
//
////////////////////////////////////////////////////////////////
//
//  AST creation:
//
//	- instantiate + build tree using "new ..." (done by parser)
//	- link
//	- type-check
//
////////////////////////////////////////////////////////////////
//
//  Tree methods:
//
//	constructor	Creates AST node.
//			Does not duplicate args, so must be passed ptrs
//			to persistent components.  In some cases, passing
//			NULL ptr will cause allocation of new components.
//
//	destructor	Deletes AST node and LEDA components.
//			Not recursive, i.e. does not delete AST children.
//
//	setParent	
//	getParent	Set/get ptr to AST parent.  Set checks parent kind.
//			The parent node typically indicates where
//			(in the TDF src) this node is defined.
//
//	getToken	Token object references original TDF source for
//			its tree node.  Tokens may be shared among AST
//			nodes, i.e. not duplicated uniquely.
//
//	getKind		Kind of AST node (symbol, expression, etc.)
//			Each subclass also has separate subclass kind.
//
//	getAnnote	Retrive annotation by index #.
//	setAnnote	Set     annotation by index #.
//	setAnnote_	Set     annotation by index # (macro with cast).
//	clearAnnotes	Reset all annotations to NULL.
//			Annotations are void-ptrs used to attach user-defined
//			data to AST nodes.  Each AST node has a collection
//			of annotations indexed by integers.  Default is
//			NULL ptr for undefined annotes.  Tree duplication
//			copies annotation indexes and ptrs, but not any
//			user-defined structures which are pointed to.
//
//	getSizeof	Returns sizeof(*this).
//			Not recursive, i.e. does not tally subtree sizes
//			or LEDA structures implemented behind pointers.
//
//	getOperator	Finds nearest enclosing operators node, NULL if none.
//
//	getScope	Finds nearest enclosing symbol table, NULL if none.
//			AST nodes which contain symbol tables (e.g. operator,
//			stmt block) return their own symbol table.
//			Symbol tables return enclosing symbol table,
//			not same as getParent.
//
//	getType		Return type (as Type node) for this AST node.
//			Fundamental types (e.g. for constants, casts)
//			are specified by user to constructor.  Derived types
//			(e.g. for '+' expr) are computed during typeCheck().
//			If node's type was removed via clearType/deleteType,
//			it is regenerated and returned
//
//	deleteType	Delete node's type subtree if it is derived/memoized.
//			Type will be automatically reconstructed by any
//			subsequent call to getType() or typeCheck()
//
//	clearType	Dissociate from node's type subtree if it is
//			derived/memoized.  Requires subsequent deletion/gc
//			of type subtree.
//
//	thread		Recursively set parent pointers in subtree.  Defunct.
//
//	link		Lookup all named objects (symbols, states, operators)
//			in present scope, and rebuild any outdated/missing
//			components (e.g. bit subscripts for symbol reference).
//			Necessary after constructing or duplicating a tree
//			and placing it in a new scope.
//			After link, the only subtree/component pointers
//			allowed to be NULL are:
//			 - StmtIf::elsePart for else-less ifs
//			 - ExprLValue::inputRetime for symbols which are not
//			   input streams
//			 - Type::widthExpr for types with static-constant
//			   or unknown width
//			 - Type::predicate for known-valid types
//
//	typeCheck	Recursively compute and verify types for subtree.
//
//	getBindTime	Recursively compute binding time for AST node.
//			(stubs, not implemented)
//
//	duplicate	Recursively duplicate AST subtree (on heap).
//			Requires relinking after plugging AST into new scope.
//
//	toString	Generate TDF source-code for subtree (LEDA string)
//	string		Conversion operator, calls toString.
//
//	map		Walk tree, mapping user-functions over each node.
//	map2		Map, allows replacing nodes during tree walk.
//			See "Tree traversal" below.
//
////////////////////////////////////////////////////////////////
//
//  Tree traversal:  (map)
//
//  - methods "map" and "map2" traverse an AST and map user functions
//	over it.  "map" allows modifying each node's contents,
//      whereas "map2" allows replacing each node entirely.
//
//  - map (pre,post,i)
//	* Traversing an AST in lexicographic order, for each node t:
//	    (1) call user function "pre(Tree *t, void *i)"
//	    (2) map over t's children if "pre" returned true
//	    (3) call user function "post(Tree *t, void *i)"
//	* Thus "pre" is called in a pre-order traversal
//	    (and may prune the traversal descent),
//	    and "post" is called in a post-order traversal
//      * "pre" and "post" may modify nodes but may not replace them.
//	    maintaining proper threading/linking/types is up to the user.
//	* "i" is a user-supplied void* passed-on to "pre" and "post".
//	    any changes made during traversal to structures pointed-to
//	    by "i" are globally visible to the rest of the traversal
//      
//
//  - map2 (h,pre,post,i,skipPre)
//      * Basic operation is similar to (*h)->map(pre,post,i),
//	    where "h" points to the root-node pointer
//	* Traversing an AST in lexicographic order, calls user functions
//	    "pre(Tree **h, void *i)", "post(Tree **h, void *i)",
//	    where "h" points to each node-pointer
//	* "pre" and "post" may replace tree nodes via *h=(non-NULL Tree*)
//	    map2() automatically threads any replaced node.
//	    map2() maps over replaced nodes (subtrees) normally.
//	* "skipPre" flag is used internally only, to skip calling
//	    "pre" on the root node.  normal use of map2() omits this
//	    argument, defaulting it to false
//
////////////////////////////////////////////////////////////////


#ifndef _TDF_TREE_
#define _TDF_TREE_


#include <sys/types.h>
#include <LEDA/core/string.h>
#include <LEDA/core/d_array.h>
#include "bindTime.h"
class Token;
class Tree;
class Type;
class SymTab;
class Operator;

using leda::string;
using leda::d_array;

typedef bool (*TreeMap) (Tree  *t, void *i);	// fn to map over tree,
						// returns true to descend

typedef bool (*TreeMap2)(Tree **t, void *i);	// fn to map over tree,
						// returns true to descend,
						// may replace mapped node


enum TreeKind { TREE_SUITE=0,			// 0
		TREE_OPERATOR,			// 1
		TREE_STATE,			// 2
		TREE_STATECASE,			// 3
		TREE_INPUTSPEC,			// 4
		TREE_STMT,			// 5
		TREE_EXPR,			// 6
		TREE_SYMTAB,			// 7
		TREE_SYMBOL,			// 8
		TREE_TYPE	};		// 9

extern string treekindToString(TreeKind t);
/*
	switch(t) {
		case TREE_SUITE: return string("TREE_SUITE");
		case TREE_OPERATOR: return string("TREE_OPERATOR");
		case TREE_STATE: return string("TREE_STATE");
		case TREE_STATECASE: return string("TREE_STATECASE");
		case TREE_INPUTSPEC: return string("TREE_INPUTSPEC");
		case TREE_STMT: return string("TREE_STMT");
		case TREE_EXPR: return string("TREE_EXPR");
		case TREE_SYMTAB: return string("TREE_SYMTAB");
		case TREE_SYMBOL: return string("TREE_SYMBOL");
		case TREE_TYPE: return string("TREE_TYPE");
		default: return string("bs");
	}
}
*/

class Tree
{
protected:
  Tree			*parent;		// parent node in AST
  Token			*token;			// associated TDF src or NULL
  TreeKind		kind;			// kind of AST node
  d_array<int,void*>	annotes;		// user annotations

  static string	indent;				// used to print code blocks
  static void	indentPush ()	{ indent+="  "; }
  static void	indentPop  ()	{ indent=indent.head(indent.length()-2); }

public:
  Tree (Token *token_i, TreeKind kind_i, Tree *parent_i=NULL)
    : parent(parent_i), token(token_i),  kind(kind_i), annotes(NULL) {}
  virtual ~Tree () {}

  void* operator new	(size_t size);
  void  operator delete	(void *t);

  virtual void		setParent	(Tree *p)	{ parent=p;	 }
  virtual Tree*		getParent	() const	{ return parent; }
  virtual Token*	getToken	() const	{ return token;  }
  virtual TreeKind	getKind		() const	{ return kind;	 }
  
  void*			getAnnote	(int key) const { return annotes[key];}
  void			setAnnote	(int key, void *data)
							{ annotes[key]=data;  }
  void			clearAnnote	(int key)	{ annotes.undefine
								       (key); }
  void			clearAnnotes	()		{ annotes.clear();    }
  #define		setAnnote_(key,data)	setAnnote((key),(void*)(data))

  virtual size_t	getSizeof	() const = 0;	// sizeof(*this)
  virtual Operator*	getOperator	() const;	// enclosing operator
  virtual SymTab*	getScope	() const;	// nearest symtab
  virtual Type*		getType		() const = 0;	// return (regen) type
  virtual void		deleteType	() = 0;		// delete memoized type
  virtual void		clearType	() = 0;		// dissoc memoized type
  virtual void		thread		(Tree *p) = 0;	// set parent pointers
  virtual bool		link		() = 0;		// linking
  virtual Type*		typeCheck	() = 0;		// compute/check types
  virtual BindTime	getBindTime	() const = 0;	// find consts/params
  virtual Tree*		duplicate	() const = 0;	// recursive copy
  virtual string	toString	() const = 0;	// text output
  virtual void		map		(TreeMap pre,
					 TreeMap post=NULL, void *i=NULL) = 0;
							// map over tree
  virtual void		map2		(Tree **h,
					 TreeMap2 pre, TreeMap2 post=NULL,
					 void *i=NULL, bool skipPre=false) = 0;
							// map over tree

  operator		string		()		{ return toString(); }
};


#endif	// #ifndef _TDF_TREE_
