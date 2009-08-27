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
// SCORE TDF compiler:  get info on memory in use (linux version)
// $Revision: 1.81 $
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <string.h>
#include <LEDA/core/string.h>

using std::ifstream;
using leda::string;

////////////////////////////////////////////////////////////////
//  memory_in_use_proc
//   - returns memory usage (kB) for this process, 0 on error

#define VSIZE_POS 23

size_t memory_in_use_proc ()
{
#ifdef __linux__
  pid_t pid=getpid();
  string procName("/proc/%ld/stat",(long)pid);
  ifstream procStream((const char*)procName);
  int fieldNum;
  char fieldBuf[256];
  for (fieldNum=1; fieldNum<=VSIZE_POS && procStream; fieldNum++)
    procStream >> fieldBuf;
  if (!procStream)
    return 0;
  else
    return atoll(fieldBuf)>>10;		// convert bytes to kB

#else	// #ifdef __linux__
  return 0;

#endif
}


////////////////////////////////////////////////////////////////
//  memory_in_use_system
//   - returns memory usage (kB) for system, 0 on error

#define MEMINFO  "/proc/meminfo"
#define MEMTOTAL  "MemTotal:"
#define MEMFREE   "MemFree:"
#define SWAPTOTAL "SwapTotal:"
#define SWAPFREE  "SwapFree:"
#define KB        "kB"

size_t memory_in_use_system ()
{
#ifdef __linux__
  FILE *fd=fopen(MEMINFO,"r");
  
  long long mtot=0;
  long long mfree=0;
  long long stot=0;
  long long sfree=0;

  char line[256];
  while (!feof(fd))
    {
      // note: not dealing with lines over 256 chars...
      // char *res=fgets(line,256,fd);
      fgets(line,256,fd);
      string sline=string(line);
      int kpos=sline.pos(KB);
      if (kpos>-1)
	{
	  int spos;
	  spos=sline.pos(MEMTOTAL);
	  if (spos>-1)
	    {
	      string cnum=sline(spos+strlen(MEMTOTAL),kpos-1);
	      mtot=atoll(cnum);
	    }
	  else
	    {
	      spos=sline.pos(MEMFREE);
	      if (spos>-1)
		{
		  string cnum=sline(spos+strlen(MEMFREE),kpos-1);
		  mfree=atoll(cnum);
		}
	      else
		{
		  spos=sline.pos(SWAPTOTAL);
		  if (spos>-1)
		    {
		      string cnum=sline(spos+strlen(SWAPTOTAL),kpos-1);
		      stot=atoll(cnum);
		    }
		  else
		    {
		      spos=sline.pos(SWAPFREE);
		      if (spos>-1)
			{
			  string cnum=sline(spos+strlen(SWAPFREE),kpos-1);
			  sfree=atoll(cnum);
			}
		    }
		}
	    }
	  
	}

    }
  fclose(fd);

  long long total_use=(mtot-mfree) + (stot-sfree) ;
  /*
  cerr << "mem=" << mtot << "-" << mfree << "=" << (mtot-mfree) 
       << " swap=" << stot << "-" << sfree << "=" << (stot-sfree) 
       << " totaluse=" << total_use << endl;
       */
  return(total_use);
#else
  return(0);
#endif

}







