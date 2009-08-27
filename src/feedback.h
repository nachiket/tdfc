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
// SCORE TDF compiler:  keep track of feedback records
// $Revision: 1.124 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_FEEDBACK_
#define _TDF_FEEDBACK_

#include <stdio.h>
#include <stdlib.h>
#include <LEDA/core/list.h>
#include <LEDA/core/array.h>
#include "symbol.h"

using leda::array;

extern string	gFeedbackPath;		// path for feedback files

#define SCORE_FEEDBACK_PATH_ENV	"SCORE_FEEDBACK_PATH"
#define DEFAULT_FEEDBACK_PATH	"."

// This type must always match with the one used in the ScoreRuntime
typedef unsigned long long ScoreIOMaskType;
#define ARGMASK_TYPE      ScoreIOMaskType
#define ARGMASK_TYPE_STR "ScoreIOMaskType"
// type for bit-mask representing operator args

class FeedbackRecord
{
public:
  FeedbackRecord(int num_args, ARGMASK_TYPE param_loc, int *nparams);
  int getParam(int which) {return(param[which]);}
  int isParam(int which) {return(is_param[which]);}
  int params() {return(param.size());}
  void merge(FeedbackRecord *);
  string toString();
protected:
  array<int> param;
  array<int> is_param;
};

int count_args(list<Symbol*> *argtypes);
ARGMASK_TYPE parameter_locs(list<Symbol*> *argtypes);

int feedback_order(FeedbackRecord * const &A,  FeedbackRecord * const &B);
list<FeedbackRecord *> *canonicalize_feedback_record_list(list <FeedbackRecord *> *ilist);

void read_feedback(FILE *fd, char *fname,
		   list<FeedbackRecord *>* flist, int args, 
		   ARGMASK_TYPE param_loc, string name);

void initFeedbackPath ();

#endif	// #ifndef _TDF_FEEDBACK_

