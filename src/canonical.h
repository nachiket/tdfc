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
// SCORE TDF compiler:  canonical-form xforms (exlining calls/mem, etc.)
// $Revision: 1.139 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_CANONICAL_
#define _TDF_CANONICAL_


class Tree;

Tree* liftInlines   (Tree *t);	// - lift nested calls/mem-access to stmt level
Tree* basicBlocks   (Tree *t);	// - decompose into basic-block states
Tree* exline	    (Tree **h);	// - exline calls/mem-acecss
Tree* removeDumbStates(Tree *t);// - remove states containing goto w/no inputs
Tree* doCopyOps     (Tree **h);	// - infer fanout and expand copy() calls
Tree* canonicalForm (Tree **h);	// - do all of the above

void  liftOrphanedLocals (Tree *t, OperatorBehavioral *op);
				// - promote orphaned locals into regs
void  checkGotos (Tree *t);	// - consistency check on goto stmts

#endif	// #ifndef _TDF_CANONICAL_
