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
// $Revision: 1.15 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_OPS_
#define _TDF_OPS_

#include "parse.h"


static inline
bool isLogicOp (int op)
{
  return (   op==LOGIC_AND
	  || op==LOGIC_OR );
}


static inline
bool isCmpOp (int op)
{
  return (   op==EQUALS
	  || op==NOT_EQUALS
	  || op==GTE
	  || op==LTE
	  || op=='<'
	  || op=='>' );
}


static inline
bool isBoolOp (int op)
{
  return isLogicOp(op) || isCmpOp(op) || op=='!';
}


static inline
bool isUop (int op)		// op _may_ be unary (+- uncertain)
{
  return (   op=='!'
	  || op=='~'
	  || op=='+'
	  || op=='-' );
}


static inline
bool isBop (int op)		// op _may_ be binary (+- uncertain)
{
  return (   op>=0
	  && op!='!'
	  && op!='~' );
}


static inline
bool isProdOp (int op)
{
  return (   op!='*'
	  && op!='/'
	  && op!='%' );
}


extern string opToString (int op);
extern string opToNodename (int op);


#endif  // #ifndef _TDF_OPS_
