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
#include <time.h>
#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include "misc.h"
#include "operator.h"
#include "symbol.h"
#include "version.h"
#include "cctypes.h"
#include "cctype.h"
#include "ccheader.h"
#include "cceval.h"

using std::ofstream;

// Signature for the function in the header file
void autoesl_functional_signature(ofstream *fout,
			   string name,
			   Symbol *rsym,
			   list<Symbol*> *argtypes,
			   string estr)
{

  int i=0;
  Symbol *sym;
  forall(sym,*argtypes)
  {
	  if (i>0) *fout << ",\n";
	  string prefix="";
	  // detect if input or output..
	  if (sym->isStream()) {
		  SymbolStream *ssym=(SymbolStream *)sym;
		  if (ssym->getDir()==STREAM_OUT) {
			  prefix="*";
		  }
	  }

	  *fout << "\t" << sym->getType()->toString() 
		  << " " << prefix << sym->getName();
	  i++;
  }

  *fout << "\n)" << estr << endl;

}

// Constructor for the function in the body
void autoesl_functional_constructor(ofstream *fout,
			   string name,
			   string classname,
			   Symbol *rsym,
			   list<Symbol*> *argtypes)
{

  int i=0;
  Symbol *sym;
  forall(sym,*argtypes)
  {
	  if (i>0) *fout << ",\n";
	  string prefix="";
	  // detect if input or output..
	  if (sym->isStream()) {
		  SymbolStream *ssym=(SymbolStream *)sym;
		  if (ssym->getDir()==STREAM_OUT) {
			  prefix="*";
		  }
	  }

	  *fout << "\t" << " " << sym->getName();
	  i++;
  }

  *fout << "\n)" << endl;
}

void ccautoeslheader (Operator *op)
{
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  classname=name;
  list<Symbol*> *argtypes=op->getArgs();

  // start new output file
  string fname=name+".h";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "// ccautoesl autocompiled header file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION   << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;
  *fout << endl;

  *fout << "#ifndef "+classname+"_H_" << endl;
  *fout << "#define "+classname+"_H_" << endl;
  *fout << "#define SC_INCLUDE_FX" << endl;
  *fout << "#include \"systemc.h\"" << endl;
  *fout << endl;

  *fout << "void " << classname << "(\n" ;
//  autoesl_constructor_signatures(fout,rsym,argtypes, true);
  autoesl_functional_signature(fout,name,rsym,argtypes,"");
  *fout << endl;


  *fout << "#endif /*"+classname+"_H_*/" << endl;
  *fout << endl;

  // close file
  fout->close();

}




