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
// SCORE TDF compiler:  generate C++ output
// $Revision: 1.100 $
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include "operator.h"

using std::ofstream;
using std::cout;
using std::endl;
using std::cerr;

#define NON_FUNCTIONAL_PREFIX "nonfunc_"


int noReturnValue(Symbol *rsym);

void functional_constructor(ofstream *fout,string name,
			    string classname,Symbol *rsym,
			    list<Symbol*> *argtypes);

void constructor_signatures(ofstream *fout,
                            Symbol *rsym,
                            list<Symbol*> *argtypes);

void constructor_signatures_notypes(ofstream *fout,
                                    Symbol *rsym,
                                    list<Symbol*> *argtypes);

void ccheader(Operator *op);

int microblaze_noReturnValue(Symbol *rsym);

void microblaze_functional_constructor(ofstream *fout,string name,
			    string classname,Symbol *rsym,
			    list<Symbol*> *argtypes);

void microblaze_constructor_signatures(ofstream *fout,
                            Symbol *rsym,
                            list<Symbol*> *argtypes);

void microblaze_constructor_signatures_notypes(ofstream *fout,
                                    Symbol *rsym,
                                    list<Symbol*> *argtypes);

void ccmicroblazeheader(Operator *op);

void cccudaheader(Operator *op);

void autoesl_functional_signature(ofstream *fout,string name,
			    Symbol *rsym,
			    list<Symbol*> *argtypes,
			    string estr);

void autoesl_functional_constructor(ofstream *fout,string name,
			    string classname,Symbol *rsym,
			    list<Symbol*> *argtypes);

void ccautoeslheader(Operator *op, bool exp, bool log, bool div, int double_type, int frac_bits, int int_bits);
