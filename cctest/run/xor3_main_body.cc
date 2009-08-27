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
#include <iostream.h>

int main()
{

   UNSIGNED_SCORE_STREAM s1, s2, s3, s4, s5;
   int len=4;
   int res[4];
   int expect[4]={0xAA,0xAA,0x00,0xFF};
   int in1[4]={0xFF,0xFF,0x00,0x55};
   int in2[4]={0x55,0xAA,0xAA,0x5A};
   int in3[4]={0x00,0xFF,0xAA,0xF0};
   int i;
   int errors=0;

   s1=NEW_UNSIGNED_SCORE_STREAM(8);
   s2=NEW_UNSIGNED_SCORE_STREAM(8);
   s3=NEW_UNSIGNED_SCORE_STREAM(8);
   // s4 is an intermediate stream inside xor3 in this instance
   s5=XOR3_NAME(8,s1,s2,s3);
   for (int i=0;i<len;i++)
     {
       STREAM_WRITE(s1,in1[i]);
       // violating abstraction, don't try this at home...
       cerr << "stream_data(s1)=" << STREAM_DATA(s1) 
	    << " stream_data(s2)=" << STREAM_DATA(s2) 
	    << " stream_data(s3)=" << STREAM_DATA(s3) << endl;
     }
   for (int i=0;i<len;i++)
     {
       STREAM_WRITE(s2,in2[i]);
       // violating abstraction, don't try this at home...
       cerr << "stream_data(s1)=" << STREAM_DATA(s1) 
	    << " stream_data(s2)=" << STREAM_DATA(s2) 
	    << " stream_data(s3)=" << STREAM_DATA(s3) << endl;
     }
   for (int i=0;i<len;i++)
     {
       STREAM_WRITE(s3,in3[i]);
       // violating abstraction, don't try this at home...
       cerr << "stream_data(s1)=" << STREAM_DATA(s1) 
	    << " stream_data(s2)=" << STREAM_DATA(s2) 
	    << " stream_data(s3)=" << STREAM_DATA(s3) << endl;
     }
   
   for (i=0;i<len;i++)
     {
       res[i]=STREAM_READ(s5);
       if (res[i]!=expect[i])
	 {
	   cerr << XOR3_NAME << "_main: " << "ERROR" << " got " 
		<< res[i] << " expected " 
		<< expect[i] << endl;
	   errors++;
	 }
       else 
	 {
	   cout << XOR3_NAME << "_main: "
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

   STREAM_FREE(s5);

   return(0);

}
