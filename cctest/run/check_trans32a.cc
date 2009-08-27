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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "check_trans32a.h"

void *check_trans32a_proc_run(void *obj)
{
  return(((check_trans32a *)obj)->proc_run());
}
check_trans32a::check_trans32a(SCORE_STREAM out0, SCORE_STREAM in0, 
			   SCORE_STREAM out1)
  {

    // always run on processor

    // ScoreGraphNode linkage
 
    declareIO(1,2);
    bindInput(0,in0,new ScoreStreamType(1,10));
    bindOutput(0,out0,new ScoreStreamType(1,8));
    bindOutput(1,out1,new ScoreStreamType(0,16));

    pthread_attr_t *a_thread_attribute=
      (pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,
				PTHREAD_CREATE_DETACHED);

    pthread_create(&rpt,a_thread_attribute,
		   &check_trans32a_proc_run,
		   this);
  }

void *check_trans32a::proc_run()
  {

    int errors=0;
    int len1=10; // up to 200+ once that works
    
    int tab[8]={-2,-1,0,1,2,4,8,16};

    cout << "create address - check_trans32a" << endl;

    // check trans32a
    for (int i=1;i<len1;i++)
      STREAM_WRITE(out[0],i);
    STREAM_CLOSE(out[0]);
    
    for (int i=1;i<len1;i++)
      {
	cout << "inside for loop - check_trans32a iteration: " 
	     << i << endl;
	int expect=tab[(i&0x7)]<<2;
	int res=STREAM_READ(in[0]);
	if (res!=expect)
	  cerr << "ro_main-->check_trans32a ERROR got " << res << " expecting " 
	       << expect << " for " << i << endl;
	else
	  cerr << "ro_main-->check_trans32a OK got " << res << " for " << i << endl;
      }

    cout << "finish the for loop - check_trans32a" << endl;
   
    while (!STREAM_EOS(in[0]))
      {
	int tmp=STREAM_READ(in[0]);
	cerr << "ro_main-->check_trans32a " << "ERROR" << " got " << tmp 
	     << " expecting eos " << endl;
	errors++;
      }
    STREAM_FREE(in[0]);
    STREAM_WRITE(out[1],errors);
    STREAM_CLOSE(out[1]);
    
    return((void *)NULL);
  }


