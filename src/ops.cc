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
// SCORE TDF compiler:  basic binary/unary operators
// $Revision: 1.30 $
//
//////////////////////////////////////////////////////////////////////////////


#include <LEDA/core/string.h>
#include "ops.h"


string opToString (int op)
{
  switch (op)
  {
    case EQUALS:	return string("==");
    case GTE:		return string(">=");
    case LEFT_SHIFT:	return string("<<");
    case LOGIC_AND:	return string("&&");
    case LOGIC_OR:	return string("||");
    case LTE:		return string("<=");
    case NOT_EQUALS:	return string("!=");
    case RIGHT_SHIFT:	return string(">>");
    case EXP:		return string("exp");
    case LOG:		return string("log");
    case SQRT:		return string("sqrt");
    default:		if (op=='%')	return string('%');
			else		return string("%c",op);
  }
}
