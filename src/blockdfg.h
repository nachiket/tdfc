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
// SCORE TDF compiler:  DFG for TDF states / stmt-blocks
// $Revision: 1.2 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_BLOCKDFG_
#define _TDF_BLOCKDFG_

class Expr;
class ExprLValue;
class Stmt;
class StmtAssign;
class Operator;
class OperatorCompose;
class OperatorBehavioral;
class SymbolVar;
class Symbol;
#include <LEDA/graph/graph.h>
#include <LEDA/core/list.h>

using leda::GRAPH;
using leda::node;
using leda::list;
using leda::node_array;
using leda::string;

////////////////////////////////////////////////////////////////
//  BlockDFG type

typedef GRAPH<Expr*,StmtAssign*> BlockDFG;
class BlockDfgInfo;


// BlockDFG is a data-flow graph (DAG) for a state-case statement block.
//
// Nodes:  represent computation and/or primary I/O to block;
//         annotated by Expr* from AST
//
//	    - PI:		ExprLValue*
//				  (Symbol reference for a register or stream
//				   defined outside this stmt block;
//				   ignore bit subscript; shared for fanout;
//				   ExprLValue* is from RHS of 1st asst that
//				   uses the sym in this block)
//	    - PO:		ExprLValue*
//				  (Symbol reference for a register or stream
//				   defined in this stmt block;
//				   ignore bit subscript; single fanin;
//				   ExprLValue* is from lvalue of last,
//				   live asst in this block; ignore dead assts)
//	    - branch-cond PO:	NULL
//				  (dummy output node for branch condition of
//				   'if (c) goto x else goto y';
//				   single fanin from c)
//	    - bit select:	ExprBitSel*
//				  (bit selection deduced from ExprLValue;
//				   single fanin from PO or computation node;
//				   single fanout, not shared;
//				   ExprBitSel* is new, not from AST)
//	    - computation:	ExprValue*   (fanins: none)
//				ExprUop*     (fanins: 1 arg)
//				ExprBop*     (fanins: left, right)
//				ExprCond*    (fanins: cond, thenPart, elsePart)
//				ExprCast*    (fanins: 1 arg being converted)
//				ExprBuiltin* [ bitsof, cat ]
//					     (fanins: arg list)
//				  (computational node;
//				   fanin from argument exprs,
//				   with in-edge order matching arg order;
//				   fanout to all uses and possibly to PO)
// 	    - (ignored):	StmtBuiltin* [ close, done, printf ]
//				StmtGoto*
//				StmtIf*
//
// Edges:  represent data-flow  (implicit or thru reg);
//	   annotated by StmtAssign* if thru reg
//
//	    - implicit:		NULL
//				  (e.g. "1+2" has edge 1--(NULL)-->+ )
//	    - explicit def:	StmtAssign*
//				  (data-flow thru locally defined reg var;
//				   edge  def--(asst)-->use  for asst "sym=def"
//				   means lvalue of asst (sym) is rvalue in use;
//				   interestingly, this means that fanin edge
//				   of a PO is NOT annotated by the asst that
//				   writes to the PO var,
//				   but it may be annotated by an asst that
//				   defines/copies the value being written;
//				   StmtAssign* may be shared by multiple
//				   edges that fanout from "def")
//
// Notes:
//   - ExprLValue PI/PO nodes are created for:
//       * PI: all used    regs/streams
//       * PO: all written regs that are live at block exit,
//             all written streams
//       * PO: branch condition for "if () goto else goto"  (NULL ExprLValue*)
//   - In AST basic block form, the only if's are at end for goto,
//       so each reg has at most 1 live def at any point in bb
//   - PI ExprLValue* comes from 1 (of possibly many) asst rvalues;
//       PI fanout edges do not need asst
//   - PO ExprLValue* comes from 1 (of possibly many) asst lvalues,
//       namely from the last def that is live at block exit
//   - Assignments induce:
//       (i)   dfg tree for RHS expr
//       (ii)  fanouts of (i) annotated with asst
//               (if sym is subsequently used with bit-select,
//                then def-->bitsel is annotated with asst, bitsel->use is not)
//       (iii) possibly 1 fanout to PO node
//
////////////////////////////////////////////////////////////////

// extern void createBlockDfg (BlockDFG *dfg, list<Stmt*> stmtlist);

extern void createBlockDfgSimple (BlockDFG *dfg, list<Stmt*> *stmts);

extern string printBlockDFG (BlockDFG *dfg,
			     node_array<int> *areas=NULL,	// "A=..."
			     node_array<int> *latencies=NULL,	// "L=..."
			     node_array<int> *depths=NULL);	// "D=..."

extern Operator* timing_instance (Operator *iop,
				  list<OperatorBehavioral*> *page_ops,
				  int cycleTime);

extern void importDfg(BlockDFG *destdfg, BlockDFG srcdfg, node destnode, ExprLValue* lval, Expr* ec, Symbol* destsym, BlockDfgInfo *dfgi, Stmt* t);


#endif	// #ifndef _TDF_BLOCKDFG_

