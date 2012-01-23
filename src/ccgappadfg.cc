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
//	cout << "node to var string : " << endl;
//	cout << "\t" << endl;
//	cout << t ? t->toString().replace_all("\n","") : string("<nil>");
	return t ? t->toString().replace_all("\n","") : string("<nil>");
}

void ccgappadfgprocrun(ofstream *fout, string name, OperatorBehavioral *bop, string type_val, string precision, int *if_nb,
	       string *exp_str)
{

      
      dictionary<string,State*>* states=bop->getStates();
      dic_item item;

	  list<string> *list_input = new list<string>();
	  list<string> *list_output = new list<string>();
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
				list_input->push(asum->getName());
				
		
			}
		}
		
      int icnt=0;
      int ocnt=0;
      //Symbol *rsym=bop->getRetSym();
      list<Symbol*> *argtypes=bop->getArgs();
    
      Symbol *sym;
      
      forall(sym,*argtypes)
	{
	  if (sym->isStream())
	    {
	      SymbolStream *ssym=(SymbolStream *)sym;
	      if (ssym->getDir()==STREAM_OUT)
		{
			list_output->push(sym->getName());
		  sym->setAnnote(CC_STREAM_ID,(void *)ocnt);
		  ocnt++;
		}
	      else if (ssym->getDir()==STREAM_IN)
		{
			
			list_input->push(sym->getName());
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
     // int snum=0;
      forall_items(item,*states)
	{
	  State* cstate=states->inf(item);
	  string sname=cstate->getName();

	  array<StateCase*>* cases=caseSort(cstate->getCases());
	  array<Symbol*>* caseIns=allCaseIns(cases);
	  //array<int>* firstUsed=inFirstUsed(caseIns,cases);
          int numNestings=0;

	  if (num_states>1)
	    cerr << "gappa does not accept more than one state" << endl;

	string atomiceofrcase="0";

	  for (int i=cases->low();i<=cases->high();i++)
	    {
	      // walk over inputs to case
	      //InputSpec *ispec;
	      StateCase *acase=(*cases)[i];

          // increment the nesting count and also output a beginning
          // nesting bracket.
          numNestings++;

	      BlockDFG dfgVal=(acase->getDataflowGraph());
	      h_array<node, Symbol*> symbolmap=(acase->getSymbolMap());
	      //h_array<Symbol*,node> *valid_map=(acase->getValidMap());
	    
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
		
			//cout << "Processing :  " ;
			//cout << nodetostring(n,(dfgVal)[n],nodenums[n], list_input) << endl;
			
			TypeKind type = ((Expr*)t)->typeCheck()->getTypeKind();
		
			//cout << "dfg->indeg(n) = " << dfg->indeg(n) << endl;
			
		  	if(!dfg->indeg(n)==0) {
				
/*				if ( nodetostring(n,(dfgVal)[n],nodenums[n], list_input) == "lambda")
				{
					cout << "input edges = " << dfg->indeg(n) << endl;
					if(type!=TYPE_STATE)
						cout << "type!=TYPE_STATE" << endl;
					else
						cout << "type==TYPE_STATE" << endl;
				} 
	*/			
				
				if(dfg->indeg(n)==3) {
					
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
						/* 
						if(edgenum==0) {
							cout << nodetofout(dfg, src, nodenums) + "--edge0";
						} else if(edgenum==1) {
							cout << nodetofout(dfg, src, nodenums) + "--edge1";
						} else if(edgenum==2) {
							cout << nodetofout(dfg, src, nodenums) + "--edge2";
						}
						*/
						
						//cout << " edgenum = " << edgenum << endl; 
						
						if(edgenum==0) {
							temp = nodetofout(dfg, src, nodenums) + type_val;
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + type_val + ") * " ;
					
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
					
								ifstr = ifstr +") + ";
							}	
					
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
					
								ifstr = ifstr +") ";
							}
						}
						
						edgenum++;
					}
					
					*fout << ifstr << ";" << endl;
					}
				
				} else if(dfg->indeg(n)==2 && t->getKind()==TREE_EXPR && ((Expr*)t)->getExprKind()==EXPR_COND) {
					
					// if operator
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
					
					forall_in_edges (e,n) {
						// - examine inputs of n
						node src=(*dfg).source(e);
						if(edgenum==0) {
							temp = nodetofout(dfg, src, nodenums) + type_val;
							ifstr = ifstr + "(" + nodetofout(dfg, src, nodenums) + type_val + ") * ";
					
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
					
						}
						edgenum++;
					}
					*fout << ifstr << ";" << endl;
				} else if(dfg->indeg(n)==2) {
					
					// binary operator
					if (typekindToCplusplus(type) == "bool")
					{
					
					char nb[255];
					sprintf(nb, "%i", *if_nb);
					string number = nb;
					
					Tree* tmp=(*dfg)[n];
					*fout << "#" <<  getnodenumber(n, tmp) <<  " is cond" << number << endl; 
					*fout << nodetofout(dfg,n,nodenums) << type_val << " " << " = ( ";
					*fout << "fixed<-1,ne> (cond" << number << ")";
					 
					 (*if_nb)++;
				    }
				    else
				    {
						
						{
						bool mac  =false;					
						 string RHS = "(";
						list<edge> dfg_in_edges_n=(*dfg).in_edges(n);
						int edgenum=0;

						// Nachiket 14/1/2012 - Is the order of visiting edges consistent??
						/*
						edge e1_test;
						forall_in_edges (e1_test,n) {
							node src=(*dfg).source(e1_test);
							string temp = nodetostring(src,(dfgVal)[src],nodenums[src], list_input);
							cout << "Loop: Edge Source=" << temp << "," << (*dfg)[e1_test] << endl;
						}

						edge in1=dfg_in_edges_n.inf(dfg_in_edges_n.get_item(0));
						node src1=(*dfg).source(in1);
						string temp1 = nodetostring(src1,(dfgVal)[src1],nodenums[src1], list_input);
						cout << "Iter: Edge Source=" << temp1 << "," << (*dfg)[in1] << endl;
						edge in2=dfg_in_edges_n.inf(dfg_in_edges_n.get_item(1));
						node src2=(*dfg).source(in2);
						string temp2 = nodetostring(src2,(dfgVal)[src2],nodenums[src2], list_input);
						cout << "Iter: Edge Source=" << temp2 << "," << (*dfg)[in2] << endl;					       
						forall_in_edges (e,n) {
						*/
					
						// edge1
						edge e1, e2;
						e1=dfg_in_edges_n.inf(dfg_in_edges_n.get_item(0));
						e2=dfg_in_edges_n.inf(dfg_in_edges_n.get_item(1));
						if((*dfg)[e1]==0 && (*dfg)[e2]==1) {
							// things are fine..
						} else {
							e1=dfg_in_edges_n.inf(dfg_in_edges_n.get_item(1));
							e2=dfg_in_edges_n.inf(dfg_in_edges_n.get_item(0));
						}
						
						node src1=(*dfg).source(e1);					
						string temp1 = nodetostring(src1,(dfgVal)[src1],nodenums[src1], list_input);
						string test1; bool found1 = false;
						
						forall (test1, *list_input) {	
							found1 = (temp1 == test1)? true: found1;
						}
					
						Tree *tree_temp1=(*dfg)[src1];
						RHS+= temp1;
						if(!((tree_temp1->getKind()==TREE_EXPR) && (((Expr*)tree_temp1)->getExprKind()==EXPR_VALUE)) || found1)  
						{
								RHS+= type_val ;
						}
						
						RHS += " ";
						string fn = nodetofnstring(n,(dfgVal)[n]);
						RHS += fn + " ";
						
						// edge2
						node src2=(*dfg).source(e2);
						string temp2 = nodetostring(src2,(dfgVal)[src2],nodenums[src2], list_input);
						string test2; bool found2 = false;
						
						forall (test2, *list_input) {	
							found2 = (temp2 == test2)? true: found2;
						}
					
						Tree *tree_temp2=(*dfg)[src2];
						RHS+= temp2;
						if(!((tree_temp2->getKind()==TREE_EXPR) && (((Expr*)tree_temp2)->getExprKind()==EXPR_VALUE)) || found2)  
						{
								RHS+= type_val ;
						}
						
						
						/*
						 * temporarily disable MAC detection..
						if(edgenum==0) {
							string fn = nodetofnstring(n,(dfgVal)[n]);
					//		cout << "function : " << nodetofnstring(n,(dfgVal)[n]) << endl; 
							if (type_val == "_cuda32" && (fn == "*" )) // check for MAC loop
							{
								edge ed;								
								
								forall_out_edges (ed,n) 
								{
									node dest=(*dfg).target(ed);
//									cout << "functions in dest : " << nodetofnstring(dest,(dfgVal)[dest]) <<  "//" << endl; 
									
									if (nodetofnstring(dest,(dfgVal)[dest]) == "+")
									{
										mac = true;
									}
									
								}
								if (mac)
									RHS = " float<cuda_32,zr>" + RHS + fn +  " ";
								else
									RHS += fn + " ";
							}
							else
								RHS += fn + " ";
						}
						edgenum++;
						*/
					
				
					if (type_val != "_m")	
					{
						if (type_val == "_cuda32" && nodetofout(dfg,n,nodenums).contains("multiply") && mac)
							*fout << nodetofout(dfg,n,nodenums) << type_val << " " << " = ";
						else
							*fout << nodetofout(dfg,n,nodenums) << type_val << " " << precision << " = ";
					}
					else
					{
						*fout << nodetofout(dfg,n,nodenums) << type_val << " " <<  " = ";
					}

				
					
					*fout << RHS;
				}
				}
					*fout << ");" << endl;
				} else if(dfg->indeg(n)==1 && type!=TYPE_STATE) {
					
					// unary operator or function?
					if (nodetostring(n,(dfgVal)[n],nodenums[n], list_input)(1,5) != "valid" )
					{
						
						bool in =false;
						string test_out;
						
						forall (test_out, *list_input)
						{	
							if (nodetostring(n,(dfgVal)[n],nodenums[n], list_input) == test_out)
								in = true;
						}
						
						
						//*fout << "\ninput ? " << in << endl;
						
						list<string>* list_total = new list<string> ();
						string test1;
						forall (test1, *list_input)
						{	
							list_total->push(test1);
						}

						forall (test1, *list_output)
						{	
							list_total->push(test1);
						}
						
						// we need a list that contains both input and output variables to not write the identifier on both of these when
						// the function nodetostring is called.
						
						if(!in) // if the variable is in the input or the parameter list then it has already been written and we do not
								// want to overwrite it
						{
					
						if (type_val != "_m")
							*fout << nodetostring(n,(dfgVal)[n],nodenums[n], list_total) << type_val  << " " << precision <<  " = ";
						else
						{
							*fout << nodetostring(n,(dfgVal)[n],nodenums[n], list_total) << type_val  << " " <<   " = ";
						}

						
						if (nodetofnstring(n,(dfgVal)[n]) == "!")
							*fout << "(1 -  ( ";
						else
						{
							string fn = nodetofnstring(n,(dfgVal)[n]);						
							
							if (type_val == "_cuda32" && (fn == "exp" ))
							{
								double x = log2(exp(1));
								*fout << fn + "2 ( "  << x <<  " * "; // if we have an exponential we want to multiply by log2(e) to transform	
																	// from exp to exp2
								if (*exp_str != "")
									*exp_str += "\t\t" ;
								*exp_str =  *exp_str + "(((" + nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_m - " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) / " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) >= 2b-23 \\/ " +
										"((" +nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_m - " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) / " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) <= -2b-23) /\\  \n" ;
							}
							else if(type_val == "_cuda32" && (fn == "log" ))
							{
								double x = log(2);
								*fout <<  x << " * " + fn + "2 ( "; // if we have a ln we want to multiply by ln(2) to transform from 
																		// ln to log2
																		
								if (*exp_str != "")
									*exp_str += "\t\t" ;
								*exp_str =  *exp_str + "(((" + nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_m - " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) / " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) >= 3b-23 \\/ " +
										"((" +nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_m - " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) / " +
										nodetostring(n,(dfgVal)[n],nodenums[n], list_total) + "_cuda32) <= -3b-23) /\\ \n";
					
							}
							else
								*fout << fn + " ( ";
						}
						edge e=dfg->first_in_edge(n);
						// - examine inputs of n
						node src=(*dfg).source(e);
						
						string temp = nodetostring(src,(dfgVal)[src],nodenums[src], list_input);
						
						
						string test;
						bool found = false;
						
						forall (test, *list_input)
						{	
							if (temp == test)
								found = true;
						}
					
						Tree *tree_temp=(*dfg)[src];
						*fout << temp;
						if(!((tree_temp->getKind()==TREE_EXPR) && (((Expr*)tree_temp)->getExprKind()==EXPR_VALUE)) || found)  
						{
								*fout << type_val ;
						}

						
						*fout <<" ";
						
						
						
						if (nodetofnstring(n,(dfgVal)[n]) == "!")
							*fout << ")";
							
						*fout << ");" << endl;
					}
				}
					
				} else if(dfg->indeg(n)==1 && type==TYPE_STATE && num_states>1) {
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
		  	//cout << " finished" << endl;
		  }
		}
	}
  }

}
