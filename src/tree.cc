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
// $Revision: 1.74 $
//
//////////////////////////////////////////////////////////////////////////////


#include "tree.h"
#include "gc.h"

string Tree::indent;				// used to print code blocks


////////////////////////////////////////////////////////////////
//  new/delete  (GC-aware)

void* Tree::operator new (size_t size)
{
  void *t=::operator new(size);
  if (gEnableGC)
    TreeGC::newTree((Tree*)t);
  return t;
}


void  Tree::operator delete (void *t)
{
  if (gEnableGC)
    TreeGC::deleteTree((Tree*)t);
  ::operator delete(t);
}


////////////////////////////////////////////////////////////////
//  misc

Operator* Tree::getOperator () const			// enclosing operator
{
  return kind == TREE_OPERATOR ? (Operator*)this :
			parent ? parent->getOperator() : NULL;
}


SymTab* Tree::getScope () const				// nearest symtab
{
  return parent ? parent->getScope() : NULL;
}
