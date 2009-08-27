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
#include "merge.h"
#include <iostream.h>

void main()
{

   SIGNED_SCORE_STREAM s1, s2, s3;
   int olen=14;
   int res[14];
   int expect[14]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,
		   0x0B,0x0C,0x0F,0x10};
   int len1=10;
   int in1[10]={0x01,0x02,0x04,0x05,0x08,0x0A,0x0B,0x0C,0x0F,0x10};
   int len2=4; 
   int in2[4]={0x03,0x06,0x07,0x09};
   int i;
   int errors=0;

   s1=NEW_SIGNED_SCORE_STREAM(16);
   s2=NEW_SIGNED_SCORE_STREAM(16);
   s3=merge(16,s1,s2);
   for (int i=0;i<len1;i++)
     STREAM_WRITE(s1,in1[i]);
   STREAM_CLOSE(s1);
   for (int i=0;i<len2;i++) 
     STREAM_WRITE(s2,in2[i]);
   STREAM_CLOSE(s2);
   
   for (i=0;i<olen;i++)
     {
       res[i]=STREAM_READ(s3);
       if (res[i]!=expect[i])
	 {
	   cout << "merge0_main: ERROR" << " got " << res[i] << 
	     " expected " << expect[i] << endl;
	   errors++;
	 }
       else 
	 {
	      cout << "merge0_main: OK got " 
		   << res[i]  
		   << endl;
	 }
     }
   

   while (!STREAM_EOS(s3))
     {
       cout << "merge0_main: !eos(s3) top of loop" << endl;
       cout << "merge0_main: before read s3 (error loop)" << endl;
       int tmp=STREAM_READ(s3);
       cout << "merge0_main: after read s3 (error loop)" << endl;
       cout << "merge0_main: ERROR" << " got " << tmp 
	    << " expecting eos " << endl;
       errors++;
       cout << "merge0_main: loopback == before eos(s3) loop" << endl;
     }
   
   cout << "Done. errors=" << errors << endl;

   STREAM_FREE(s3);

   return(0);

}
