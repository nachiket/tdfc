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
// SCORE TDF compiler:  keep track of feedback records
// $Revision: 1.124 $
//
//////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "feedback.h"

using std::cerr;
using std::endl;

string	 gFeedbackPath;		// path for feedback files


FeedbackRecord::FeedbackRecord(int num_args, ARGMASK_TYPE param_loc, 
			       int *nparams): 
  param(num_args), is_param(num_args)
{
  // param=array<int>(num_args);
  // is_param=array<int>(num_args);
  for (int i=0;i<num_args;i++)
    {
      param[i]=nparams[i];
      is_param[i]=((param_loc>>i) & 0x01);
    }
}

void FeedbackRecord::merge(FeedbackRecord *nrec)
{
  // currently a noop
  // in future merge in feedback statistics
}

string FeedbackRecord::toString()
{
  string res;
  for (int i=0;i<param.size();i++)
    {
      if (is_param[i])
	if (i>0)
	  res=string("%s,%d",res,param[i]);
	else
	  res=string("(%d",param[i]);
      else
	if (i>0)
	  res=string("%s,",res);
	else
	  res=string("(");
    }
  res=string("%s)",res);
  return(res);
}


int count_args(list<Symbol*> *argtypes)
{
  int cnt=0;
  Symbol *sym;
  forall(sym,*argtypes)
      cnt++;
  return(cnt);
}

ARGMASK_TYPE parameter_locs(list<Symbol*> *argtypes)
{
  int loc=0;
  ARGMASK_TYPE res=0;
  Symbol *sym;
  forall(sym,*argtypes)
    {
      if (sym->isParam())
	res|=(((ARGMASK_TYPE)1)<<loc);
      loc++;
    }
  return(res);
}

void read_feedback(FILE *fd, char *fname,
		   list<FeedbackRecord *>* flist, int args, 
		   ARGMASK_TYPE param_loc, string name)
{
  #define MAX_LINE_LEN 4096
  // HACK:  max line length 4096 chars (~50 lines)

  int lcnt=0;
  char line[MAX_LINE_LEN];
  while (!feof(fd))
    {
      lcnt++;
      char *res=fgets(line,MAX_LINE_LEN,fd);
      string sline=string(line);
      int spos=sline.pos(name);
      if (spos>-1)
	{
	  if ((sline[spos+name.length()]==' ') || 
	      (sline[spos+name.length()]=='('))
	    {
	    
	      int argpos;
	      argpos=0;
	      int * nparams=new int[args];
	      if (args>0)
		{
		  spos=sline.pos(string("("),spos+name.length());
		  if (spos>0)
		    spos++; // skip over it
		  int cpos=0;
		  while ((argpos<(args-1)) && (cpos>=0))
		    {
		      cpos=sline.pos(string(","),spos);
		      if (cpos>0)
			{
			  if ((cpos-spos)>0)
			    if ((param_loc>>argpos) & 0x01)
			      {
				nparams[argpos]=strtoul(sline(spos,cpos-1),
							(char **)NULL,0);
			      }
			    else
			      nparams[argpos]=0;
			  else
			    nparams[argpos]=0;
			  spos=cpos+1;
			  argpos++;
			}
		    }
		  cpos=sline.pos(string(")"),spos);
		  if (cpos>0)
		    {
		      if ((cpos-spos)>0)
			if ((param_loc>>argpos) & 0x01)
			  {
			    nparams[argpos]=strtoul(sline(spos,cpos-1),
						    (char **)NULL,0);
			  }
			else
			  nparams[argpos]=0;
		      else
			nparams[argpos]=0;
		      spos=cpos;
		      argpos++;
		    }
		}
	      if (argpos==args)
		{

		  FeedbackRecord *rec=new FeedbackRecord(args,param_loc,
							 nparams);
		  flist->push(rec);
		}
	      else
		{
		  cerr << fname << ": "
		       << "arg count mismatch on line " << lcnt
		       << " got: " << argpos 
		       << " expecting: " << args << endl;
		}
	    }
	  else
	    {
	      cerr << fname << ": "
		   << "did not match operator name [" << name
		   << "] in line " << lcnt << endl;
	      cerr << "\t[" << line << "]" << endl;
	    }
	}
      else
	if (res!=(char *)NULL)
	  {
	    cerr << fname << ": "
		 << "couldn't match operator name [" << name
		 << "] in line " << lcnt << endl;
	    cerr << "\t[" << line << "]" << endl;
	  }
    }

}


int feedback_order(FeedbackRecord * const &A,  FeedbackRecord * const &B)
{
  for (int i=0;i<A->params();i++)
    {
      if (A->isParam(i)) {
	if (A->getParam(i) > B->getParam(i)) {
	  return(-1);
	} else if (A->getParam(i) < B->getParam(i)) {
	  return(1);
	}
      }
    }
  return(0); // all equal
}

list<FeedbackRecord *> *canonicalize_feedback_record_list(list <FeedbackRecord *> *ilist)
{

  array<FeedbackRecord *> *alist=new array<FeedbackRecord *>(ilist->size());
  int i=0;
  FeedbackRecord *rec;
  forall(rec,*ilist)
    {
      (*alist)[i]=rec;
      i++;
    }
  alist->sort(&feedback_order);
  
  list<FeedbackRecord *> *res=new list<FeedbackRecord *>();
  if (alist->size() > 0)
    {
      FeedbackRecord *current=(*alist)[0];
      for(int i=1;i<alist->size();i++)
	{
	  if (feedback_order((*alist)[i],current)==0)
	    current->merge((*alist)[i]);
	  else
	    {
	      res->push(current);
	      current=((*alist)[i]);
	    }
	}
      // include final
      res->push(current);
    }
  return(res);
}


void initFeedbackPath ()
{
  // - initialize gFeedbackPath

  char *fpath=getenv(SCORE_FEEDBACK_PATH_ENV);
  if (fpath==(char *)NULL)
    gFeedbackPath=string(DEFAULT_FEEDBACK_PATH);
  else
    gFeedbackPath=string(fpath);
}

