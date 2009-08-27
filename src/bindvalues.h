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
// SCORE TDF compiler:  bind parameter values
// $Revision: 1.125 $
//
//////////////////////////////////////////////////////////////////////////////

#include "feedback.h"
#include "operator.h"
#include "expr.h"

// TEMPORARY until revamp bindvalues to use map2 and resolve all this
Expr *EvaluateGetWidth(Expr *expr);

// note: this probably becomes local as well
Expr *EvaluateExpr(Expr *orig); 

// - bind + propagate actuals of calls into called operators
void set_values (ExprCall *call, bool bindSegOps=false);
void set_values (Operator *op,   bool bindSegOps=false);  // do all calls in op

// - bind + propagate feedback record values into op
void bindvalues(Operator *op, FeedbackRecord *rec);

// - get bound value of array, chasing array param up call stack if necessary
ExprArray* chaseArrayValue (Symbol *sym);
