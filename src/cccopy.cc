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
// SCORE TDF compiler:  generate code for supporting special copy stream operator
// $Revision: 1.85 $
//
//////////////////////////////////////////////////////////////////////////////
#define SCORE_COPY_NAME "ScoreCopy"
#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include <LEDA/core/set.h>
#include "type.h"
#include "operator.h"
#include "expr.h"

using std::ofstream;
using std::endl;
using leda::set;
using leda::string;

// need this set so I only write one definition for each kind of copy
static set<string> *page_set=(set<string> *)NULL;
string ccCopyPageName(OperatorBuiltin *op)
{
  // args, type
  list<Symbol *> *args=op->getArgs();
  int size=args->size();
  Type *t=(args->front())->getType();
  if (t->getTypeKind()==TYPE_BOOL)
    return(string("copy%d_bool",size));
  else if (t->getTypeKind()==TYPE_INT)
    {
      long long width=0;
      if (t->getWidth() >= 0)
	width=t->getWidth();
      else 
	{
	  Expr *we=t->getWidthExpr();
	  if (we->getExprKind()==EXPR_VALUE)
	    width=((ExprValue *)we)->getIntVal();
	  else
	    fatal(-1,"non-constant width in copy type",
		  (args->front())->getToken());
	}
      if (t->isSigned())
	return(string("copy%d_sint%d",size,width));
      else
	return(string("copy%d_uint%d",size,width));
    }
  else if (t->getTypeKind()==TYPE_FIXED)
    {
      long long iwidth=0;
      long long fwidth=0;
      TypeFixed *tf=(TypeFixed *)t;
      if (tf->getIntWidth() >= 0)
	iwidth=tf->getIntWidth();
      else 
	{
	  Expr *we=tf->getIntWidthExpr();
	  if (we->getExprKind()==EXPR_VALUE)
	    iwidth=((ExprValue *)we)->getIntVal();
	  else
	    fatal(-1,"non-constant width in copy type",
		  (args->front())->getToken());
	}
      if (tf->getFracWidth() >= 0)
	fwidth=tf->getFracWidth();
      else 
	{
	  Expr *we=tf->getFracWidthExpr();
	  if (we->getExprKind()==EXPR_VALUE)
	    fwidth=((ExprValue *)we)->getFracVal();
	  else
	    fatal(-1,"non-constant width in copy type",
		  (args->front())->getToken());
	}
      if (tf->isSigned())
	return(string("copy%d_sfixed%d_%d",size,iwidth,fwidth));
      else
	return(string("copy%d_ufixed%d_%d",size,iwidth,fwidth));
    }
  else
    {
      fatal(-1,string("unknown type %d in copy",(int)t->getTypeKind()),
	    op->getToken());
      return(string("FUBAR"));
    }
}

// this resets set of definitions known (written to file)
void ccCopyPageInit()
{
  if (page_set==(set<string> *)NULL)
    page_set=new set<string>();
  else
    {
      string str;
      delete page_set;
      page_set=new set<string>();
    }
}

void ccCopyPage(ofstream *fout, OperatorBuiltin *op)
{

  string cname=ccCopyPageName(op);
  if (page_set->member(cname))
    return; // don't need to define again

  // TODO: Actually define page
  *fout << "// MISSING page definition for " << cname << endl;
    
  page_set->insert(cname);  
  
}
