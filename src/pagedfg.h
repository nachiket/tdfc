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
// SCORE TDF compiler:  Page-level data-flow graph
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_PAGEDFG_
#define _TDF_PAGEDFG_

#include <LEDA/graph/graph.h>
class Operator;
class OperatorCompose;
class OperatorBehavioral;
class Tree;
class Symbol;

using leda::GRAPH;

////////////////////////////////////////////////////////////////
//  page DFG definition

// - PageDFG = page-level data-flow graph
// - node = Operator* (page or built-in, e.g. segment op)
//          or Symbol* for a primary I/O (formal arg) of compose-op
// - arc  = Symbol* for a local or primary I/O stream of compose-op

typedef GRAPH<Tree*,Symbol*> PageDFG;


////////////////////////////////////////////////////////////////

void   buildPageDFG (OperatorCompose *iop, PageDFG *dfg);

string printPageDFG (PageDFG *dfg);

////////////////////////////////////////////////////////////////

#endif	// #ifndef _TDF_PAGENETLIST_
