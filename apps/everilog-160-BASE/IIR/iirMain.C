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
// $Revision: 1.1.1.1 $
//
//////////////////////////////////////////////////////////////////////////////

#include "Score.h"
#include "iirTop.h"
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>

#define PI 3.1415927

UNSIGNED_SCORE_STREAM s;
SIGNED_SCORE_STREAM p,q;

ofstream outFile("newout",ios::out );
//ifstream inFile(".raw",ios::in);

int main(int argc, char *argv[])
{
  int inToken;
  //int sizes[16] = {3,5,7,5,1,5,5,3,5,3,3,5,7,5,1,5};
  //int sizes[16] = {1,2,3,2,0,2,2,1,2,1,1,2,3,2,0,2};
//  float a,b,c; // single precision for now

  score_init();

  q = NEW_READ_SIGNED_SCORE_STREAM(16);
  p = NEW_WRITE_SIGNED_SCORE_STREAM(16);
  cerr << "Stream p was created " << (unsigned int) p << endl;
  //s = NEW_WRITE_UNSIGNED_SCORE_STREAM(2);

  new iirTop(16,p,q, 8,131,7*16, 8,231,170, 5,32,51,26,
	     6,-47,15, 7,167,62, 7,128,210,89,
	     10,256,131,256, 7,32,-53,32);

  for (int index = 0; index < 80; index++) {    
    for (int subIndex = 0; subIndex < 20; subIndex++) {
      STREAM_WRITE(p,4096);
    }
    for (int subIndex = 0; subIndex < 20; subIndex++) {
      STREAM_WRITE(p,-4096);
    }
  }

  printf("Done writing to all the streams\n");

  STREAM_CLOSE(p);

  for (int index = 0; index < (80*40-16); index++) {
    int token;
    STREAM_READ(q,token);
    outFile.put((unsigned char) ((token>>24)&255));
    outFile.put((unsigned char) ((token>>16)&255));
    outFile.put((unsigned char) ((token>>8)&255));
    outFile.put((unsigned char) (token&255));
  }

  STREAM_FREE(q);
  score_exit();
 
  return(0);

}
