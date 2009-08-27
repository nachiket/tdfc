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
// SCORE TDF compiler:  rate_tables
// $Revision: 1.43 $
// $Date: 1999/07/16 20:58:03 $
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _TDF_RATE_TABLE_
#define _TDF_RATE_TABLE_

#include <assert.h>
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/set.h>
#include <LEDA/core/dictionary.h>
#include <LEDA/core/tuple.h>
#include <LEDA/core/array.h>
#include <LEDA/core/array2.h>
#include "tree.h"
#include "operator.h"
#include "state.h"
#include "symbol.h"
#include "expr.h"
#include "stmt.h"

using leda::array2;
using leda::array;
using leda::two_tuple;

typedef two_tuple<int,int> Pair;

class Token; 
class Type;
class SymTab;
class Operator;
class State;
class Stmt;

class RateTable
{

private: // Don't plan to derive any classes
  string           name;
  int              noStreams;
  array2<Pair>     rates;
  array<Symbol*>   ioArgs;
  array<int>       isInput; // is corresponding entry in ioArgs an Input?
  // currently assuming if something isn't an input it's an output :(
  void      buildTable(OperatorBehavioral *p);
  void      buildIOList(list<Symbol*>* orgList);
  void      calculate(int a, int b, OperatorBehavioral *bop);
  Pair      calculateState(int a, int b, State* st); 
  Pair      calculateStateCase(int a, int b, StateCase* stc);
public:
  RateTable(const string &name_i, Operator *input_op);
  // everything interesting will happen in the constructor
  virtual ~RateTable () {}

  void      printout();
  string    getName() const {return name;}
  int       getNoStreams() const {return noStreams;}
  Pair      lookup (int a, int b)  const {return rates(a,b);}
  
};



#endif // #ifndef _TDF_RATE_TABLE_




