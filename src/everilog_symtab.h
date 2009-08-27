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
// SCORE TDF compiler:  Symbol table for Verilog emission
// $Revision: 1.2 $
//
//////////////////////////////////////////////////////////////////////////////

// - used internally by "everilog.cc" and "everilog_compose.cc"


#ifndef _TDF_EVERILOG_SYMTAB_
#define _TDF_EVERILOG_SYMTAB_

#include "tree.h"
#include "misc.h"
#include <LEDA/core/string.h>
#include <LEDA/core/set.h>
#include <LEDA/core/d_array.h>

using leda::set;

extern void verilogKeywords (set<string> *keywords);

class VerilogSymTab			// - Symbol table of Verilog names,
{					//   built to avoid name conflicts
private:
  d_array<string,Tree*> symToTree;	// - Symbol table contents
					//    (map verilog name to TDF object)
  set<string>           keywords;	// - Verilog reserved keywords

public:
  VerilogSymTab () : symToTree(NULL) { verilogKeywords(&keywords); }

  Tree* lookup (const string& s) {
    if (symToTree.defined(s))  return symToTree[s];
    else                       return NULL;
  }

  bool insert (const string& s, Tree *t) {
    if (symToTree.defined(s))  {                 return false; }
    else                       { symToTree[s]=t; return true;  }
  }

  string insertRename (const string& s, Tree *t) {
    if (!keywords.member(s) && insert(s,t))
      return s;
    else {
      for (int i=1; i<=10000; i++) {
	string newname = s+string("_%d",i);
	if (insert(newname,t))
	  return newname;
      }
      fatal(1, "Probable error, "+s+" has 10,000 Verilog name conflicts",
	    (t ? t->getToken() : NULL));
    }
    return "";	// - dummy
  }

  bool remove (const string& s) {
    if (symToTree.defined(s))  { symToTree.undefine(s); return true;  }
    else                       {                        return false; }
  }

  void clear ()  { symToTree.clear(); }

  int size ()  { return symToTree.size(); }
};


#endif	// #ifndef _TDF_EVERILOG_SYMTAB_
