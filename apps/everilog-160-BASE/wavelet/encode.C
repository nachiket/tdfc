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
// $Revision: 1.4 $
//
//////////////////////////////////////////////////////////////////////////////

#include "Score.h"
#include "compWaveHuff.h"
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <pthread.h>
#include <stdlib.h>

#include <unistd.h>

int hTab[256] = {
  // first sixteen bits represent data,
  // last 4 bits represent length
        3,      100,    149,    230,    423,    375,    744,    1784,
        4649,   7929,   7850,   10650,  8442,   32667,  8090,   67,
        20,     485,    166,    39,     919,    1064,   120,    5673,
        6889,   2169,   7210,   1401,   19499,  409,    51,     84,
        293,    213,    501,    934,    982,    886,    679,    1767,
        471,    1655,   631,    2600,   1704,   1448,   3496,   1944,
        1496,   3448,   1272,   553,    2296,   7721,   3545,   5241,
        3833,   2809,   3321,   11818,  12698,  6554,   7290,   1578,
        2458,   12186,  11307,  16042,  3754,   6266,   1146,   15098,
        15403,  15482,  14458,  250,    9338,   26155,  20011,  27691,
        3995,   3627,   20731,  2795,   3116,   35884,  16283,  19579,
        25339,  20379,  3195,   763,    28332,  32220,  31788,  13691,
        11948,  11387,  21884,  17148,  4347,   130525, 42541,  12540,
        6907,   53661,  54013,  37276,  130093, 61100,  24028,  60141,
        64989,  47516,  60540,  15837,  40188,  108077, 20893,  86781,
        19181,  81373,  14748,  63900,  105949, 78589,  5499,   28925,
        10989,  40413,  27772,  51950,  7420,   27374,  9773,   13053,
        4509,   215790, 31997,  8956,   97533,  75309,  187134, 70045,
        114141, 4860,   7646,   73181,  48605,  31132,  174830, 109293,
        94461,  64557,  44716,  56796,  30075,  54653,  48381,  117485,
        258302, 107263, 187646, 195838, 241918, 113917, 212222, 125678,
        373503, 92910,  392447, 89343,  217502, 238334, 120189, 242430,
        246526, 482559, 180990, 154366, 253182, 76526,  119198, 326911,
        234238, 119549, 94973,  285439, 160510, 158446, 193278, 299775,
        138718, 318207, 168702, 261374, 23295,  183022, 110847, 81151,
        56575,  318719, 192766, 365311, 389375, 389887, 350975, 45822,
        372991, 86430,  252670, 324351, 312063, 291583, 377599, 384255,
        343295, 383743, 250270, 176382, 122111, 256750, 64767,  88831,
        21245,  49919,  258814, 220415, 43758,  207598, 351487, 41726,
        61694,  223982, 369407, 111359, 84718,  121599, 176894, 127231,
        219902, 130303, 115455, 127743, 23806,  103167, 146686, 172798,
        154878, 29439,  45310,  62207,  15614,  37631,  1431,   56063
};

SIGNED_SCORE_STREAM p;
UNSIGNED_SCORE_STREAM a,bb,c,d,ee,f,g;
UNSIGNED_SCORE_SEGMENT hTable;

ifstream *inFile = 0;
ofstream outdcFile("dcout.raw",ios::out);
ofstream outac1File("ac1out.raw",ios::out);
ofstream outac2File("ac2out.raw",ios::out);
ofstream outac3File("ac3out.raw",ios::out);
ofstream outac4File("ac4out.raw",ios::out);
ofstream outac5File("ac5out.raw",ios::out);
ofstream outac6File("ac6out.raw",ios::out);

int outerIte = 512;

int main(int argc, char *argv[])
{
  char *inFileName = 0;

  if (argc != 1) {
    inFileName = argv[1];
  } else {
    inFileName = "/project/cs/brass/a/jyehtmp/database/raw/lena.raw";
  }

  assert(inFileName);

  inFile = new ifstream(inFileName, ios::in);

  if (!(*inFile)) {
    cerr << "could not open requested file\n";
    exit(1);
  }

  score_init();	

  p = NEW_WRITE_SIGNED_SCORE_STREAM(9);
  a = NEW_READ_UNSIGNED_SCORE_STREAM(5);
  bb = NEW_READ_UNSIGNED_SCORE_STREAM(8);
  c = NEW_READ_UNSIGNED_SCORE_STREAM(8);
  d = NEW_READ_UNSIGNED_SCORE_STREAM(8);
  ee = NEW_READ_UNSIGNED_SCORE_STREAM(8);
  f = NEW_READ_UNSIGNED_SCORE_STREAM(8);
  g = NEW_READ_UNSIGNED_SCORE_STREAM(8);  

  hTable = NEW_UNSIGNED_SCORE_SEGMENT(256,20);
  
  cout << "STREAMID for p is " << p->streamID << endl;
  cout << "STREAMID for a is " << a->streamID << endl;
  cout << "STREAMID for bb is " << bb->streamID << endl;
  cout << "STREAMID for c is " << c->streamID << endl;
  cout << "STREAMID for d is " << d->streamID << endl;
  cout << "STREAMID for ee is " << ee->streamID << endl;
  cout << "STREAMID for f is " << f->streamID << endl;
  cout << "STREAMID for g is " << g->streamID << endl;

  unsigned long long *copyPtr = (unsigned long long *) hTable->data();

  for (int index = 0; index < 256; index++) {
    copyPtr[index] = (unsigned long long) hTab[index];
  }

  new compWaveHuff(512, outerIte, 255, 30, p, a, bb, c, d, ee, f, g, 
	           hTable,hTable,hTable,hTable,hTable,hTable);
//		   hTable);


  cout << "################## producer ***" << endl;
  for (int index = 0; index < 512*outerIte; index++) {
    //cout << "encodeHV: before write " << index << endl;
    unsigned char ch;
    inFile->get(ch);
    STREAM_WRITE(p,(int) ch);
  }
  STREAM_CLOSE(p);

  cout << "################## consumer_dc ***" << endl;
  int res;
  for (int index = 0; index < 8*outerIte; index++) {
    // cout << "consumer_dc(): before read " << index << endl;
    STREAM_READ(a,res);
    // putInt(&outdcFile, res);
    outdcFile.put((unsigned char) res);
//    cout << "consumer_dc(): after read " << index << endl;
  }
	
  STREAM_FREE(a); outdcFile << flush;

  cout << "################## consumer_ac1 ***" << endl;
  int index=0;
  while (1) {
    if (STREAM_EOS(bb)) {break;}
//    cout << "consumer_ac1(): before read " << index << endl;
    STREAM_READ(bb,res);
    // putInt(&outac1File, res);
    outac1File.put((unsigned char) res);
//    cout << "consumer_ac1(): after read " << index << endl;
    index++;
  }
  STREAM_FREE(bb); outac1File << flush;

  cout << "################## consumer_ac2 ***" << endl;
  index=0;
  while (1) {
    if (STREAM_EOS(c)) {break;}
//    cout << "consumer_ac2(): before read " << index << endl;
    STREAM_READ(c,res);
    // putInt(&outac2File, res);
    outac2File.put((unsigned char) res);
//    cout << "consumer_ac2(): after read " << index << endl;
    index++;
  }
  STREAM_FREE(c); outac2File << flush;

  cout << "################## consumer_ac3 ***" << endl;
  index=0;
  while (1) {
    if (STREAM_EOS(d)) {break;}
//    cout << "consumer_ac3(): before read " << index << endl;
    STREAM_READ(d,res);
    // putInt(&outac3File, res);
    outac3File.put((unsigned char) res);
//    cout << "consumer_ac3(): after read " << index << endl;
    index++;
  }
  STREAM_FREE(d); outac3File << flush;

  cout << "################## consumer_ac4 ***" << endl;
  index=0;
  while (1) {
    if (STREAM_EOS(ee)) {break;}
//    cout << "consumer_ac4(): before read " << index << endl;
    STREAM_READ(ee,res);
    // putInt(&outac4File, res);
    outac4File.put((unsigned char) res);
//    cout << "consumer_ac4(): after read " << index << endl;
    index++;
  }
  STREAM_FREE(ee); outac4File << flush;

  cout << "################## consumer_ac5 ***" << endl;
  index=0;
  while (1) {
    if (STREAM_EOS(f)) {break;}
//    cout << "consumer_ac5(): before read " << index << endl;
    STREAM_READ(f,res);
    // putInt(&outac5File, res);
    outac5File.put((unsigned char) res);
//    cout << "consumer_ac5(): after read " << index << endl;
    index++;
  }
  STREAM_FREE(f); outac5File << flush;

  cout << "################## consumer_ac6 ***" << endl;
  index=0;
  while (1) {
    if (STREAM_EOS(g)) {break;}
//    cout << "consumer_ac6(): before read " << index << endl;
    STREAM_READ(g,res);
    // putInt(&outac6File, res);
    outac6File.put((unsigned char) res);
//    cout << "consumer_ac6(): after read " << index << endl;
    index++;
  }
  STREAM_FREE(g); outac6File << flush;

  cout << "encodeHV: after closing input" << endl;

  score_exit();
  
  return(0);

}
