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
  *fout << "  int N=1024;" << endl ; // randomly choose 1024 samples in the stream for now..
  
  // Variable Declarations
  Symbol *sym;
  forall(sym,*argtypes)
  {
	if(sym->isParam())
		*fout << "  const " << sym->getType()->toString() << " " << sym->getName() << " = (" << sym->getType()->toString() << ")1;" << endl;
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
  *fout << "    " << name << " (" << endl;
  int j=0;
  forall(sym,*argtypes)
  {
	if(j>=1)
		*fout << ",\n";

	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_IN && sym->isParam()) {
		*fout << "      " << sym->getName() << "";
	} else if(ssym->getDir() == STREAM_IN) {
		*fout << "      " << sym->getName() << "[i]";
	} else {
		*fout << "      &" << sym->getName() << "[i]";
	}
	j++;
  }
  *fout << "\n    );" << endl;
  *fout << "  }" << endl << endl;

  // Print results
  *fout << "  // Print results (typecasted to prevent printf errors)" << endl;
  *fout << "  printf(\"" << name << ":\\n\");" << endl;
  forall(sym,*argtypes)
  {
	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_OUT) {
	  //Type-casting output to prevent printf errors
	  *fout << "  for (int i=0; i<N; i++) printf(\"" << sym->getName() << "[%d] = %g\\n\", i, " 
		<< sym->getName() << "[i]);" << endl;
	}
  }
  *fout << endl;

  // Cleanup
  *fout << "  // Cleanup" << endl;
  forall(sym,*argtypes)
  {
	if(!sym->isParam()) {
		*fout << "  free(" << sym->getName() << "); " << endl;
	}
  }


  *fout << endl;


  *fout << "} // main" << endl;


  // close up
  fout->close();

}

/**
 * TCL script
 */
void ccautoesltcl (Operator *op, bool exp, bool log, bool div)
{
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  classname=name;
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+".tcl";

  ofstream *fout=new ofstream(fname);
  *fout << "# tdfc-autoesl autocompiled wrapper file" << endl;
  *fout << "# tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "# " << ctime(&currentTime) << endl;

  // simple synthesis options
  *fout << "set TOP [file rootname [info script]]" << endl; 
  *fout << "delete_project	${TOP}_batch.prj" << endl; 
  *fout << "open_project	${TOP}_batch.prj" << endl; 
  *fout << "add_file 		${TOP}.cpp" << endl; 
  *fout << "set_top  		${TOP}" << endl; 
  *fout << "open_solution	solution1" << endl; 
  *fout << "add_library		xilinx/virtex6/virtex6_fpv5" << endl; 
  *fout << "add_library		xilinx/virtex6/virtex6" << endl; 
  *fout << "create_clock	-period 3ns" << endl; 
  *fout << "set_directive_pipeline " << name << endl; 
  *fout << "set_directive_dataflow " << name << endl; 
  if(exp)
	  *fout << "set_directive_inline -off exp_flopoco" << endl; 
  if(log)
	  *fout << "set_directive_inline -off log_flopoco" << endl; 
  if(div)
	  *fout << "set_directive_inline -off div_flopoco" << endl; 


  *fout << "elaborate -lm" << endl; 
  *fout << "autosyn" << endl; 
  *fout << "autoimpl -setup -rtl vhdl" << endl; 
  
  // close up
  fout->close();

}


/**
 * Simple makefile
 */
void ccautoeslmake (Operator *op)
{
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  classname=name;
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname="Makefile."+name;

  ofstream *fout=new ofstream(fname);
  *fout << "# tdfc-autoesl autocompiled wrapper file" << endl;
  *fout << "# tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "# " << ctime(&currentTime) << endl;

  // simple synthesis options
  // expects AUTOESL_ROOT and ARCH to be set already..
  *fout << "DESIGN\t\t:=" << name << endl;
  *fout << "CC\t\t:= g++" << endl;
  *fout << "LDFLAGS\t\t+= -lm -lsystemc -L$(AUTOESL_ROOT)/$(AUTOESL_ARCH)/tools/systemc/lib" << endl;
  *fout << "CXXFLAGS\t+= -I$(AUTOESL_ROOT)/$(AUTOESL_ARCH)/tools/systemc/include -I$(AUTOESL_ROOT)/include" << endl;

  *fout << "sim: $(DESIGN)" << endl; 
  *fout << "\t./$(DESIGN)" << endl; 
  *fout << "autoesl: $(DESIGN).tcl" << endl; 
  *fout << "\tautoesl $<" << endl; 
  *fout << "hardware: $(DESIGN)_batch.prj/solution1/impl/vhdl/impl.sh" << endl; 
  *fout << "\tcd $(DESIGN)_batch.prj/solution1/impl/vhdl; ./impl.sh; cd ../../../../" << endl; 
  *fout << "$(DESIGN): $(DESIGN).o $(DESIGN)_test.o $(DESIGN).h" << endl;
  *fout << "\t$(CC) -o $@ $(filter %.o, $^) $(LDFLAGS)"  << endl;
  
  // close up
  fout->close();

}


