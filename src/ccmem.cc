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
// SCORE TDF compiler:  generate C++ output
// $Revision: 1.89 $
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include "misc.h"
#include "operator.h"
#include "cctypes.h"

string ccMemoryOperatorName(Operator *op)
{
  if (op->getOpKind()!=OP_BUILTIN)
    fatal(-1,"ccMemoryName given non-builtin operator!",op->getToken());

  OperatorBuiltin *bop=(OperatorBuiltin *)op;
  if (bop->getBuiltinKind()!=BUILTIN_SEGMENT)
    fatal(-1,"ccMemoryName given non-segment builtin operator!",
          op->getToken());

  OperatorSegment *sop=(OperatorSegment *)bop;
  switch (sop->getSegmentKind())
    {
    case(SEGMENT_R):
      return(TYPE_SCORE_SEGMENT_OPERATOR_RANDOM_READ);
    case(SEGMENT_W):
      return(TYPE_SCORE_SEGMENT_OPERATOR_RANDOM_WRITE);
    case(SEGMENT_RW):
      return(TYPE_SCORE_SEGMENT_OPERATOR_RANDOM_READ_WRITE);
    case(SEGMENT_SEQ_R):
      return(TYPE_SCORE_SEGMENT_OPERATOR_SEQUENTIAL_READ);
    case(SEGMENT_SEQ_CYCLIC_R):
      return(TYPE_SCORE_SEGMENT_OPERATOR_SEQUENTIAL_CYCLIC_READ);
    case(SEGMENT_SEQ_W):
      return(TYPE_SCORE_SEGMENT_OPERATOR_SEQUENTIAL_WRITE);
    case(SEGMENT_SEQ_RW):
      return(TYPE_SCORE_SEGMENT_OPERATOR_SEQUENTIAL_READ_WRITE);
    default:
      {
        fatal(-1,string("ccMemoryOperatorName encountered unknown segment kind [%d]",
                        (int)sop->getSegmentKind()),
              sop->getToken());
        return(string("ccMemoryOperatorName_FUBAR"));
      }
    }
}


string ccMemoryName(Operator *op)
{
  if (op->getOpKind()!=OP_BUILTIN)
    fatal(-1,"ccMemoryName given non-builtin operator!",op->getToken());

  OperatorBuiltin *bop=(OperatorBuiltin *)op;
  if (bop->getBuiltinKind()!=BUILTIN_SEGMENT)
    fatal(-1,"ccMemoryName given non-segment builtin operator!",
	  op->getToken());

  OperatorSegment *sop=(OperatorSegment *)bop;
  switch (sop->getSegmentKind())
    {
    case(SEGMENT_R):
      return(TYPE_SCORE_SEGMENT_RANDOM_READ);
    case(SEGMENT_W):
      return(TYPE_SCORE_SEGMENT_RANDOM_WRITE);
    case(SEGMENT_RW):
      return(TYPE_SCORE_SEGMENT_RANDOM_READ_WRITE);
    case(SEGMENT_SEQ_R):
      return(TYPE_SCORE_SEGMENT_SEQUENTIAL_READ);
    case(SEGMENT_SEQ_W):
      return(TYPE_SCORE_SEGMENT_SEQUENTIAL_WRITE);
    case(SEGMENT_SEQ_RW):
      return(TYPE_SCORE_SEGMENT_SEQUENTIAL_READ_WRITE);
    default:
      {
	fatal(-1,string("ccMemoryName encountered unknown segment kind [%d]",
			(int)sop->getSegmentKind()),
	      sop->getToken());
	return(string("ccMemoryName_FUBAR"));
      }
    }
}
  
