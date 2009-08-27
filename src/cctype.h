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
// $Revision: 1.130 $
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
#include "symbol.h"
#include "type.h"
#include "symbol.h"


int           ccTypeCausesInstance  (Type *atype);
int           ccParamCausesInstance (Symbol *sym);
int           ccCountParameters     (list<Symbol*> *argtypes);
unsigned long ccParameterLocs       (list<Symbol*> *argtypes);

string getCCStreamType      (Symbol *rsym);
string getCCStreamType      (Symbol *rsym,int tas);
string getCCtypeConstructor (Symbol *rsym, bool tas=0, bool in_pagestep=0);
string getCCtype            (Symbol *rsym, bool tas=0);
string getCCobjConvert      (Symbol *rsym,string name);
string getCCidConvert       (Symbol *rsym,string name);

string getCCvarType (Type   *stype);
string getCCvarType (Symbol *rsym);
int    compoundVar  (Symbol *rsym);


