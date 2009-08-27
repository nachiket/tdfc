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
// SCORE TDF compiler:  tree annotation ids
// $Revision: 1.141 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_ANNOTES_
#define _TDF_ANNOTES_


enum AnnoteID
{
  // - CC back-end
  CC_STREAM_ID =0,	// =0 for backward compatibility with ccannote.h
  CC_CALL_SET,
  CC_SYM_NOT_SET,
  CC_PAGES,
  CC_SEGMENTS,

  // - ccinstance
  CC_FORMAL,		// hmm, I thought formal and environment were used by
			//  instance not ccinstance -- amd 10/21/99
  CC_ENVIRONMENT,
  CC_NULL_ENVIRONMENT,
  CC_ALLPRODUCERS,	// for storing the producer mask for each state
			// case when we run allProducers() so that we do
			// not duplicate work. -- mmchu 11/9/99
			// Annote is ptr to 64-bit ARGMASK_TYPE - eylon 1/20/01

  // - ccbody + ccinstance
  MAX_RETIME_DEPTH,

  // - instance (new version in the works)
  // CC_COPY_SRC,
  // CC_COPY_SINK,
  // CC_ARGTMP,
  ANNOTE_IS_READ,
  ANNOTE_IS_WRITTEN,
  ANNOTE_VALUE_PASSED,

  // - canonical xform:  lift-inlines
  ANNOTE_LIFT_SYMTAB,
  ANNOTE_LIFT_PRE_STMTS,
  ANNOTE_LIFT_POST_STMTS,

  // - canonical xform:  convert memory arrays to segment ops
  ANNOTE_ARRAY_READ,
  ANNOTE_ARRAY_WRITTEN,
  ANNOTE_ARRAY_NEWFORMAL,
  ANNOTE_ARRAY_SEG,

  // - operator decomposition:  dismantle shared register/stream access
  ANNOTE_IS_SIMPLE_REG,		// True for register symbols
				//   that do feed-forward dataflow w/o hazards
  ANNOTE_WAS_SINGLE_STATE_OP,	// True for ops derived from single-state ops

  // - misc
  ANNOTE_PRINTF_STRING_TOKEN,	// Token* for printf format string

  // -IR
  TO_NEWREP_NODE,
  TO_NEWREP_IMAGE_NODE,
  TO_NEWREP_IMAGE_EDGE,
  TO_BEH_OP_INSTANCE,
  TO_OUT_PIPE,
  TO_IN_PIPE,

  // - dummy last
  ANNOTE_LAST
};


#endif	// #ifndef _TDF_ANNOTES_
