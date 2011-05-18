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
// SCORE TDF compiler:  generate CUDA header
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

int cuda_noReturnValue(Symbol *rsym)
{
  return((rsym==(Symbol *)NULL) ||
	 (rsym->getType()->getTypeKind()==TYPE_NONE));
}


void cuda_constructor_signatures(ofstream *fout,
			    Symbol *rsym,
			    list<Symbol*> *argtypes, bool state_id)
{

  int i=1;
  if(state_id) {
    *fout << "int n_start_state" ; // April 10th 2010
  } else {
    i=0;
  }

  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (i>0) *fout << ",";
      *fout << sym->getType()->toString() << "* n_" << sym->getName() ;
      i++;
    }
  if (i>0)
    *fout << ",int N";

  
}

void cuda_functional_signature(ofstream *fout,
			   string name,
			   Symbol *rsym,
			   list<Symbol*> *argtypes,
			  string endstr)
{

  if (!cuda_noReturnValue(rsym))
    {
      if (rsym->isStream())
	{
	  *fout << getCCtype(rsym) << " " << name << "(";
	}
      else
	{
	  return;// presumably we've already issued this complaint above
	}
    }

  // N.B.: assumes tdf names can become C++ names, can 
  //   get us into trouble

  int i=0;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (i>0) *fout << ",";
      *fout << getCCtype(sym)  << " " // was: " i" << i ;
	    << sym->getName();
      i++;
    }

  *fout << ")" << endstr << endl;

}

void cccudaheader (Operator *op)
{
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (cuda_noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name; //NON_FUNCTIONAL_PREFIX = "non_func_"
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+".cuh";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "// tdfc-cuda autocompiled header file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION   << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;
  *fout << endl;

  *fout << "#ifndef "+classname+"_CUH_" << endl;
  *fout << "#define "+classname+"_CUH_" << endl;
  *fout << endl;

  *fout << "#ifdef __cplusplus" << endl;
  *fout << "extern \"C\" {" << endl;
  *fout << "#endif" << endl;
  *fout << endl;


  *fout << "__global__ void " << classname << "(" ;
  cuda_constructor_signatures(fout,rsym,argtypes, false);
  *fout << "  );" << endl;


  *fout << "#ifdef __cplusplus" << endl;
  *fout << "}" << endl;
  *fout << "#endif" << endl;
  *fout << endl;

  *fout << "#endif /*"+classname+"_CUH_*/" << endl;
  *fout << endl;

  // close file
  fout->close();

}


