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
#include "compDecoder.h"
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <pthread.h>

#include <unistd.h>


SIGNED_SCORE_STREAM p;
UNSIGNED_SCORE_STREAM a,b,c,d,e,f,g;
UNSIGNED_SCORE_SEGMENT hTable;

ofstream outFile("newout.pgm",ios::out );
ifstream inFile("dectable.out",ios::in);
ifstream indcFile("dcout.raw",ios::in);
ifstream inac1File("ac1out.raw",ios::in);
ifstream inac2File("ac2out.raw",ios::in);
ifstream inac3File("ac3out.raw",ios::in);
ifstream inac4File("ac4out.raw",ios::in);
ifstream inac5File("ac5out.raw",ios::in);
ifstream inac6File("ac6out.raw",ios::in);

int main(int argc, char *argv[])
{
  unsigned char ch;

  score_init();

  p = NEW_READ_SIGNED_SCORE_STREAM(9);
  a = NEW_WRITE_UNSIGNED_SCORE_STREAM(5);
  b = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);
  c = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);
  d = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);
  e = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);
  f = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);
  g = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);  

  hTable = NEW_UNSIGNED_SCORE_SEGMENT(65536,8);

  outFile << "P5" << endl;
  outFile << "512 512" << endl;
  outFile << "255" << endl;

  unsigned long long *copyPtr = (unsigned long long *) hTable->data();

  for (int index = 0; index < 65536; index++) {
    inFile.get(ch);
    copyPtr[index] = (unsigned long long) ch;
  }

  new compDecoder(512, 512, 255, 30, p, a, b, c, d, e, f, g, 
		   hTable,hTable,hTable,hTable,hTable,hTable);

  for (int index = 0; index < 4096; index++) {
    indcFile.get(ch);
    STREAM_WRITE(a,(int) ch);
  }

  while (!inac1File.eof()) {
    inac1File.get(ch);
    STREAM_WRITE(b,(int) ch);
  }
  while (!inac2File.eof()) {inac2File.get(ch);STREAM_WRITE(c,(int) ch);}
  while (!inac3File.eof()) {inac3File.get(ch);STREAM_WRITE(d,(int) ch);}
  while (!inac4File.eof()) {inac4File.get(ch);STREAM_WRITE(e,(int) ch);}
  while (!inac5File.eof()) {inac5File.get(ch);STREAM_WRITE(f,(int) ch);}
  while (!inac6File.eof()) {inac6File.get(ch);STREAM_WRITE(g,(int) ch);}

  printf("Done writing to all the streams\n");

  for (int index = 0; index < (512*512); index++) {
    int token;
    STREAM_READ(p,token);
    outFile.put((unsigned char) token);
  }

  STREAM_CLOSE(a); STREAM_CLOSE(b); STREAM_CLOSE(c); STREAM_CLOSE(d);
  STREAM_CLOSE(e); STREAM_CLOSE(f); STREAM_CLOSE(g);

  STREAM_FREE(p);
  score_exit();
 
  return(0);

}
