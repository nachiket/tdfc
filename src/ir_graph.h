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
// SCORE TDF compiler:	Internal Representation
// $Revision: 1.131 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_IR_GRAPH_
#define _TDF_IR_GRAPH_

#include "compares.h"
#include <LEDA/core/list.h>
#include <LEDA/graph/graph.h>
#include "state.h"
#include "expr.h"
#include "ir_analyze_registers.h"

using leda::GRAPH;
using leda::node;
using leda::edge;

class OFNode;
class OFEdge;
class SFNode;
class SCCell;
class SFEdge;
class DFNode;
class DFEdge;
typedef GRAPH<OFNode*,OFEdge*>* OFGraph;
typedef GRAPH<SFNode*,SFEdge*>* SFGraph;
typedef GRAPH<DFNode*,DFEdge*>* DFGraph;
typedef list< set<node>* > * Schedule;

enum OFNodeKind {OFN_BEHAVIORAL, OFN_PIPELINE, OFN_MEMORY};


class OFNode{
 protected:
  GRAPH<SFNode*,SFEdge*>*		to_SFG;
  GRAPH<DFNode*,DFEdge*>*		to_DFG;	//for pipes; should really subclass here...
  Operator*				oper; 
  OFNodeKind				kind;
  ExprCall*				exprcall;
  dictionary <string,Symbol*>*		registers;	//the op registers
  dictionary <string,ExprValue*>*	ini_reg_values;	//the op initial registers val
  dictionary <string,Symbol*>*		streams_in;  
  dictionary <string,Symbol*>*		streams_out;	
  list < list<Tree*>*>	*		non_bool_conds;
  // the following is the list of all unique expressions (flattened into a list 
  // of tree nodes, for comparing expressions among each others) that go into 
  // muxes, for each register and foreach outstream, within one operator.
  // ex: state a: reg1=1 --- state b: reg1=a --> the reg1 exprs_to_mux entry  will 
  // have 2 elements in the list: '1' and 'a', flattened into list of tree nodes.
  dictionary <string,list<list<Tree*>*>*> * exprs_to_muxes;
  dictionary <string,int>*		mux_bw;
  int					area;
  int					ip_ex_area;
  int					op_ex_area;
  int					unex_area;
  string				name_for_graph;
  dictionary <string,DefUsePairList*>*	defUsePairs;
 public:
  OFNode(Operator* op, OFNodeKind k, ExprCall* ex=nil);
  GRAPH<SFNode*,SFEdge*>*	  getSFG()	     const {return to_SFG;}
  GRAPH<DFNode*,DFEdge*>*	  getDFG()	     const {return to_DFG;}
  Operator*			  getOperator()	     const {return oper;}
  ExprCall*			  getExprCall()	     const {return exprcall;}
  OFNodeKind			  getKind()	     const {return kind;}
  dictionary <string,Symbol*>*	  getRegisters()     const {return registers;}
  dictionary <string,ExprValue*>* getInitialvalues() const {return ini_reg_values;}
  dictionary <string,Symbol*>*	  getStreamsIn()     const {return streams_in;}
  dictionary <string,Symbol*>*	  getStreamsOut()    const {return streams_out;}
  list <list<Tree*>*>*		  getConditions()    const {return non_bool_conds;}
  dictionary <string,list<list<Tree*>*>*> * getExprsToMuxes() 
    const {return exprs_to_muxes;}
  dictionary <string,int>*	  getMuxBw()	     const {return mux_bw;}
  void	      setRegisters(dictionary <string,Symbol*>* reg) {registers=reg;}
  void	      setArea(int a)				   {area = a;}
  int	      getArea()				     const {return area;}
  void	      addUnextractedArea(int a)			   {unex_area +=a;}
  int	      getUnextractedArea()		     const {return unex_area;}
  void	      setNameforGraph(string st)		   {name_for_graph=st;}
  string      getNameforGraph()			     const {return name_for_graph;}
  dictionary<string,DefUsePairList*>* getDefUsePairs() const {return defUsePairs;}
  void setDefUsePairs(string s,DefUsePairList* d)
						     const {defUsePairs->insert(s,d);}
};


class SCCell{
 protected:
  GRAPH<DFNode*,DFEdge*>*		to_DFG;
  SFNode*				to_SFN;
  StateCase*				statecase;
  dictionary <string,DFNode*>*		defined_registers;
  dictionary <string,node>*		used_registers;
  dictionary <string,DFNode*>*		written_streams;
  dictionary <string,node>*		used_streams;
  Schedule				the_schedule;
  int					area;
  int					ip_ex_area;
  int					op_ex_area;
  int					unex_area;
  bool					done;
  int					hidden;
  set<string>*				IPinputs;
  set<string>*				OPoutputs;
  set<string>*				IPinterm;
  set<string>*				OPinterm;
  set<string>*				in_str_left_in;
 public:
  SCCell(SFNode* sfn,StateCase* sc=nil);
  GRAPH<DFNode*,DFEdge*>*     getDFG()	     const	{return to_DFG;}
  SFNode*		      getSfn()	     const	{return to_SFN;}
  dictionary<string,DFNode*>* getDefReg()    const	{return defined_registers;}
  dictionary<string,node>*    getUsedReg()   const	{return used_registers ;}
  dictionary<string,DFNode*>* getWrittenStreams()const	{return written_streams ;}
  dictionary<string,node>*    getUsedStreams()const	{return used_streams ;}
  Schedule		      getSchedule()  const	{return the_schedule;}
  StateCase*		      getStateCase() const	{return statecase;}
  void			      setStateCase(StateCase* s){statecase=s;}
  void			      setArea(int a)		{area = a;}
  int			      getArea()	     const	{return area;}
  void			      addIPArea(int a)		{ip_ex_area+=a;}
  int			      getIPArea()    const	{return ip_ex_area;}
  void			      addOPArea(int a)		{op_ex_area+=a;}
  int			      getOPArea()    const	{return op_ex_area;}
  void			      setUnextractedArea(int a) {unex_area =a;}
  int			      getUnextractedArea() const{return unex_area;}
  bool			      getDone()	     const	{return done;}	    
  void			      setDone()			{done=1;}      
  int			      getHidden()    const	{return hidden;}      
  void			      setHidden(int i)		{hidden=i;}  
  set<string>*		  getIPInputs()		const	{return IPinputs;}
  set<string>*		  getOPOutputs()	const	{return OPoutputs;}
  set<string>*		  getIPInterm()		const	{return IPinterm;}
  set<string>*		  getOPInterm()		const	{return OPinterm;}
  set<string>*		  getInStrLeftIn()	const	{return in_str_left_in;}
};


class SFNode{
 protected:
  State *	   state;
  SCCell*	   regular_sccell;// the signature without eos
  list<SCCell*>*   sccells;	  // all signatures -- (member above is first 
				  // element of this)
  set<string>*	   consumed_streams; // at the moment all statecases consume 
				     // the same streams, so it is a state-level 
				     // quantity rather than an SCCell one. 
				     // later on this might change!(w/ partition)
  int		   area;     // sum of area of all signatures;
 public:
  SFNode(State*);
  State*	     getState()		const	{return state;}
  SCCell*	     getRegSCCell()	const	{return regular_sccell;}
  list<SCCell*>*     getSCCells()	const	{return sccells;}  
  set<string>*	     getConsumedStreams() const {return consumed_streams;}
  int		     getArea()		const	{return area;}
  void		     setArea(int a)		{area=a;}
};


class DFEdge{
 protected:
  //ExprLValue* lvalue;
  string name;
  bool	 ready_sched;
  bool	 ready_IPextr;
  bool	 ready_OPextr;
  edge	 to_image_edge; // pointer to the image edge, i.e. the one in the OFG
 public:
  DFEdge(string);
  string      getName()	      const {return name;}
  bool	      isReadySched()  const {return ready_sched;}
  void	      makeReadySched()	    {ready_sched=1;}
  bool	      isReadyIPExtr() const {return ready_IPextr;}
  void	      makeReadyIPExtr()	    {ready_IPextr=1;}
  bool	      isReadyOPExtr() const {return ready_OPextr;}
  void	      makeReadyOPExtr()	    {ready_OPextr=1;}
  edge	      getImageEdge()  const {return to_image_edge;}
  void	      setImageEdge(edge ed) {to_image_edge = ed;} 
};


enum DFNodeKind {DFN_OP,       // ExprBop ExprUop   ....................//0
		 DFN_ASSIGN,   // StmtAssign				//1
		 DFN_CAST,     // ExprCast				//2
		 DFN_INVAL,    // ExprValue				//3
		 DFN_INLVAL,   // ExprLValue non Register		//4
		 DFN_REG,      // ExprLValue Register			//5
		 DFN_STR,      // ExprLValue Stream			//6
		 DFN_ENDCOND,  // fake node, the root of an ExprCond	//7
		 DFN_TOP_REG,  // fake node, a top level reg node	//8
		 DFN_TOP_STR,  // fake node, a top level stream node	//9
		 DFN_SELECTOR}; // exlv[a:b] edges to this node are exlv,a,b


class DFNode{
 protected: 
  Expr*			ast_node_expr;	
  DFNodeKind		dfnkind;
  string		name;	      //always set
  ExprLValue*		dest;	      //only for _ASSIGN, _REG 
  bool			IP_extracted;
  bool			OP_extracted;
  Expr*			assigned;     //only for _ASSIGN
  int			area;	      //in LUTs
  int			scheduled;
  string		name_for_graph;//name for the emitted .vcg graph
  list<ExprLValue*>*	inputs;	       //it's now set only for ASSIGN and COND
  edge			to_ofedge;     // this is used during pipeline extraction.
  DFEdge*		to_dfe;

 public:
  DFNode(Expr* ex,DFNodeKind dk,string nm,ExprLValue* dst =nil);
  Expr *		getExpr()	    const    {return ast_node_expr;}
  Expr*			getAssigned()	    const    {return assigned;}
  void			setAssigned(Expr* as)	     {assigned=as;}
  DFNodeKind		getDFNodeKind()	    const    {return dfnkind;}
  list<ExprLValue*>*	getInputs()	    const    {return inputs;}
  ExprLValue *		getDest()	    const    {return dest;}
  string		getName()	    const    {return name;}
  int			isScheduled()	    const    {return scheduled;}
  void			makeScheduled(int step)	     {scheduled=step;}
  bool			isIPExtracted()	     const   {return IP_extracted;}
  void			makeIPExtracted()	     {IP_extracted=1;}
  bool			isOPExtracted()	     const   {return OP_extracted;}
  void			makeOPExtracted()	     {OP_extracted=1;}
  void			setArea(int a)		     {area=a;}
  int			getArea()	    const    {return area;}
  void			setNameforGraph(string st)   {name_for_graph=st;}
  string		getNameforGraph()   const    {return name_for_graph;}
  edge			getOFEdge()	    const    {return to_ofedge;}
  void			setOFEdge(edge oe)	     {to_ofedge=oe;}
  DFEdge*		getDFEdge()	    const    {return to_dfe;}
};


class OFEdge{
 protected:
  string  name;		// symbol name;
 public:
  OFEdge(string st):name(st){};
  string	       getName()	  const {return name;}
};


class SFEdge{
 protected:
  SCCell*     sc_cell;	   // which statecase is it referring to
  Expr*	      condition;   // and which condition has it come from (nil if no cond)
  bool	      outcome;	   // true or false branch?
  string      cond_string; // CONDi
  bool	      all_bool;
  dictionary<string,int>*  rank_mux;
	// for all regs and streams defined in this statecase, 
	// rank_mux (above) says which is the rank of the correspondent 
	// bit in the list of mux outputs (in the kiss2 form)
 public:
  SFEdge(SCCell* , Expr* , bool);
  SCCell*     getSCCell()    const {return sc_cell;}  
  Expr*	      getCond()	     const {return condition;}
  bool	      getOutcome()   const {return outcome;}
  bool	      getAllBool()   const {return all_bool;}
  void	      setAllBool(bool ab)  {all_bool=ab;}
  string      getConditionString() {return cond_string;}
  void	      setConditionString(string st)	{cond_string =st;}
  dictionary<string,int>*  getRankMux()	  const {return rank_mux;}
};


struct to_pass {
  ExprValue* val;
  string reg;
};

//-----------------------

// print 

//void print_OFG(OFGraph);




#endif	// #ifndef _TDF_IR_GRAPH_
