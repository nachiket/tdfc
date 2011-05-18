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
// $Revision: 1.146 $
//
//////////////////////////////////////////////////////////////////////////////
#include "parse.h"
#include "ops.h"
#include "symbol.h"
#include "expr.h"
#include "operator.h"
#include "bindvalues.h" // temporary until revamp bindvalues to change things
//  in place with map2, so can gc EvaluateGetExprWidth from here...
#include "cctype.h"
#include "cceval.h"

/***********************************************************************
Note: builtins expecting to handle here
   done
   close
   cat
   widthof
   bitsof
***********************************************************************/
using std::cout;
using std::endl;

string retimeName(string name, Expr * dexpr, bool retime, bool cuda)
{

 if(retime) {
// cout << "Nonsense!!" << endl; exit(-1);
   if (dexpr==(Expr *)NULL)
      return(string("%s_retime[0]",name));
   else if (dexpr->getExprKind()==EXPR_VALUE)
      return(string("%s_retime[%d]",name,((ExprValue *)dexpr)->getIntVal()));
   else
      // 6/22 think this is right?
      return(string("%s_retime[%s]",(const char *)name,
		  (const char *)ccEvalExpr(dexpr, retime, cuda)));
 } else {
// cout << "Yeah?!!" << endl; exit(-1);
   if (dexpr==(Expr *)NULL)
      return(string("%s",name));
   else if (dexpr->getExprKind()==EXPR_VALUE)
      return(string("%s",name,((ExprValue *)dexpr)->getIntVal()));
   else
      // 6/22 think this is right?
      return(string("%s",(const char *)name,
		  (const char *)ccEvalExpr(dexpr, retime, cuda)));
 }
}

string getWidth(Type *atype)
{
  switch (atype->getTypeKind())
    {
    case TYPE_BOOL: return(string(1));
    case TYPE_FLOAT: return(string(32));
    case TYPE_DOUBLE: return(string(64));
    case TYPE_INT:
      {
	int width=atype->getWidth();
	if (width>=0)
	  return(string("%d",width));
	else
	  {
	    Expr *esize=atype->getWidthExpr();
	    return(ccEvalExpr(esize)); 
	  }
      }
    case TYPE_FIXED:
      {
	TypeFixed *ftype=(TypeFixed *)atype;
	int iwidth=ftype->getIntWidth();
	int fwidth=ftype->getFracWidth();
	string istr;
	if (iwidth>=0)
	  istr=string("%d",iwidth);
	else
	  {
	    Expr *esize=ftype->getIntWidthExpr();
	    // debug 
	    istr=ccEvalExpr(esize); 
	  }
	string fstr;
	if (fwidth>=0)
	  fstr=string("%d",fwidth);
	else
	  {
	    Expr *esize=ftype->getFracWidthExpr();
	    fstr=ccEvalExpr(esize); 
	  }
	return(istr+"+"+fstr);
      }
    case TYPE_ARRAY:
      error(-1,"array types not allowed in widthof expression",
	    atype->getToken());
      return("0");
    default:
      error(-1,(string("type unknown in widthof expression [%s]",
		       atype->toString())),atype->getToken());
      return("0");
    }
}

string getWidth(Expr *expr)
{
  Type *atype=expr->getType();
  return(getWidth(atype));
}

string getBits(Expr *expr)
{
  Type *atype=expr->getType();
  switch (atype->getTypeKind())
    {
    case TYPE_BOOL:
    case TYPE_INT:
      if (!atype->isSigned())
	return(ccEvalExpr(expr));
      else {
	// - bitsof() is unsigned, so cast signed arg to unsigned  (EC 7/3/03)
	// - specifically, cast to type of parent, which is bitsof(expr)
	Tree *p=expr->getParent();
	assert(p && p->getKind()==TREE_EXPR &&
	       ((Expr*)p)->getExprKind()==EXPR_BUILTIN &&
	       ((OperatorBuiltin*)(((ExprBuiltin*)p)->getOp()))
					->getBuiltinKind()==BUILTIN_BITSOF);
	return( "((" + getCCvarType(expr->getParent()->getType()) +
		")(" + ccEvalExpr(expr) + "))" );
      }
    case TYPE_FIXED:
      {
	string val=ccEvalExpr(expr);
	
	// TODO:  this almost certainly isn't right, 
        //      but, we need to work out more on how we're representing
	return(val);
      }
    case TYPE_ARRAY:
      error(-1,"array types not allowed in bitsof expression",expr->getToken());
      return("0");
    default:
      error(-1,(string("type unknown in bitsof expression [")+
		expr->toString()+
		string("]")),
	    expr->getToken());
      return("0");
    }

}

string simplify_select(string name, Expr *high, Expr *low, bool ll)
{
  // ll==true iff name represents "long long" cctype
  string one=ll?"1ll":"1";	// if ll, use "long long" mask type

  if ((low->getExprKind()==EXPR_VALUE)
      && (high->getExprKind()==EXPR_VALUE))
    if ((((ExprValue *)high)->getIntVal()-((ExprValue *)low)->getIntVal())<30)
      if (((ExprValue *)low)->getIntVal()==0)
	return("(("+name+string(") & 0x%x)",
				(int)((1ll<<(((ExprValue *)high)->getIntVal()-
					     ((ExprValue *)low)->getIntVal()+1))-1)));
      else
	return("(("+name+string(">>%d) & 0x%x)",
				(int)((ExprValue *)low)->getIntVal(),
				(int)((1ll<<(((ExprValue *)high)->getIntVal()-
					     ((ExprValue *)low)->getIntVal()+1))-1)));
    else
      return("(("+name+string(">>%d) & ",(int)((ExprValue *)low)->getIntVal())
	     +"(("+one+string("<<(%d))-1))",
			      (int)(((ExprValue *)high)->getIntVal()-
				    ((ExprValue *)low)->getIntVal()+1)
			      ));
  else if (low->getExprKind()==EXPR_VALUE)
    if (((ExprValue *)low)->getIntVal()==0) // no shift
      return("("+name+ "& (("+one+"<<(" +ccEvalExpr(high) +"+1))-1))");
    else if (((ExprValue *)low)->getIntVal()==1) // only high in mask
      return("(("+name+">>"+one+")"+ " & ((1<<(" +ccEvalExpr(high) + "))-1))");
    else
      return("(("+name+string(">>%d)",(int)((ExprValue *)low)->getIntVal()) +
	     " & (("+one+"<<(" +ccEvalExpr(high) + 
	     string("-%d))-1))",(int)(((ExprValue *)low)->getIntVal()-1))
	     );
  else if (high->getExprKind()==EXPR_VALUE)
    return("(("+name+">>"+ccEvalExpr(low)+") & (("+one+
	   string("<<(%d-",(int)(((ExprValue *)high)->getIntVal()+1)) 
	   + ccEvalExpr(low)+"))-1))");
  else
    return("(("+name+">>"+ccEvalExpr(low)+") & (("+one+
	   "<<(" +ccEvalExpr(high) + 
	   "-" + ccEvalExpr(low)+"+1))-1))");

}

string ccEvalExpr(Expr *expr, bool retime, bool cuda)
{
  if (expr==(Expr *)NULL)
    {
      warn(string("ccEvalExpr: bad found null pointer"),(Token *)NULL);
      abort();
    }

  if (expr->getType()->getTypeKind()==TYPE_ANY) {
    expr->typeCheck();
    // warn(string("ccEvalExpr: generated type for ")+expr->toString());
    // - paranoid about expr that has never been type-checked  (EC 1/8/02)
    // - if we do not catch it here, bad type will propagate thru folding
    // - a better solution that obviates this step would be
    //     to mark un-type-checked trees with type=NULL, not TYPE_ANY
    //     (requires modifying parser and many tree constructors)
  }
  switch(expr->getExprKind())
    {
    case EXPR_VALUE:
      return(expr->toString()); // may need to check if this right
    case EXPR_LVALUE:
      {
	ExprLValue *lexpr=(ExprLValue *)expr;
	string name;
	if ((lexpr->getSymbol()->isStream())
	    && (((SymbolStream *)(lexpr->getSymbol()))->getDir()==STREAM_IN))
	{
	  if(cuda)
	    name=retimeName(lexpr->getSymbol()->getName(),
			  lexpr->getRetime(), retime, cuda)+string("[idx]");
	  else
	    name=retimeName(lexpr->getSymbol()->getName(),
			  lexpr->getRetime(), retime, cuda);
	}
	else
	  name=lexpr->getSymbol()->getName();
	if (lexpr->usesAllBits())
	  return(name);
	else
	  return(simplify_select(name,lexpr->getPosHigh(),lexpr->getPosLow(),
				 (getCCvarType(lexpr->getSymbol())
						      .pos("long long")>=0)));
      }
    case EXPR_CALL:
      {
	fatal(-1,"not supporting calls in exprs",expr->getToken());
	return("0");
      }
    case EXPR_BUILTIN:
      {
	ExprBuiltin *bexpr=(ExprBuiltin *)expr;
	OperatorBuiltin *bop =(OperatorBuiltin *)bexpr->getOp();
	switch(bop->getBuiltinKind())
	  {
	  case BUILTIN_CAT:
	    {
	      string bexpr_cctype=getCCvarType(bexpr->getType()), cast;
	      // - EC:  bexpr_cctype is C type of resulting concatenation;
	      //   may need to cast each arg to this type before shifting in C
	      list<Expr *> *args=bexpr->getArgs();
	      if (args->size()>0)
		{
		  Expr *exp;
		  Expr *prevexp;
		  string res=string("");
		  for (int i=0;i<(args->size()*2-1);i++)
		    res=res+string("(") ; // initial parens
		  int first=1;
		  forall(exp,*args)
		    {
		      if (first==1)
			first=0;
		      else
			res=res+cast+ccEvalExpr(prevexp, retime, cuda)+string(")")+
			  string("<<(")+ccEvalExpr(EvaluateGetWidth(exp), retime, cuda)
			  +string("))|");
		      // NOTE: evaluate get width here is probably
		      //  temporary until revamp bindvalues to use map2
		      prevexp=exp;
		      string exp_cctype = getCCvarType(exp->getType());
		      cast = (bexpr_cctype==exp_cctype)
				? string() : ("("+bexpr_cctype+")");
		    }
		  res=res+string("(")+cast+ccEvalExpr(prevexp, retime, cuda)+string("))");
		  return(res);
		}
	      else
		return("0");
	    }
	  case BUILTIN_WIDTHOF:
	    {
	      Expr *first=bexpr->getArgs()->head(); 
	      return(getWidth(first));
	    }
	  case BUILTIN_BITSOF:
	    {
	      Expr *first=bexpr->getArgs()->head(); 
	      return(getBits(first));	
	    }
	  case BUILTIN_CLOSE:
	  case BUILTIN_FRAMECLOSE:
	    {
	      error(-1,"close and frameclose should not appear in expression",
		    expr->getToken());
	      // Expr *first=bexpr->getArgs()->head();
	      // return("STREAM_CLOSE("+ccEvalExpr(first)+")");
	      return("0");
	    }
	  case BUILTIN_DONE:
	    {
	      error(-1,"done should not appear in expression",
		    expr->getToken());
	      // Expr *first=bexpr->getArgs()->head();
	      // return("STREAM_CLOSE("+ccEvalExpr(first)+")");
	      return("0");
	    }
	  default:
	    {
	      fatal(-1, string("unknown expr builtin [")+expr->toString()+"]",
		    expr->getToken());
	      break;
	    }
	  }
      }
    case EXPR_COND:
      {
	ExprCond * cexpr=(ExprCond *)expr;
	return("(("+ccEvalExpr(cexpr->getCond(), retime, cuda)+")?("+
	       ccEvalExpr(cexpr->getThenPart(), retime, cuda)+"):("+
	       ccEvalExpr(cexpr->getElsePart(), retime, cuda)+"))");
      }
    case EXPR_BOP:
      {
	ExprBop *bexpr=(ExprBop *)expr;

	// wrong thing for "." operator...
        // TODO: deal properly with fixed point construction/representation
	return("("+ccEvalExpr(bexpr->getExpr1(), retime, cuda)+
	       opToString(bexpr->getOp())+
	       ccEvalExpr(bexpr->getExpr2(), retime, cuda)+")");
      }
    case EXPR_UOP:
      {
	ExprUop *uexpr=(ExprUop *)expr;
	string ops=opToString(uexpr->getOp());
	Expr *iexpr=uexpr->getExpr();
	string istr=ccEvalExpr(iexpr, retime, cuda);
	return(("("+ops)+("("+istr)+"))");
      }
    case EXPR_CAST:
      {
	ExprCast     *cexpr = (ExprCast *)expr;
	Expr     *real_expr = cexpr->getExpr();
	Type         *ctype = cexpr->getType(),
	         *real_type = real_expr->getType();

	if (real_type->getTypeKind()==TYPE_ANY) {
	  real_type=real_expr->typeCheck();
	  // warn(string("ccEvalExpr: generated type for cast arg ")+
	  //      real_expr->toString());
	  // - paranoid about expr that has never been type-checked (EC 1/8/02)
	  // - if we do not catch it here, bad type will propagate thru folding
	  // - a better solution that obviates this step would be
	  //     to mark un-type-checked trees with type=NULL, not TYPE_ANY
	  //     (requires modifying parser and many tree constructors)
	}

	string     c_cctype = getCCvarType(ctype),
	        real_cctype = getCCvarType(real_type);

	if (c_cctype!=real_cctype)
	  return( "(("+c_cctype+")("+ccEvalExpr(real_expr, retime, cuda)+"))" );
	else
	  return( ccEvalExpr(real_expr, retime, cuda) );

	// EC:  My code above generalizes Andre's code below.
	//      Above code assumes that type checking was done,
	//        i.e. cast is valid; we blindly do a corresponding C cast.
	//      Warning:  code should (but does not) mask a narrowing cast;
	//        fortunately, narrowing casts are generated only by
	//        Andre's const-folding of '%', which itself applies the mask.
	//        Narrowing casts can appear in TDF source but are discouraged.

	// // only thing I need to do is deal with unsigned->signed upgrade
	// 
	// if (ctype->isSigned() && !real_type->isSigned())
	//   return(string("((")+getCCvarType(ctype)+string(")")+
	//          ccEvalExpr(real_expr)+string(")"));
	// else
	//   return(ccEvalExpr(real_expr));
      }
    default:
      fatal(-1,(string("ccEvalExpr: unknown Expression type [")+expr->toString()+
		string("]")),
	    expr->getToken());
      return("0");
    }

}
