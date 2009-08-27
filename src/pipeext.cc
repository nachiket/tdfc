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
// SCORE TDF compiler:  pipeline extraction
// $Revision: 1.2 $
//
//////////////////////////////////////////////////////////////////////////////


#include "state.h"
#include "operator.h"
#include "annotes.h"
#include "pipeext.h"


bool isTrivialPipeline (OperatorBehavioral *op)
{
  // - return true if *op is a trivial pipeline operator, i.e.:
  //     *op has 1 state (with 1 state-case) and no register writes
  // - this definition of trivial pipeline is conservative,
  //     since op may use registers that do not carry state-to-state
  //     dataflow (i.e. privatizable regs that should be locals);
  //     should call this function after register privatization.

  // - assume that we are called after bindvalues(), so that
  //     written vars have been found and annotated ANNOTE_IS_WRITTEN

  if (op->getStates()->size()>1)
    return false;
  State *start = op->getStartState();
  if (!start)
    return true;		// - no states --> trivial pipeline
  else
    if (start->getCases()->size()>1)
      return false;

  Symbol *var;
  forall (var, *op->getVars()->getSymbols())
    if (var->getAnnote(ANNOTE_IS_WRITTEN))
      return false;

  return true;
}
