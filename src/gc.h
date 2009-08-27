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
// SCORE TDF compiler:  Garbage collector
// $Revision: 1.81 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_GC_
#define _TDF_GC_

#include "tree.h"
#include "misc.h"
#include "suite.h"
#include <LEDA/core/set.h>

using leda::set;

extern int gEnableGC;			// enable GC (-gc option)


class TreeGC				// garbage collector for Tree (AST)
{
private:
  static set<Tree*> rootSet;		// - known Tree roots
  static set<Tree*> allocSet;		// - all allocated Tree nodes

  friend void*	Tree::operator new	(size_t size);	// - calls newTree
  friend void	Tree::operator delete	(void *t);	// - calls deleteTree
  static void	newTree			(Tree *t)	{ allocSet.insert(t);}
  static void	deleteTree		(Tree *t)	{ allocSet.del(t);   }

public:
  static void addRoot		(const Tree *t)	{ rootSet.insert((Tree*)t); }
  static void removeRoot	(const Tree *t)	{ rootSet.del((Tree*)t);    }
  static void clearRoots	()		{ rootSet.clear();	    }
  static bool isRoot		(const Tree *t)	{ return
						  rootSet.member((Tree*)t); }
  static bool isReachable	(const Tree *t);
  static void gc		();
};


#endif	// #ifndef _TDF_GC_
