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
// SCORE TDF compiler:  file operations
// $Revision: 1.144 $
//
//////////////////////////////////////////////////////////////////////////////


// - which compiler/version to use for preprocessing:  (default "g++")
#ifndef CXX
#define CXX "g++"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#if defined(__CYGWIN32__) || defined(__APPLE__)
#include <sys/wait.h>
#else
#include <wait.h>
#endif


#include <LEDA/core/string.h>
namespace leda {
	inline int compare(leda::string* const& a, leda::string* const& b) {
		return a==b? 0: a<b? -1:1;
	}
}

#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include <LEDA/core/set.h>
#include "parse.h"
#include "file.h"
#include "suite.h"
#include "misc.h"

static set<string*> gFileNames;		      // copy of file names for tokens
static string	    gNilFileName("<none>");


////////////////////////////////////////////////////////////////
//  preprocessFile
//   - invoke C preprocessor via g++

void preprocessFile (string inFile, string outFile, list<char*> *flags)
{
  string cxx;
  const char* cxxenv = getenv("CXX");		// - Try CXX env variable
  if (cxxenv)
    cxx = which(cxxenv);
  if (cxx.length()==0)				// - Try CXX macro
    cxx = which(CXX);
  if (cxx.length()==0)				// - Give up
    fatal(-1, "cannot find g++ compiler for preprocessing, "
	      "use CXX environment variable to override");

  int id=vfork();
  if (id)
  {
    // - parent waits
    int status;
    wait(&status);
    if (status)
      fatal(-1, string("preprocessing failed for file \"")+inFile+"\"");
  }
  else
  {
    // - child exec's g++/cpp
    // - warning: g++-specific flags (?)
    const char *cppArgv[256];
    int   cppArgc=0;
    cppArgv[cppArgc++]	 = cxx;
    cppArgv[cppArgc++]	 = "-x";
    cppArgv[cppArgc++]	 = "c++";
    cppArgv[cppArgc++]	 = "-E";
    cppArgv[cppArgc++]	 = "-D__TDFC__";
    if (flags)
    {
      char *flag;
      forall (flag,*flags)
	cppArgv[cppArgc++] = flag;
    }
    cppArgv[cppArgc++]	 = "-o";
    cppArgv[cppArgc++]	 = outFile;
    cppArgv[cppArgc++]	 = inFile;
    cppArgv[cppArgc++]	 = NULL;
    execv(cxx, (char*const*)cppArgv);		// moronic cast to satisfy g++
    error(-1,string("failed to launch ")+cxx+" for preprocessing", NULL, true);
    _exit(-1);		// required for vfork(), instead of exit()
  }
}


////////////////////////////////////////////////////////////////
//  ParseFile
//   - parse one source file, saving operators in gSuite

void parseFile (const char *fileName, int debug)
{
  // - open file  (yyin belongs to flex)
  yyin = fopen(fileName,"r");
  if (yyin==NULL)
    fatal(-1, string("Could not open file \"")+fileName+"\"", NULL, true);
  
  // - set globals for bison parser
  yydebug	= debug;
  gLineNum	= 1;
  gFileName	= new string(fileName);
  gFileNames.insert(gFileName);
  gParsing	= true;

  // - parse file
  yyparse();

  // - reset parser globals to denote no active file
  gParsing	= false;
  gLineNum	= 0;
  gFileName	= &gNilFileName;
}


////////////////////////////////////////////////////////////////
//  copyFile
//   - buffered copy between FILEs, returns #bytes copied
//   - does not open/close/verify FILEs

size_t copyFile (FILE *in, FILE *out)
{
  #define BUFFER_SIZE 1024
  size_t totalBytes=0;
  while (!feof(in))
  {
    char buffer[BUFFER_SIZE];
    size_t numBytes = fread(buffer,1,BUFFER_SIZE,in);
    fwrite(buffer,1,numBytes,out);
    totalBytes += numBytes;
  }
  return totalBytes;
}


////////////////////////////////////////////////////////////////
//  which
//   - locates an executable file using the $PATH environment variable
//   - returns full pathname or empty string if not found

string which (const char *progName)
{
  // first, accept fully-qualified paths + paths relative to CWD (working dir)
  if (access(progName,X_OK)==0)
    return string(progName);
  
  char *path     = getenv("PATH"),
       *pathCopy = new char[strlen(path)+1],
       *pathElem;
  strcpy(pathCopy,path);
  for (pathElem=strtok(pathCopy,":"); pathElem; pathElem=strtok(NULL,":"))
  {
    string progPath = string(pathElem)+"/"+progName;
    if (access(progPath,X_OK)==0)
    {
      delete[] pathCopy;
      return progPath;
    }
  }
  delete pathCopy;
  return string();
}


string which (const char *fname, const char *path, int mode) // =F_OK
{
  // - generalization of which() (above) for any path, any access mode

  // first, accept fully-qualified paths + paths relative to CWD (working dir)
  if (access(fname,mode)==0)
    return string(fname);
  
  char *pathCopy = new char[strlen(path)+1],
       *pathElem;
  strcpy(pathCopy,path);
  for (pathElem=strtok(pathCopy,":"); pathElem; pathElem=strtok(NULL,":"))
  {
    string fpath = string(pathElem)+"/"+fname;
    if (access(fpath,mode)==0)
    {
      delete[] pathCopy;
      return fpath;
    }
  }
  delete pathCopy;
  return string();
}


////////////////////////////////////////////////////////////////
//  lockfile, unlockfile
//   - lock/unlock a file (by descriptor), block if locking a locked file
//   - uses lockf() (wrapper for fcntl), locks entire file contents
//   - returns error codes from lockf(), 0 on success

int lockfile (int fd)
{
  // amd -- comment out locking for cygwin where F_LOCK not defined...
#if defined(F_LOCK) && !defined(__APPLE__)
  off_t pos = lseek(fd,0,SEEK_CUR);	// - get present file offset
  lseek(fd,0,SEEK_SET);			// - go to beginning of file
  int ret = lockf(0,F_LOCK,0);		// - lock present file contents
  lseek(fd,pos,SEEK_SET);		// - restore file offset
  return ret;
#else
  return 0;
#endif
}


int unlockfile (int fd)
{
  // amd -- comment out locking for cygwin where F_LOCK not defined...
#if defined(F_LOCK) && !defined(__APPLE__)
  off_t pos = lseek(fd,0,SEEK_CUR);	// - get present file offset
  lseek(fd,0,SEEK_SET);			// - go to beginning of file
  int ret = lockf(0,F_ULOCK,0);		// - unlock present file contents
  lseek(fd,pos,SEEK_SET);		// - restore file offset
  return ret;
#else
  return 0;
#endif
}
