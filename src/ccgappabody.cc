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
#include "ccGappaStmt.h"
#include "ccprep.h"
#include "ccmem.h"
#include "instance.h" // blah, just for unique_name()
#include "cccopy.h" 
#include "cctestif.h"

int ccwritegappa(ofstream *fout, list<Symbol*> *argtypes,
				Operator *op, string classname, string type,
				string precision, string *exp );

/***********************************************************************
Note: This will use a simple round-robin scheduling technique..
***********************************************************************/    

using leda::list_item;
using leda::dic_item;
using std::ofstream;

int gappa_notation(ofstream *fout,
			    list<Symbol*> *argtypes,
			    Operator *op,
			    string classname,
			    string *exp,
			    int fixed_bits,
			    bool uncertain)
{
	// fixed point size
	*fout << "@fx = fixed<-"<< fixed_bits << ",ne>;\n" << endl; 
	
	// variables and parameters with different precisions 
  
  int if_nb;
  
  ccwritegappa(fout, argtypes, op, classname, "_m",  "float<ieee_64,ne>", exp);
  // capture uncertainty in parameters with _u..
  if(uncertain) {ccwritegappa(fout, argtypes, op, classname, "_u",  "float<ieee_64,ne>", exp);}

  ccwritegappa(fout, argtypes, op, classname, "_dbl",  "float<ieee_64,ne>", exp);
  if(false) {
  	ccwritegappa(fout, argtypes, op, classname, "_fl",  "float<ieee_32,ne>", exp);
	ccwritegappa(fout, argtypes, op, classname, "_cuda32",  "float<cuda_32,ne>", exp);
  }
  if_nb = ccwritegappa(fout, argtypes, op, classname, "_fx",  "fx", exp);

  return if_nb;
}

int ccgappaprocrun(ofstream *fout, string classname, Operator *op, string type,string precision, string *exp )
{
  //cout << "\n\n new procrun \n\n " << endl;	
  int if_nb = 0;
  
  //bool if_stmt_present = false;
  
  if (op->getOpKind()==OP_COMPOSE)
    {
      *fout << "  printf(\"proc_run should never be called for a compose operator!\\n\");" << endl;
    }
  else if (op->getOpKind()==OP_BEHAVIORAL)
    {
      OperatorBehavioral *bop=(OperatorBehavioral *)op; // cast
      //dictionary<string,State*>* states=bop->getStates();
      //dic_item item;
	  
	  
	  SymTab *symtab=bop->getVars();
      list<Symbol*>* lsyms=symtab->getSymbolOrder();
      list_item item2;
	  
      forall_items(item2,*lsyms)
		{
			Symbol *sum=lsyms->inf(item2);
			SymbolVar *asum=(SymbolVar *)sum;
			Expr* val=asum->getValue();
			if (val!=(Expr *)NULL)
			{
				*fout <<  asum->getName() << type ;
				*fout << "= " << precision << "(" << ccEvalExpr(EvaluateExpr(val), false) ;
				*fout << ");" << endl;
			}
		}
		
	  	
	  ccgappadfgprocrun(fout, classname, bop, type, precision, &if_nb, exp);
	  
	  

	/*  list<OperatorBehavioral*> *bop_list = new list<OperatorBehavioral*>();
	  bop_list->push(bop);
	  //list<DFGraph>* graph_list = Create_DFG(op, bop);
	  list<DFGraph>* graph_list = Create_DFG2(op, bop_list);
	  
	  node n;
	  DFGraph graph;
	  forall(graph, *graph_list)
	  {
		  forall(n, graph->all_nodes())
		  {
			  DFNode* dfn=graph->inf(n);
			  cout << "node name is "<< dfn->getName()<< endl;
		  }
	  }
	  */  
//
    
    /*
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
			{
				*fout <<  asum->getName() ;
				*fout << "=" << ccEvalExpr(EvaluateExpr(val), false) ;
				*fout << ";" << endl;
			}
		}
	
		// we do not need to declare the variables in gappa
	  int ocnt = 0;
	  int *early_close=new int[ocnt];
	  
      int num_states=states->size();
      if (num_states>1)
	  {
		  // gappa does not handle multiple dtate
		  warn("Gappa does not handle multiple state");
		  *fout << "    switch(state) {" << endl;
	  }
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
			
				Stmt* stmt;			
				
				forall(stmt,*(acase->getStmts()))
				{
					bool if_detected =  false;
					int nb_nesting = 0;
					detect_if (stmt, &if_detected, &nb_nesting);	
					cout << "if detected  : " << if_detected << endl;	
					cout << "nb nesting = 	" << nb_nesting << endl;	
					
					if (if_detected)
						if_stmt_present = true;
					
					 if (!if_detected)
						ccGappaStmt(fout,string(" "),stmt,early_close,
							STATE_PREFIX,0, false, false, false,true, type, precision, classname); // 0 was default for ccStmt.h
					 else if (nb_nesting <=1)
					 {
						 int nb_var = 0;
						 count_variable(stmt, &nb_var);
						 
						 string **table_var = new string*[nb_var];
						 for (int i=0;i<nb_var;i++)
							table_var[i]=new string[2];
						 
						ccGappaIfStmt(fout, stmt, type, precision, nb_var, table_var); // 0 was default for ccStmt.h
						
						for (int j =0; j<nb_var;j++)
						{
							if (table_var[j][0] != "")
							*fout << table_var[j][0] << type << " " << precision << " = " << table_var[j][1] << ";" << endl;
						}
				
						for (int i=0;i<nb_var;i++)
							delete table_var[i];
				
						delete[] table_var;
						
					 }
					 
					// added false to remove retiming						
				}
				
				//cout << "value of if_nb after execution of ccGappaIfStmt : " << if_nb << endl;	     
			}
			// default case will be to punt out of loop (exit/done)

			// close the nesting brackets.
			*fout << "  ";
			for (; numNestings>1; numNestings--) {
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
		*/
//
    }
  else
    {
      fatal(-1,string("Don't know how to handle opKind=%d",
		      (int)op->getOpKind()),
		      op->getToken());
	    
    }  
    
    /*if (if_stmt_present)
		return 1;
	else 
		return 0;
		*/
	return if_nb;
}

int ccwritegappa(ofstream *fout, list<Symbol*> *argtypes,
			    Operator *op,string classname,string type,string precision, string *exp )
{
	Symbol *sym;
	forall(sym,*argtypes)
    {
      if(sym->isParam()) 
      {
		  if (type == "_u")
		  {
		        // for uncertain parameters, write out dummy constants
			*fout << sym->getName() << type <<" = "<< precision << "(" << sym->getName() << type <<  "_ );"<<endl;
		  }
		  else 
		  {
			  //cout << "in gappa body number = " << ((SymbolVar*)sym)->getNumber() << endl ; 
			  if (((SymbolVar*)sym)->getNumber() != "")
				*fout << sym->getName() << type <<" = "<< precision << "(" << ((SymbolVar*)sym)->getNumber() <<  ");"<<endl;			  
			  else
				*fout << sym->getName() << type <<" = "<< precision << "(" << sym->getName() <<  ");"<<endl;
		  }
	  }
	  
	  if (sym->isStream())
	  {
		  
		  
		  if (((SymbolStream*)sym)->getDir()==STREAM_IN)
		  {
			  
			  if (sym->getType()->toString() == "boolean")
				 *fout << sym->getName() << type <<" = "<< "fixed<-1,ne>" << "(" << sym->getName() <<  ");"<<endl;
			  else
			  {
				  if (type != "_m")
				  {
					  *fout << sym->getName() << type <<" = "<< precision << "(" << sym->getName() <<  ");"<<endl;
				  }
				  else 
				  {
					  *fout << sym->getName() << type <<" = "<< precision << "(" << sym->getName() <<  ");"<<endl;			  
				  }
			  }
		  }
	  }
		
    }
    int if_nb = ccgappaprocrun(fout,classname,op, type, precision, exp);
    *fout << endl;
    
    return if_nb;
}

void ccgappalogical(ofstream *fout, list<Symbol*> *argtypes, Operator *op, int if_nb, bool OO, string exp, bool uncertain, int uncertain_perc_exp)
{
	int nbinput = 0; // amount of input streams
	int nboutput = 0; // amount of output streams
	Symbol *sym;
	string indent = "\t\t";
		
	forall(sym,*argtypes)
    {
		if (sym->isStream())
		{  
			if (((SymbolStream*)sym)->getDir()==STREAM_IN)
				nbinput++;
			else if (((SymbolStream*)sym)->getDir()==STREAM_OUT)
				nboutput++;
		}
	}
		  
	*fout << "{\n" << endl;
	
	int n = 0; // temporary variable to determine if we have written all
				// inputs or outputs
	
	
	forall(sym,*argtypes)
    {
		if (sym->isStream())
		{  
		  if (((SymbolStream*)sym)->getDir()==STREAM_IN)
		  {
			  *fout << indent;
			  
			  if(sym->getType()->toString() == "boolean" && OO)
				*fout << "(" << sym->getName() << " in [0,0] \\/ " << sym->getName() << " in [1,1])" ;
			  else if (sym->getType()->toString() == "boolean" && !OO)
				*fout << sym->getName() << " in [0,1] " ;
			  else 
				*fout << sym->getName() + " in " + ((SymbolStream*)sym)->getRange();
			  
			  n++;
		      *fout << " /" << "\\ " << endl;
		  }
		} else if (uncertain && sym->isParam()) {
			
			*fout << indent;
			// handle user-supplied uncertainty...
			if (((SymbolVar*)sym)->getNumber() != "") {
				double limit = strtod(((SymbolVar*)sym)->getNumber(), NULL);
				// do something with this... ((SymbolVar*)sym)->getNumber();
				*fout << sym->getName()<< "_u_" <<" in "
					<< "[" << std::setprecision (15) << limit-limit*pow(10,uncertain_perc_exp)
					<< "," << std::setprecision (15) << limit+limit*pow(10,uncertain_perc_exp)
					<< "]";	  
		      		*fout << " /" << "\\ " << endl;
			}
		}
	}
	n = 0;
	
	*fout << indent << exp << endl;
	
	forall(sym,*argtypes)
    {
		if (sym->isStream())
		{  
		  if (((SymbolStream*)sym)->getDir()==STREAM_OUT)
		  {
			  if ( ((SymbolStream*)sym)->getRange() != "" )
			  {
				*fout << indent << sym->getName() + "_m in " + ((SymbolStream*)sym)->getRange() + "/\\ " << endl;
				*fout << indent << sym->getName() + "_dbl in " + ((SymbolStream*)sym)->getRange() + "/\\ " << endl;
  				if(false) {
					*fout << indent << sym->getName() + "_fl in " + ((SymbolStream*)sym)->getRange() + "/\\ " << endl;
					*fout << indent << sym->getName() + "_cuda32 in " + ((SymbolStream*)sym)->getRange() + "/\\ " << endl;
				}
				*fout << indent << sym->getName() + "_fx in " + ((SymbolStream*)sym)->getRange() + "/\\ " << endl;
			  }
			  *fout << indent << "(" << sym->getName() + "_m >= 0x1p-53 \\/ "  ; // are never equal to zero
			  *fout << sym->getName() + "_m <= - 0x1p-53)"; // since we want to have the relative error
															// and therefore divide by the output.
															// therefore we make sure that it is greater than the
			  
			 												// smallest double value 
			  n++;
			  if ( n != nboutput)
			    *fout << " /" << "\\ " << endl;
			  else 
			  {	
				  if (if_nb > 0)
					{
						*fout << " /" << "\\ " << endl;
					}
				  else
					*fout << endl;
			  }
		  }
		}
	}
	
	
	for (int i = 0; i < if_nb ; i++)
	//if (if_nb == 1)
	{
		//int i = 0;
		*fout << indent;
		if (!OO)
			*fout << "cond" << i << " in [0,1]" ;
		else
			*fout << "(cond" << i << " in [0,0] \\/ cond" << i << " in [1,1])" ;
		  if ( i != if_nb-1)
			*fout << " /" << "\\ " << endl;
		  else 	
			*fout << endl;
		  

	}
	
	n = 0;
	*fout << "\n" + indent + "->"  << endl;
	
	
	
	forall(sym,*argtypes)
    {
		if (sym->isStream())
		{  
			if (((SymbolStream*)sym)->getDir()==STREAM_OUT)
			{
				n++; // we ask the question regarding the relative error on all of the outputs
				*fout << indent;
				
  			    if(!uncertain) {
			    
				*fout << "(" + sym->getName() + "_dbl-" + sym->getName() + "_m"
			          +")/" + sym->getName() +"_m in ?"; 
			          
			    	*fout << " /" << "\\ "  << endl; 
			    	*fout << indent; 
			          
				*fout << "(" + sym->getName() + "_fx-" + sym->getName() + "_m"
			          +")/" + sym->getName() +"_m in ?"; 
			    
			    	*fout << " /" << "\\ "  << endl; 
			    	*fout << indent; 
			    }
			    
  			    if(uncertain) {
				*fout << "(" + sym->getName() + "_u-" + sym->getName() + "_m"
			          +")/" + sym->getName() +"_m in ?"; 
			           
			    	*fout << " /" << "\\ "  << endl; 
			    	*fout << indent;			    

//				*fout << "(" + sym->getName() + "_dbl-" + sym->getName() + "_u"
//			          +")/" + sym->getName() +"_u in ?"; 
//			           
//			    	*fout << " /" << "\\ "  << endl; 
//			    	*fout << indent;			    
//				
//				*fout << "(" + sym->getName() + "_fx-" + sym->getName() + "_u"
//			          +")/" + sym->getName() +"_u in ?"; 
//			           
//			    	*fout << " /" << "\\ "  << endl; 
//			    	*fout << indent;			    

			    }

  			    if(false) {
				*fout << "(" + sym->getName() + "_fl-" + sym->getName() + "_m"
			          +")/" + sym->getName() +"_m in ?"; 
			           
			    *fout << " /" << "\\ "  << endl; 
			    *fout << indent;			    

			    	*fout << "(" + sym->getName() + "_cuda32-" + sym->getName() + "_m"
			          +")/" + sym->getName() +"_m in ?"; 
			           
			    	*fout << " /" << "\\ "  << endl; 
			    	*fout << indent;			    
			    }
			    
			    
			    // we also add the question for absolute error
			    
			    *fout << "(" + sym->getName() + "_m-" + sym->getName() + "_fx) in ?";

			    *fout << " /" << "\\ "  << endl; 
			    *fout << indent; 
			    
			    *fout << "(" + sym->getName() + "_m-" + sym->getName() + "_dbl) in ?";

  			    if(false) {

			    	*fout << " /" << "\\ "  << endl; 
			    	*fout << indent; 
			    
			    	*fout << "(" + sym->getName() + "_m-" + sym->getName() + "_fl) in ?";			    
			    
			    	*fout << " /" << "\\ "  << endl; 
			    	*fout << indent; 
			    
			    	*fout << "(" + sym->getName() + "_m-" + sym->getName() + "_cuda32) in ?";			    
			    }
			    
  			    if(uncertain) {

			    	*fout << " /" << "\\ "  << endl; 
			    	*fout << indent; 
			    
			    	*fout << "(" + sym->getName() + "_m-" + sym->getName() + "_u) in ?";			    
			    }
			    
			    if (n != nboutput)      
					*fout << " /" << "\\ "  << endl; 
			}
	    }
	}
	*fout << "\n}" << endl;
	
}

bool ccCheckRanges(Operator *op)
{
	list<Symbol*> *argtypes=op->getArgs(); // get input/output arguments of the operator
	Symbol *sym;
	forall(sym,*argtypes)
    {
		if (sym->isStream())
		{  
		  if (((SymbolStream*)sym)->getDir()==STREAM_IN)
		  {
			   
			  if (((SymbolStream*)sym)->getRange() == "" && sym->getType()->toString() != "boolean")
			  {
				  cout << "error : the range for input \"" << sym->getName() << "\" was not specified" << endl;
				  return false;
			  }
		  }
	    }
	    
	    if (sym->isParam())
		{  
			if (((SymbolVar*)sym)->getNumber() == "")
			{
				 cout << "error : the value for parameter \"" << sym->getName() << "\" was not specified" << endl;
				  return false;
			}
		}
	}
	return true;
}


//
////////////////////////////////////////////////////////////////////////
// Helene Martorell: Top level routine to create master gappa++ code
void ccgappabody (Operator *op, bool OO, bool uncertain, int fixed_bits, int uncertain_perc_exp)
{
  //N.B. assumes renaming of variables to avoid name conflicts
  //  w/ keywords, locally declared, etc. has already been done
  
  string name=op->getName();
  Symbol *rsym=op->getRetSym();
  string classname;
  if (noReturnValue(rsym)) // if rsym == NULL or rsym has kind TYPE_NONE
      classname=name;
  else
    classname=NON_FUNCTIONAL_PREFIX + name; //NON_FUNCTIONAL_PREFIX = "nonfunc_"
  list<Symbol*> *argtypes=op->getArgs(); // get input/output arguments of the operator
  // start new output file
  string fname=name;
  if (!OO)
	fname+="01";
	
  fname+=".g";
  // how convert string -> char * ?
  ofstream *fout=new ofstream(fname); // construc an object and open a file with the name fname
  *fout << "# tdfc-gappa backend autocompiled body file" << endl;
  *fout << "# tdfc version " << TDFC_VERSION << endl;
  time_t currentTime;
  time (&currentTime);
  *fout << "# " << ctime(&currentTime) << endl;  
  *fout << endl;

  string exp ="";

  // Notation for making the script readable
  int if_nb = gappa_notation(fout,argtypes, op, classname, &exp, fixed_bits, uncertain); // defines the fixed point precision and all
						//different variables associated with different 
						//precisions
  *fout <<  endl ;

  // logical fomula gappa should proof
 
  ccgappalogical(fout, argtypes, op, if_nb, OO, exp, uncertain, uncertain_perc_exp);
  *fout << endl;

  // hint to help gappa find the proof
  // => for now no particular hint
  // maybe to be added later

  *fout << "#" << classname << endl;


  // close up
  fout->close();

}
