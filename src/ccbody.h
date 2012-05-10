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
// SCORE TDF compiler:  generate C++ output (master side)
// $Revision: 1.53 $
//
//////////////////////////////////////////////////////////////////////////////
#include "operator.h"
#include "blockdfg.h"
#include <LEDA/core/array.h>
#include <LEDA/graph/node_set.h>
#include <LEDA/graph/node_list.h>

using leda::node_list;

#define STATE_PREFIX "STATE_"

void ccbody(Operator *op,int debug_logic);
void ccdfgbody(Operator *op, int debug_logic);
void ccmicroblazebody(Operator *op);
void ccautoeslbody(Operator *op, bool *exp, bool *log, bool *div);
void ccautoeslwrapper(Operator* op);
void ccautoeslmake(Operator* op);
void ccautoesltcl(Operator* op, bool exp, bool log, bool div);
void cccudabody(Operator *op);
void cccudawrapper(Operator *op);
//void ccgappabody(Operator *op, bool OO = false, int fixed_bits=100000);
void ccgappabody (Operator *op, bool OO, bool uncertain, int fixed_bits, int uncertain_perc_exp)

bool ccCheckRanges(Operator *op);

void ccgappadfgprocrun(ofstream *fout, string name, OperatorBehavioral *bop, string type, string precision, int *if_nb,string* exp = NULL);


void ccdfgprocrun(ofstream *fout, string name, Operator *op,int debug_logic=0);
void ccdfgconstruct(ofstream *fout,string name, Operator *op);
void computeASAPOrdering(BlockDFG* dfg, node_list* arranged_list, node_array<int>* depths);
string nodetostring(node n, Tree* t, int nodenum, list<string>* list_input = NULL, bool LHS = false);
string nodetovarstring(node n, Tree* t);
string nodetofnstring(node n, Tree* t);
string nodetofout(BlockDFG* dfg, node src, node_array<int> nodenums); // simplify node name generation for all types of operations
