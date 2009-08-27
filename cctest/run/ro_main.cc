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
// SCORE runtime support (test)
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////

#include "ScoreStream.h"
#include "ScoreSegment.h"
#include "table_trans.h"
#include "check_trans32a.h"
#include "check_trans31b.h"
#include <iostream.h>

int main()
{

  SCORE_STREAM s1a, s2a, s3a, s1b,s2b,s3b;
  s1a=NEW_SIGNED_SCORE_STREAM(10);
  s2a=NEW_SIGNED_SCORE_STREAM(8);
  s3a=NEW_UNSIGNED_SCORE_STREAM(16);
  s1b=NEW_SIGNED_SCORE_STREAM(10);
  s2b=NEW_SIGNED_SCORE_STREAM(8);
  s3b=NEW_UNSIGNED_SCORE_STREAM(16);
  int errors=0;
  int tablen=8;
  int tab[8]={-2,-1,0,1,2,4,8,16};
  int tab2[8]={12,6,3,0,-3,-3,-6,-12};
  ScoreSegment *asegment=new ScoreSegment(8,8);// 8 elements, 8b wide

  int *atable=(int *)asegment->data();
  for (int i=0;i<tablen;i++)
    atable[i]=tab[i];

  if (DEBUG) {
    for (int i=0;i<tablen;i++) {
      cout << "data in atable entry " << i << " is " << atable[i] << endl;
    }
  }

  new table_trans(3,2,asegment,s1a,s2a);
  new check_trans32a(s1a,s2a,s3a);    
  // should interlock on table_trans completion and free of segment
  if (DEBUG) {
    cout << "ro_main: before table rewrite " << endl;
  }
  for (int i=0;i<tablen;i++)
    atable[i]=tab2[i];
  if (DEBUG) {
    cout << "ro_main: after table rewrite " << endl;
  }
  new table_trans(3,1,asegment,s1b,s2b);
  new check_trans31b(s1b,s2b,s3b);    

  if (DEBUG) {
    cout << "ro_main: get errors " << endl;
  }

  errors+=STREAM_READ(s3a);
  errors+=STREAM_READ(s3b);
  STREAM_FREE(s3a);
  STREAM_FREE(s3b);

  cout << "Done. errors=" << errors << endl;

  delete(asegment);

  return(0);

}
