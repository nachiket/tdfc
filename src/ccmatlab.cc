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
// SCORE TDF compiler:  generate Matlab output
// $Revision: 1.115 $
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
Note:
  According to our discussion of 7/20/99, this should 
  * not handle direct memory references
  * not handle inlined calls/inline memory blocks
  * not handle bit-level composition operators
  * not handle nested calls
  * not have to deal with scoping heirarchy for operators
  Does have to handle the following builtins:
  + compose:
    - memory builtins (still not supported in frontend, so still TODO)
    - bitsof
    - widthof
    - cat (for exprs only)
    - copy   (9/99)
    = terminate (9/6/99)
  + behavioral:
    - done
    - cat 
    - bitsof
    - widthof
    - close
***********************************************************************/    

using leda::list_item;
using leda::dic_item;
using leda::string;
using std::ofstream;


void matlab_constructor_signatures(ofstream *fout,
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
		if(sym->isParam()) {
			if (i>0) *fout << ",";
			*fout << " " << sym->getName();
		}

		if(sym->isStream()) {
			SymbolStream *ssym=(SymbolStream *)sym;
			if (ssym->getDir()==STREAM_IN) {
				if (i>0) *fout << ",";
				*fout << " " << sym->getName();
			}
		}
		i++;
	}
}

void matlab_constructor_for_montecarlo(ofstream *fout,
		Symbol *rsym,
		list<Symbol*> *argtypes)
{

	int i=0;

	Symbol *sym;
	forall(sym,*argtypes)
	{
		if(sym->isParam()) {
			*fout << " numel(" << sym->getName() << ")";
		}

		if(sym->isStream()) {
			SymbolStream *ssym=(SymbolStream *)sym;
			if (ssym->getDir()==STREAM_IN) {
				*fout << " numel(" << sym->getName() << ")";
			}
		}
		i++;
	}
}

void matlab_constructor_for_dummyeval(ofstream *fout,
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
		if(sym->isParam()) {
			if (i>0) *fout << ",";
			*fout << " ones(SAMPLES,1).*" << sym->getName()<<"_mean";
		}

		if(sym->isStream()) {
			SymbolStream *ssym=(SymbolStream *)sym;
			if (ssym->getDir()==STREAM_IN) {
				if (i>0) *fout << ",";
				*fout << " " << sym->getName();
			}
		}
		i++;
	}
}


int matlab_get_input_count(Symbol *rsym,
		list<Symbol*> *argtypes)
{

	int i=0;

	Symbol *sym;
	forall(sym,*argtypes)
	{
		if(sym->isParam()) {
			i++;
		}

		if(sym->isStream()) {
			SymbolStream *ssym=(SymbolStream *)sym;
			if (ssym->getDir()==STREAM_IN) {
				i++;
			}
		}
	}

	return i;
}


//
////////////////////////////////////////////////////////////////////////
// procrun for master instance

//
////////////////////////////////////////////////////////////////////////
// procrun for master instance

void ccprocrun(ofstream *fout, string classname, Operator *op, bool fixed)
{

  if (op->getOpKind()==OP_COMPOSE)
    {
      *fout << "  disp(\"proc_run should never be called for a compose operator!\");" << endl;
    }
  else if (op->getOpKind()==OP_BEHAVIORAL)
    {
      OperatorBehavioral *bop=(OperatorBehavioral *)op;
      dictionary<string,State*>* states=bop->getStates();
      dic_item item;

      // declare top level vars
      SymTab *symtab=bop->getVars();
      list<Symbol*>* lsyms=symtab->getSymbolOrder();
      list_item item2;
      forall_items(item2,*lsyms)
	{
	  Symbol *sum=lsyms->inf(item2);
	  SymbolVar *asum=(SymbolVar *)sum;
	  Expr* val=asum->getValue();

	  if (val!=(Expr *)NULL)
	    *fout << "=" << ccEvalExpr(EvaluateExpr(val), false) ;
	  *fout << ";" << endl;

	}

      int icnt=0;
      int ocnt=0;

      int *early_free=new int[icnt];
      for (int i=0;i<icnt;i++)
	      early_free[i]=0;
      int *early_close=new int[ocnt];
      for (int i=0;i<ocnt;i++)
	      early_close[i]=0;

      Symbol *rsym=op->getRetSym();
      list<Symbol*> *argtypes=op->getArgs();
      if (!noReturnValue(rsym))
	{
	  if (rsym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)rsym;
	      if (ssym->getDir()!=STREAM_OUT)
		{
		  // error already issues
		}
	      else
		{
		  rsym->setAnnote(CC_STREAM_ID,(void *)ocnt);
		  ocnt++;
		}
	    }
	  else
	    {
	      // presumably we've already issued this complaint above
	    }
	}
      Symbol *sym;
      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
		  sym->setAnnote(CC_STREAM_ID,(void *)ocnt);
		  ocnt++;
		}
	      else if (ssym->getDir()==STREAM_IN)
		{
		  sym->setAnnote(CC_STREAM_ID,(void *)icnt);
		  icnt++;
		}
	      else
		{
		  // already complained
		}
	    }
	}

      int num_states=states->size();
      if (num_states>1)
	      cerr << "Currently ccmatlab does not support multiple states" << endl;

      int snum=0;
      forall_items(item,*states)
	{
	  State* cstate=states->inf(item);
	  string sname=cstate->getName();

	  array<StateCase*>* cases=caseSort(cstate->getCases());
	  array<Symbol*>* caseIns=allCaseIns(cases);
	  array<int>* firstUsed=inFirstUsed(caseIns,cases);
          int numNestings=0;

	  string atomiceofrcase="0";

	  for (int i=cases->low();i<=cases->high();i++)
	    {
	      // walk over inputs to case
	      StateCase *acase=(*cases)[i];

              // increment the nesting count and also output a beginning
              // nesting bracket.
              numNestings++;

	      // foreach statement write out Matlab code (should just be C-like for the most part)
	      Stmt* stmt;
	      forall(stmt,*(acase->getStmts()))
		{
		  ccStmt(fout,string("          "),stmt,early_close,
			 STATE_PREFIX,0, false, false, false, false, classname, NULL, NULL, NULL, true, fixed); 
		  // 0 was default for ccStmt.h
		}
	    }
	    // default case will be to punt out of loop (exit/done)
	    
	    snum++;
	}
    }
  else
    {
      fatal(-1,string("Don't know how to handle opKind=%d",
		      (int)op->getOpKind()),
		      op->getToken());
	    
    }

}

//
////////////////////////////////////////////////////////////////////////
// Top level routine to create Matlab code for operator
void ccmatlab (Operator *op, bool fixed)
{
  //N.B. assumes renaming of variables to avoid name conflicts
  //  w/ keywords, locally declared, etc. has already been done
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name;


  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=fixed?name+"_fixed.m":name+".m";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "\% cctdfc autocompiled file" << endl;
  *fout << "\% tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "\% " << ctime(&currentTime) << endl;

  // find the sole output of the function..
  // TODO: create array to pack multiple outputs...
  string single_output_name;
  Symbol* sym;
  if (noReturnValue(rsym))
  {
      forall(sym,*argtypes) {
	  if (sym->isStream())
	  {
		  SymbolStream *ssym=(SymbolStream *)sym;
		  if (ssym->getDir()==STREAM_OUT)
		  {
			  single_output_name = ssym->getName();
		  }
	  }
      }
  }
  // broiler name
  *fout << "function " << single_output_name << (fixed?"_fx = ": " = ") << name << (fixed?"_fixed(":"(");
  matlab_constructor_signatures(fout, rsym, argtypes, false);
  *fout << ")" << endl;
  if(fixed) {
	*fout << "\t\tglobal frac_bits;" << endl;
	*fout << "\t\ttotal_bits = frac_bits+8;" << endl;
  }

  // proc_run
  ccprocrun(fout,classname,op, fixed);
  *fout << endl;
 
  if(fixed) {
    *fout << "\t\t" << single_output_name << "_fx = " << single_output_name << ".double;" << endl;
  }

  // close up
  fout->close();

}

////////////////////////////////////////////////////////////////////////
// Generate the test wrapper to explore the entire design space..
void ccmatlabwrapper (Operator *op)
{
  //N.B. assumes renaming of variables to avoid name conflicts
  //  w/ keywords, locally declared, etc. has already been done
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name;


  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+"_test.m";

  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "\% cctdfc autocompiled file" << endl;
  *fout << "\% tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "\% " << ctime(&currentTime) << endl;

  // find the sole output of the function..
  // TODO: create array to pack multiple outputs...
  string single_output_name;
  Symbol* sym;
  if (noReturnValue(rsym))
  {
      forall(sym,*argtypes) {
	  if (sym->isStream())
	  {
		  SymbolStream *ssym=(SymbolStream *)sym;
		  if (ssym->getDir()==STREAM_OUT)
		  {
			  single_output_name = ssym->getName();
		  }
	  }
      }
  }

  // generate the input distributions..
  *fout << "\% Declare the uncertainty percentages and #sample counts..." << endl;
  *fout << "function ["+single_output_name+"_fx, "+single_output_name+"_dbl, "+single_output_name+"_dbl_correct] = " << name << "_test(UNCERTAINTY, SAMPLES)" << endl;
  *fout << endl;
  
  *fout << "\% declare all mean parameter mean values upfront..." << endl;
  forall(sym,*argtypes) {
	  if (sym->isParam()) {
		  if (((SymbolVar*)sym)->getNumber() != "")
			  *fout << sym->getName() << "_mean = "<< "(" << ((SymbolVar*)sym)->getNumber() <<  ");"<<endl;
		  else
			  cerr << "Unsupported uninitialized parameters.." << endl;
	  }
  }
  *fout << endl;

  *fout << "\% construct arrays of SAMPLES length from uniform random distribution.." << endl;
  forall(sym,*argtypes) {
	  if (sym->isParam()) {
		  //*fout << sym->getName() << " = " << sym->getName() << "_mean - " << sym->getName() << "_mean*(UNCERTAINTY/100) + (2*" << sym->getName() << "_mean*(UNCERTAINTY/100)).*rand(SAMPLES,1);"<<endl;
		  *fout << sym->getName() << " = unifrnd(" << sym->getName() << "_mean - " << sym->getName() << "_mean*UNCERTAINTY," << sym->getName() << "_mean + "+sym->getName()+"_mean*UNCERTAINTY, [SAMPLES 1]);"<<endl;
	  }
  }
  *fout << endl;
  
  *fout << "\% user supplied range for inputs..." << endl;
  if (noReturnValue(rsym))
  {
	  forall(sym,*argtypes) {
		  if (sym->isStream())
		  {
			  SymbolStream *ssym=(SymbolStream *)sym;
			  if (ssym->getDir()==STREAM_IN)
			  {
				  //cout << "Input=" << sym->getName() << endl;
				  *fout << sym->getName() + " = linspace(" + ((SymbolStream*)sym)->getMatlabRange() << ",SAMPLES);" << endl; // Hard-wired input range samples to 10
			  }
		  }
	  }
  }
  *fout << endl;

  *fout << endl;
  
  int input_count = matlab_get_input_count(rsym, argtypes);
  *fout << "\% map the generated samplespace over the device evaluations.." << endl;
  *fout << classname << "_inputs_dbl = allprod("; 
  matlab_constructor_signatures(fout, rsym, argtypes, false);
  *fout << ");" << endl;
  *fout << single_output_name << "_dbl_temp = arrayfun(@" << classname << ","; 
  for(int cnt=0; cnt<input_count-1;cnt++) {
  	*fout << classname <<"_inputs_dbl(:,"<<(cnt+1)<<"), ";
  }
  *fout << classname << "_inputs_dbl(:,"<<(input_count)<<"));" << endl;

  *fout << single_output_name << "_dbl = " << "reshape("<< single_output_name <<"_dbl_temp, fliplr([";
  matlab_constructor_for_montecarlo(fout, rsym, argtypes);
  *fout << "]));" << endl;
  *fout << "dlmwrite('"<< classname <<"_"<<single_output_name<<"_dbl.mat',"<<single_output_name<<"_dbl,'precision',16);" << endl;
  *fout << endl;
  
  *fout << "\% dummy perfect evaluations.." << endl;
  *fout << classname << "_inputs_dbl_correct = allprod("; 
  matlab_constructor_for_dummyeval(fout, rsym, argtypes, false);
  *fout << ");" << endl;
  *fout << single_output_name << "_dbl_correct_temp = arrayfun(@" << classname << ","; 
  for(int cnt=0; cnt<input_count-1;cnt++) {
  	*fout << classname <<"_inputs_dbl_correct(:,"<<(cnt+1)<<"), ";
  }
  *fout << classname << "_inputs_dbl_correct(:,"<<(input_count)<<"));" << endl;

  *fout << single_output_name << "_dbl_correct = " << "reshape("<< single_output_name <<"_dbl_temp_correct, fliplr([";
  matlab_constructor_for_montecarlo(fout, rsym, argtypes);
  *fout << "]));" << endl;
  *fout << "dlmwrite('"<< classname <<"_"<<single_output_name<<"_dbl_correct.mat',"<<single_output_name<<"_dbl_correct,'precision',16);" << endl;
  *fout << endl;
  
  *fout << classname << "_inputs_fx = allprod("; 
  matlab_constructor_signatures(fout, rsym, argtypes, false);
  *fout << ");" << endl;
  *fout << single_output_name << "_fx_temp = arrayfun(@" << classname << "_fixed,";
  for(int cnt=0; cnt<input_count-1;cnt++) {
  	*fout << classname <<"_inputs_fx(:,"<<(cnt+1)<<"), ";
  }
  *fout << classname << "_inputs_fx(:,"<<(input_count)<<"));" << endl; // AHA! the second one has one extra random variable..

  *fout << single_output_name << "_fx = " << "reshape("<< single_output_name <<"_fx_temp, fliplr([";
  matlab_constructor_for_montecarlo(fout, rsym, argtypes);
  *fout << "]));" << endl;
  *fout << "dlmwrite('"<< classname <<"_"<<single_output_name<<"_fx.mat',"<<single_output_name<<"_fx,'precision',64);" << endl; // For fixed-point matrices, not sure how much precision is adequate!
  *fout << endl;
  
  //*fout << "\% computing absolute errors w.r.t. mean double-precision value.." << endl;
  *fout << endl;

  // close up
  fout->close();

}

////////////////////////////////////////////////////////////////////////
// Generate the script that runs the test across different unceratinty/bitwidth permutations..
void ccmatlabscript (Operator *op)
{
  //N.B. assumes renaming of variables to avoid name conflicts
  //  w/ keywords, locally declared, etc. has already been done
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name;


  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+"_script.m";

  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "\% cctdfc autocompiled file" << endl;
  *fout << "\% tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "\% " << ctime(&currentTime) << endl;

  // find the sole output of the function..
  // TODO: create array to pack multiple outputs...
  string single_output_name;
  Symbol* sym;
  if (noReturnValue(rsym))
  {
      forall(sym,*argtypes) {
	  if (sym->isStream())
	  {
		  SymbolStream *ssym=(SymbolStream *)sym;
		  if (ssym->getDir()==STREAM_OUT)
		  {
			  single_output_name = ssym->getName();
		  }
	  }
      }
  }

  // generate the input distributions..
  *fout << "\% Declare the global math preferences..." << endl;
  *fout << "globalfimath(F);" << endl;
  *fout << "saveglobalfimathpref;" << endl;
  *fout << "global frac_bits;" << endl;
  *fout << endl;

  *fout << "UNCERTAINTY = logspace(-1,-20,20);" << endl;
  *fout << "SAMPLES = 100;" << endl;
  *fout << "BITWIDTH = 8:8:80;" << endl;
  *fout << endl;
  
  *fout << "\% Loop over the different combinations" << endl;
  *fout << "for frac_bits = BITWIDTH" << endl;
  *fout << "\tfor u = UNCERTAINTY" << endl;
  *fout << endl;
  *fout << "\t\t["<<single_output_name<<"_fx,"<<single_output_name<<"_dbl,"<<single_output_name<<"_dbl_correct] = " << name+"_test(u,SAMPLES);" << endl;
  *fout << endl;
  
  string maxstr,minstr,tailstr;
  int input_count = matlab_get_input_count(rsym, argtypes);
  for(int cnt=0; cnt<input_count-1;cnt++) {
  	maxstr += "max(";
  	minstr += "min(";
	tailstr += ")";
  }

  *fout << "\t\tmax_error="<<maxstr<<"("<<single_output_name<<"_fx-"<<single_output_name<<"_dbl_correct)"<<tailstr<<"" << endl;
  *fout << "\t\tmin_error="<<minstr<<"("<<single_output_name<<"_fx-"<<single_output_name<<"_dbl_correct)"<<tailstr<<"" << endl;
  *fout << "\tend" << endl;
  *fout << "end" << endl;

  *fout << endl;
  
  // close up
  fout->close();

}

