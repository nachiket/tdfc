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
// SCORE TDF compiler:  "assert.h" with TDF parser diagnostics, for gcc
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


//  "assert.h" with tdf parser diagnostics for gcc
//  adapted from "/usr/sww/include/assert.h"
//  Eylon Caspi, 4/1/99

#include <stdio.h>
#include <LEDA/core/string.h>
#include "parse.h"

/* Allow this file to be included multiple times
   with different settings of NDEBUG.  */
#undef assert
#undef __assert

#ifdef NDEBUG
#define assert(ignore) ((void) 0)
#else

#ifndef __GNUC__

#define assert(expression)  \
  ((void) ((expression) ? 0 : __assert (expression, __FILE__, __LINE__)))

#define __assert(expression, file, lineno)  \
  (printf ("%s:%u: failed assertion (parser at %s:%d)\n", \
	   file, lineno, (const char*)gFileName, gLineNum), \
   abort (), 0)

/*
#define __assert(expression, file, lineno)  \
  (printf ("%s:%u: failed assertion\n", file, lineno),	\
   abort (), 0)
*/

#else

#if defined(__STDC__) || defined (__cplusplus)

/* Defined in libgcc.a */
#ifdef __cplusplus
extern "C" {
extern void __eprintf (const char *, const char *, unsigned, const char *);
}
#else
extern void __eprintf (const char *, const char *, unsigned, const char *);
#endif

#define assert(expression)  \
  ((void) ((expression) ? 0 : __assert (#expression, __FILE__, __LINE__)))

#define __assert(expression, file, lineno)  \
  (printf ("%s:%u: failed assertion `%s' (parser at %s:%d)\n", \
	   file, lineno, expression, (const char*)gFileName, gLineNum), \
   abort (), 0)

/*
#define __assert(expression, file, line)  \
  (__eprintf ("%s:%u: failed assertion `%s'\n",		\
	      file, line, expression), 0)
*/

#else /* no __STDC__ and not C++; i.e. -traditional.  */

extern void __eprintf (); /* Defined in libgcc.a */

#define assert(expression)  \
  ((void) ((expression) ? 0 : __assert (expression, __FILE__, __LINE__)))

#define __assert(expression, file, lineno)  \
  (printf ("%s:%u: failed assertion `%s' (parser at %s:%d)\n", \
	   file, lineno, "expression", (const char*) gFileName, gLineNum), \
   abort (), 0)

/*
#define __assert(expression, file, lineno)  \
  (__eprintf ("%s:%u: failed assertion `%s'\n",		\
	      file, lineno, "expression"), 0)
*/

#endif /* no __STDC__ and not C++; i.e. -traditional.  */
#endif /* no __GNU__; i.e., /bin/cc.  */
#endif
