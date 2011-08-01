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

void cuda_constructor(ofstream *fout,
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
        *fout << sym->getType()->toString() << " " << sym->getName() ;
      else
        *fout << sym->getType()->toString() << "* " << sym->getName() ;
      i++;
    }
  if (i>0)
    *fout << ",int N";

  
}

void cccudaprocrun(ofstream *fout, string classname, Operator *op)
{

  if (op->getOpKind()==OP_COMPOSE)
    {
      *fout << "  printf(\"proc_run should never be called for a compose operator!\\n\");" << endl;
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
	  *fout << "  " << getCCvarType(asum) << " " << asum->getName() ;
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

      int num_states=states->size();
      if (num_states>1)
	*fout << "    switch(state) {" << endl;
      int snum=0;
      forall_items(item,*states)
	{
	  State* cstate=states->inf(item);
	  string sname=cstate->getName();

	  array<StateCase*>* cases=caseSort(cstate->getCases());
          int numNestings=0;

	  if (num_states>1)
	    *fout << "      case " << STATE_PREFIX << sname
		  << ": { " << endl;

	  string atomiceofrcase="0";

	  for (int i=cases->low();i<=cases->high();i++)
	    {
	      // walk over inputs to case
	      StateCase *acase=(*cases)[i];

              // increment the nesting count and also output a beginning
              // nesting bracket.
              numNestings++;
              *fout << "  if(idx<N)" << endl << "  {" << endl;

	      Stmt* stmt;
	      forall(stmt,*(acase->getStmts()))
		{
		  ccStmt(fout,string("          "),stmt,early_close,
			 STATE_PREFIX,0, false, false, true, classname); // 0 was default for ccStmt.h
			 // added false to remove retiming
		}
	     
	    }
	  // default case will be to punt out of loop (exit/done)

	  // close the nesting brackets.
	  *fout << "  ";
	  for (; numNestings>0; numNestings--) {
	    *fout << "}" << endl;
	  }

	  if (num_states>1)
	    {
	      *fout << "        break; " << endl;
	      *fout << "      } // end case " << STATE_PREFIX << 
		sname << endl;
	    }
	  snum++;
	}
      if (num_states>1)
	{
	  *fout << "      default: printf(\"ERROR unknown state encountered in " << classname << "_proc_run\\n\");" << endl;
	  *fout << "        return((void*)NULL);" << endl;
	  *fout << "    }" << endl;
	}

    }
  else
    {
      fatal(-1,string("Don't know how to handle opKind=%d",
		      (int)op->getOpKind()),
		      op->getToken());
	    
    }

//  *fout << "}" << endl;
  
}

//
////////////////////////////////////////////////////////////////////////
// Cody Huang: Top level routine to create master CUDA code
void cccudabody (Operator *op)
{
  //N.B. assumes renaming of variables to avoid name conflicts
  //  w/ keywords, locally declared, etc. has already been done
  
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (noReturnValue(rsym))
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name; //NON_FUNCTIONAL_PREFIX = "non_func_"
  list<Symbol*> *argtypes=op->getArgs();
  // start new output file
  string fname=name+".cu";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "// tdfc-cuda backend autocompiled body file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;

  // some includes
  *fout << "#include <stdio.h>" << endl;
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

  // broiler name
  *fout << endl;

  // constructor
  *fout << "__global__ void " << classname << "(" ;
  cuda_constructor(fout,rsym,argtypes, false);
  *fout << "  )" << endl << "{" << endl;

  // thread indexer
  *fout << "  int idx = blockIdx.x * blockDim.x + threadIdx.x;" << endl;

  *fout << endl;

  // proc_run
  cccudaprocrun(fout,classname,op);

  *fout << "} //" << classname << endl;

  // if necessary, functional version
//  if (!noReturnValue(rsym))
//    microblaze_functional_constructor(fout,name,classname,rsym,argtypes);

  // close up
  fout->close();

}





