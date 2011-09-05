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
// SCORE TDF compiler:  generate CUDA output for NVIDIA GPU
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


/***********************************************************************
Note: This will use a simple round-robin scheduling technique..
***********************************************************************/    

using leda::list_item;
using leda::dic_item;
using std::ofstream;

//
////////////////////////////////////////////////////////////////////////
// Cody Huang: Top level routine to create CUDA wrapper code
////////////////////////////////////////////////////////////////////////

void cccudawrapper (Operator *op)
{
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name; //NON_FUNCTIONAL_PREFIX = "non_func_"
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+".cpp";

  ofstream *fout=new ofstream(fname);
  *fout << "// tdfc-cuda autocompiled wrapper file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;

  // some includes
  *fout << "#include <stdio.h>" << endl;
  *fout << "#include <cuda.h>" << endl;
  *fout << "#include \"" << name << ".cuh\"" << endl;
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
  *fout << "int main(void)" << endl << "{" << endl ;
  
  // Variable Declarations
  Symbol *sym;
  forall(sym,*argtypes)
  {
	if(sym->isParam())
		*fout << "  const " << sym->getType()->toString() << " " << sym->getName() << "_d = (" << sym->getType()->toString() << ")1;" << endl;
	else
		*fout << "  " << sym->getType()->toString() << " *" << sym->getName() << "_h, *" << sym->getName() << "_d;" << endl;
  }
  *fout << "  const int N = 1024; // Number of elements in array" << endl;

  // Memory Allocations
  forall(sym,*argtypes)
  {
	if(!sym->isParam()) {
		*fout << endl << "  // " << sym->getName() << endl;
		*fout << "  size_t " << sym->getName() << "_size = N * sizeof(" << sym->getType()->toString() << ");" << endl;
		*fout << "  " << sym->getName() << "_h = (" << sym->getType()->toString() << " *)malloc(" << sym->getName() << "_size);" << endl;
		*fout << "  cudaMalloc((void **) &" << sym->getName() << "_d, " << sym->getName() << "_size);" << endl;
	}
  }
  *fout << endl << "  // Initialize input contents and copy to Device memory" << endl;

  // Initialize Input Memory Contents
  *fout << "  for(int i=0; i<N; i++) {" << endl; //Streams
  forall(sym,*argtypes)
  {
	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_IN && !sym->isParam())
	  *fout << "    " << sym->getName() << "_h[i] = (" << sym->getType()->toString() << ")i;" << endl;
  }
  *fout << "  }" << endl << endl;



  // Performance Calculation Start
  *fout << "  //Performance Calculation" << endl;
  *fout << "#ifdef PERF" << endl;
  *fout << "  cudaEvent_t start, stop;" << endl << "  float time;" << endl;
  *fout << "  cudaEventCreate(&start);" << endl;
  *fout << "  cudaEventCreate(&stop);" << endl;
  *fout << "  cudaEventRecord(start, 0);" << endl;
  *fout << "#endif" << endl << endl;

  // Copy to Device Memory
  forall(sym,*argtypes)
  {
	if(!sym->isParam()) {
		SymbolStream *ssym=(SymbolStream *)sym;
		if(ssym->getDir() == STREAM_IN)
		  *fout << "  cudaMemcpy(" << sym->getName() << "_d, " << sym->getName() << "_h, "
			<< sym->getName() << "_size, cudaMemcpyHostToDevice);" << endl;
	}
  }
  *fout << endl;

  // Do calculation on device
  *fout << "  // Do calculation on Device" << endl;
  *fout << "  int block_size = 8;" << endl;
  *fout << "  int n_blocks = N/block_size + (N%block_size == 0 ? 0:1);" << endl << endl;
  *fout << "  " << name << " <<< n_blocks, block_size >>> (";
  forall(sym,*argtypes)
  {
	*fout << sym->getName() << "_d, ";
  }
  *fout << "N);" << endl << endl;

  // Retrieve result from device and store it in host array
  *fout << "  // Retrieve results" << endl;
  forall(sym,*argtypes)
  {
	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_OUT)
	  *fout << "  cudaMemcpy(" << sym->getName() << "_h, " << sym->getName() << "_d, "
		<< sym->getName() << "_size, cudaMemcpyDeviceToHost);" << endl;
  }
  *fout << endl;

  // Performance Calculation Stop
  *fout << "#ifdef PERF" << endl;
  *fout << "  cudaEventRecord(stop, 0);" << endl;
  *fout << "  cudaEventSynchronize(stop);" << endl;
  *fout << "  cudaEventElapsedTime(&time, start, stop);" << endl;
  *fout << "  cudaEventDestroy(start);" << endl;
  *fout << "  cudaEventDestroy(stop);" << endl;
  *fout << "  printf(\"GPU time for " << name << " is %f ms\\n\", time);"<< endl;
  *fout << "#endif" << endl << endl;

  // Print results
  *fout << "#ifndef PERF" << endl;
  *fout << "  // Print results (typecasted to prevent printf errors)" << endl;
  *fout << "  printf(\"" << name << ":\\n\");" << endl;
  forall(sym,*argtypes)
  {
	SymbolStream *ssym=(SymbolStream *)sym;
	if(ssym->getDir() == STREAM_OUT) {
	  //Type-casting output to prevent printf errors
	  *fout << "  for (int i=0; i<N; i++) printf(\"" << sym->getName() << "_h[%d] = %f\\n\", i, (float)" 
		<< sym->getName() << "_h[i]);" << endl;
	}
  }
  *fout << "#endif" << endl << endl;
  *fout << endl;

  // Cleanup
  *fout << "  // Cleanup" << endl;
  forall(sym,*argtypes)
  {
	if(!sym->isParam()) {
		*fout << "  free(" << sym->getName() << "_h); ";
		*fout << "  cudaFree(" << sym->getName() << "_d);" << endl;
	}
  }


  *fout << endl;


  *fout << "} // main" << endl;


  // close up
  fout->close();

}





