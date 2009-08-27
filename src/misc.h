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
// SCORE TDF compiler:  miscellaneous utility functions
// $Revision: 1.133 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_MISC_
#define _TDF_MISC_

#include <LEDA/core/string.h>
#include "parse.h"

class OperatorCompose;

					// - Global Variables:
extern int	gErrorCount;		// number of errors   during compile
extern int	gWarningCount;		// number of warnings during compile
extern int	gPrintTime;		// print time at timestamps
extern int	gPrintMemory;		// print memory usage at timestamps
extern int	gDebugShared;		// debug mode: find shared AST nodes
extern int	gDeleteType;		// enable deletion of types
extern int	gEmitVCG;		// enables emission of .vcg files
extern int	gProfileStates;		// generate code to profile states

void tdf_init ();			// initialize TDF compiler
void tdf_exit ();			// cleanup    TDF compiler

void warn  (const string &msg,
	    Token *errToken=NULL,
	    bool usePerror=false);	// print warning Re. src at errToken
void error (int exitCode,
	    const string &msg,
	    Token *errToken=NULL,
	    bool usePerror=false);	// print error diagnostic (+ terminate)
void fatal (int exitCode,
	    const string &msg,
	    Token *errToken=NULL,
	    bool usePerror=false);	// print error diagnostic  + terminate

void timestamp (string note);		// print time stamp
void timestamp (string note, Tree *t);

const char* printstr  (string &sref);	// debug in gdb:  "print ..."
const char* printtree (Tree   *tptr);

void associateScopes  (Tree *t, SymTab *scope);	// as/dis-sociate local symtabs
void dissociateScopes (Tree *t, SymTab *scope);	//   with/from given scope

void debugStateFiring (Tree *t);	// instrmnt states to print firing info

int  countBits	(long long val);	// count significant low-order bits

string tdfcComment ();			// "Emitted by tdfc" comment

list<Operator*> findCalledOps (OperatorCompose *op);	// return called ops


#endif	// #ifndef _TDF_MISC_
