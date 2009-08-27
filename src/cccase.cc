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
// SCORE TDF compiler:  preprocess case information for C++ codegen
// $Revision: 1.105 $
//
//////////////////////////////////////////////////////////////////////////////
#include "compares.h"
#include <LEDA/core/array.h>
#include "misc.h"
#include "symbol.h"
#include "state.h"

using leda::array;

int caseorder(StateCase * const &A,  StateCase * const &B)
{
  // first order by number of streams consumed from
  //  then order by total number of streams mentioned in case (so include eos)
  //  not currently doing anything about multiple tokens conumed
  int Aconsume=0;
  int Aeos=0;
  int Bconsume=0;
  int Beos=0;
  InputSpec *ispec;
  forall (ispec,*(A->getInputs()))
    {
      if (ispec->isEosCase())
	Aconsume++;
      else 
	Aeos++;
    }
  forall (ispec,*(B->getInputs()))
    {
      if (ispec->isEosCase())
	Bconsume++;
      else 
	Beos++;
    }
  if (Aconsume>Bconsume)
    return(-1);
  else if (Bconsume>Aconsume)
    return(1);
  else // must be equal
    if (Aeos>Beos)
      return(-1);
    else if (Beos>Aeos)
      return(1);
    else // all equal
      // return(-1);	// changed to 0, EC 12/7/99
      return(0);
}

array<StateCase *> *caseSort(set<StateCase*>* scases)
{
  array<StateCase*> *result=new array<StateCase*>(scases->size());
  StateCase *scase;
  int i=result->low();
  forall(scase,*scases)
    {
      (*result)[i]=scase;
      i++;
    }
  result->sort(&caseorder);
  return(result);
}

array<Symbol*> *allCaseIns(array<StateCase*> *cases)
{

  set<Symbol*>* inset=new set<Symbol*>();
  for (int i=cases->low();i<=cases->high();i++)
    {
      InputSpec *ispec;      
      StateCase *acase=(*cases)[i];
      forall (ispec,*(acase->getInputs()))
	inset->insert(ispec->getStream());
    }
  Symbol *asym;
  array<Symbol*>* result=new array<Symbol*>(inset->size());
  int i=result->low();
  forall (asym,*inset)
    {
      (*result)[i]=asym;
      i++;
    }
  result->sort();
  return(result);

}

array<int> *inFirstUsed(array<Symbol*> *caseIns,
			array<StateCase*> *cases)
{

  array<int>* result;

  if (caseIns->size()==0)
    return(new array<int>(0));


  result=new array<int>(caseIns->low(),caseIns->high());
  for (int i=result->low();i<=result->high();i++)
    (*result)[i]=-1;
  for (int i=cases->low();i<=cases->high();i++)
    {
      InputSpec *ispec;
      StateCase *acase=(*cases)[i];
      forall (ispec,*(acase->getInputs()))
	{
	  int loc=caseIns->binary_search(ispec->getStream());
	  if (loc<result->low())
	    fatal(-1,"inconsistency in inFirstUsed search",
		  ispec->getStream()->getToken());
	  if ((*result)[loc]==-1)
	    (*result)[loc]=i;
	}
    }
  return(result);
  
}

