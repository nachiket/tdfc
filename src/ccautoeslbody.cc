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
// SCORE TDF compiler:  generate C output for Xilinx AutoESL compiler
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

////////////////////////////////////////////////////////////////////////
// procrun for master instance
////////////////////////////////////////////////////////////////////////

void ccautoeslprocbody(ofstream *fout, string classname, Operator *op, bool *exp, bool *log, bool *div)
{
	
  if (op->getOpKind()==OP_COMPOSE)
    {
      *fout << "  printf(\"procbody should never be called for a compose operator!\\n\");" << endl;
    }
  else if (op->getOpKind()==OP_BEHAVIORAL)
    {
		*fout << "{" << endl;
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
	  //*fout << "  " << getCCvarType(asum) << " " << asum->getName() ;
	  
	  //if (getCCvarType(asum) != "boolean")
			*fout << "  " << "data_t" << " " << asum->getName() ;
	  //else 
		//	*fout << "  " << "bool" << " " << asum->getName() ;
	  
	  Expr* val=asum->getValue();
	  if (val!=(Expr *)NULL)
	  { 
	      *fout << "=" << ccEvalExpr(EvaluateExpr(val), false) ;
	  }
	  *fout << ";" << endl;
	}

      // define constants and constant values...
      Symbol *sym;
      list<Symbol*> *argtypes=op->getArgs();
      forall(sym,*argtypes)
      {
	      if(sym->isParam())
	      {
		      if (((SymbolVar*)sym)->getNumber() != "")
		      	    *fout << "    data_t " << sym->getName() <<" = "<< "(" << ((SymbolVar*)sym)->getNumber() <<  ");"<<endl;                     
		      else
		      	    *fout << "    data_t " << sym->getName() <<";" << endl;
	      }
      }

      // declare variable/type for holding input stream tokens
      int icnt=0;
      int ocnt=0;
      Symbol *rsym=op->getRetSym();

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
	  array<Symbol*>* caseIns=allCaseIns(cases);
	  array<int>* firstUsed=inFirstUsed(caseIns,cases);
          int numNestings=0;

	  if (num_states>1) {
	    *fout << "      case " << STATE_PREFIX << sname
		  << ": { " << endl;
	  } else {
	    *fout << "{ " << endl;
	  }

	  string atomiceofrcase="0";

	  for (int i=cases->low();i<=cases->high();i++)
	    {
	      // walk over inputs to case
	      InputSpec *ispec;
	      StateCase *acase=(*cases)[i];

              // increment the nesting count and also output a beginning
              // nesting bracket.
              numNestings++;

	      // reducing this to a simple statement processing scenario
	      
	      Stmt* stmt;
	      forall(stmt,*(acase->getStmts()))
		{
		  ccStmt(fout,string("\t"),stmt,early_close,
			 STATE_PREFIX,0, false, false, false, true, classname, exp, log, div);
		}
	    }
	  // default case will be to punt out of loop (exit/done)

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
	  *fout << "      default: printf(\"ERROR unknown state encountered in " << classname << "_procbody\\n\");" << endl;
	  *fout << "    }" << endl;
	}

    }
  else
    {
      fatal(-1,string("Don't know how to handle opKind=%d",
		      (int)op->getOpKind()),
		      op->getToken());
	    
    }
  *fout << "}" << endl;
  *fout << "}" << endl;
}

////////////////////////////////////////////////////////////////////////
// Top level routine to create master C++ code
////////////////////////////////////////////////////////////////////////
void ccautoeslbody (Operator *op, bool *exp, bool *log, bool *div)
{
  //N.B. assumes renaming of variables to avoid name conflicts
  //  w/ keywords, locally declared, etc. has already been done
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  classname=name;
  
  list<Symbol*> *argtypes=op->getArgs();
  
  // start new output file
  string fname=name+".cpp";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname);
  *fout << "// tdfc-autoesl backend autocompiled body file" << endl;
  *fout << "// tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "// " << ctime(&currentTime) << endl;

  // some includes
  *fout << "#include \"math.h\"" << endl;
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

  // autoesl only uses functional versions
  *fout << "void " << classname << "(" << endl ;
  autoesl_functional_signature(fout,name,rsym,argtypes,"");
  ccautoeslprocbody(fout,name,op, exp, log, div);
  
  if (*exp)
  {
	  //*fout << "void exp_flopoco(double in, double *out)" << endl;
	  *fout << "data_t exp_flopoco(data_t in)" << endl;
	  *fout << "{" << endl;
	  //*fout << "\t*out = in;" << endl;
	  *fout << "\treturn in*in;" << endl;
	  *fout << "}" << endl;
  }
  
  
  if (*log)
  {
	  //*fout << "void log_flopoco(double in, double *out)" << endl;
	  *fout << "data_t log_flopoco(data_t in)" << endl;
	  *fout << "{" << endl;
	  //*fout << "\t*out = in;" << endl;
	  *fout << "\treturn in*in;" << endl;
	  *fout << "}" << endl;
  }
  
  if (*div)
  {
	  //*fout << "void log_flopoco(double in, double *out)" << endl;
	  *fout << "data_t div_flopoco(data_t in0, data_t in1)" << endl;
	  *fout << "{" << endl;
	  //*fout << "\t*out = in;" << endl;
	  *fout << "\treturn in0/in1;" << endl;
	  *fout << "}" << endl;
  }
  
  

  // close up
  fout->close();

}




