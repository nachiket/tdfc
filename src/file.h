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
// $Revision: 1.124 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_FILE_
#define _TDF_FILE_

#include <stdio.h>
#include <unistd.h>
#include <LEDA/core/string.h>
#include <LEDA/core/set.h>
#include "parse.h"


extern void   preprocessFile (string inFile, string outFile,
			      list<char*> *flags=NULL);		// call cpp
extern void   parseFile      (const char *fileName, int debug);	// TDF --> AST
extern size_t copyFile       (FILE *in, FILE *out);		// FILE-->FILE
extern string which          (const char *progName);		// search $PATH
extern string which          (const char *fname,
			      const char *path, int mode=F_OK);	// search path
extern int    lockfile       (int fd);				// lock/suspend
extern int    unlockfile     (int fd);				// unlock


#endif	// #ifndef _TDF_FILE_
