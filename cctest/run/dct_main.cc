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
#include "faan_dct.h"
#include <iostream.h>

void main()
{

   SIGNED_SCORE_STREAM p[8];
   SIGNED_SCORE_STREAM q[8];
   int len=4;
   int res[8];
   int expect[8][4]={{1287,1732,1171,1191},{-517,  -4,-106,-219},
		       { -15, -19,  61,  74},{  61,   5, -23, -20},
		       {  -7,  -2,  -9,-121},{  -7,   3,   1, -14},
		      {  -9,  -3, -11,  12},{  -1,   0,   4,   1}};

   int in[8][4]={{0x63,0xd6,0x88,0x71},{0x6d,0xd8,0x8e,0x91},
		   {0x72,0xd8,0x8b,0x97},{0x90,0xdb,0x89,0x75},
		   {0xb6,0xdb,0x8d,0x81},{0xcd,0xd9,0x8f,0xb0},
		   {0xdb,0xda,0xa6,0xb8},{0xd7,0xd5,0xa7,0xb0}};
   
   int errors=0;

   for (int index=0; index<8; index++) {
     p[index]=NEW_SIGNED_SCORE_STREAM(9);
     q[index]=NEW_SIGNED_SCORE_STREAM(9);
   }

   new faan_dct(9, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
		q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7]);

   for (int index=0;index<len;index++)  
     {
       cout << "dct_main: before write " << index << endl;
       for (int subindex=0; subindex<8; subindex++) 
	 STREAM_WRITE(p[subindex],in[subindex][index]);
       cout << "dct_main: after write " << index << endl;
     }
    
   for (int index=0; index<len; index++)
     {
       cout << "dct_main: before read " << index << endl;
       for (int subindex=0; subindex<8; subindex++) 
	 res[subindex]=STREAM_READ(q[subindex]);  
       cout << "dct_main: after read " << index << endl;
       for (int subindex=0; subindex<8; subindex++) {
	 if (res[subindex]!=expect[subindex][index])
	   {
	     cout << "dct_main: ERROR" << " got " << res[subindex] 
		  << " expected " << expect[subindex][index] << endl;
	     errors++;
	   }
	 else 
	   {
	     cout << "dct_main: OK got " << res[subindex]  
		  << endl;
	   }
       }
     }
   
   cout << "dct_main: before closing inputs" << endl;
   
   for (int index=0; index<8; index++) 
     STREAM_CLOSE(p[index]);  
   cout << "dct_main: after closing inputs " << endl;

   for (int index=0; index<8; index++) {

     cout << "dct_main: before eos(q[" << index << "]) loop" << endl;
     while (!STREAM_EOS(q[index]))
       {
	 cout << "dct_main: !eos(q[" << index << "]) top of loop" << endl;
	 cout << "dct_main: before read q[" << index 
	      << "] (error loop)" << endl;
	 int tmp=STREAM_READ(q[index]);
	 cout << "dct_main: after read q[" << index 
	      << "] (error loop)" << endl;
	 cout << "dct_main: ERROR" << " got " << tmp 
	      << " expecting eos " << endl;
	 errors++;
	 cout << "dct_main: loopback == before eos(q[" << index <<
	   "]) loop" << endl;
       }
   }
     
   cout << "Done. errors=" << errors << endl;

   for (int index=0; index<8; index++)
     STREAM_FREE(q[index]);  
   
   return(0);

}
