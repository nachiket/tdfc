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
// SCORE TDF compiler:  Verilog back-end for compositional ops, pages
// $Revision: 1.5 $
//
//////////////////////////////////////////////////////////////////////////////


#include <time.h>
#include <fstream>
#include "state.h"
#include "operator.h"
#include "stmt.h"
#include "expr.h"
#include "misc.h"
#include "version.h"
#include "bindvalues.h"
#include "everilog.h"
#include "symbol.h"
#include "everilog_symtab.h"

#include <sstream>
#include <iostream>

#include "Q_blackbox.h"
#include "Q_wire.h"
#include "Q_srl.h"
#include "Q_fifo_bubl.h"
#include "Q_fifo_bubl_noen.h"
#include "Q_pipe.h"
#include "Q_pipe_noretime.h"
#include "Q_fwd_pipe.h"
#include "Q_fwd_pipe_noretime.h"
#include "Q_srl_reserve.h"
#include "Q_srl_reserve_lwpipe.h"
#include "Q_srl_reserve_nolwpipe.h"
#include "Q_lwpipe.h"
#include "Q_lowqli.h"
#include "Q_lowqli_in.h"
#include "Q_lowqli_out.h"
#include "Q_lowqli_p_p_srl_r.h"
#include "Q_lowqli_in_p_p_srl_r.h"
#include "Q_lowqli_out_p_p_srl_r.h"
#include "Q_lowqli_r_p_srl_r.h"
#include "Q_lowqli_in_r_p_srl_r.h"
#include "Q_lowqli_out_r_p_srl_r.h"

#include "SEG_rw.h" // 16/Oct/2011
#include "SEG_rw_fsm.h" // 20/Oct/2011
#include "SEG_rw_dp.h" // 20/Oct/2011

#include "SEG_r.h" // 22/Oct/2011
#include "SEG_r_fsm.h" // 22/Oct/2011
#include "SEG_r_dp.h" // 22/Oct/2011

#include "SEG_seq_r.h" // 22/Oct/2011
#include "SEG_seq_r_fsm.h" // 22/Oct/2011
#include "SEG_seq_r_dp.h" // 22/Oct/2011

#include "SEG_seq_cyclic_r.h" // 23/Oct/2011
#include "SEG_seq_cyclic_r_fsm.h" // 23/Oct/2011
#include "SEG_seq_cyclic_r_dp.h" // 23/Oct/2011
#include <LEDA/core/list.h>
#include <LEDA/core/array.h>
//#include <LEDA/core/map.h>
#include <LEDA/core/d_array.h>
#include <LEDA/core/h_array.h>

using leda::h_array;
using leda::list_item;
using std::ofstream;

// - Command line options to set stream implementation
//   for Verilog page emission (-everilog)
//     * "-qim" "-qid" "-qili" "-qilo" "-qiw" for page  input queues
//     * "-qom" "-qod" "-qoli" "-qolo" "-qow" for page output queues
//     * "-qlm" "-qld" "-qlli" "-qllo" "-qlw" for page  local streams
// - Depths to be replaced by stream depth bounds analysis
string gPageInputQueueModule         = "Q_srl";	// "-qim" module         , in  q: Q_srl
int    gPageInputQueueDepth          =  2; 	// "-qid" depth          , in  q:  2
int    gPageInputQueueLogicInDepth   = -1; 	// "-qil" logic in  depth, in  q: -1
int    gPageInputQueueLogicOutDepth  = -1; 	// "-qil" logic out depth, in  q: -1
int    gPageInputQueueWireDepth      = -1; 	// "-qiw" wire      depth, in  q: -1
string gPageOutputQueueModule        = "Q_srl";	// "-qom" module         , out q: Q_srl
int    gPageOutputQueueDepth         =  2; 	// "-qod" depth          , out q:  2
int    gPageOutputQueueLogicInDepth  = -1; 	// "-qol" logic in  depth, out q: -1
int    gPageOutputQueueLogicOutDepth = -1; 	// "-qol" logic out depth, out q: -1
int    gPageOutputQueueWireDepth     = -1; 	// "-qow" wire      depth, out q: -1
string gPageLocalQueueModule         = "Q_srl";	// "-qlm" module         , local: Q_srl
int    gPageLocalQueueDepth          =  2; 	// "-qld" depth          , local:  2
int    gPageLocalQueueLogicInDepth   = -1; 	// "-qll" logic in  depth, local: -1
int    gPageLocalQueueLogicOutDepth  = -1; 	// "-qll" logic out depth, local: -1
int    gPageLocalQueueWireDepth      = -1; 	// "-qlw" wire      depth, local: -1

// - Netlist stream settings are presently hardwired, no command line options
string gNetlistInputQueueModule      = "Q_wire";//        module         , in  q: Q_wire
int    gNetlistInputQueueDepth         =  0; 	//        depth          , in  q:  0
int    gNetlistInputQueueLogicInDepth  = -1; 	//        logic in  depth, in  q: -1
int    gNetlistInputQueueLogicOutDepth = -1; 	//        logic out depth, in  q: -1
int    gNetlistInputQueueWireDepth     = -1; 	//        wire      depth, in  q: -1
string gNetlistOutputQueueModule     = "Q_wire";//        module         , out q: Q_wire
int    gNetlistOutputQueueDepth        =  0; 	//        depth          , out q:  0
int    gNetlistOutputQueueLogicInDepth = -1; 	//        logic in  depth, out q: -1
int    gNetlistOutputQueueLogicOutDepth= -1; 	//        logic out depth, out q: -1
int    gNetlistOutputQueueWireDepth    = -1; 	//        wire      depth, out q: -1
string gNetlistLocalQueueModule      = "Q_wire";//        module         , local: Q_wire
int    gNetlistLocalQueueDepth         =  0; 	//        depth          , local:  0
int    gNetlistLocalQueueLogicInDepth  = -1; 	//        logic in  depth, local: -1
int    gNetlistLocalQueueLogicOutDepth = -1; 	//        logic out depth, local: -1
int    gNetlistLocalQueueWireDepth     = -1; 	//        wire      depth, local: -1


////////////////////////////////////////////////////////////////
//  Info scanned from TDF Compositional Operators

class EVerilogInfoCompose {		  // - info for verilog compos emission
public:
  VerilogSymTab	        vsymtab;	  // - symbol table for verilog names
  h_array<Symbol*,string>   input_data,	  // - data  input  for input stream
                        input_eos,	  // - eos   input  for input stream
                        input_valid,	  // - valid input  for input stream
                        input_bp,	  // - backpressure output for in strm
                        output_data,	  // - data  output for output stream
                        output_eos,	  // - eos   output for output stream
                        output_valid,	  // - valid output for output stream
                        output_bp,	  // - backpressure input for out strm
					  // - return symbol becomes  out strm
                        stream_data,	  // - data  wire for local stream
                        stream_eos,	  // - eos   wire for local stream
                        stream_valid,	  // - valid wire for local stream
                        stream_bp;	  // - backpressure wire for local strm
  d_array<string,string>
			qin, qout;	  // - _qin and _qout counterparts
					  //     for each wire of queued stream
					  //     (map from verilog to verilog)
  h_array<Symbol*,int>	min_queue,	  // - minimum size for stream queue...
			max_queue;	  // - maximum size for stream queue
					  //     that guarantees no deadlock
  set<Symbol*>		usedStreams;	  // - I/O + streams actually used
  list<ExprCall*>	composeCalls;	  // - calls to compositional ops
  list<ExprCall*>	behavCalls;	  // - calls to behavioral ops
  list<ExprBuiltin*>	segmentCalls;	  // - calls to segment ops
  int                   isPage; 	  // - true if op is page, not netlist
  EVerilogInfoCompose () {}
};


////////////////////////////////////////////////////////////////
//  Scan TDF Compositional

bool tdfToVerilog_compose_scanTdf_map (Tree *t, void *i)
{
  // - Scan TDF compositional operator in preparation for Verilog emission
  // - build up and return results in (EVerilogInfoCompose*) i,
  //     including selecting verilog names and building verilog symbol table,

  EVerilogInfoCompose *info = (EVerilogInfoCompose*)i;

  switch (t->getKind())
  {
    case TREE_OPERATOR:	{
			  assert(((Operator*)t)->getOpKind()==OP_COMPOSE);
			  OperatorCompose *op=(OperatorCompose*)t;

			  // - create verilog inputs, outputs, wires
			  //     for I/O streams and local streams
			  // - return stream, if any, is first verilog I/O
			  list<Symbol*> syms = args_with_retsym_first(op);
			  Symbol *var;
			  forall (var, *op->getVars()->getSymbolOrder()) {
			    // - add local streams to syms
			    if (!var->isArray())
			      syms.append(var);
			  }
			  Symbol *sym;
			  forall (sym, syms) {
			    if (sym->isParam()) {
			      // - params should already be bound, ignore
			      // - will catch actual use in ExprLValue below
			      continue;
			    }
			    string sym_d  = sym->getName()+"_d";
			    string sym_e  = sym->getName()+"_e";
			    string sym_v  = sym->getName()+"_v";
			    string sym_b  = sym->getName()+"_b";
			    sym_d  = info->vsymtab.insertRename(sym_d,sym);
			    sym_e  = info->vsymtab.insertRename(sym_e,sym);
			    sym_v  = info->vsymtab.insertRename(sym_v,sym);
			    sym_b  = info->vsymtab.insertRename(sym_b,sym);
			    string sym_qin_d = sym->getName()+"_qin_d";
			    string sym_qin_e = sym->getName()+"_qin_e";
			    string sym_qin_v = sym->getName()+"_qin_v";
			    string sym_qin_b = sym->getName()+"_qin_b";
			    sym_qin_d = info->vsymtab.insertRename(sym_qin_d,sym);
			    sym_qin_e = info->vsymtab.insertRename(sym_qin_e,sym);
			    sym_qin_v = info->vsymtab.insertRename(sym_qin_v,sym);
			    sym_qin_b = info->vsymtab.insertRename(sym_qin_b,sym);
			    string sym_qout_d = sym->getName()+"_qout_d";
			    string sym_qout_e = sym->getName()+"_qout_e";
			    string sym_qout_v = sym->getName()+"_qout_v";
			    string sym_qout_b = sym->getName()+"_qout_b";
			    sym_qout_d = info->vsymtab.insertRename(sym_qout_d,sym);
			    sym_qout_e = info->vsymtab.insertRename(sym_qout_e,sym);
			    sym_qout_v = info->vsymtab.insertRename(sym_qout_v,sym);
			    sym_qout_b = info->vsymtab.insertRename(sym_qout_b,sym);
			    if (sym->isStream()) {
			      if (((SymbolStream*)sym)->getDir()==STREAM_IN) {
				// sym is STREAM_IN
				info->input_data [sym] = sym_d;
				info->input_eos  [sym] = sym_e;
				info->input_valid[sym] = sym_v;
				info->input_bp   [sym] = sym_b;
			      }
			      else {  // sym is STREAM_OUT
				info->output_data [sym] = sym_d;
				info->output_eos  [sym] = sym_e;
				info->output_valid[sym] = sym_v;
				info->output_bp   [sym] = sym_b;
			      }
			    }
			    else {  // sym is local stream
				info->stream_data [sym] = sym_d;
				info->stream_eos  [sym] = sym_e;
				info->stream_valid[sym] = sym_v;
				info->stream_bp   [sym] = sym_b;
			    }
			    info->qin [sym_d] = sym_qin_d;
			    info->qin [sym_e] = sym_qin_e;
			    info->qin [sym_v] = sym_qin_v;
			    info->qin [sym_b] = sym_qin_b;
			    info->qout[sym_d] = sym_qout_d;
			    info->qout[sym_e] = sym_qout_e;
			    info->qout[sym_v] = sym_qout_v;
			    info->qout[sym_b] = sym_qout_b;
			  }

			  return true;
			}  // case TREE_OPERATOR:

    case TREE_STMT:	{
			  return true;
			}  // case TREE_STMT:

    case TREE_EXPR:	{
			  // - record referenced I/O + local streams
			  if (((Expr*)t)->getExprKind()==EXPR_LVALUE) {
			    Symbol *sym = ((ExprLValue*)t)->getSymbol();
			    if (info->input_data.defined(sym)  ||
				info->output_data.defined(sym) ||
				info->stream_data.defined(sym))
			      info->usedStreams.insert(sym);
			  }
			  // - record calls to compositional + behavioral ops
			  else if (((Expr*)t)->getExprKind()==EXPR_CALL) {
			    Operator *calledop = ((ExprCall*)t)->getOp();
			    if (calledop->getOpKind()==OP_COMPOSE)
			      info->composeCalls.append((ExprCall*)t);
			    if (calledop->getOpKind()==OP_BEHAVIORAL)
			      info->behavCalls.append((ExprCall*)t);
			  }
			  // - record calls to segment ops
			  else if (((Expr*)t)->getExprKind()==EXPR_BUILTIN) {
			    OperatorBuiltin *calledop = (OperatorBuiltin*)
							((ExprBuiltin*)t)->getOp();
			    assert(calledop->getOpKind()==OP_BUILTIN);
			    if (calledop->getBuiltinKind()==BUILTIN_SEGMENT)
			      info->segmentCalls.append((ExprBuiltin*)t);
			    else
			      fatal(1,"-everilog cannot handle builtin \""+
				      calledop->getName()+
				      "\" in a compositional operator",
				    t->getToken());
			  }
			  return true;
			}  // case TREE_EXPR:

    default:		{
			  return true;
			}
  }
}


void tdfToVerilog_compose_scanTdf (OperatorCompose *op,
				   EVerilogInfoCompose *info)
{
  // - Scan TDF compositional operator in preparation for Verilog emission
  // - Build up and return results in *info,
  //     including selecting verilog names and building verilog symbol table

  // - Scan TDF code
  op->map(tdfToVerilog_compose_scanTdf_map, (TreeMap)NULL, (void*)info);

  // - Ensure composition is either netlist or page
  int opIsNetlist = !info->composeCalls.empty() ;
  int opIsPage    = (!info->behavCalls.empty()   && info->composeCalls.empty());

cout << info->composeCalls.size() << "," << info->behavCalls.size() << "," << info->segmentCalls.size() <<endl;

  if (!opIsNetlist && !opIsPage && info->segmentCalls.empty())
    fatal(1, "-everilog cannot handle compositional op "+op->getName()+", "
	     "expecting either a netlist (that calls no behavioral ops) "
	     "or a page (that calls no compositional ops)", op->getToken());
  info->isPage = opIsPage;
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_q    (emit queues for output, local streams)
//  tdfToVerilog_qin  (emit queues for input streams

string tdfToVerilog_q_args_toString (OperatorCompose *op,
				     EVerilogInfoCompose *info,
				     int doInputQueues,
				     int doOutputQueues,
				     int doLocalQueues)
{
  // - emit argument list for Verilog queue module:
  //     clock, reset, stream data/eos/valid/bp (input  to   queue),
  //                   stream data/eos/valid/bp (output from queue)
  // - doInputQueues, doOutputQueues, doLocalQueues should be
  //     true  to handle streams of that type (input, output, local) or
  //     false to omit   streams of that type

  string ret;

  ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";

  // - queue module args:  streams
  list<Symbol*> syms = args_with_retsym_first(op);
  Symbol *var;
  forall (var, *op->getVars()->getSymbolOrder()) {
    // - add local streams to syms
    if (!var->isArray())
      syms.append(var);
  }
  Symbol *sym;
  forall (sym, syms) {
    if (sym->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    string sym_d, sym_e, sym_v, sym_b;
    if (sym->isStream()) {
      if (((SymbolStream*)sym)->getDir()==STREAM_IN) {	// sym is STREAM_IN
	if (!doInputQueues)
	  continue;
	sym_d = info->input_data [sym];
	sym_e = info->input_eos  [sym];
	sym_v = info->input_valid[sym];
	sym_b = info->input_bp   [sym];
      }
      else {  // sym is STREAM_OUT
	if (!doOutputQueues)
	  continue;
	sym_d = info->output_data [sym];
	sym_e = info->output_eos  [sym];
	sym_v = info->output_valid[sym];
	sym_b = info->output_bp   [sym];
      }
    }
    else {  // sym is local stream
      if (!doLocalQueues)
	continue;
      if (!info->usedStreams.member(sym))	// - skip stream if not used
	continue;
      sym_d = info->stream_data [sym];
      sym_e = info->stream_eos  [sym];
      sym_v = info->stream_valid[sym];
      sym_b = info->stream_bp   [sym];
    }
    ret += info->qin [sym_d] + ", "
        +  info->qin [sym_e] + ", "
        +  info->qin [sym_v] + ", "
        +  info->qin [sym_b] + ", ";
    ret += info->qout[sym_d] + ", "
        +  info->qout[sym_e] + ", "
        +  info->qout[sym_v] + ", "
        +  info->qout[sym_b] + ", ";
  }  // forall (sym, syms)

  // - remove final ", "
  ret = ret(0, ret.length()-1-2);

  return ret;
}


string tdfToVerilog_q_argTypes_toString (OperatorCompose *op,
					 EVerilogInfoCompose *info,
					 string indent,
					 int doInputQueues,
					 int doOutputQueues,
					 int doLocalQueues)
{
  // - emit argument type declarations (input/output) for Verilog queue module:
  //     clock, reset, stream data/eos/valid/bp (input  to   queue),
  //                   stream data/eos/valid/bp (output from queue)
  // - doInputQueues, doOutputQueues, doLocalQueues should be
  //     true  to handle streams of that type (input, output, local) or
  //     false to omit   streams of that type

  string ret;

  // - page module arg types:  clock, reset
  ret += indent + "input  " + CLOCK_NAME + ";\n";
  ret += indent + "input  " + RESET_NAME + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - page module arg types:  streams
  list<Symbol*> syms = args_with_retsym_first(op);
  Symbol *var;
  forall (var, *op->getVars()->getSymbolOrder()) {
    // - add local streams to syms
    if (!var->isArray())
      syms.append(var);
  }
  Symbol *sym;
  forall (sym, syms) {
    if (sym->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    int symWidth;
    if (!isConstWidth(sym->getType(),&symWidth))	// - (sets argWidth)
      fatal(1,"-everilog cannot handle non-constant width " +
	      ( sym->getType()->getWidthExpr() ?
	       (sym->getType()->getWidthExpr()->toString()+" ") : string()) +
	      "of stream " + sym->getName() +
	      " of op " + op->getName(),
	    op->getToken());
    string sym_d, sym_e, sym_v, sym_b;
    if (sym->isStream()) {
      if (((SymbolStream*)sym)->getDir()==STREAM_IN) {	// sym is STREAM_IN
	if (!doInputQueues)
	  continue;
	sym_d = info->input_data [sym];
	sym_e = info->input_eos  [sym];
	sym_v = info->input_valid[sym];
	sym_b = info->input_bp   [sym];
      }
      else {  // sym is STREAM_OUT
	if (!doOutputQueues)
	  continue;
	sym_d = info->output_data [sym];
	sym_e = info->output_eos  [sym];
	sym_v = info->output_valid[sym];
	sym_b = info->output_bp   [sym];
      }
    }
    else {  // sym is local stream
      if (!doLocalQueues)
	continue;
      if (!info->usedStreams.member(sym))	// - skip stream if not used
	continue;
      sym_d = info->stream_data [sym];
      sym_e = info->stream_eos  [sym];
      sym_v = info->stream_valid[sym];
      sym_b = info->stream_bp   [sym];
    }
    ret += indent + "input  " + (symWidth>1 ? string("[%d:0] ",symWidth-1)
      					    : string())
      			      + info->qin[sym_d] + ";\n";
    ret += indent + "input  " + info->qin[sym_e] + ";\n";
    ret += indent + "input  " + info->qin[sym_v] + ";\n";
    ret += indent + "output " + info->qin[sym_b] + ";\n";
    ret += indent + "output " + (symWidth>1 ? string("[%d:0] ",symWidth-1)
      					    : string())
      			      + info->qout[sym_d] + ";\n";
    ret += indent + "output " + info->qout[sym_e] + ";\n";
    ret += indent + "output " + info->qout[sym_v] + ";\n";
    ret += indent + "input  " + info->qout[sym_b] + ";\n";
  }

  return ret;
}


string tdfToVerilog_q_calls_toString (OperatorCompose *op,
				      EVerilogInfoCompose *info,
				      string indent,
				      int doInputQueues,
				      int doOutputQueues,
				      int doLocalQueues)
{
  // - emit queue instances (calls) for Verilog queue module
  // - doInputQueues, doOutputQueues, doLocalQueues should be
  //     true  to handle streams of that type (input, output, local) or
  //     false to omit   streams of that type

  string ret;

  // - queue module calls:  queue for each stream
  list<Symbol*> syms = args_with_retsym_first(op);
  Symbol *var;
  forall (var, *op->getVars()->getSymbolOrder()) {
    // - add local streams to syms
    if (!var->isArray())
      syms.append(var);
  }
  Symbol *sym;
  forall (sym, syms) {
    if (sym->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    int depth;
    int symWidth;
    int lidepth, lodepth, wdepth;
    if (!isConstWidth(sym->getType(),&symWidth))	// - (sets argWidth)
      fatal(1,"-everilog cannot handle non-constant width " +
	      ( sym->getType()->getWidthExpr() ?
	       (sym->getType()->getWidthExpr()->toString()+" ") : string()) +
	      "of stream " + sym->getName() +
	      " of op " + op->getName(),
	    op->getToken());
    string sym_d, sym_e, sym_v, sym_b, qName;
    if (sym->isStream()) {
      if (((SymbolStream*)sym)->getDir()==STREAM_IN) {	// sym is STREAM_IN
	if (!doInputQueues)
	  continue;
	sym_d   = info->input_data [sym];
	sym_e   = info->input_eos  [sym];
	sym_v   = info->input_valid[sym];
	sym_b   = info->input_bp   [sym];
	depth   = info->isPage ?    gPageInputQueueDepth
	                       : gNetlistInputQueueDepth;
	lidepth = info->isPage ?    gPageInputQueueLogicInDepth
	                       : gNetlistInputQueueLogicInDepth;
	lodepth = info->isPage ?    gPageInputQueueLogicOutDepth
	                       : gNetlistInputQueueLogicOutDepth;
	wdepth  = info->isPage ?    gPageInputQueueWireDepth
	                       : gNetlistInputQueueWireDepth;
	qName   = info->isPage ?    gPageInputQueueModule
	                       : gNetlistInputQueueModule;
      }
      else {  // sym is STREAM_OUT
	if (!doOutputQueues)
	  continue;
	sym_d   = info->output_data [sym];
	sym_e   = info->output_eos  [sym];
	sym_v   = info->output_valid[sym];
	sym_b   = info->output_bp   [sym];
	depth   = info->isPage ?    gPageOutputQueueDepth
	                       : gNetlistOutputQueueDepth;
	lidepth = info->isPage ?    gPageOutputQueueLogicInDepth
	                       : gNetlistOutputQueueLogicInDepth;
	lodepth = info->isPage ?    gPageOutputQueueLogicOutDepth
	                       : gNetlistOutputQueueLogicOutDepth;
	wdepth  = info->isPage ?    gPageOutputQueueWireDepth
	                       : gNetlistOutputQueueWireDepth;
	qName   = info->isPage ?    gPageOutputQueueModule
	                       : gNetlistOutputQueueModule;
      }
    }
    else {  // sym is local stream
      if (!doLocalQueues)
	continue;
      if (!info->usedStreams.member(sym))	// - skip stream if not used
	continue;
      sym_d   = info->stream_data [sym];
      sym_e   = info->stream_eos  [sym];
      sym_v   = info->stream_valid[sym];
      sym_b   = info->stream_bp   [sym];
      depth   = info->isPage ?    gPageLocalQueueDepth
	                     : gNetlistLocalQueueDepth;
      lidepth = info->isPage ?    gPageLocalQueueLogicInDepth
	                     : gNetlistLocalQueueLogicInDepth;
      lodepth = info->isPage ?    gPageLocalQueueLogicOutDepth
	                     : gNetlistLocalQueueLogicOutDepth;
      wdepth  = info->isPage ?    gPageLocalQueueWireDepth
	                     : gNetlistLocalQueueWireDepth;
      qName   = info->isPage ?    gPageLocalQueueModule
	                     : gNetlistLocalQueueModule;
    }
    string lwdepthstr = (lodepth>-1 ? string(", %d",lodepth) : "") +
			( wdepth>-1 ? string(", %d", wdepth) : "") +
                        (lidepth>-1 ? string(", %d",lidepth) : "") ;
    ret += indent + qName
		  + string(" #(%d, %d", depth, symWidth+1) + lwdepthstr + ") "
		  + "q_" + sym->getName() + " (";
    ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";
    ret += "{" + info->qin [sym_d] + ", "
        +        info->qin [sym_e] + "}, "
        +        info->qin [sym_v] + ", "
        +        info->qin [sym_b] + ", ";
    ret += "{" + info->qout[sym_d] + ", "
        +        info->qout[sym_e] + "}, "
        +        info->qout[sym_v] + ", "
        +        info->qout[sym_b] + ");\n";
  }

  return ret;
}


string tdfToVerilog_q_toString (OperatorCompose *op,
				EVerilogInfoCompose *info)
{
  // - emit Verilog for non-input queue module for compositional operator *op

  string ret;
  string indent;

  // - queue module declaration
  ret += indent + "module " + op->getName() + "_q (";
  ret += tdfToVerilog_q_args_toString(op,info, false,true,true);
  ret += ");\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_q_argTypes_toString(op,info,indent, false,true,true);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_q_calls_toString(op,info,indent, false,true,true);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "_q\n";

  return ret;
}


string tdfToVerilog_qin_toString (OperatorCompose *op,
				  EVerilogInfoCompose *info)
{
  // - emit Verilog for input queue module for compositional operator *op

  string ret;
  string indent;

  // - queue module declaration
  ret += indent + "module " + op->getName() + "_qin (";
  ret += tdfToVerilog_q_args_toString(op,info, true,false,false);
  ret += ");\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_q_argTypes_toString(op,info,indent, true,false,false);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_q_calls_toString(op,info,indent, true,false,false);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "_qin\n";

  return ret;
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_page  (emit page FSM+DP+queues composition)
//
//  DEFUNCT -- this old version instantiates a unified queue module
//             and all operators;  superceded by new "page" and "noin"

/*  NOT USED
 * 
string tdfToVerilog_page_args_toString (OperatorCompose *op,
					EVerilogInfoCompose *info)
{
  // - emit argument list for Verilog page module:
  //     clock, reset, stream data/eos/valid/bp

  string ret;

  ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";

  // - page module args:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {	// arg is STREAM_IN
      ret += info->input_data  [arg] + ", ";
      ret += info->input_eos   [arg] + ", ";
      ret += info->input_valid [arg] + ", ";
      ret += info->input_bp    [arg] + ", ";
    }
    else {  // arg is STREAM_OUT
      ret += info->output_data [arg] + ", ";
      ret += info->output_eos  [arg] + ", ";
      ret += info->output_valid[arg] + ", ";
      ret += info->output_bp   [arg] + ", ";
    }    
  }

  // - remove final ", "
  ret = ret(0, ret.length()-1-2);

  return ret;
}
*/


/*  NOT USED
 *
string tdfToVerilog_page_argTypes_toString (OperatorCompose *op,
					    EVerilogInfoCompose *info,
					    string indent)
{
  // - emit argument type declarations (input/output) for Verilog page module:
  //     clock, reset, stream data/eos/valid/bp

  string ret;

  // - page module arg types:  clock, reset
  ret += indent + "input  " + CLOCK_NAME + ";\n";
  ret += indent + "input  " + RESET_NAME + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - page module arg types:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    assert(arg->isStream());
    int argWidth;
    if (!isConstWidth(arg->getType(),&argWidth))	// - (sets argWidth)
      fatal(1,"-everilog cannot handle non-constant width " +
	      ( arg->getType()->getWidthExpr() ?
	       (arg->getType()->getWidthExpr()->toString()+" ") : string()) +
	      "of formal argument " + arg->getName() +
	      " of op " + op->getName(),
	    op->getToken());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {	// arg is STREAM_IN
      ret += indent + "input  " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
      					      : string())
      				+ info->input_data  [arg] + ";\n";
      ret += indent + "input  " + info->input_eos   [arg] + ";\n";
      ret += indent + "input  " + info->input_valid [arg] + ";\n";
      ret += indent + "output " + info->input_bp    [arg] + ";\n";
    }
    else {  // arg is STREAM_OUT
      ret += indent + "output " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
      					      : string())
      				+ info->output_data [arg] + ";\n";
      ret += indent + "output " + info->output_eos  [arg] + ";\n";
      ret += indent + "output " + info->output_valid[arg] + ";\n";
      ret += indent + "input  " + info->output_bp   [arg] + ";\n";
    }
  }

  return ret;
}
*/


/*  NOT USED
 *
string tdfToVerilog_page_wires_toString (OperatorCompose *op,
					 EVerilogInfoCompose *info,
					 string indent)
{
  // - emit wire declarations for Verilog page module,
  //     namely queue input/output wires

  string ret;

  // - page module wires:  wires for queued streams
  list<Symbol*> syms = args_with_retsym_first(op);
  Symbol *var;
  forall (var, *op->getVars()->getSymbolOrder()) {
    // - add local streams to syms
    if (!var->isArray())
      syms.append(var);
  }
  Symbol *sym;
  forall (sym, syms) {
    if (sym->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    int symWidth;
    if (!isConstWidth(sym->getType(),&symWidth))	// - (sets symWidth)
      fatal(1,"-everilog cannot handle non-constant width " +
	      ( sym->getType()->getWidthExpr() ?
	       (sym->getType()->getWidthExpr()->toString()+" ") : string()) +
	      "of stream " + sym->getName() +
	      " of op " + op->getName(),
	    op->getToken());
    string sym_d, sym_e, sym_v, sym_b;
    if (sym->isStream()) {
      if (((SymbolStream*)sym)->getDir()==STREAM_IN) {	// sym is STREAM_IN
	sym_d = info->input_data [sym];
	sym_e = info->input_eos  [sym];
	sym_v = info->input_valid[sym];
	sym_b = info->input_bp   [sym];
      }
      else {  // sym is STREAM_OUT
	sym_d = info->output_data [sym];
	sym_e = info->output_eos  [sym];
	sym_v = info->output_valid[sym];
	sym_b = info->output_bp   [sym];
      }
    }
    else {  // sym is local stream
      if (!info->usedStreams.member(sym))	// - skip stream if not used
	continue;
      sym_d = info->stream_data [sym];
      sym_e = info->stream_valid[sym];
      sym_v = info->stream_eos  [sym];
      sym_b = info->stream_bp   [sym];
    }
  
    ret += indent + "wire   " + (symWidth>1 ? string("[%d:0] ",symWidth-1)
					    : string())
			      + info->qin [sym_d] + ", "
			      + info->qout[sym_d] + ";\n";
    ret += indent + "wire   " + info->qin [sym_e] + ", "
			      + info->qout[sym_e] + ";\n";
    ret += indent + "wire   " + info->qin [sym_v] + ", "
			      + info->qout[sym_v] + ";\n";
    ret += indent + "wire   " + info->qin [sym_b] + ", "
			      + info->qout[sym_b] + ";\n";
  }

  return ret;
}
*/


/*  NOT USED
 *
string tdfToVerilog_page_assign_toString (OperatorCompose *op,
					  EVerilogInfoCompose *info,
					  string indent)
{
  // - emit "assign" stmts for Verilog page module
  //     to equate I/O wires with certain queue wires

  string ret;

  // - page module assigns:  equate I/O with wires for queued streams
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {	// arg is STREAM_IN
      string arg_d = info->input_data [arg];
      string arg_e = info->input_eos  [arg];
      string arg_v = info->input_valid[arg];
      string arg_b = info->input_bp   [arg];
      ret += indent + "assign " + info->qin[arg_d] + " = " + arg_d + ";\n";
      ret += indent + "assign " + info->qin[arg_e] + " = " + arg_e + ";\n";
      ret += indent + "assign " + info->qin[arg_v] + " = " + arg_v + ";\n";
      ret += indent + "assign " + arg_b            + " = " + info->qin[arg_b] + ";\n";
    }
    else {  // arg is STREAM_OUT
      string arg_d = info->output_data [arg];
      string arg_e = info->output_eos  [arg];
      string arg_v = info->output_valid[arg];
      string arg_b = info->output_bp   [arg];
      ret += indent + "assign " + arg_d + " = " + info->qout[arg_d] + ";\n";
      ret += indent + "assign " + arg_e + " = " + info->qout[arg_e] + ";\n";
      ret += indent + "assign " + arg_v + " = " + info->qout[arg_v] + ";\n";
      ret += indent + "assign " + info->qout[arg_b]
					+ " = " + arg_b              + ";\n";
    }
  }

  return ret;
}
*/


/*  NOT USED
 *
string tdfToVerilog_page_calls_toString (OperatorCompose *op,
					 EVerilogInfoCompose *info,
					 string indent)
{
  // - emit module calls for Verilog page module:
  //     queue, called behavioral ops

  string ret;

  // - page module calls:  queue
  ret += indent + op->getName() + "_q " + op->getName() + "_q_ ("
                + tdfToVerilog_q_args_toString(op,info) + ");\n";

  addEmptyLine(&ret);		// ----------------  

  // - page module calls:  called behavioral ops
  ExprCall *call;
  forall (call, info->behavCalls) {
    OperatorBehavioral *calledop = (OperatorBehavioral*)call->getOp();
    assert(calledop->getOpKind()==OP_BEHAVIORAL);
    ret += indent + calledop->getName() + " " + calledop->getName() + "_ (";
    ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";

    list<Symbol*> formals = args_with_retsym_first(calledop); // - TDF formals
    list<Expr*>   actuals = *call->getArgs();		      // - TDF actuals
							      //     sans ret

    // - if called op has return stream
    //     * return stream becomes first verilog actual
    //        (other than clock, reset)
    //     * return stream is already in "formals", just add it to "actuals"
    Symbol *retsym = calledop->getRetSym();
    if (retsym && !retsym->equals(sym_none)) {
      Tree *p = call->getParent();
      assert(p->getKind()==TREE_STMT);
      assert(((Stmt*)p)->getStmtKind()==STMT_ASSIGN);
      Expr *actual = ((StmtAssign*)p)->getLValue();
      actuals.push(actual);
    }

    // - emit verilog actuals to mirror TDF actuals
    //     (including return stream, if any, which is added above)
    list_item formals_it, actuals_it;
    for (formals_it=formals.first(), actuals_it=actuals.first();
	 formals_it && actuals_it;
	 formals_it=formals.succ(formals_it),
	 actuals_it=actuals.succ(actuals_it)) {
      Symbol *formal = formals.inf(formals_it);
      Expr   *actual = actuals.inf(actuals_it);
      if (formal->isParam()) {
	// - params should already be bound, ignore  (ignore arrays too)
	continue;
      }
      if (actual->getExprKind()!=EXPR_LVALUE)
	fatal(1, "-everilog cannot handle non-stream call argument " +
	         actual->toString(), actual->getToken());
      string actual_d, actual_e, actual_v, actual_b;
      Symbol *actualSym = ((ExprLValue*)actual)->getSymbol();
      if (actualSym->isStream()) {
	if (((SymbolStream*)actualSym)->getDir()==STREAM_IN) {
	  actual_d = info->input_data [actualSym];
	  actual_e = info->input_eos  [actualSym];
	  actual_v = info->input_valid[actualSym];
	  actual_b = info->input_bp   [actualSym];
	}
	else {  // actual is STREAM_OUT
	  actual_d = info->output_data [actualSym];
	  actual_e = info->output_eos  [actualSym];
	  actual_v = info->output_valid[actualSym];
	  actual_b = info->output_bp   [actualSym];
	}
      }
      else {  // actual is local stream
	if (!info->usedStreams.member(actualSym))  // - skip stream if not used
	  continue;
	actual_d = info->stream_data [actualSym];
	actual_e = info->stream_eos  [actualSym];
	actual_v = info->stream_valid[actualSym];
	actual_b = info->stream_bp   [actualSym];
      }
      assert(formal->isStream());
      if (((SymbolStream*)formal)->getDir()==STREAM_IN) {
	ret += info->qout[actual_d] + ", "
	    +  info->qout[actual_e] + ", "
	    +  info->qout[actual_v] + ", "
	    +  info->qout[actual_b] + ", ";
      }
      else {  // formal is STREAM_OUT
	ret += info->qin[actual_d] + ", "
	    +  info->qin[actual_e] + ", "
	    +  info->qin[actual_v] + ", "
	    +  info->qin[actual_b] + ", ";
      }
    }  // for (formals_it, actuals_it ...)

    // - remove final ", "
    ret = ret(0, ret.length()-1-2);

    ret += ");\n";
  }  // forall (call, info->behavCalls)

  return ret;
}
*/


/*  NOT USED
 *
string tdfToVerilog_page_toString (OperatorCompose *op,
				   EVerilogInfoCompose *info)
{
  // - emit Verilog for page (FSM+DP+queues) for compositional operator *op

  string ret;
  string indent;

  // - page module declaration
  ret += indent + "module " + op->getName() + " (";
  ret += tdfToVerilog_page_args_toString(op,info);
  ret += ");\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_page_argTypes_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_page_wires_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_page_assign_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_page_calls_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "\n";

  return ret;
}
*/


////////////////////////////////////////////////////////////////
//  tdfToVerilog_composeTop  (emit page FSM+DP+Q composition, w/only input q)
//  tdfToVerilog_noin        (emit page FSM+DP+Q composition, w/o    input q)
//
//  NEW

string tdfToVerilog_composeTop_args_toString (OperatorCompose *op,
					      EVerilogInfoCompose *info)
{
  // - emit argument list for Verilog top/page module / noin module:
  //     clock, reset, stream data/eos/valid/bp

  string ret;

  ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";

  // - top/page module args:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {	// arg is STREAM_IN
      ret += info->input_data  [arg] + ", ";
      ret += info->input_eos   [arg] + ", ";
      ret += info->input_valid [arg] + ", ";
      ret += info->input_bp    [arg] + ", ";
    }
    else {  // arg is STREAM_OUT
      ret += info->output_data [arg] + ", ";
      ret += info->output_eos  [arg] + ", ";
      ret += info->output_valid[arg] + ", ";
      ret += info->output_bp   [arg] + ", ";
    }    
  }

  // - remove final ", "
  ret = ret(0, ret.length()-1-2);

  return ret;
}


string tdfToVerilog_composeTop_argTypes_toString (OperatorCompose *op,
						  EVerilogInfoCompose *info,
						  string indent)
{
  // - emit argument type declarations (input/output)
  //     for Verilog top/page module / noin module:
  //     clock, reset, stream data/eos/valid/bp

  string ret;

  // - top/page module arg types:  clock, reset
  ret += indent + "input  " + CLOCK_NAME + ";\n";
  ret += indent + "input  " + RESET_NAME + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - top/page module arg types:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    assert(arg->isStream());
    int argWidth;
    if (!isConstWidth(arg->getType(),&argWidth))	// - (sets argWidth)
      fatal(1,"-everilog cannot handle non-constant width " +
	      ( arg->getType()->getWidthExpr() ?
	       (arg->getType()->getWidthExpr()->toString()+" ") : string()) +
	      "of formal argument " + arg->getName() +
	      " of op " + op->getName(),
	    op->getToken());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {	// arg is STREAM_IN
      ret += indent + "input  " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
      					      : string())
      				+ info->input_data  [arg] + ";\n";
      ret += indent + "input  " + info->input_eos   [arg] + ";\n";
      ret += indent + "input  " + info->input_valid [arg] + ";\n";
      ret += indent + "output " + info->input_bp    [arg] + ";\n";
    }
    else {  // arg is STREAM_OUT
      ret += indent + "output " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
      					      : string())
      				+ info->output_data [arg] + ";\n";
      ret += indent + "output " + info->output_eos  [arg] + ";\n";
      ret += indent + "output " + info->output_valid[arg] + ";\n";
      ret += indent + "input  " + info->output_bp   [arg] + ";\n";
    }
  }

  return ret;
}


string tdfToVerilog_composeTop_wires_toString (OperatorCompose *op,
					       EVerilogInfoCompose *info,
					       string indent,
					       bool noin_mode=false)
{
  // - emit wire declarations for Verilog top/page module / noin module
  //     namely queue input/output wires
  // - noin_mode=false for assignments in module "page" (internal input queues)
  //   noin_mode=true  for assignments in module "noin" (external input queues)

  string ret;

  // - top/page module wires:  wires for queued streams
  list<Symbol*> syms = args_with_retsym_first(op);
  Symbol *var;
  forall (var, *op->getVars()->getSymbolOrder()) {
    // - add local streams to syms
    if (!var->isArray())
      syms.append(var);
  }
  Symbol *sym;
  forall (sym, syms) {
    if (sym->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    int symWidth;
    if (!isConstWidth(sym->getType(),&symWidth))	// - (sets symWidth)
      fatal(1,"-everilog cannot handle non-constant width " +
	      ( sym->getType()->getWidthExpr() ?
	       (sym->getType()->getWidthExpr()->toString()+" ") : string()) +
	      "of stream " + sym->getName() +
	      " of op " + op->getName(),
	    op->getToken());
    string sym_d, sym_e, sym_v, sym_b;
    if (sym->isStream()) {
      if (((SymbolStream*)sym)->getDir()==STREAM_IN) {	// sym is STREAM_IN
	sym_d = info->input_data [sym];
	sym_e = info->input_eos  [sym];
	sym_v = info->input_valid[sym];
	sym_b = info->input_bp   [sym];
      }
      else {  // sym is STREAM_OUT
	sym_d = info->output_data [sym];
	sym_e = info->output_eos  [sym];
	sym_v = info->output_valid[sym];
	sym_b = info->output_bp   [sym];
      }
    }
    else {  // sym is local stream
      if (!noin_mode)
	continue;
      if (!info->usedStreams.member(sym))	// - skip stream if not used
	continue;
      sym_d = info->stream_data [sym];
      sym_e = info->stream_valid[sym];
      sym_v = info->stream_eos  [sym];
      sym_b = info->stream_bp   [sym];
    }
  
    ret += indent + "wire   " + (symWidth>1 ? string("[%d:0] ",symWidth-1)
					    : string())
			      + info->qin [sym_d] + ", "
			      + info->qout[sym_d] + ";\n";
    ret += indent + "wire   " + info->qin [sym_e] + ", "
			      + info->qout[sym_e] + ";\n";
    ret += indent + "wire   " + info->qin [sym_v] + ", "
			      + info->qout[sym_v] + ";\n";
    ret += indent + "wire   " + info->qin [sym_b] + ", "
			      + info->qout[sym_b] + ";\n";
  }

  return ret;
}


string tdfToVerilog_composeTop_assign_toString (OperatorCompose *op,
						EVerilogInfoCompose *info,
						string indent,
						bool   noin_mode=false)
{
  // - emit "assign" stmts for Verilog page/top module / noin module
  //     to equate I/O wires with certain queue wires
  // - noin_mode=false for assignments in module "page" (internal input queues)
  //   noin_mode=true  for assignments in module "noin" (external input queues)

  string ret;

  // - top/page module assigns:  equate I/O with wires for queued streams
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {	// arg is STREAM_IN
      string arg_d = info->input_data [arg];
      string arg_e = info->input_eos  [arg];
      string arg_v = info->input_valid[arg];
      string arg_b = info->input_bp   [arg];
      string qwire_d = noin_mode ? info->qout[arg_d] : info->qin[arg_d];
      string qwire_e = noin_mode ? info->qout[arg_e] : info->qin[arg_e];
      string qwire_v = noin_mode ? info->qout[arg_v] : info->qin[arg_v];
      string qwire_b = noin_mode ? info->qout[arg_b] : info->qin[arg_b];
      ret += indent + "assign " + qwire_d + " = " + arg_d   + ";\n";
      ret += indent + "assign " + qwire_e + " = " + arg_e   + ";\n";
      ret += indent + "assign " + qwire_v + " = " + arg_v   + ";\n";
      ret += indent + "assign " + arg_b   + " = " + qwire_b + ";\n";
    }
    else {  // arg is STREAM_OUT
      string arg_d = info->output_data [arg];
      string arg_e = info->output_eos  [arg];
      string arg_v = info->output_valid[arg];
      string arg_b = info->output_bp   [arg];
      string qwire_d = info->qout[arg_d];
      string qwire_e = info->qout[arg_e];
      string qwire_v = info->qout[arg_v];
      string qwire_b = info->qout[arg_b];
      ret += indent + "assign " + arg_d   + " = " + qwire_d + ";\n";
      ret += indent + "assign " + arg_e   + " = " + qwire_e + ";\n";
      ret += indent + "assign " + arg_v   + " = " + qwire_v + ";\n";
      ret += indent + "assign " + qwire_b + " = " + arg_b   + ";\n";
    }
  }

  return ret;
}


string tdfToVerilog_noin_wires_toString (OperatorCompose *op,
					 EVerilogInfoCompose *info,
					 string indent)
{
  return tdfToVerilog_composeTop_wires_toString(op,info,indent,true);
}


string tdfToVerilog_noin_assign_toString (OperatorCompose *op,
					  EVerilogInfoCompose *info,
					  string indent)
{
  return tdfToVerilog_composeTop_assign_toString(op,info,indent,true);
}


string tdfToVerilog_noin_calls_toString (OperatorCompose *op,
					 EVerilogInfoCompose *info,
					 string indent)
{
  // - emit module calls for Verilog noin module:  (page w/o input queues)
  //     queue, called behavioral ops, called segment ops

  string ret;

  // - noin module calls:  queue
  ret += indent + op->getName() + "_q " + op->getName() + "_q_ ("
                + tdfToVerilog_q_args_toString(op,info, false,true,true)
		+ ");\n";

  // 29/1/2013 - Nachiket added code to allow multiple instances of same 
  // function to have unique instances names! Bejesus!
  int unique_instance_id_counter = 0; 
  
  addEmptyLine(&ret);		// ----------------  

  // - noin module calls:  called compositional/behavioral ops +
  //                       called segment ops
  list<ExprCall*> allCalls;	// = composeCalls + behavCalls + segmentCalls
  ExprCall *call;
  forall (call, info->composeCalls)		// - only if op is netlist
    allCalls.append(call);
  forall (call, info->behavCalls)		// - only if op is page
    allCalls.append(call);
  forall (call, info->segmentCalls)
    allCalls.append(call);
  forall (call, allCalls)
  {
    Operator *calledop = call->getOp();
    assert( calledop->getOpKind()==OP_COMPOSE    ||
	    calledop->getOpKind()==OP_BEHAVIORAL ||
	   (calledop->getOpKind()==OP_BUILTIN &&
	    ((OperatorBuiltin*)calledop)->getBuiltinKind()==BUILTIN_SEGMENT));
    // unique instance numbers
    std::ostringstream ss;
    ss << unique_instance_id_counter;

    // assign values to parameters in segments at least!
    if(calledop->getOpKind()==OP_BUILTIN &&
		    ((OperatorBuiltin*)calledop)->getBuiltinKind()==BUILTIN_SEGMENT) {
	    ret += indent + calledop->getName() + " #(";

	    // - module arg types:  stream I/O
	    list<Symbol*> args = args_with_retsym_first(calledop);
	    Symbol *arg;
	    forall (arg, args) {
		    if (arg->isParam()) {
			    // - params should already be bound, ignore
			    // 24/1/2013 - Yes, in an ideal world.. but this is fucked right now! - Nachiket
			    //continue;
			    Expr* e_val = ((SymbolVar*)arg)->getValue();
			    if(e_val!=NULL && e_val->getExprKind()==EXPR_VALUE) {
				    int value = ((ExprValue*)e_val)->getIntVal();
				    ret += "." + ((SymbolVar*)arg)->toString() + " (" + string("%d",value) + "), ";
			    }
		    }
	    }

	    ret = ret(0,ret.length()-1-2);        // - drop last ", "
	    ret += ") ";

	    //ret += " #(.nelems () ,.awidth () ,.dwidth () ) ";
	    ret += " " + calledop->getName() + "_"+ ss.str().c_str() 
		    ret += " (";
    } else {
	    ret += indent + calledop->getName() + " " + calledop->getName() + "_"+ ss.str().c_str() +" (";
    }

    
    unique_instance_id_counter ++;
    ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";

    list<Symbol*> formals = args_with_retsym_first(calledop); // - TDF formals
    list<Expr*>   actuals = *call->getArgs();		      // - TDF actuals
							      //     sans ret

    // - if called op has return stream
    //     * return stream becomes first verilog actual
    //        (other than clock, reset)
    //     * return stream is already in "formals", just add it to "actuals"
    Symbol *retsym = calledop->getRetSym();
    if (retsym && !retsym->equals(sym_none)) {
      Tree *p = call->getParent();
      assert(p->getKind()==TREE_STMT);
      assert(((Stmt*)p)->getStmtKind()==STMT_ASSIGN);
      Expr *actual = ((StmtAssign*)p)->getLValue();
      actuals.push(actual);
    }

    // - emit verilog actuals to mirror TDF actuals
    //     (including return stream, if any, which is added above)
    list_item formals_it, actuals_it;
    for (formals_it=formals.first(), actuals_it=actuals.first();
	 formals_it && actuals_it;
	 formals_it=formals.succ(formals_it),
	 actuals_it=actuals.succ(actuals_it)) {
      Symbol *formal = formals.inf(formals_it);
      Expr   *actual = actuals.inf(actuals_it);
      if (formal->isParam()) {
	// - params should already be bound, ignore  (ignore arrays too)
	// Exception: SEGMENTS
	continue;
      }
      if (actual->getExprKind()!=EXPR_LVALUE)
	fatal(1, "-everilog cannot handle non-stream call argument " +
	         actual->toString(), actual->getToken());
      string actual_d, actual_e, actual_v, actual_b;
      Symbol *actualSym = ((ExprLValue*)actual)->getSymbol();
      if (actualSym->isStream()) {
	if (((SymbolStream*)actualSym)->getDir()==STREAM_IN) {
	  actual_d = info->input_data [actualSym];
	  actual_e = info->input_eos  [actualSym];
	  actual_v = info->input_valid[actualSym];
	  actual_b = info->input_bp   [actualSym];
	}
	else {  // actual is STREAM_OUT
	  actual_d = info->output_data [actualSym];
	  actual_e = info->output_eos  [actualSym];
	  actual_v = info->output_valid[actualSym];
	  actual_b = info->output_bp   [actualSym];
	}
      }
      else {  // actual is local stream
	if (!info->usedStreams.member(actualSym))  // - skip stream if not used
	  continue;
	actual_d = info->stream_data [actualSym];
	actual_e = info->stream_eos  [actualSym];
	actual_v = info->stream_valid[actualSym];
	actual_b = info->stream_bp   [actualSym];
      }
      assert(formal->isStream());
      if (((SymbolStream*)formal)->getDir()==STREAM_IN) {
	ret += info->qout[actual_d] + ", "
	    +  info->qout[actual_e] + ", "
	    +  info->qout[actual_v] + ", "
	    +  info->qout[actual_b] + ", ";
      }
      else {  // formal is STREAM_OUT
	ret += info->qin[actual_d] + ", "
	    +  info->qin[actual_e] + ", "
	    +  info->qin[actual_v] + ", "
	    +  info->qin[actual_b] + ", ";
      }
    }  // for (formals_it, actuals_it ...)

    // - remove final ", "
    ret = ret(0, ret.length()-1-2);

    ret += ");\n";
  }  // forall (call, allCalls)

  return ret;
}


string tdfToVerilog_composeTop_calls_toString (OperatorCompose *op,
					       EVerilogInfoCompose *info,
					       string indent)
{
  // - emit module calls for Verilog top/page module:
  //     input queue, "noin" module (ops)

  string ret;

  // - top/page module calls:  queue
  string inputQueueModule = (info->isPage ? gPageInputQueueModule
			                  : gNetlistInputQueueModule);
  ret += indent + op->getName() + "_qin qin ("
                + tdfToVerilog_q_args_toString(op,info, true,false,false)
		+ ");\n";

  addEmptyLine(&ret);		// ----------------  

  // - page module calls:  noin
  ret += indent + op->getName() + "_noin noin (";
  ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore  (ignore arrays too)
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {	// arg is STREAM_IN
      ret += info->qout[info->input_data  [arg]] + ", ";
      ret += info->qout[info->input_eos   [arg]] + ", ";
      ret += info->qout[info->input_valid [arg]] + ", ";
      ret += info->qout[info->input_bp    [arg]] + ", ";
    }
    else {  // arg is STREAM_OUT
      ret += info->qout[info->output_data [arg]] + ", ";
      ret += info->qout[info->output_eos  [arg]] + ", ";
      ret += info->qout[info->output_valid[arg]] + ", ";
      ret += info->qout[info->output_bp   [arg]] + ", ";
    }    
  }
  ret = ret(0, ret.length()-1-2);	  // - remove final ", "
  ret += ");\n";

  return ret;
}


string tdfToVerilog_noin_toString (OperatorCompose *op,
				   EVerilogInfoCompose *info,
				   string *altModuleName=NULL)
{
  // - emit Verilog for noin module (FSM+DP+queues, without input queues)
  //     for compositional operator *op

  string ret;
  string indent;

  // - noin module declaration
  string moduleName = altModuleName ? (*altModuleName)
				    : (op->getName()+"_noin");
  ret += indent + "module " + moduleName + " (";
  ret += tdfToVerilog_composeTop_args_toString(op,info);
  ret += ");\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_composeTop_argTypes_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_noin_wires_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_noin_assign_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_noin_calls_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + moduleName + "\n";

  return ret;
}


string tdfToVerilog_composeTop_toString (OperatorCompose *op,
					 EVerilogInfoCompose *info)
{
  // - emit Verilog top/page for compositional operator *op

  string ret;
  string indent;

  // - top/page module declaration
  string syn_hier_str = info->isPage
                      ? "/* synthesis syn_hier=\"flatten,firm\" */"
                      : "/* synthesis syn_hier=\"firm\" */" ;
  ret += indent + "module " + op->getName() + " (";
  ret += tdfToVerilog_composeTop_args_toString(op,info);
  ret += ") " + syn_hier_str + " ;\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_composeTop_argTypes_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_composeTop_wires_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_composeTop_assign_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_composeTop_calls_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "\n";

  return ret;
}

////////////////////////////////////////////////////////////////
//  tdfToVerilog_..._toFile  (emit to files)

void tdfToVerilog_base_segment_toFile (const char* fileName,
				     const char* fileContent)
{
  // - emit Verilog base segment implementation "SEG_xxx.v"
  //     by writing *fileContent into file named *fileName*
  //     in current directory

  ofstream fout(fileName);
  if (!fout)
    fatal(1,"-everilog could not open output file "+string(fileName));

  // - comment
  string comment = "// Verilog base segment " + string(fileName) + "\n"
                   "// " + tdfcComment() + "\n";
  fout << comment;

  // - Verilog module
  fout << fileContent;

  fout.close();
}



////////////////////////////////////////////////////////////////
//  tdfToVerilog_..._toFile  (emit to files)

void tdfToVerilog_base_queue_toFile (const char* fileName,
				     const char* fileContent)
{
  // - emit Verilog base queue implementation "Q_xxx.v"
  //     by writing *fileContent into file named *fileName*
  //     in current directory

  ofstream fout(fileName);
  if (!fout)
    fatal(1,"-everilog could not open output file "+string(fileName));

  // - comment
  string comment = "// Verilog base queue " + string(fileName) + "\n"
                   "// " + tdfcComment() + "\n";
  fout << comment;

  // - Verilog module
  fout << fileContent;

  fout.close();
}

void tdfToVerilog_base_segments_toFile ()
{
  // also write segments
  tdfToVerilog_base_segment_toFile("SEG_rw.v",	     	     SEG_rw);
  tdfToVerilog_base_segment_toFile("SEG_rw_dp.v",	     SEG_rw_dp);
  tdfToVerilog_base_segment_toFile("SEG_rw_fsm.v",	     SEG_rw_fsm);

  tdfToVerilog_base_segment_toFile("SEG_r.v",	     	     SEG_r);
  tdfToVerilog_base_segment_toFile("SEG_r_dp.v",	     SEG_r_dp);
  tdfToVerilog_base_segment_toFile("SEG_r_fsm.v",	     SEG_r_fsm);
  
  tdfToVerilog_base_segment_toFile("SEG_seq_r.v",	     	     SEG_seq_r);
  tdfToVerilog_base_segment_toFile("SEG_seq_r_dp.v",	     SEG_seq_r_dp);
  tdfToVerilog_base_segment_toFile("SEG_seq_r_fsm.v",	     SEG_seq_r_fsm);
  
  tdfToVerilog_base_segment_toFile("SEG_seq_cyclic_r.v",	     	     SEG_seq_cyclic_r);
  tdfToVerilog_base_segment_toFile("SEG_seq_cyclic_r_dp.v",	     SEG_seq_cyclic_r_dp);
  tdfToVerilog_base_segment_toFile("SEG_seq_cyclic_r_fsm.v",	     SEG_seq_cyclic_r_fsm);
}

void tdfToVerilog_base_queues_toFile ()
{

  tdfToVerilog_base_queue_toFile("Q_blackbox.v",	     Q_blackbox);
  tdfToVerilog_base_queue_toFile("Q_wire.v",		     Q_wire);
  tdfToVerilog_base_queue_toFile("Q_srl.v",		     Q_srl);		  // Q_srl_oreg3_prefull.v
  tdfToVerilog_base_queue_toFile("Q_fifo_bubl.v",	     Q_fifo_bubl);
  tdfToVerilog_base_queue_toFile("Q_fifo_bubl_noen.v",	     Q_fifo_bubl_noen);
  tdfToVerilog_base_queue_toFile("Q_pipe.v",		     Q_pipe);
  tdfToVerilog_base_queue_toFile("Q_pipe_noretime.v",	     Q_pipe_noretime);
  tdfToVerilog_base_queue_toFile("Q_fwd_pipe.v",	     Q_fwd_pipe);
  tdfToVerilog_base_queue_toFile("Q_fwd_pipe_noretime.v",    Q_fwd_pipe_noretime);
  tdfToVerilog_base_queue_toFile("Q_srl_reserve.v",          Q_srl_reserve);	  // Q_srl_reserve_oreg3_prefull_preresv_SIMPLE.v
  tdfToVerilog_base_queue_toFile("Q_srl_reserve_lwpipe.v",   Q_srl_reserve_lwpipe);
  tdfToVerilog_base_queue_toFile("Q_srl_reserve_nolwpipe.v", Q_srl_reserve_nolwpipe);
  tdfToVerilog_base_queue_toFile("Q_lwpipe.v",               Q_lwpipe);
  tdfToVerilog_base_queue_toFile("Q_lowqli.v",               Q_lowqli);
  tdfToVerilog_base_queue_toFile("Q_lowqli_in.v",            Q_lowqli_in);
  tdfToVerilog_base_queue_toFile("Q_lowqli_out.v",           Q_lowqli_out);
  tdfToVerilog_base_queue_toFile("Q_lowqli_p_p_srl_r.v",     Q_lowqli_p_p_srl_r);
  tdfToVerilog_base_queue_toFile("Q_lowqli_in_p_p_srl_r.v",  Q_lowqli_in_p_p_srl_r);
  tdfToVerilog_base_queue_toFile("Q_lowqli_out_p_p_srl_r.v", Q_lowqli_out_p_p_srl_r);
  tdfToVerilog_base_queue_toFile("Q_lowqli_r_p_srl_r.v",     Q_lowqli_r_p_srl_r);
  tdfToVerilog_base_queue_toFile("Q_lowqli_in_r_p_srl_r.v",  Q_lowqli_in_r_p_srl_r);
  tdfToVerilog_base_queue_toFile("Q_lowqli_out_r_p_srl_r.v", Q_lowqli_out_r_p_srl_r);
}


void tdfToVerilog_q_toFile (OperatorCompose *op, EVerilogInfoCompose *info)
{
  // - emit Verilog non-input queue module for *op to file <opname>_q.v
  //     (queues for local and output streams, NOT input streams)

  string fileName_q        = op->getName() + "_q.v";
  string fileName_local_q  = (info->isPage ? gPageLocalQueueModule
			                   : gNetlistLocalQueueModule)  + ".v";
  string fileName_output_q = (info->isPage ? gPageOutputQueueModule
			                   : gNetlistOutputQueueModule) + ".v";

  ofstream fout(fileName_q);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_q);

  // - comment
  string comment = "// Verilog " + string(info->isPage?"page":"inter-page")
                 +   " non-input queue module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - includes
  fout << "`include \"" + fileName_local_q  + "\"\n";
  fout << "`include \"" + fileName_output_q + "\"\n\n";

  // - Verilog module
  fout << tdfToVerilog_q_toString(op,info);

  fout.close();
}


void tdfToVerilog_qin_toFile (OperatorCompose *op, EVerilogInfoCompose *info)
{
  // - emit Verilog input queue module for *op to file <opname>_q.v

  string fileName_qin     = op->getName() + "_qin.v";
  string fileName_input_q = (info->isPage ? gPageInputQueueModule
			                  : gNetlistInputQueueModule) + ".v";

  ofstream fout(fileName_qin);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_qin);

  // - comment
  string comment = "// Verilog " + string(info->isPage?"page":"inter-page")
                 +   " input queue module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - includes
  fout << "`include \"" + fileName_input_q + "\"\n\n";

  // - Verilog module
  fout << tdfToVerilog_qin_toString(op,info);

  fout.close();
}


void tdfToVerilog_dpq_toFile (OperatorCompose *op, EVerilogInfoCompose *info)
{
  // - emit Verilog page module without input queues or fsms
  //     for *op to file <opname>.v
  // - this module captures datapath + output queue,
  //     specifically for characterizing datapath w/pipelining + retiming,
  //     and is not referenced by any other outer module

  assert(info->isPage);

  string fileName_dpq = op->getName() + "_dpq.v";
  string fileName_q   = op->getName() + "_q.v";

  ofstream fout(fileName_dpq);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_dpq);

  // - comment
  string comment = "// Verilog page module without input queues or fsms for "
		 +     op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - includes:  q
  fout << "`include \"" + fileName_q + "\"\n";

  // - includes:  called ops
  ExprCall *call;
  forall (call, info->behavCalls) {
    OperatorBehavioral *calledop = (OperatorBehavioral*)call->getOp();
    assert(calledop->getOpKind()==OP_BEHAVIORAL);
    string fileName_calledop = calledop->getName() + "_dp.v";
    fout << "`include \"" + fileName_calledop + "\"\n";
  }
  
  // - includes:  segment ops
  forall (call, info->segmentCalls) {
    OperatorSegment *calledop = (OperatorSegment*)call->getOp();
    assert(calledop->getOpKind()==OP_BUILTIN &&
	   ((OperatorBuiltin*)calledop)->getBuiltinKind()==BUILTIN_SEGMENT);
    string fileName_calledop = calledop->getName() + ".v";
    fout << "`include \"" + fileName_calledop + "\"\n";
  }
  fout << "\n";

  // - Verilog modules:  called ops with black-box FSMs
  forall (call, info->behavCalls) {
    OperatorBehavioral *calledop = (OperatorBehavioral*)call->getOp();
    assert(calledop->getOpKind()==OP_BEHAVIORAL);
    // - NOTE: we are scanning each called op an extra time here,
    //         since it is also scanned when emitting its standalone file
    EVerilogInfo* calledinfo = tdfToVerilog_scanTdf_alloc(calledop);
    fout << tdfToVerilog_fsmbb_toString (calledop,calledinfo) << "\n";
    fout << tdfToVerilog_fsm_dp_toString(calledop,calledinfo) << "\n";
    tdfToVerilog_scanTdf_dealloc(calledop,calledinfo);
  }

  // - Verilog module
  string opName_dpq = op->getName() + "_dpq";
  fout << tdfToVerilog_noin_toString(op,info,&opName_dpq);

  fout.close();
}


void tdfToVerilog_noin_toFile (OperatorCompose *op, EVerilogInfoCompose *info)
{
  // - emit Verilog top/page module without input queues
  //     for *op to file <opname>.v

  string fileName_noin = op->getName() + "_noin.v";
  string fileName_q    = op->getName() + "_q.v";

  ofstream fout(fileName_noin);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_noin);

  // - comment
  string comment = "// Verilog " + string(info->isPage?"page":"inter-page")
                 +   " module without input queues for "
		 +     op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - includes:  q
  //fout << "`include \"" + fileName_q + "\"\n";

  // - includes:  called ops
  list<ExprCall*> &calls = info->isPage ? info->behavCalls :info->composeCalls;
  ExprCall *call;
  forall (call, calls) {
    Operator *calledop = call->getOp();
    assert(calledop->getOpKind()==OP_COMPOSE ||
	   calledop->getOpKind()==OP_BEHAVIORAL);
    string fileName_calledop = calledop->getName() + ".v";
    //   fout << "`include \"" + fileName_calledop + "\"\n";
  }
  
  // - includes:  segment ops
  forall (call, info->segmentCalls) {
    OperatorSegment *calledop = (OperatorSegment*)call->getOp();
    assert(calledop->getOpKind()==OP_BUILTIN &&
	   ((OperatorBuiltin*)calledop)->getBuiltinKind()==BUILTIN_SEGMENT);
    string fileName_calledop = calledop->getName() + ".v";
    //fout << "`include \"" + fileName_calledop + "\"\n";
  }
  fout << "\n";

  // - Verilog module
  fout << tdfToVerilog_noin_toString(op,info);

  fout.close();
}


void tdfToVerilog_composeTop_toFile (OperatorCompose *op,
				     EVerilogInfoCompose *info)
{
  // - emit Verilog top/page module for *op to file <opname>.v
  //     containing qin (input queues) and noin (everything else)

  string fileName_page = op->getName() + ".v";
  string fileName_qin  = op->getName() + "_qin.v";
  string fileName_noin = op->getName() + "_noin.v";

  ofstream fout(fileName_page);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_page);

  // - comment
  string comment = "// Verilog " + string(info->isPage?"page":"inter-page")
                 +   " module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - includes
  // Xilinx XST does NOT like stray includes! Jesus!
  //fout << "`include \"" + fileName_qin  + "\"\n";
  //fout << "`include \"" + fileName_noin + "\"\n\n";

  // - Verilog module
  fout << tdfToVerilog_composeTop_toString(op,info);

  fout.close();
}


void tdfToVerilog_compose_toFile (OperatorCompose *op)
{
  // - Emit Verilog for compositional *op to ".v" files
  // - Creates files in current working directory:  <op>.v  <op>_qin.v <op>_q.v
  //                                                <op>_noin.v <op>_dpq.v
  //     and for each called behavioral op X:       <X>.v  <X>_fsm.v  <X>_dp.v
  // - Compositional op may have 2 forms:
  //     (1) a netlist:  composition of only compositions (inter-page)
  //     (2) a page:     composition of only behavioral ops
  //         HACK: for now, either form may also include segment ops

  EVerilogInfoCompose info;
  tdfToVerilog_compose_scanTdf(op,&info);

  tdfToVerilog_base_queues_toFile();		// - base queues "Q_xxx.v"
  tdfToVerilog_base_segments_toFile();		// - base segments "SEG_xxx.v"

  tdfToVerilog_q_toFile          (op,&info);	// - output + local queues
  tdfToVerilog_qin_toFile        (op,&info);	// - input queues

  // Not sure why this isn't working properly - 29/1/2013
  // Nachiket commented this crap out..
  //if (info.isPage)
  //   tdfToVerilog_dpq_toFile        (op,&info);	// - w/o input queues, fsms

  tdfToVerilog_noin_toFile       (op,&info);	// - w/o input queues
  tdfToVerilog_composeTop_toFile (op,&info);	// - top level

  if (!info.isPage) {				// - op is netlist
    ExprCall *call;
    forall (call, info.composeCalls) {
      OperatorCompose *calledop = (OperatorCompose*)call->getOp();
      assert(calledop->getOpKind()==OP_COMPOSE);
      tdfToVerilog_compose_toFile(calledop);
    }
  }

  if (info.isPage) {				// - op is page
    ExprCall *call;
    forall (call, info.behavCalls) {
      OperatorBehavioral *calledop = (OperatorBehavioral*)call->getOp();
      assert(calledop->getOpKind()==OP_BEHAVIORAL);
      tdfToVerilog_toFile(calledop);
    }
  }

  ExprBuiltin *segmentCall;
  forall (segmentCall, info.segmentCalls) {
    OperatorSegment *calledop = (OperatorSegment*)segmentCall->getOp();
    assert(calledop->getOpKind()==OP_BUILTIN &&
	   ((OperatorBuiltin*)calledop)->getBuiltinKind()==BUILTIN_SEGMENT);
	// following tweo lines of code were added to deal with constant folding..
        resolve_bound_values((Operator**)(&calledop));
        set_values(calledop, true);
    	tdfToVerilog_seg_toFile(calledop);
   	//tdfToVerilog_blackbox_toFile(calledop);    
  }
}



////////////////////////////////////////////////////////////////
//  instanceSegmentOps  (make instances of segment ops)

bool instanceSegmentOps_premap (Tree *t, void *i)
{
  switch (t->getKind()) {
    case TREE_OPERATOR:
    case TREE_STMT:
      return true;
    case TREE_EXPR:
      if (((Expr*)t)->getExprKind()==EXPR_BUILTIN &&
	  (((OperatorBuiltin*)((ExprBuiltin*)t)->getOp())->
				getBuiltinKind()==BUILTIN_SEGMENT)) {
	// - found call to segment op -- make instance
	ExprBuiltin *segCall = (ExprBuiltin*)t;
	Operator    *segOp   = segCall->getOp();
        warn("Emitting Verilog for segment " + segOp->getName());
////	Operator *newSegOp   = (Operator*)segOp->duplicate();	// - uniq dup
////	newSegOp->link();
////	segCall->setOp(newSegOp);
////	static int segmentOpNum = 0;				// - uniq name
////	newSegOp->setName(segOp->getName()+string("_%d",segmentOpNum++));
	set_values(segCall,true);				// - bind vals
        resolve_bound_values(&segOp);
////	warn("*** CALL "+t->toString()+" INDUCES "+ newSegOp->toString());
      }
      return false;
    default:
      return false;
  }
}


void instanceSegmentOps (Operator *op)
{
  // - Make instances of called segment ops
  //     (unique duplicate, unique rename, bind param values)
  // - Segment ops are of type OperatorBuiltin and have no behavioral code,
  //     so instancing them is useful only for -everilog w/black box seg ops
//cout << " Wheeeeeeeeeeeeeee" << endl;
  op->map(instanceSegmentOps_premap);
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_compose

void tdfToVerilog_compose (OperatorCompose *iop)
{
  // - emit Verilog for compositional operator *iop
  // - WARNING:  modifies iop by making instances of called segment ops

  extern bool gPagePartitioning, gPagePartitioning1, gPagePartitioningMetis;

  warn("Emitting Verilog for compose " + iop->getName());

  // - make instances of segment ops  (WARNING: modifies iop)
  instanceSegmentOps(iop);

  if (gPagePartitioning || gPagePartitioning1 || gPagePartitioningMetis) {
    // - emit Verilog after page partitioning
    // - top level composition (*iop) calls page compositional ops,
    //     so we emit Verilog for (*iop) AND each called operator
    tdfToVerilog_compose_toFile(iop);
    list<Operator*> calledops = findCalledOps(iop);
    Operator *calledop;
    forall (calledop, calledops) {
      assert(calledop->getOpKind()==OP_COMPOSE);
      // warn("*** EMITTING VERILOG FOR PAGE "+calledop->getName());
      set_values(calledop,true);				// - bind vals
      resolve_bound_values(&calledop);
      tdfToVerilog_compose_toFile((OperatorCompose*)calledop);
    }
  }
  else {
    // - emit Verilog without page partitioning
    // - top level composition (*op) calls behavioral ops,
    //     so we treat it as a single page
    tdfToVerilog_compose_toFile(iop);
  }
}
