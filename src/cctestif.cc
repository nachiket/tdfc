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
// $Revision: 1.132 $
//
//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <fstream>
#include "operator.h"
#include "stmt.h"
#include "state.h"
#include "annotes.h"
#include "bindvalues.h"
#include "cctype.h"
#include "ccannote.h"
#include "cceval.h"
#include "ccGappaStmt.h"
#include "cctestif.h"


using std::cout;
using std::endl;
using leda::list_item;

/***********************************************************************
Note: builtins expecting to handle here 
   done
   close
  (only two left at stmt level)
***********************************************************************/

void Display(Stmt *stmt)
{
	if (stmt == NULL)
		cout << "stmt is null" << endl;
	else
	{
		switch (stmt->getStmtKind())
		{
			case STMT_GOTO:
			{
				cout << "stmt type = STMT_GOTO" << endl;
				break;
			}
			case STMT_IF:
			{
				cout << "stmt type = STMT_IF" << endl;
				break;
			}
			case STMT_CALL:
			{
				cout << "stmt type = STMT_CALL" << endl;
				break;
			}
			case STMT_BUILTIN:
			{
				cout << "stmt type = STMT_BUILTIN" << endl;
				break;
			}
			case STMT_ASSIGN:
			{
				cout << "stmt type = STMT_ASSIGN" << endl;
				break;
			}
			case STMT_BLOCK :
			{
				cout << "stmt type = STMT_BLOCK " << endl;
				break;
			}
			default :
				break;
		}
	}
}


void count_variable(Stmt *stmt, 	int *i)
{
	if ( stmt != (Stmt *)NULL)
	{
		switch (stmt->getStmtKind())
		{
			case STMT_GOTO:
			{
				break;
			}
			case STMT_IF:
			{
				StmtIf* ifstmt = (StmtIf *)stmt;
				Stmt* then_part = ifstmt->getThenPart();
				Stmt* else_part = ifstmt->getElsePart();
				count_variable(then_part, i);
				count_variable(else_part, i);
				break;
			}
			case STMT_CALL:
			{
				break;
			}
			case STMT_BUILTIN:
			{
				break;
			}
			case STMT_ASSIGN:
			{
				(*i)++;
				break;
			}
			case STMT_BLOCK :
			{
				StmtBlock *bstmt=(StmtBlock *)stmt;
				
				SymTab *symtab=bstmt->getSymtab();
				Stmt* astmt;
				forall(astmt,*(bstmt->getStmts()))
				{
					count_variable(astmt, i);
				}
				break;
			}
			default :
				break;
		}
	}
}

void ccGappaIfStmt(ofstream *fout, string indent, Stmt *stmt, int *early_close,
	    string state_prefix, bool in_pagestep, bool retime,  string type,
	    string precision, string classname, int *if_nb, string **variables , int nb_variables ,string previous_cond)
{
	
//	cout << "enter ccGappaIfStmt" << endl;
	
//	cout << "\n\n" ;
//	cout << "previous cond is : " << previous_cond ;
//	cout << "\n\n" ;
	
	bool cuda = false;
	bool gappa = true;
	bool mblaze = false;
	
//	Display(stmt);
	
  switch (stmt->getStmtKind())
    {

    case STMT_GOTO:
      {
			StmtGoto *gstmt=(StmtGoto *)stmt;
			
		  //cout << "exit ccGappaIFStmt" << endl;
		  
			return;
      }
    case STMT_IF:
     {
		 
		(*if_nb) += 1;
		
		StmtIf *if_stmt = (StmtIf *)stmt;
		Stmt *then_part = if_stmt->getThenPart();
		Stmt *else_part =  if_stmt->getElsePart();
		
		cout << "condition" << 
		ccEvalExpr(EvaluateExpr(if_stmt->getCond()), retime, cuda, gappa, type) 
		<< " is cond" << *if_nb << endl;
		
		*fout << "\n" ;
		*fout << "#condition : " << 
		EvaluateExpr(if_stmt->getCond())->toString() 
		<< " is cond" << *if_nb << endl;
		
		
		
		char nb[255];
		sprintf(nb, "%i", *if_nb);
		
		string temp = nb;
		
		string prev_cond_else = previous_cond;
		
		if (prev_cond_else != "")
			prev_cond_else = prev_cond_else + " *(1- cond" + temp +")"; 
		else
			prev_cond_else = "(1 - cond" +  temp + ")"; 	
		
		if (previous_cond != "")
			previous_cond = previous_cond + " * cond" + temp; 
		else
			previous_cond = "cond" +  temp; 	
		
//		cout << "deal with then_part" << endl;
		
		        
        ccGappaIfStmt(fout,string("%s  ",indent),then_part,
	    early_close,state_prefix,in_pagestep, retime, type, precision, 
	    classname, if_nb, variables , nb_variables, previous_cond);
	    
		if (else_part !=(Stmt *)NULL)
		{
			
//			cout << "##\t\tdeal with else_part" << endl;
			ccGappaIfStmt(fout,string("%s  ",indent),else_part,
			early_close,state_prefix,in_pagestep, retime, type, precision, 
			classname, if_nb,variables , nb_variables,prev_cond_else);
		}
		
		return;
      }
    case STMT_CALL:
      {
		 warn("ccStmt: expecting calls to be transformed out before calling this",
	     stmt->getToken());
	     
	     
		  //cout << "exit ccGappaIFStmt" << endl;
		  
		 return;
      }
    case STMT_BUILTIN:
      {
		StmtBuiltin     *bstmt=(StmtBuiltin *)stmt;
		ExprBuiltin     *bexpr=bstmt->getBuiltin();
		list<Expr*>     *args =bexpr->getArgs();
		Expr            *first=args->empty()?NULL:args->head();
		Operator        *op   =bexpr->getOp();
		OperatorBuiltin *bop  =(OperatorBuiltin *)op;
		if (bop->getBuiltinKind()==BUILTIN_CLOSE)
		  {
			if (first->getExprKind()!=EXPR_LVALUE)
				warn(string("close given invalid argument %s",
				  first->toString()),first->getToken());
			ExprLValue *lexpr=(ExprLValue *)first;
			long id=(long)(lexpr->getSymbol()->getAnnote(CC_STREAM_ID));
			*fout << indent << "STREAM_CLOSE(";
			*fout << "out[" << id << "]";
			
			*fout << ");" << endl;

			early_close[id]=1;
			*fout << indent ;
			*fout << "output_close[" << id << "]=1;" ;
			*fout << endl;
		  }
		  
		// Added by Nachiket on 10/6/2009 to support frameclose operation
		else if (bop->getBuiltinKind()==BUILTIN_FRAMECLOSE)
		{
			if (first->getExprKind()!=EXPR_LVALUE)
				warn(string("frameclose given invalid argument %s",
					  first->toString()),first->getToken());
			else
	     	{
				ExprLValue *lexpr=(ExprLValue *)first;
				long id=(long)(lexpr->getSymbol()->getAnnote(CC_STREAM_ID));
				*fout << indent << "FRAME_CLOSE(";
				*fout << "out[" << id << "]";
				*fout << ");" << endl;

				/* Nachiket: Don't think any of these are valid.. are they?
				early_close[id]=1;
				*fout << indent
					  << "output_close[" << id << "]=1;"
					  << endl;
				*/
			}
		}	  
		else if (bop->getBuiltinKind()==BUILTIN_DONE)
		  {
			*fout << indent 
			 << "done=1;" << endl;
		  }
		else if (bop->getBuiltinKind()==BUILTIN_PRINTF)
		  {
			// *fout << indent << "printf(\""
			*fout << indent << "fprintf(stderr,\""
				<< ((Token*)bexpr->getAnnote(ANNOTE_PRINTF_STRING_TOKEN))->str
				  << "\"";
			
			for (list_item i=args->first(); i; i=args->succ(i)) {
				// Nachiket's modifications to support floating-point casting
				Expr* orig=args->inf(i);
				if (orig->getType()->getTypeKind()!=TYPE_FLOAT &&
						orig->getType()->getTypeKind()!=TYPE_DOUBLE) {
					//*fout << ", (long long)" --> Not sure if this is such a good idea in any case
					*fout << ", "
						<< ccEvalExpr(EvaluateExpr(args->inf(i)), retime, cuda) << "";
				} else {
					*fout << ", " << ccEvalExpr(EvaluateExpr(args->inf(i)), retime, cuda, gappa , type) << "";
				}
			}
			*fout << ");" << endl;
		  }
		else
		  {
			fatal(-1,string("Unexpected builtin Operator [%d] in Statement",
					(int)bop->getBuiltinKind()),bop->getToken());
		  }
		  //cout << "exit ccGappaIFStmt" << endl;
		  
		return;
      }
    case STMT_ASSIGN:
      {
		StmtAssign *astmt=(StmtAssign *)stmt;
		ExprLValue *lval=astmt->getLValue();
		// added by Nachiket on Sep29th to use fancy floating-point stream access functions in the Score runtime
		Symbol *asym=lval->getSymbol();
		Expr *rexp=astmt->getRhs();
		
		cout << "\n\tvariable to be assigned : " << asym->getName() << endl;
		
		if (asym->isStream())
		{
			SymbolStream *ssym=(SymbolStream *)asym;
			if (ssym->getDir()==STREAM_OUT)
		    {
				  //if ( var_name == NULL)
				  {					    		
						/*if (type !="_m")
							*fout << asym->getName() << type << " " << precision <<  " = "  << previous_cond //" * (cond" << *if_nb
							 << " * " << ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type) << endl;
						else 
							*fout << asym->getName() <<type << " = "  << previous_cond//"(cond" << *if_nb
							 << " * " << ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type) << endl;
						*/
						
						string value = previous_cond + " * " + ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type);;
						bool variable_found = false;
						int available = 0;
						if (variables != NULL)
						{
							for (int k =0; k < nb_variables; k++)
							{
								if ( asym->getName() == variables[k][0])
								{
									if ( variables[k][1] == "")
										variables[k][1] += value; 
									else 
										variables[k][1] = variables[k][1] + " + " +value; 							
									
									variable_found = true;
								}
								else if (variables[k][0] != "")
								{
									available++;
								}
							}
							if (!variable_found && available != nb_variables)
							{
								variables[available+1][0] = asym->getName();
								variables[available+1][1] = value;
							}
						}
						
				  }
				  /*else if (*var_name == asym->getName() && *process_variable)
				  {
					  cout << "var name corresponds" << endl;
					  *fout << "+"  << previous_cond 
							<< " * " << ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type) << endl;
					  
				  } */
			  }
			else
			  {
			warn("writing to input stream!",lval->getToken());
			// someone else complained?
			  }
		  }
		else
		  { 
			/* don't have to handle retime here since it shouldn't appear */

			/* MAYBE: add mask here to get rid of any bits out of type range */
		   
			if (lval->usesAllBits())
			{

						string value = previous_cond + " * " + ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type);;
						bool variable_found = false;
						int available = 0;
						if (variables != NULL)
						{
							for (int k =0; k < nb_variables; k++)
							{
								if ( asym->getName() == variables[k][0])
								{
									if ( variables[k][1] == "")
										variables[k][1] += value; 
									else 
										variables[k][1] = variables[k][1] + " + " +value; 							
									
									variable_found = true;
								}
								else if (variables[k][0] != "")
								{
									available++;
								}
							}
							if (!variable_found && available != nb_variables)
							{
								variables[available+1][0] = asym->getName();
								variables[available+1][1] = value;
							}
						}
				
				//if ( var_name == NULL)
/*				  {
					    if (type !="_m")
							*fout << asym->getName() << type << " " << precision <<  " = "  << previous_cond  
							<< " * " << ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type) << endl;
						else 
							*fout << asym->getName() <<type << " = "  << previous_cond
							 << " * " << ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type) << endl;
				  }
				  else if (*var_name == asym->getName() && *process_variable)
				  {
					  cout << "var name corresponds" << endl;
					  *fout << "+"  << previous_cond 
							<< " * " << ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type) << endl;
					  
				  }
*/
//					*fout<<indent<<asym->getName()<< type <<" = "
//					<<ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa,type)<<";"<<endl;
			}
			else
			{
				Expr *low_expr=lval->getPosLow();
				Expr *high_expr=lval->getPosHigh();
				string lstr=ccEvalExpr(EvaluateExpr(low_expr), retime, cuda, gappa, type);
				string hstr=ccEvalExpr(EvaluateExpr(high_expr), retime, cuda, gappa, type);
				string rstr=ccEvalExpr(EvaluateExpr(rexp), retime, cuda, gappa, type);
				string one =getCCvarType(asym).pos("long long")>=0 ? "1ll":"1";
				*fout << indent
					  << asym->getName() << "="
					  << "(" << asym->getName() << " & " 
					  << "~(((" << one << "<<("      << hstr
					  << "+1))-1)-((" << one << "<<" << lstr
					  << ")-1))"
					  << ")"
					  << " | (" << rstr << "<<" << lstr << ")"
					  << ";" << endl;
				  
			  }

			// TODO: somewhere here handles memories, or something like that
				   
		  }
		
		  //cout << "exit ccGappaIFStmt" << endl;
		  	
		return;
      }
    case STMT_BLOCK:
      {
			StmtBlock *bstmt=(StmtBlock *)stmt;
			
			SymTab *symtab=bstmt->getSymtab();
			Stmt* astmt;
			forall(astmt,*(bstmt->getStmts()))
			{
				ccGappaIfStmt(fout,string("%s  ",indent),astmt,early_close,state_prefix,
				in_pagestep, retime, type, precision, classname, if_nb,variables , nb_variables, previous_cond);
				
			}

		  //cout << "exit ccGappaIFStmt" << endl;
		  
			return;
      }
    default:
      {
		fatal(-1,string("unknown statement kind [%d] in ccStmt",
		(int)stmt->getStmtKind()),
		stmt->getToken());
      }

    }

}


