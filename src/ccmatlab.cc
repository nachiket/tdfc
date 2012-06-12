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
		if (i>0) *fout << ",";
		if(sym->isParam())
			*fout << " " << sym->getName() ;
		else
			*fout << " " << sym->getName() ;
			//*fout << sym->getType()->toString() << "* " << sym->getName() ;
		i++;
	}


}


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
  *fout << "function " << single_output_name << " = " << name << "(";
  matlab_constructor_signatures(fout, rsym, argtypes, false);
  if(fixed) {
	*fout << ", total_bits, fixed_bits)" << endl;
  } else {
  	*fout << ")" << endl;
  }

  // proc_run
  ccprocrun(fout,classname,op);
  *fout << endl;

  // close up
  fout->close();

}

