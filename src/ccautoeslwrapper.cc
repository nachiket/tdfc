/////////////////////////////////////////////////////////////////////////////
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
// SCORE TDF compiler:  generate CPP wrapper file and TCL compile script for AutoESL implementation..
//
//////////////////////////////////////////////////////////////////////////////

#include "compares.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include <LEDA/core/array.h>
#include "operator.h"
#include "expr.h"
#include "misc.h"
#include "ops.h"
#include "symbol.h"
#include "stmt.h"
#include "state.h"
#include "annotes.h"
#include "version.h"
#include "feedback.h"
#include "bindvalues.h"
#include "cctype.h"
#include "ccannote.h"
#include "cccase.h"
#include "ccheader.h"
#include "ccbody.h"
#include "cceval.h"
#include "ccstmt.h"
#include "ccprep.h"
#include "ccmem.h"
#include "instance.h" // blah, just for unique_name()
#include "cccopy.h" 


using leda::list_item;
using leda::dic_item;
using std::ofstream;

//
////////////////////////////////////////////////////////////////////////
// Top level routine to create AutoESL test wrapper and TCL compile script 
////////////////////////////////////////////////////////////////////////

void ccautoeslwrapper (Operator *op)
{
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  classname=name;
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+"_test.cpp";

  ofstream *fout=new ofstream(fname);
  *fout << "// tdfc-autoesl autocompiled wrapper file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;

  // some includes
  *fout << "#include <stdio.h>" << endl;
  *fout << "#include <math.h>" << endl;
  *fout << "#include \"" << name << ".h\"" << endl;
  *fout << endl;

  // include anythying I depend upon
  ccprep(op); // generic for things need to be done on pre pass
  set<Operator *>* called_ops=(set<Operator *>*)op->getAnnote(CC_CALL_SET);

  if (called_ops!=(set<Operator *>*)NULL)
    {
      Operator *cop;
      forall(cop,*called_ops)
	{
	  if (cop->getOpKind()!=OP_BUILTIN)
	    *fout << "#include \"" << cop->getName() << ".h\"" << endl;
	}
    }

  // constructor
  *fout << "int main()" << endl << "{" << endl ;
  *fout << "\tint N=1024;" << endl ; // randomly choose 1024 samples in the stream for now..
  
  // Variable Declarations
  Symbol *sym;
  forall(sym,*argtypes)
  {
	if(sym->isParam())
		*fout << "  const " << sym->getType()->toString() << " " << sym->getName() << "_d = (" << sym->getType()->toString() << ")1;" << endl;
	else
		*fout << "  " << sym->getType()->toString() << " *" << sym->getName() << ";" << endl;
  }

  // Memory Allocations
  forall(sym,*argtypes)
  {
	if(!sym->isParam()) {
		*fout << endl << "  // " << sym->getName() << endl;
		*fout << "  size_t " << sym->getName() << "_size = N * sizeof(" << sym->getType()->toString() << ");" << endl;
		*fout << "  " << sym->getName() << " = (" << sym->getType()->toString() << " *)malloc(" << sym->getName() << "_size);" << endl;
	}
  }
  *fout << endl << "  // Initialize input contents and copy to Device memory" << endl;

  // Initialize Input Memory Contents to some arbitrary values for now..
  *fout << "  for(int i=0; i<N; i++) {" << endl; //Streams
  forall(sym,*argtypes)
  {
	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_IN && !sym->isParam())
	  *fout << "    " << sym->getName() << "[i] = (" << sym->getType()->toString() << ")i;" << endl;
  }
  *fout << "  }" << endl << endl;

  // Do calculation in AutoESL
  *fout << "  // Loop" << endl;
  *fout << "  for(int i=0; i<N; i++) {" << endl; //Streams
  *fout << "  " << name << " (";
  int j=0;
  forall(sym,*argtypes)
  {
	if(j>1)
		*fout << ",";

	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_OUT) {
		*fout << sym->getName() << "[i]";
	} else {
		*fout << "*" << sym->getName() << "[i]";
	}
	j++;
  }
  *fout << ");" << endl << endl;

  // Print results
  *fout << "  // Print results (typecasted to prevent printf errors)" << endl;
  *fout << "  printf(\"" << name << ":\\n\");" << endl;
  forall(sym,*argtypes)
  {
	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_OUT) {
	  //Type-casting output to prevent printf errors
	  *fout << "  for (int i=0; i<N; i++) printf(\"" << sym->getName() << "_h[%d] = %f\\n\", i, (float)" 
		<< sym->getName() << "[i]);" << endl;
	}
  }
  *fout << endl;

  // Cleanup
  *fout << "  // Cleanup" << endl;
  forall(sym,*argtypes)
  {
	if(!sym->isParam()) {
		*fout << "  free(" << sym->getName() << "); ";
	}
  }


  *fout << endl;


  *fout << "} // main" << endl;


  // close up
  fout->close();

}





