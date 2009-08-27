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
// SCORE TDF compiler:  rename all variables so as to avoid name/keywd conflicts in c++
// $Revision: 1.107 $
//
//////////////////////////////////////////////////////////////////////////////

#include "operator.h"
#include "tree.h"
#include <LEDA/core/set.h>
#include <LEDA/core/list.h>

using leda::set;
using leda::list_item;


bool add_prefix(Tree *t, void *aux)
{
  if (t->getKind()==TREE_SYMTAB)
    {
      set<Symbol *> *syms=new set<Symbol *>();
      SymTab * symtab=(SymTab *)t;
      list<Symbol*>* lsyms=symtab->getSymbolOrder();
      list_item item;
      forall_items(item,*lsyms)
	{
	  Symbol *sym=lsyms->inf(item);
	  syms->insert(sym);
	}
      string prefix=*((string *)aux);
      Symbol *sym;
      forall(sym,*syms)
	sym->setName(prefix+sym->getName());
      return(0); // don't keep searching
    }
  else if ((t->getKind()==TREE_EXPR)	
	   || (t->getKind()==TREE_SYMBOL)
	   || (t->getKind()==TREE_TYPE))
    return(0);
  else
    return(1);
    
}

bool ccrename_treemap_false(Tree *t, void *aux)
{ return (0); }

void ccrename(Operator *op)
{

  string *prefix=new string("cc_");
  // may have split return value and operator name, so put back

  string opSaveName=op->getName();
  op->setName(*prefix+op->getName());
  // note: this op set name needs to occur before the map...

  op->map(add_prefix,ccrename_treemap_false,(void *)prefix);

  // recover original name....necessary to make final file name/function
  //  match user expectations...
  // op->setName(op->getToken()->str);	// fails on NULL token, EC 12/7/99
  op->setName(opSaveName);
}
