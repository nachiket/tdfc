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
// SCORE TDF compiler:  operators
// $Revision: 1.43 $
// $Date: 1999/07/16 20:58:03 $
//
//////////////////////////////////////////////////////////////////////////////

#include "operator.h"
#include "rateTable.h"
#include "cccase.h"


using std::cout;
using std::endl;
using leda::dic_item;

///////////////////////////////////////////////////////////////////////////

// build our own lists

int countIO(Operator *op) {
  
  int numStreams=0;
  list<Symbol*> *args = op->getArgs();
  Symbol *s;
  forall (s,*args)
    if (s->getSymKind() == SYMBOL_STREAM)
      numStreams++;
  
  return numStreams;
}

int pairsEq(Pair x, Pair y) {

  return ((x.first() == y.first()) ||
	  (x.second() == y.second()));

}

void printPair(Pair x) {
  cout << "(" << x.first() << "," << x.second() << ")";
}

RateTable::RateTable(const string &name_i, Operator *input_op)
  :name(name_i),noStreams(countIO(input_op)),
   ioArgs(noStreams),isInput(noStreams),rates(noStreams,noStreams)
{

  assert(name);
  if (input_op->getOpKind()==OP_COMPOSE)
    {
      fatal(-1,string("rateTable construction should never be called for a compose operator!"));
    }
  else if (input_op->getOpKind()==OP_BEHAVIORAL)
    {
      buildIOList(input_op->getArgs());
      buildTable((OperatorBehavioral *)input_op);
    }
  else 
    {
      fatal(-1,string("Don't know how to handle opKind=%d",
		      input_op->getOpKind()),
	    input_op->getToken());
    }


}

void RateTable::buildIOList(list<Symbol*>* orgList)
  // build an initial list of I/O's with input/output
  // labels in the isInput array
{
  int index=0;

  Symbol *s;

  forall (s,*orgList)  
    if (s->getSymKind() == SYMBOL_STREAM) {
      ioArgs[index] = s;
      SymbolStream *syms = (SymbolStream *)s;
      if (syms->getDir() == STREAM_IN) 
	isInput[index++] = 1;
      else
	isInput[index++] = 0;
    }
  
}

void RateTable::buildTable(OperatorBehavioral *p)
  // build a table of rates where entry (i,j) is a tuple
  // (n,m) where n tokens from i are produced/consumed for
  // every m tokens from j that are produced/consumed
{
  // this will be made smarter in due time
  for (int index = 0; index < noStreams; index++) 
    for (int subindex = 0; subindex < noStreams; subindex++)
      calculate(index, subindex, p);

}

void RateTable::calculate(int a, int b, OperatorBehavioral *bop)
  // calculates every entry of the rates table
{

  Pair stateResult,lastStateResult;
  int first = 1;
  int dynamic = 0;

  dictionary<string,State*>* states = bop->getStates(); 
  
  if (a == b) {
    Pair q(1,1);
    rates(a,b) = q;
  }
  else if (a > b) {
    Pair q(rates(b,a).second(),rates(b,a).first());
    rates(a,b) = q; // already been calculated
  }
  else { // begin "hard-work" case
    dic_item item;
    forall_items(item, *states) 
      {
	if (first) {
	  lastStateResult = calculateState(a,b,states->inf(item));
	  first = 0;
	  // states->inf(item)  returns state pointer
	}
	else {
	  stateResult = calculateState(a,b,states->inf(item));
	  if (!pairsEq(stateResult,lastStateResult)) {
	    dynamic = 1;
	  } // if (stateResult != lastStateResult) 	    
	} // else
      } // forall_items
    if (dynamic) {
      Pair q(0,0);
      rates(a,b) = q;
    } else {
      rates(a,b) = stateResult;
    }
  } // end "hard-work" case
  
}

Pair RateTable::calculateState(int a, int b, State* st)
  // calculates token ratio for a particular state in a operator
{

  Pair stateResult,lastStateResult;
  int first = 1;
  int dynamic = 0;

  array<StateCase*>* cases = caseSort(st->getCases());
  for (int index=cases->low(); index<=cases->high(); index++) 
    {
      StateCase *thisCase=(*cases)[index];
      calculateStateCase(a,b,thisCase);
    }
  
}


Pair RateTable::calculateStateCase(int a, int b, StateCase* stc)
  // calculates token ratio for a particular case in a state
{
   
  int p=0,q=0;
  InputSpec *ispec;

  // so perhaps it would be better not to have any of this
  // "isInput" business and have ispec search for a and b.
  // but in later editions I would like to have prior know-
  // ledge of whether things are inputs are not.

  if (isInput[a]) {
    forall (ispec, *(stc->getInputs()))
      {
	if (ispec->getStream() == ioArgs[a]){
  	  Expr* nT;
	  nT = ispec->getNumTokens();	
	  if (nT->getExprKind() != EXPR_VALUE)
	    fatal(-1,"Can't handle non EXPR_VALUE expr in Inputspec");	
	  p = ((ExprValue *)nT)->getIntVal();
	}
      }
  } else { // assume either input or output
    Stmt *stmt;
    forall (stmt, *(stc->getStmts()))
      {
	if (stmt->getStmtKind() == STMT_ASSIGN) {
	  Symbol *asym=((StmtAssign *)stmt)->getLValue()->getSymbol();
	  if (asym == ioArgs[a]){
	    p++; // not technically correct because if p>1 that would
	    // actually be flawed code
	  }
	}
      }

  };

  if (isInput[b]) {
    forall (ispec, *(stc->getInputs()))
      {
	if (ispec->getStream() == ioArgs[b]){
	  Expr* nT;
	  nT = ispec->getNumTokens();	
	  if (nT->getExprKind() != EXPR_VALUE)
	    fatal(-1,"Can't handle non EXPR_VALUE expr in Inputspec");	
	  q = ((ExprValue *)nT)->getIntVal();
	}
      }
  } else {
    Stmt *stmt;
    forall (stmt, *(stc->getStmts()))
      {
	if (stmt->getStmtKind() == STMT_ASSIGN) {
	  Symbol *asym=((StmtAssign *)stmt)->getLValue()->getSymbol();
	  if (asym == ioArgs[b]){
	    q++; // see above comment!
	  }
	}
      }
  }

}

void RateTable::printout() {

  // printing out first line
  cout << name << endl;

  // printing out second line
  cout << '\t' ;

  for (int index=0; index<noStreams; index++) {
    cout << ioArgs[index]->getName() << '\t';
  }

  cout << endl;

  // and here come the rest

  for (int index=0; index<noStreams; index++) {
    cout << ioArgs[index]->getName() << '\t';
    for (int subIndex=0; subIndex<noStreams; subIndex++) {
      printPair(rates(index,subIndex));
    }
    cout << endl;
  }
}
