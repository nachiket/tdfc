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
#include "sigma10.h"
#include <iostream.h>

int main()
{

  int len1=50;
  SCORE_STREAM s1, s2;
  s1=NEW_UNSIGNED_SCORE_STREAM(8);
  s2=NEW_UNSIGNED_SCORE_STREAM(12);
  int errors=0;
  new sigma10(s1,s2);
  for (int i=1;i<len1;i++)
    {
      STREAM_WRITE(s1,i);
      cout << "stream_data(result)=" << STREAM_DATA(s2) << endl;
    }
  cout << "stream_data(result)=" << STREAM_DATA(s2) << " before read " << endl;
  int result=STREAM_READ(s2);
  if (result!=55)
    {
      cerr << "sigma10_main " << "ERROR" << " got " << result << 
	" expected " << 55 << endl;
      errors++;
    }
  else 
    {
      cout << "sigma10_main " << "OK got "  << result << endl;
    }
   
  STREAM_CLOSE(s1);
  cout << "Closed input Stream 1" << endl;

  while (!STREAM_EOS(s2))
    {
      int tmp=STREAM_READ(s2);
      cerr << "sigma10_main " << "ERROR" << " got " << tmp 
	    << " expecting eos " << endl;
       errors++;
    }
  STREAM_FREE(s2);

  cout << "Done. errors=" << errors << endl;

  return(0);

}
