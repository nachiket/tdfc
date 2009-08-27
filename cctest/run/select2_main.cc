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
#include "select2.h"
#include <iostream.h>

int main()
{

   SIGNED_SCORE_STREAM s1, s2,  s5;
   BOOLEAN_SCORE_STREAM s3;
   int len=6;
   int res[6];
   int expect[6]={1,2,4,3,5,6};
   int in1[6]={1,3,5,7,9,11};
   int in2[6]={2,4,6,8,10,12};
   int in3[6]={0,1,1,0,0,1};
   int i;
   int errors=0;

   s1=NEW_SIGNED_SCORE_STREAM(8);
   s2=NEW_SIGNED_SCORE_STREAM(8);
   s3=NEW_BOOLEAN_SCORE_STREAM();
   s5=NEW_SIGNED_SCORE_STREAM(8);
   new select2(8,s1,s2,s3,s5);
   for (int i=0;i<len;i++)
     {
       STREAM_WRITE(s1,in1[i]);
     }
   for (int i=0;i<len;i++)
     {
       STREAM_WRITE(s2,in2[i]);
     }
   for (int i=0;i<len;i++)
     {
       STREAM_WRITE(s3,in3[i]);
     }
   
   for (i=0;i<len;i++)
     {
       res[i]=STREAM_READ(s5);
       if (res[i]!=expect[i])
	 {
	   cerr << "select2_main: " << "ERROR" << " got " << res[i] << 
	     " expected " << expect[i] << endl;
	   errors++;
	 }
       else 
	 {
	      cout << "select2_main: " 
		   << "OK got " 
		   << res[i]  
		   << endl;
	 }
     }
   
   STREAM_CLOSE(s1);
   cout << "Closed input Stream 1" << endl;
   STREAM_CLOSE(s3);
   cout << "Closed input Stream 3" << endl;
   STREAM_CLOSE(s2);
   cout << "Closed input Stream 2" << endl;

   while (!STREAM_EOS(s5))
     {
       int tmp=STREAM_READ(s5);
       cerr << "ERROR" << " got " << tmp 
	    << " expecting eos " << endl;
       errors++;
     }
   
   cout << "Done. errors=" << errors << endl;


   return(0);

}
