#include "compares.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <LEDA/core/sortseq.h>
#include <LEDA/core/string.h>
#include <LEDA/core/array.h>
#include <LEDA/graph/node_set.h>
#include <LEDA/graph/node_list.h>
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
#include "blockdfg.h" // Added by Nachiket on 11/29/2009


using leda::sortseq;
using leda::list_item;
using leda::dic_item;
using std::ofstream;
using leda::node_array;
using leda::node;
using leda::edge;
using leda::node_set;
using leda::node_list;
using leda::array;

string 	getnodenumber(node n, Tree* t) {
	cout << "node to var string : " << endl;
	cout << "\t" << endl;
	cout << t ? t->toString().replace_all("\n","") : string("<nil>");
	return t ? t->toString().replace_all("\n","") : string("<nil>");
}

void ccgappadfgprocrun(ofstream *fout, string name, Operator *op, string type_val, string precision, int *if_nb,
	       int debug_logic)
{
	
	
  if (op->getOpKind()==OP_BEHAVIORAL)
    {
      OperatorBehavioral *bop=(OperatorBehavioral *)op;
      dictionary<string,State*>* states=bop->getStates();
      dic_item item;

    //  *fout << "  enum state_syms {" ;
	
      
      // declare top level vars
      SymTab *symtab=bop->getVars();
      list<Symbol*>* lsyms=symtab->getSymbolOrder();
      list_item item2;
      /*forall_items(item2,*lsyms)
		{
			Symbol *sum=lsyms->inf(item2);
			SymbolVar *asum=(SymbolVar *)sum;
			//*fout << "  " << getCCvarType(asum) << " " << asum->getName() ;
			Expr* val=asum->getValue();
			if (val!=(Expr *)NULL)
			{
				*fout <<  asum->getName() ;
				*fout << "=" << ccEvalExpr(EvaluateExpr(val), false) ;
				*fout << ";" << endl;
			}
		}
*/
      // declare/initialize retiming slots for input streams
      // (1) map over and create annotations with all retiming exprs
      //op->map(collect_retime_exprs,ccbody_treemap_false,(void *)NULL);
      // n.b. This "works" by ripping an expression out of context
      //   If we allow the user to define a local variable in 
      //    a state then use that variable (or a chain of such variables)
      //    as a retiming offset, then this will fail because
      //    it won't have picked up the local variables.
      //   If we support that...we should come back and FIX this.
      //   (...but I'm going to fix other things first -- amd 7/14/99)
      // (2) walk over all inputs (below) and declare/initialize

      // declare/initialize stream open / free state
      //  (a) compile time version
      //  (b) run time version
      // declare variable/type for holding input stream tokens
      int icnt=0;
      int ocnt=0;
      Symbol *rsym=op->getRetSym();
      list<Symbol*> *argtypes=op->getArgs();
      if (!noReturnValue(rsym))
	{
	  if (rsym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)rsym;
	      if (ssym->getDir()!=STREAM_OUT)
		{
		  // error already issues
		}
	      else
		{
		  rsym->setAnnote(CC_STREAM_ID,(void *)ocnt);
		  ocnt++;
		}
	    }
	  else
	    {
	      // presumably we've already issued this complaint above
	    }
	}
      Symbol *sym;
      list<string> *list_input = new list<string>();
      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
			list_input->push(sym->getName());
		  sym->setAnnote(CC_STREAM_ID,(void *)ocnt);
		  ocnt++;
		}
	      else if (ssym->getDir()==STREAM_IN)
		{
			
			list_input->push(sym->getName());
/*		  *fout << "  " << getCCvarType(sym) << " " 
			<< sym->getName() << ";" << endl;
		  sym->setAnnote(CC_STREAM_ID,(void *)icnt);
		  // Declare and initialize retime chain here
		  set<Expr *> *rset=
		    (set <Expr *> *)sym->getAnnote(MAX_RETIME_DEPTH);
		  set<Expr *> *ncset=new set<Expr *>();
		  int constmax=0;
		  Expr *rexp;
		  if (rset!=(set <Expr *> *)NULL)
		    {
		      forall(rexp,*rset)
			{
			  if (rexp->getExprKind()==EXPR_VALUE)
			    {
			      int eval=((ExprValue *)rexp)->getIntVal();
			      if (eval>constmax) constmax=eval;
			    }
			  else
			    ncset->insert(rexp);
			}
		    }
		  *fout << "    "
			<< sym->getName()
		    // TODO: assign initial values instead of 0
			<< "=0;" << endl;
*/
		  icnt++;
		}
	      else
		{
		  // already complained
		}
	    }
	    
		else if (sym->isParam())
		{
			//cout << endl;
			//cout << "\tparameter  : " << sym->getName()<< endl; 
			//cout << endl;
			list_input->push(sym->getName());
		}
	}
      int *early_free=new int[icnt];
      for (int i=0;i<icnt;i++)
	early_free[i]=0;
      int *early_close=new int[ocnt];
      for (int i=0;i<ocnt;i++)
	early_close[i]=0;
      
      int num_states=states->size();
      if (num_states>1)
			cerr << "gappa does not accept more than one state" << endl;
			//*fout << "    switch(state) {" << endl;
      int snum=0;
      forall_items(item,*states)
	{
	  State* cstate=states->inf(item);
	  string sname=cstate->getName();

	  array<StateCase*>* cases=caseSort(cstate->getCases());
	  array<Symbol*>* caseIns=allCaseIns(cases);
	  array<int>* firstUsed=inFirstUsed(caseIns,cases);
          int numNestings=0;

	  if (num_states>1)
	    cerr << "gappa does not accept more than one state" << endl;

	string atomiceofrcase="0";

	  for (int i=cases->low();i<=cases->high();i++)
	    {
	      // walk over inputs to case
	      InputSpec *ispec;
	      StateCase *acase=(*cases)[i];

          // increment the nesting count and also output a beginning
          // nesting bracket.
          numNestings++;

	      BlockDFG dfgVal=(acase->getDataflowGraph());
	      h_array<node, Symbol*> symbolmap=(acase->getSymbolMap());
	      h_array<Symbol*,node> *valid_map=(acase->getValidMap());
	    
	      BlockDFG* dfg=&dfgVal;
	      if(dfg==NULL) 
	      {
	    	  cout << "No dataflow graph found in state=" << sname << "["<< cstate << "] acase="<< acase <<"!" << endl;
	      } 
	      else
	      {			  
	    	  cout << "Dataflow graph found in state=" << sname << "["<< cstate << "] acase="<< acase <<" DFG="<< &dfgVal << "!" << endl;
	    	  // dump out the dataflow graph..
	    	  int nodenum=0;

	    	  //cout << "Graph" << endl;
	    	  printf("dfg statistics->  %d nodes, %d edges\n",dfg->number_of_nodes(), dfg->number_of_edges());
	    	  fflush(stdout);
	    	  node n;

		  // compute latency ordered list of nodes based on ASAP scheduling
		  node_list arranged_list;
		  node_array<int> depths;

		  computeASAPOrdering(dfg, &arranged_list, &depths);
		  //cout << "ASAP ordering computed " << endl;
	
		  
		  //cout << "Started" << endl;
		  node_array<int> nodenums(*dfg);		    
		  
		  forall(n,arranged_list) 
		  {
		  	nodenums[n]=nodenum++;
		  }

		  // TODO: From the ASAP ordering, start printing out nodes... How about just building the dataflow expression?
		  forall(n,arranged_list) {
		
			Tree* t=(dfgVal)[n];
		
			cout << "Processing :  " ;
			cout << nodetostring(n,(dfgVal)[n],nodenums[n], list_input) << endl;
			
			TypeKind type = ((Expr*)t)->typeCheck()->getTypeKind();
		
			//cout << "dfg->indeg(n) = " << dfg->indeg(n) << endl;
			
		  	if(!dfg->indeg(n)==0) {
				
				if(dfg->indeg(n)==3) {
					
					//cout << "dfg->indeg(n)==3" << endl;
					
					// if operator
					//*fout << "          // IF Node: Type=" << typekindToCplusplus(type) << endl;
					//*fout << "          " << typekindToCplusplus(type) << " ";
					
					
					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					int edgenum=0;
					edge e;
					string ifstr=" ";
					string temp = "";
					bool true_m = false;
					forall_in_edges (e, n) {
						node src=(*dfg).source(e);
						if ( nodetofout(dfg, src, nodenums) == "true")
							true_m = true;
					}
					if ( !true_m)
					{
					
					if (type_val != "_m")
						*fout << nodetofout(dfg, n, nodenums) << type_val << " " << precision << " = ";
					else 
						*fout << nodetofout(dfg, n, nodenums) << type_val << " " << " = ";
					

					string name = nodetofout(dfg, n, nodenums);
					
					forall_in_edges (e, n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						
						if(edgenum==0) {
							cout << nodetofout(dfg, src, nodenums) + "--edge0";
						} else if(edgenum==1) {
							cout << nodetofout(dfg, src, nodenums) + "--edge1";
						} else if(edgenum==2) {
							cout << nodetofout(dfg, src, nodenums) + "--edge2";
						}
						
						//cout << " edgenum = " << edgenum << endl; 
						
						if(edgenum==0) {
							temp = nodetofout(dfg, src, nodenums) + type_val;
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + type_val + ") * " ;
							//ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") ? " ;
						} else if(edgenum==1) {
							if ( nodetofout(dfg, src, nodenums) != name)
							{
								string test;
								bool found = false;
								forall (test, *list_input)
								{
									if (nodetofout(dfg, src, nodenums) == test)
										found = true;
								}

								ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) ;
								Tree *tree_temp=(*dfg)[src];
								if(!((tree_temp->getKind()==TREE_EXPR) && (((Expr*)tree_temp)->getExprKind()==EXPR_VALUE)) || found)  
								{
									ifstr = ifstr + type_val ;
								}
								//+ type_val + ") + ";
								ifstr = ifstr +") + ";
							}	
							//ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") : ";
						} else if(edgenum==2) {
							if ( nodetofout(dfg, src, nodenums) != name)
							{
								string test;
								bool found = false;
								forall (test, *list_input)
								{
									if (nodetofout(dfg, src, nodenums) == test)
										found = true;
								}
																							
								ifstr = ifstr +  + "(1 - " + temp + ") * "+"(" + nodetofout(dfg, src, nodenums) ;
								Tree *tree_temp=(*dfg)[src];
								if(!((tree_temp->getKind()==TREE_EXPR) && (((Expr*)tree_temp)->getExprKind()==EXPR_VALUE)) || found)  
								{
									ifstr = ifstr + type_val ;
								}
								//+ type_val +") ";
								ifstr = ifstr +") ";
							}
						}
						
						edgenum++;
					}
					
					*fout << ifstr << ";" << endl;
					}
				} else if(dfg->indeg(n)==2 && t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND) {
					
					
					//cout << "dfg->indeg(n)==2 && t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND" << endl;
					
					// if operator
					//*fout << "          // IF ? Node: Type=" << typekindToCplusplus(type) << endl;
					//*fout << "          " << typekindToCplusplus(type) << " ";
					if (type_val !="_m")
						*fout << nodetofout(dfg, n, nodenums) << type_val << " " << precision << " = ";
					else
						*fout << nodetofout(dfg, n, nodenums) << type_val << " " <<  " = ";

					string name = nodetofout(dfg, n, nodenums);
					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					int edgenum=0;
					edge e;
					string ifstr=" ";
					string temp;
					//forall (e,dfg_in_edges_n) {
						forall_in_edges (e,n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						/*if(edgenum==0) {
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") : (" + nodetofout(dfg,n,nodenums) + ")";
						} else if(edgenum==1) {
							ifstr = "(" + nodetofout(dfg, src, nodenums) + ") ? " + ifstr;
						}*/
						if(edgenum==0) {
							temp = nodetofout(dfg, src, nodenums) + type_val;
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + type_val + ") * ";
							//ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ") ? ";
						} else if(edgenum==1) {
							if ( nodetofout(dfg, src, nodenums) != name)
							{
								ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) ;
								
								string test;
								bool found = false;
								forall (test, *list_input)
								{
									if (nodetofout(dfg, src, nodenums) == test)
										found = true;
								}

								
								Tree *tree_temp=(*dfg)[src];
								if(!((tree_temp->getKind()==TREE_EXPR) && (((Expr*)tree_temp)->getExprKind()==EXPR_VALUE)) || found)  
								{
									ifstr = ifstr + type_val ;
								}
								
								ifstr = ifstr +") ";
							}
							if ( nodetofout(dfg, n, nodenums) != name)
							{
								ifstr = ifstr + "+ (1- "+ temp +")* (" + nodetofout(dfg,n,nodenums);
								
								string test;
								bool found = false;
								forall (test, *list_input)
								{
									if (nodetofout(dfg, n, nodenums) == test)
										found = true;
								}

								
								Tree *tree_temp=(*dfg)[n];
								if(!((tree_temp->getKind()==TREE_EXPR) && (((Expr*)tree_temp)->getExprKind()==EXPR_VALUE)) || found)  
								{
									ifstr = ifstr + type_val ;
								}
								
								ifstr = ifstr +") ";
								
							}
							//ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + ")  (" + nodetofout(dfg,n,nodenums) + ")";
						}
						edgenum++;
					}
					*fout << ifstr << ";" << endl;
				} else if(dfg->indeg(n)==2) {
					
					//cout << "dfg->indeg(n)==2" << endl;
					
					// binary operator
					//*fout << "          // Binary Node: Type=" << typekindToCplusplus(type) << endl;
					//cout << "          // Binary Node: Type=" << typekindToCplusplus(type) << endl;
					//*fout << "          " << typekindToCplusplus(type) << " ";
					if (typekindToCplusplus(type) == "bool")
					{
					
					char nb[255];
					sprintf(nb, "%i", *if_nb);
					string number = nb;
					
					Tree* tmp=(*dfg)[n];
					*fout << "#" <<  getnodenumber(n, tmp) <<  " is cond" << number << endl; 
					//if (type_val!="_m")
						//*fout << nodetofout(dfg,n,nodenums) << type_val << " " << precision << " = ( ";
					//else
					{
						*fout << nodetofout(dfg,n,nodenums) << type_val << " " << " = ( ";
					}
					
					
					 *fout << "fixed<-1,ne> (cond" << number << ")";
					 
					 (*if_nb)++;
				    }
				    else
				    {
						if (type_val != "_m")	
							*fout << nodetofout(dfg,n,nodenums) << type_val << " " << precision << " = ( ";
						else
						{
							*fout << nodetofout(dfg,n,nodenums) << type_val << " " <<  " = ( ";
						}
						
						 
						list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
						int edgenum=0;
						edge e;
						forall_in_edges (e,n) {
					
						// - examine inputs of n
						node src=(*dfg).source(e);
						//cout << "before node to string" << endl;						
						
						string temp = nodetostring(src,(dfgVal)[src],nodenums[src], list_input);
						//string temp = getnodenumber(src,(dfgVal)[src]);//,nodenums[src], true);
						
						string test;
						bool found = false;
						
						forall (test, *list_input)
						{	
							if (temp == test)
								found = true;
						}
												
						//cout << temp << endl;
						Tree *tree_temp=(*dfg)[src];
						*fout << temp;
						if(!((tree_temp->getKind()==TREE_EXPR) && (((Expr*)tree_temp)->getExprKind()==EXPR_VALUE)) || found)  
						{
								*fout << type_val ;
						}
						
						*fout << " ";
						 
						//cout << "after node to string " << endl;
					//	*fout << nodetofout(dfg, src, nodenums) << type_val << " ";

						if(edgenum==0) {
							*fout << nodetofnstring(n,(dfgVal)[n]) + " ";
						}
						edgenum++;
					}
				}
					*fout << ");" << endl;
				} else if(dfg->indeg(n)==1 && type!=TYPE_STATE) {
					
					//cout << "dfg->indeg(n)==1 && type!=TYPE_STATE" << endl;
					
					// unary operator or function?
					//*fout << "          // Unary Node: Type=" << typekindToCplusplus(type) << endl;
					//*fout << "          " << typekindToCplusplus(type) << " ";
					if (nodetostring(n,(dfgVal)[n],nodenums[n], list_input)(1,5) != "valid" )
					{
						//*fout << nodetofout(dfg,n,nodenums) << type_val  << " " << precision <<  " = ";
						if (type_val != "_m")
							*fout << nodetostring(n,(dfgVal)[n],nodenums[n], list_input) << type_val  << " " << precision <<  " = ";
						else
						{
							*fout << nodetostring(n,(dfgVal)[n],nodenums[n], list_input) << type_val  << " " <<   " = ";
						}
						
						
						if (nodetofnstring(n,(dfgVal)[n]) == "!")
							*fout << "(1 -  ( ";
						else
							*fout << nodetofnstring(n,(dfgVal)[n]) + " ( ";
	//					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
						
						
						
						edge e=dfg->first_in_edge(n);
	//					forall (e,dfg_in_edges_n) {
							// - examine inputs of n
							node src=(*dfg).source(e);
							
							string temp = nodetostring(src,(dfgVal)[src],nodenums[src], list_input);
							*fout << temp << type_val <<" ";
							//*fout << nodetofout(dfg, src, nodenums) << type_val<<" ";
//						}
						if (nodetofnstring(n,(dfgVal)[n]) == "!")
							*fout << ")";
							
						*fout << ");" << endl;
					}
					
				} else if(dfg->indeg(n)==1 && type==TYPE_STATE && num_states>1) {
					
					//cout << "dfg->indeg(n)==1 && type==TYPE_STATE && num_states>1" << endl;
					
					// unary operator or function?
					*fout << "          // Unary State Node: Type=" << typekindToCplusplus(type) << endl;
					*fout << "          state = ( ";

					list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
					edge e;
					forall_in_edges (e,n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						// @2/12/2010 apply prefix at right code point... *fout << "STATE_" << nodetofout(dfg, src, nodenums) << " ";
						*fout << nodetofout(dfg, src, nodenums) << " ";
					}

					*fout << ");" << endl;
				}
			}
		  	cout << " finished" << endl;
		  }
/*
		  // Assign output nodes
		  forall(n,arranged_list) {
			  // fanout-0 nodes
			  if(dfg->outdeg(n)==0) {
				  Tree* t=(dfgVal)[n];
				  if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_LVALUE) {
					  Symbol *asym=((ExprLValue*)t)->getSymbol();
					  if (asym!=NULL && asym->isStream())
					  {
						  SymbolStream *ssym=(SymbolStream *)asym;
						  if (ssym->getDir()==STREAM_OUT)
						  {
						  	TypeKind outType = ((ExprLValue*)t)->typeCheck()->getTypeKind();
							bool floattype = (outType==TYPE_FLOAT);
						        bool doubletype = (outType==TYPE_DOUBLE);

							node valid_node = (*valid_map)[asym];
//							cout << "sym=" << asym ; asym=NULL;
//							cout << ", node=" << valid_node << " " << n << "=" << ((ExprLValue*)(*dfg)[valid_node])->getSymbol() << ", lval=" << ((ExprLValue*)(*dfg)[valid_node]) << " symbolmapped=" << symbolmap[valid_node] << endl;
							if(valid_node!=NULL) {
							        *fout << "          " ;
								*fout << "if(" << symbolmap[valid_node]->toString() << ") {" << endl;
							        *fout << "  " ;
							}

							int id=(long)(ssym->getAnnote(CC_STREAM_ID));
							*fout << "          " ;
							*fout  << (floattype?"STREAM_WRITE_FLOAT":doubletype?"STREAM_WRITE_DOUBLE":"STREAM_WRITE_NOACC")
								<< "(out[" << id << "]," << nodetofout(dfg,n,nodenums) << ");" << endl; // asym->toString();
							if(valid_node!=NULL) {
							        *fout << "          }" << endl;
							}
						  }
					  }
				  } else if(t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_BUILTIN) {
					  ExprBuiltin *bexpr = (ExprBuiltin*)t;
					  Symbol *asym=bexpr->getSymbol();
					  if (asym!=NULL && asym->isStream())
					  {
						  SymbolStream *ssym=(SymbolStream *)asym;
						  if (ssym->getDir()==STREAM_OUT)
						  {
							int id=(long)(ssym->getAnnote(CC_STREAM_ID));
							*fout << "          " ;
							if(((OperatorBuiltin*)bexpr->getOp())->getBuiltinKind()==BUILTIN_CLOSE) {
								*fout  << "STREAM_CLOSE(out[" << id << "]);" << endl;
							} else if(((OperatorBuiltin*)bexpr->getOp())->getBuiltinKind()==BUILTIN_FRAMECLOSE) {
								*fout  << "FRAME_CLOSE(out[" << id << "]);" << endl;
							}
						  }
					  }
				  }
			  }
		  } */
		  //cout << "Ended.." << endl;
		  //exit(-1);


	      }
		{
		  //ccStmt(fout,string("          "),stmt,early_close,
			// STATE_PREFIX,0, false); // 0 was default for ccStmt.h
		}
	   //   *fout << "        else" << endl;
	    }
	    // default case will be to punt out of loop (exit/done)
	    // For now. split eofr cases into separate statecases.. wtf!?
	  /* *fout << "        {" << endl ;
	  *fout << "        if (" << atomiceofrcase.cstring() << ") {}" << endl ;
	  *fout << "        else" << endl;
	  *fout << "         done=1;" << endl;
	  *fout << "        }" << endl;
	    */
	  // close the nesting brackets.
	  *fout << "        ";
	//  for (; numNestings>0; numNestings--) {
	 //   *fout << "}";
	  //}

	  /*if (num_states>1)
	    {
	      *fout << "        break; " << endl;
	      *fout << "      } // end case " << STATE_PREFIX << 
		sname << endl;
	    }*/
	  snum++;
	}
      /*if (num_states>1)
	{
	  *fout << "      default: cerr << \"ERROR unknown state [\" << (int)state << \"] encountered in " << name << "::proc_run\" << endl;" << endl;
	  *fout << "        abort();" << endl;
	  *fout << "    }" << endl;
	}*/

      //*fout << "  }" << endl;
      // any final stuff
      /*if (!noReturnValue(rsym))
		if (early_close[(long)(rsym->getAnnote(CC_STREAM_ID))])
	  {
	    *fout <<"  if (!output_close[" 
		  <<  (long)(rsym->getAnnote(CC_STREAM_ID))
		  << "])" << endl;
	    *fout << "  STREAM_CLOSE(" 
		  << "out[" << (long)(rsym->getAnnote(CC_STREAM_ID))
		  << "]" 
		  << ");" << endl;
	  }
	else
	  *fout << "  STREAM_CLOSE(" 
		<< "out[" << (long)(rsym->getAnnote(CC_STREAM_ID))
		<< "]"
		<< ");" << endl;
      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
		  if (early_close[(long)(ssym->getAnnote(CC_STREAM_ID))])
		    {
		      *fout <<"  if (!output_close[" 
			    <<  (long)(ssym->getAnnote(CC_STREAM_ID))
			    << "])" << endl;
		      *fout << "  STREAM_CLOSE(" 
			    << "out[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			    << "]" 
			    << ");" << endl;
		    }
		  else
		    *fout << "  STREAM_CLOSE(" 
			  << "out[" << (long)(ssym->getAnnote(CC_STREAM_ID)) 
			  << "]" 
			  << ");" << endl;
		}
	      else
		if (early_free[(long)(ssym->getAnnote(CC_STREAM_ID))])
		  {
		    cerr << "DEBUG early free[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			 << "]=" << early_free[(long)(ssym->getAnnote(CC_STREAM_ID))]
			 << endl;
		    
		    *fout <<"  if (!input_free[" 
			  << (long)(ssym->getAnnote(CC_STREAM_ID)) 
			  << "])" << endl;
		    *fout << "    STREAM_FREE(" 
			  << "in[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			  << "]" 
			  << ");" << endl;
		  }
		else
		  *fout << "  STREAM_FREE(" 
			<< "in[" << (long)(ssym->getAnnote(CC_STREAM_ID))
			<< "]" 
			<< ");" << endl;
	    }
	}*/
    }
  
}


