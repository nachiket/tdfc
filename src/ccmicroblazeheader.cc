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

int microblaze_noReturnValue(Symbol *rsym)
{
  return((rsym==(Symbol *)NULL) ||
	 (rsym->getType()->getTypeKind()==TYPE_NONE));
}


void microblaze_parameter_variables(ofstream *fout,
			 list<Symbol*> *argtypes)
{
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (ccParamCausesInstance(sym))
	{
	  *fout << "  " << getCCtype(sym) << " " <<
	    sym->getName() << ";" << endl;
	}
    }
}

void microblaze_constructor_signatures(ofstream *fout,
			    Symbol *rsym,
			    list<Symbol*> *argtypes)
{

  int i=0;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (i>0) *fout << ",";
      *fout << "ScoreStream* n_" << sym->getName() ;
      i++;
    }

  
}

void microblaze_constructor_signatures_notypes(ofstream *fout,
                                    Symbol *rsym,
                                    list<Symbol*> *argtypes)
{

  int i=0;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (i>0) *fout << ",";
      *fout << " n_" << sym->getName() ;
      i++;
    }

 
}

void microblaze_functional_signature(ofstream *fout,
			   string name,
			   Symbol *rsym,
			   list<Symbol*> *argtypes,
			  string endstr)
{

  if (!microblaze_noReturnValue(rsym))
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

void microblaze_functional_constructor(ofstream *fout,
			    string name,
			    string classname,
			    Symbol *rsym,
			    list<Symbol*> *argtypes)
{
  
  microblaze_functional_signature(fout,name,rsym,argtypes," {");
  
  Expr *pexpr=rsym->getType()->getPredExpr();
  if (pexpr!=(Expr *)NULL)
    {
      string pstr=ccEvalExpr(pexpr, false);
      *fout << " if (" << pstr << " == 0 ) " << endl;
      *fout << "    throw (\"runtime type error constructing " 
	    << name << "--failed type expression.\n";
      *fout << "\texpr: " << pstr << "\");" << endl;
    }

  *fout << " " << classname << " *res=new " << classname << "(";

  int first=1;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (first)
	first=0;
      else
	*fout << "," ;
      *fout << sym->getName();
    }
  *fout << ");" << endl;

  *fout << " return(res->getResult());" << endl;
  *fout << "}" << endl;
}


void ccmicroblazeheader (Operator *op)
{
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (microblaze_noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name;
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+".h";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "// cctdfc autocompiled header file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION   << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;
  *fout << endl;

  *fout << "#ifndef "+classname+"_H_" << endl;
  *fout << "#define "+classname+"_H_" << endl;
  *fout << endl;

  *fout << "#ifdef __cplusplus" << endl;
  *fout << "extern \"C\" {" << endl;
  *fout << "#endif" << endl;
  *fout << endl;

  // define shared variables for this class..
//  *fout << "int id=0;" << endl;
//  if (op->getOpKind()==OP_BEHAVIORAL) {
//    *fout << "pthread_t rpt;" << endl;
//  } else if(op->getOpKind()==OP_COMPOSE) {
//    *fout << "    // skipping pthread for composite operator... // pthread_t rpt;" << endl;
//  } else {
//    cerr << "Unsupported opKind in ccmicroblazeheader.h" << endl;
//    exit(1);
//  }
//  *fout << endl;

  *fout << "void* " << classname << "_create(" ;
  microblaze_constructor_signatures(fout,rsym,argtypes);
  *fout << "  );" << endl;
  *fout << "void* "+classname+"_proc_run(void*);" << endl;

  if (!microblaze_noReturnValue(rsym))
    *fout << "    " << getCCtype(rsym) << " getResult() "
	  << "{ return result;}" << endl;
  
  if (!microblaze_noReturnValue(rsym))
    *fout << "    " << getCCtype(rsym) << " result;" << endl;

  *fout << endl;

  // if necessary, functional version
  if (!microblaze_noReturnValue(rsym))
    microblaze_functional_signature(fout,name,rsym,argtypes,";");



  *fout << "#ifdef __cplusplus" << endl;
  *fout << "}" << endl;
  *fout << "#endif" << endl;
  *fout << endl;

  *fout << "#endif /*"+classname+"_H_*/" << endl;
  *fout << endl;

  // close file
  fout->close();

}




