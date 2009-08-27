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
#include "xor.h"
#include <iostream.h>

void main()
{

   UNSIGNED_SCORE_STREAM s1, s2, s3;
   int len=4;
   int res[4];
   int expect[4]={0xAA,0x55,0xAA,0x0F};
   int in1[4]={0xFF,0xFF,0x00,0x55};
   int in2[4]={0x55,0xAA,0xAA,0x5A};
   int i;
   int errors=0;

   s1=NEW_UNSIGNED_SCORE_STREAM(8);
   s2=NEW_UNSIGNED_SCORE_STREAM(8);
   s3=xor(8,s1,s2);
   for (int i=0;i<len;i++)
     {
       cout << "onexor_main: before write to s1" << endl;
       STREAM_WRITE(s1,in1[i]);
       cout << "onexor_main: after write to s1" << endl;
     }
   for (int i=0;i<len;i++) 
     {
       cout << "onexor_main: before write to s2" << endl;
       STREAM_WRITE(s2,in2[i]);
       cout << "onexor_main: after write to s2" << endl;
     }
   
   for (i=0;i<len;i++)
     {
       cout << "onexor_main: before read s3 i=" << i << endl;
       res[i]=STREAM_READ(s3);
       cout << "onexor_main: before read s3 i=" << i << endl;
       if (res[i]!=expect[i])
	 {
	   cout << "onexor_main: ERROR" << " got " << res[i] << 
	     " expected " << expect[i] << endl;
	   errors++;
	 }
       else 
	 {
	      cout << "onexor_main: OK got " 
		   << res[i]  
		   << endl;
	 }
     }
   
   cout << "onexor_main: before close s1" << endl;
   STREAM_CLOSE(s1);
   cout << "onexor_main: after close s1" << endl;

   cout << "onexor_main: before close s2" << endl;
   STREAM_CLOSE(s2);
   cout << "onexor_main: after close s2" << endl;


   cout << "onexor_main: before eos(s3) loop" << endl;
   while (!STREAM_EOS(s3))
     {
       cout << "onexor_main: !eos(s3) top of loop" << endl;
       cout << "onexor_main: before read s3 (error loop)" << endl;
       int tmp=STREAM_READ(s3);
       cout << "onexor_main: after read s3 (error loop)" << endl;
       cout << "onexor_main: ERROR" << " got " << tmp 
	    << " expecting eos " << endl;
       errors++;
       cout << "onexor_main: loopback == before eos(s3) loop" << endl;
     }
   
   cout << "Done. errors=" << errors << endl;

   /*   
   delete(s1);
   delete(s2);
   delete(s3);
   */
   STREAM_FREE(s3);

   return(0);

}
