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
// $Revision: 1.130 $
//
//////////////////////////////////////////////////////////////////////////////
#include <time.h>
#include <iostream>
#include <fstream>
#include <LEDA/core/string.h>
#include "misc.h"
#include "type.h"
#include "symbol.h"
#include "bindvalues.h"
#include "cctypes.h"
#include "cctype.h"
#include "cceval.h"

int ccTypeCausesInstance(Type *atype)
{
  if (atype->getTypeKind()==TYPE_ARRAY)
    return(0);
  else return(1);
}

int ccParamCausesInstance(Symbol *sym)
{
  if (!sym->isParam())
    return(0);
  else
    return(ccTypeCausesInstance(((SymbolVar *)sym)->getType()));
}

int ccCountParameters(list<Symbol*> *argtypes)
{
  int cnt=0;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (ccParamCausesInstance(sym))
	{
	  cnt++;
	}
    }
  return(cnt);
}

unsigned long ccParameterLocs(list<Symbol*> *argtypes)
{
  int loc=0;
  unsigned long res=0;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (ccParamCausesInstance(sym))
	res|=(1<<loc);
      loc++;
    }
  return(res);
}

string getCCobjConvert (Symbol *rsym,string name)
{
  if (rsym->isStream())
    return("(("+ getCCtype(rsym)+")"+
	   (TYPE_STREAM_OBJ_CONVERT+("("+name))+"))");
  else if (rsym->isArray())
    return("(("+ getCCtype(rsym)+")"+			// eylon: added cast
	   (TYPE_SEGMENT_OBJ_CONVERT+("("+name))+"))");	//        10/12/99
  else
    return(name);
}

string getCCidConvert (Symbol *rsym,string name)
{
  if (rsym->isStream())
    return((TYPE_STREAM_ID_CONVERT+("("+name))+")");
  else if (rsym->isArray())
    return((TYPE_SEGMENT_ID_CONVERT+("("+name))+")");
  else
    return(name);
}

string getCCvarType (Type *stype)
{
  TypeKind tk=stype->getTypeKind();
  if (tk==TYPE_ARRAY)
    {
      TypeArray *atype=(TypeArray *)stype;
      Type *etype = atype->getElemType();
      switch (etype->getTypeKind())
	{
	case (TYPE_BOOL): return(TYPE_SCORE_SEGMENT_BOOLEAN);
	case (TYPE_FIXED): 
	  {
	    if (etype->isSigned())
	      return(TYPE_SCORE_SEGMENT_SIGNED_FIXED);
	    else
	      return(TYPE_SCORE_SEGMENT_UNSIGNED_FIXED);
	  }
	case (TYPE_INT): 
	  {
	    if (etype->isSigned())
	      return(TYPE_SCORE_SEGMENT_SIGNED);
	    else
	      return(TYPE_SCORE_SEGMENT_UNSIGNED);
	  }
	default:
	  {
	    fatal(-1,
		  string("internal error/getCCtype: not expecting type=%d as element type for array",(int)etype->getTypeKind()),
		  stype->getToken());
	    return("FUBAR");
	  }
	}
    }
  if (tk==TYPE_BOOL)
  {
    return (string("int"));
  }
  else if (tk==TYPE_FLOAT)
  {
    return (string("float"));
  }
  else if (tk==TYPE_DOUBLE) 
  {
    return (string("double"));
  }
  else if (tk==TYPE_INT)
    {
      int width=stype->getWidth();
      if ((width>=0) && (width<32) && (stype->isSigned()))
	return(string("long"));
      else if ((width>=0) && (width<33) && (!stype->isSigned()))
	return(string("unsigned long"));
      else if (stype->isSigned())
	return(string("long long"));
      else
	return(string("unsigned long long"));
    }
  else if (tk==TYPE_FIXED)
    {
      // TODO: work out fixed rep and fix this...

      TypeFixed *ftype=(TypeFixed *)stype;
      int iwidth=ftype->getIntWidth();
      int fwidth=ftype->getFracWidth();
      if ((iwidth>=0) && (fwidth>=0) && (iwidth+fwidth<32) && 
	  (stype->isSigned()))
	return(string("long"));
      else if ((iwidth>=0) && (fwidth>=0) && (iwidth+fwidth<33) && 
	  (!stype->isSigned()))
	return(string("unsigned long"));
      else if (stype->isSigned())
	return(string("long long"));
      else 
	return(string("unsigned long long"));

    }
  else
    {
      error(-1,string("unhandled type %d",(int)tk),stype->getToken());
      return("void");
    }
  
}

string getCCvarType (Symbol *rsym)
{
  Type *stype=rsym->getType();
  return(getCCvarType(stype));
}

int compoundVar (Symbol *rsym)
{
  // placeholder until I figure out how to identify something which is compound
  return(0);
  
}

string getCCStreamType(Symbol *rsym)
{
  return(getCCStreamType(rsym,0));
}

string getCCStreamType(Symbol *rsym,int treat_as_stream)
{
  if (rsym->isStream() || treat_as_stream)
    {
      Type *stype=rsym->getType();
      TypeKind tk=stype->getTypeKind();
      if (tk==TYPE_BOOL)
	return ("new ScoreStreamType(0,1)");
      else 
	{
	  string prefix;
	  if (stype->isSigned())
	    prefix=string("new ScoreStreamType(1,");
	  else 
	    prefix=string("new ScoreStreamType(0,");
	  if (tk==TYPE_INT)
	    {
	      int width=stype->getWidth();
	      
	      if (width>=0)
		{
		  string res("%s%d)",prefix,width);
		  return(res);
		}
	      else
		{
		  Expr *esize=stype->getWidthExpr();
		  Expr *e=EvaluateExpr(esize); 
#ifdef TYPE_DEBUG
		  cerr << "width: " << esize->toString() 
		       << "  --> " << e->toString();
#endif
		  string size=ccEvalExpr(e);
#ifdef TYPE_DEBUG
		  cerr << "    --> " << size << endl;
#endif
		  string res("%s)",(prefix+size));
		  return(res);
		}
	    }
	  else if (tk==TYPE_FIXED)
	    {
	      TypeFixed *ftype=(TypeFixed *)stype;
	      int iwidth=ftype->getIntWidth();
	      int fwidth=ftype->getFracWidth();
	      string p2;
	      if (iwidth>=0)
		p2=string("%s%d,",prefix,iwidth);
	      else
		{
		  Expr *eisize=ftype->getIntWidthExpr();
		  string isize=ccEvalExpr(EvaluateExpr(eisize));
		  p2=string("%s,",(prefix+isize));
		}
	      string res;
	      if (fwidth>=0)
		res=string("%s,%d)",p2,fwidth);
	      else
		{
		  Expr *efsize=ftype->getFracWidthExpr();
		  string fsize=ccEvalExpr(EvaluateExpr(efsize));
		  res=string("%s)",(p2+fsize));
		}
	      return(res);
	    }
	  else
	    return("(ScoreStreamType *)NULL)"); // blah
	}
    }
  else
    {
      fatal(-1,"getCCStreamType should only be called with streams",
	    rsym->getToken());
      return("(ScoreStreamType *)NULL)"); // blah
    }
  
}


string getCCtypeConstructor (Symbol *rsym, bool treat_as_stream,
			                   bool in_pagestep)
{
  if (rsym->isArray())
    {
      SymbolVar *vsym=(SymbolVar *)rsym;
      Type *vtype=vsym->getType();
      if (vtype->getTypeKind()==TYPE_ARRAY)
	{
	  TypeArray *atype=(TypeArray *)vtype;
	  Type      *etype= atype->getElemType();

	  string estr;	// nelems
	  if (atype->getNelems()>=0)
	    estr=string("%d",(long)atype->getNelems());
	  else
	    estr=ccEvalExpr(atype->getNelemsExpr());

	  string wstr;	// elem width
	  if (etype->getWidth()>=0)
	    wstr=string("%d",(long)etype->getWidth());
	  else
	    wstr=ccEvalExpr(etype->getWidthExpr());

	  string segment_constructor;
	  switch (etype->getTypeKind())
	    {
	    case (TYPE_BOOL): 
	      segment_constructor=string(TYPE_SCORE_SEGMENT_BOOLEAN_CONSTRUCTOR);
	      break;
	    case (TYPE_FIXED): 
	      {
		if (etype->isSigned())
		  segment_constructor=string(TYPE_SCORE_SEGMENT_SIGNED_FIXED_CONSTRUCTOR);
		else
		  segment_constructor=string(TYPE_SCORE_SEGMENT_UNSIGNED_FIXED_CONSTRUCTOR);
		break;
	      }
	    case (TYPE_INT): 
	      {
		if (etype->isSigned())
		  segment_constructor=string(TYPE_SCORE_SEGMENT_SIGNED_CONSTRUCTOR);
		else
		  segment_constructor=string(TYPE_SCORE_SEGMENT_UNSIGNED_CONSTRUCTOR);
		break;
	      }
	    default:
	      {
		fatal(-1,
		      string("internal error/getCCtype: not expecting type=%d as element type for array",(int)etype->getTypeKind()),
		      rsym->getToken());
		segment_constructor=string("FUBAR");
	      }
	    }
	  return(segment_constructor+
		 string("(%s,%s)",
			// (const char *)getWidth(atype->getElemType()),
			// (const char *)estr)
			// - fixed EC 12/7/99, was printing hex garbage
			(const char *)estr,
			(const char *)wstr)
		 ); 
	}
      else
	{
	  error(-1, string("symbol [") + rsym->getName() +
		    "] isArray, but does not have array type?!?",
		rsym->getToken());
	  return("FUBAR()");
	}
    }
  else if (rsym->isStream() || treat_as_stream)
    {
      Type *stype=rsym->getType();
      TypeKind tk=stype->getTypeKind();
      Expr *dh = (rsym->getSymKind()==SYMBOL_VAR)
		   ? ((SymbolVar*)rsym)->getDepth() : NULL;   // dh=depth hint
      string dh_str = dh ? ccEvalExpr(EvaluateExpr(dh)) : string();
      string dh_end = dh ? TYPE_DEPTH_HINT_SUFFIX : "";
      string ps_end = in_pagestep ? TYPE_PAGESTEP_SUFFIX : "";
      if (tk==TYPE_BOOL)
	return(string(TYPE_BOOLEAN_SCORE_STREAM_CONSTRUCTOR) + dh_end + ps_end
	       + "(" + dh_str + ")");
      else if (tk==TYPE_INT)
	{
	  int width=stype->getWidth();

	  if (width>=0)
	    {
	      string common_end = string("(%d",width) +
					(dh?(","+dh_str):string()) + ")";
	      if (stype->isSigned())
		return(string(TYPE_SIGNED_SCORE_STREAM_CONSTRUCTOR)
		       + dh_end + ps_end + common_end);
	      else
		return(string(TYPE_UNSIGNED_SCORE_STREAM_CONSTRUCTOR)
		       + dh_end + ps_end + common_end);
	    }
	  else
	    {
	      Expr *esize=stype->getWidthExpr();
	      string size=ccEvalExpr(EvaluateExpr(esize)); 

	      if (stype->isSigned())
		return(string(TYPE_SIGNED_SCORE_STREAM_CONSTRUCTOR)
		       + dh_end + ps_end
		       + "(" + size + (dh?(","+dh_str):string()) + ")");
	      else
		return(string(TYPE_UNSIGNED_SCORE_STREAM_CONSTRUCTOR)
		       + dh_end + ps_end
		       + "(" + size + (dh?(","+dh_str):string()) + ")");
	    }
	}
      else if (tk==TYPE_FIXED)
	{
	  TypeFixed *ftype=(TypeFixed *)stype;
	  int iwidth=ftype->getIntWidth();
	  int fwidth=ftype->getFracWidth();
	  string prefix;
	  if (stype->isSigned())
	    prefix=TYPE_SIGNED_FIXED_SCORE_STREAM_CONSTRUCTOR  +dh_end+ps_end;
	  else
	    prefix=TYPE_UNSIGNED_FIXED_SCORE_STREAM_CONSTRUCTOR+dh_end+ps_end;
	  string p2;
	  if (iwidth>=0)
	    p2=prefix+"("+string(iwidth);
	  else
	    {
	      Expr *eisize=ftype->getIntWidthExpr();
	      string isize=ccEvalExpr(EvaluateExpr(eisize));
	      p2=prefix+"("+isize;
	    }
	  string res;
	  if (fwidth>=0)
	    res=p2+","+string(fwidth);
	  else
	    {
	      Expr *efsize=ftype->getFracWidthExpr();
	      string fsize=ccEvalExpr(EvaluateExpr(efsize));
	      res=p2+","+fsize;
	    }
	  res = res + (dh?(","+dh_str):string()) + ")";
	  return(res);
	}
      else
	{
	  fatal(-1,
		string("internal error/getCCtype: not expecting type=%d as element type for array",(int)tk),
		      rsym->getToken());
	  return(string("FUBAR()")); 
	}
    }
  else 
    {
      return("");
    }
}


string getCCtype (Symbol *rsym, bool treat_as_stream)
{
  if (rsym->isArray())
    {
      if (rsym->getType()->getTypeKind()!=TYPE_ARRAY)
	{
	  fatal(-1,"internal error/getCCtype: found rsym->isArray which does not have TYPE_ARRAY",rsym->getToken());
	  return("FUBAR");
	}
      else
	{
	  TypeArray *atype=(TypeArray *)rsym->getType();
	  Type *etype = atype->getElemType();
	  switch (etype->getTypeKind())
	    {
	    case (TYPE_BOOL): return(TYPE_SCORE_SEGMENT_BOOLEAN);
	    case (TYPE_FIXED): 
	      {
		if (etype->isSigned())
		  return(TYPE_SCORE_SEGMENT_SIGNED_FIXED);
		else
		  return(TYPE_SCORE_SEGMENT_UNSIGNED_FIXED);
	      }
	    case (TYPE_INT): 
	      {
		if (etype->isSigned())
		  return(TYPE_SCORE_SEGMENT_SIGNED);
		else
		  return(TYPE_SCORE_SEGMENT_UNSIGNED);
	      }
	    default:
	      {
		fatal(-1,
		      string("internal error/getCCtype: not expecting type=%d as element type for array",(int)etype->getTypeKind()),
		      rsym->getToken());
		return(TYPE_SCORE_SEGMENT_BOOLEAN);
	      }
	    }
	}
    }
  else if (rsym->isStream()||treat_as_stream)
    {
      Type *stype=rsym->getType();
      TypeKind tk=stype->getTypeKind();
      if (tk==TYPE_BOOL)
	return(TYPE_BOOLEAN_SCORE_STREAM);
      else if (tk==TYPE_INT)
	if (stype->isSigned())
	  return(TYPE_SIGNED_SCORE_STREAM);
	else
	  return(TYPE_UNSIGNED_SCORE_STREAM);
      else if (tk==TYPE_FIXED)
	if (stype->isSigned())
	  return(TYPE_SIGNED_FIXED_SCORE_STREAM);
	else
	  return(TYPE_UNSIGNED_FIXED_SCORE_STREAM);
      else
	return(TYPE_SCORE_STREAM); // blah, don't know
    }
  else if (ccParamCausesInstance(rsym))
    {
      // all parameters should be castable to long's
      //  ...but may want to add some abstraction here in the future
      Type *stype=rsym->getType();
      if (stype->isSigned())
	return("long");
      else
	return("unsigned long");
    }
  else 
    { 
      error(-1,string("unexpected type ["+rsym->toString()+"]",rsym->getToken()));
      return("void");
    }
  
}








