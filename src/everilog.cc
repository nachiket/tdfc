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
// SCORE TDF compiler:  Verilog back end for behavioral ops
// $Revision: 1.7 $
//
//////////////////////////////////////////////////////////////////////////////


// To emit Verilog, given a behavioral operator:
//
// (1) Scan source for info
//       - input streams        (to create wires)
//       - output streams       (to create wires)
//       - input history depth  (to create history regs)
//       - register variables   (to create registers)
//       - local variables      (to create wires, uniquely named)
//       - done                 (to know if need done state)
//       - # of state cases     (to know width of state-case selector)
//       - branch decisions     (to know how many flag wires)
//
// (2) Emit Verilog
//       - top module
//           + input, output streams
//           + wires connecting FSM to DP
//           + instantiate FSM, DP
//       - FSM module
//           + header:  all inputs, outputs, wires, regs
//           + always block for state reg asst
//           + always block for FSM combi, case (state), for each state
//               if (fire sig 1)  { flow ctl/consumption, next state }
//               if (fire sig 2)  { flow ctl/consumption, next state }
//               if (default EOS) { flow ctl, close streams, next state done }
//               if (otherwise)   { flow ctl for stall, next state same }
//               --> flow ctl must include safe default
//       - DP module
//           + header:  all inputs, outputs, wires, regs
//           + always block for reg asst
//           + always block for DP combi, case (state), for each state
//               if (statecase 1) { history select, output select, reg select }
//               if (statecase 2) { history select, output select, reg select }
//               if (otherwise)   { history select, output select, reg select }
//               --> history & reg select must include assignment of same
//               --> output select may include don't-care
//
// (3) Not implemented
//       - arithmetic shift  (Synplify does not have <<<, >>>)
//       - default EOS case closes + terminates
//       - prevent multiple stream emission / closing
//       - zero-width I/O streams do not need data line
//       - suppress consumption/production on reset
//       - type width of negative constants may be wrong  (see parse_tdf.y)
//       - apps:  (1<<x) is 0, should be (ONE<<x) with ONE of wide type


#include <stdio.h>
#include <stdlib.h>
#include "expr.h"
#include "expr.h"
#include "state.h"
#include <LEDA/core/d_array.h>
#include <LEDA/core/h_array.h>
#include <fstream>
#include "stmt.h"
#include "misc.h"
#include "ops.h"
#include "version.h"
#include "annotes.h"
#include "bindvalues.h"
#include "everilog.h"
#include "symbol.h"
#include "everilog_symtab.h"
#include <LEDA/core/list.h>
#include <LEDA/core/array.h>
#include <LEDA/core/map.h>

using leda::string;
using leda::map;
using leda::list;
using leda::array;
using leda::d_array;
using leda::h_array;
using leda::dic_item;
using leda::list_item;

using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;

bool gIgnoreEos = false;	// - generate Verilog that ignores EOS
				// - need to turn this into cmd-line option


////////////////////////////////////////////////////////////////
//  Info scanned from TDF Behavioral Operators

class EVerilogInfo {			  // - info for verilog emission
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
                        reg,		  // - reg for TDF register variable
					  //     (does not include state or
					  //      input history registers)
                        local,		  // - reg for TDF local    variable
			rom_addr,	  // - ROM addr in  for TDF array var
			rom_data;	  // - ROM data out for TDF array var
  d_array<string,string>
		        always_assignable;// - always-assignable reg for any
					  //     (map from verilog to verilog)
					  //     (is id for locals, temps, rom)
  h_array<Expr*,string> flag;		  // - branch decision flag I/O
  h_array<Expr*,string>	temp;		  // - additional temporary reg
  d_array<string,bool>	temp_sign;        // - sign of temporary reg
					  //     (not same as sign of expr)
  d_array<string,int>	temp_width;	  // - bit width of temporary reg
					  //     (not same as width of expr)
  h_array<Symbol*,int>      history_depth;    // - history depth for input stream
  h_array<Symbol*,array<string> >
			history_regs;	  // - history regs for input stream,
					  //     indexed by history offset (@x)
  h_array<State*,string>	state;		  // - state parameters
  list<string>		state_ordered;	  // - state parameters in encodd order
					  //     (start ... done)
					  //     (add done iff TDF has done())
  string                state_reg;	  // - state register
  string                state_out;        // - state output from FSM to DP
  bool			needs_done;	  // - true if op needs done state
  list<string>		statecase_ordered;// - state-case parameters
  string	        statecase_out;    // - state case output from FSM to DP
  int		        max_statecases;   // - max #of state cases in any state
  string		did_goto;         // - goto (early exit) indicator temp

//map<Tree*,set<Symbol*> > assigned_syms; // - map stmt list/block to symbols
                                          //     assigned in that block;
                                          //     src tree may be:
                                          //     (1) statecase, (2) stmtblock
                                          //     (3) if children stmts
                                          //     [ expect no defs in calls ]
  Tree*                 assign_block;     // - present block for assigned_syms
  int			temp_count;	  // - present #of temps in op
					  //     (used to name temporaries)

  EVerilogInfo () : history_depth(0), needs_done(false), max_statecases(0) {}
};


string printEVerilogInfo (EVerilogInfo *info)
{
  string ret;

  string s;
  forall_defined (s, info->always_assignable)
    ret += "always assignable:  " + s
        +  " --> "                + info->always_assignable[s] + "\n";

  Symbol *sym;
  forall_defined (sym, info->reg)
    ret += "reg:  " + sym->toString()
        +  " --> "  + info->reg[sym] + "\n";

  forall_defined (sym, info->local)
    ret += "local:  " + sym->toString()
        +  " --> "    + info->local[sym] + "\n";

  Expr *e;
  forall_defined (e, info->flag)
    ret += "flag:  " + e->toString()
        +  " --> "   + info->flag[e] + "\n";

  forall_defined (e, info->temp)
    ret += "temp:  " + e->toString()
        +  " --> "   + info->temp[e] + "\n";

  forall_defined (sym, info->history_depth) {
    int depth = info->history_depth[sym];
    ret += "history depth:  " + sym->getName()
        +  " --> "            + string("%d",depth);
    if (depth>0) {
      ret += "  { ";
      for (int d=0; d<depth; d++)
	ret += info->history_regs[sym][d] + " ";
      ret += "}";
    }
    ret += "\n";
  }

  ret += "state reg:         " + info->state_reg                    + "\n";
  ret += "state out:         " + info->state_out                    + "\n";
  ret += "state done needed: " + string(info->needs_done ? "yes\n" : "no\n");
  State *state;
  forall_defined (state, info->state) {
    string state_par =   info->state[state];
    ret += "state parameter:  " + state->getName()
        +  " --> "              + state_par + "\n";
  }

  ret += "max #state cases:  " + string("%d",info->max_statecases) + "\n";
  ret += "state case out:    " + info->statecase_out + "\n";
  string scase_par;
  forall (scase_par, info->statecase_ordered)
    ret += "state case parameter:  " + scase_par +"\n";

  /*
  Tree *t;
  forall_defined (t, info->assigned_syms) {
    ret += "assigned symbols  { ";
    Symbol *sym;
    forall (sym, info->assigned_syms[t])
      ret += sym->toString() + " ";
    ret += "}  in:  ";
    ret += t->toString();
    if (ret[ret.length()-1]!='\n')
      ret += "\n";
  }
  */

  return ret;
}


////////////////////////////////////////////////////////////////
//  Scan TDF Behavioral

bool symConsumedHere (Symbol *sym, StateCase *scase)
{
  // - return true if *sym is an input symbol
  //     consumed as data (not EOS) in *scase's input signature

  if (sym->isStream()) {
    InputSpec *ispec;
    forall (ispec, *scase->getInputs())
      if (ispec->getStream()==sym && !ispec->isEosCase())
	return true;
  }
  return false;
}


bool isConstWidth (Type *t, int *width,
		   int private_recurse_top)
{
  // - if type *t is boolean or integer type with constant width,
  //     then return true and set *width
  // - for integer type, width may be from t->width or t->widthExpr
  // - private_recurse_top is used privately, for recursion

  if (t->getTypeKind()==TYPE_BOOL) {
    *width = 1;
    return true;
  }
  // added by Nachiket 16/8/2011.. testing floating-point processing...
  else if (t->getTypeKind()==TYPE_DOUBLE) {
    *width=64;
    return true;
  }
  else if (t->getTypeKind()==TYPE_INT) {
    if (t->getWidth()>=0) {
      *width = t->getWidth();
      return true;
    }
    else if (t->getWidthExpr()->getExprKind()==EXPR_VALUE) {
      *width = ((ExprValue*)t->getWidthExpr())->getIntVal();
      return true;
    }
    else {
      // - try to constant-fold the type
      //     * Andre's bindvalues() does not completely const-fold types
      //     * my version:  evalType() in blockdfc.cc
      if (private_recurse_top) {
	Type *evalType (Type* t);  // - dups + const folds type; in blockdfg.cc
	Type *tt = evalType(t);
	    //cout << "Offending type=" << typekindToString(tt->getTypeKind()) << " width=" << *width << endl;
	bool ret = isConstWidth(tt,width,false);  // - (sets width)
	if (!ret) {
	  // - fatal diagnostic here is more meaningful than assert elsewhere
	  Tree *p=t->getParent();
	  fatal(1, "-everilog: we think: cannot handle non-constant width in type " +
		   tt->toString() + (p ? (" of "+p->toString()) : string()),
		p ? p->getToken() : t->getToken());
	}
	delete tt;
	return ret;
      }
      else
	// - already constant-folded and recursed once, give up
	return false;
    }
  }
  else {
    // - t is neither bool nor int
    return false;
  }

  return false;		// - dummy
}


bool isConstNelems (TypeArray *t, int *nelems,
		    int private_recurse_top=true)
{
  // - if type *t is array type with constant number of elements,
  //     then return true and set *nelems
  // - nelems may be from t->nelems or t->nelemsExpr
  // - private_recurse_top is used privately, for recursion

  if (t->getNelems()>=0) {
    *nelems = t->getNelems();
    return true;
  }
  else if (t->getNelemsExpr()->getExprKind()==EXPR_VALUE) {
    *nelems = ((ExprValue*)t->getNelemsExpr())->getIntVal();
    return true;
  }
  else {
    // - try to constant-fold the type
    //     * Andre's bindvalues() does not completely const-fold types
    //     * my version:  evalType() in blockdfc.cc
    if (private_recurse_top) {
      Type *evalType (Type* t);  // - dups + const folds type; in blockdfg.cc
      Type *tt = evalType(t);
      assert(tt->getTypeKind()==TYPE_ARRAY);
      TypeArray *tta = (TypeArray*)tt;
      bool ret = isConstNelems(tta,nelems,false);  // - (sets nelems)
      if (!ret) {
	// - fatal diagnostic here is more meaningful than assert elsewhere
	Tree *p=t->getParent();
	fatal(1, "-everilog: cannot handle non-constant number of elements "
		 "in type "+
		 tt->toString() + (p ? (" of "+p->toString()) : string()),
	      p ? p->getToken() : t->getToken());
      }
      delete tt;
      return ret;
    }
    else
      // - already constant-folded and recursed once, give up
      return false;
  }

  return false;		// - dummy
}


void createTemporary (Expr *e, EVerilogInfo *info, bool sign, int width)
{
  // - create a Verilog temporary "tmp..." for an expression *e,
  //     using sign "sign" and bit width "width" (may be different than *e)
  // - fill *info accordingly  (info->temp, info->temp_sign, info->temp_width)

  string tmp = string("tmp%d_", info->temp_count++);
         tmp = info->vsymtab.insertRename(tmp,e);
  info->temp[e] = tmp;
  info->always_assignable[tmp] = tmp;  // - always-assignable for tmp is itself
  info->temp_sign [tmp] = sign;
  info->temp_width[tmp] = width;
}


bool requiresTemporary (Expr *e, EVerilogInfo *info,
			bool *sign=NULL, int *width=NULL)
{
  // - return true iff expr *e should be assigned to a Verilog temporary reg
  //     to properly handle bit width / sign extension
  // - if returning true, and width/sign are provided (not NULL),
  //     then set *width and *sign to the requisite width and sign of the temp
  //     (may be different than the width and sign of *e)
  // - needs temp:         <<, >>, operand of cast
  // - needs signed temp:  unsigned operand of signed binary op or unary minus
  //                       operand of cast to signed

  // - check for << or >>
  if (e->getExprKind()==EXPR_BOP) {
    int bop=((ExprBop*)e)->getOp();
    if (bop==RIGHT_SHIFT || bop==LEFT_SHIFT) {
      // - e is << or >>
      if (sign)
	*sign = e->getType()->isSigned();
      if (width)
	if (!isConstWidth(e->getType(),width))		// - (sets *width)
	  assert(!"-everilog non-const width");
      return true;
    }
  }

  Tree *p = e->getParent();

  // - check for unsigned operand of signed binary op
  if (p && p->getKind()==TREE_EXPR
        && ((Expr*)p)->getExprKind()==EXPR_BOP) {
    int bop=((ExprBop*)p)->getOp();
    switch (bop)
    {
      case '+':
      case '-':
      case '*':
      case '/':
      case '%':
      case '<':
      case '>':
      case LTE:
      case GTE:
      case EQUALS:
      case NOT_EQUALS: {
	Expr *e1=((ExprBop*)p)->getExpr1(), *e2=((ExprBop*)p)->getExpr2();
	if (e1->getType()->isSigned() != e2->getType()->isSigned()) {
	  if (!e->getType()->isSigned()) {
	    // - e is unsigned operand of a signed binary op
	    // - use signed temporary to do sign upgrade; widen *e by 1 bit
	    if (sign)
	       *sign=true;
	    if (width) {
	      if (!isConstWidth(e->getType(),width))	// - (sets *width)
		assert(!"-everilog non-const width");
	      (*width)++;
	    }
	    return true;
	  }
	}
	break;
      }
    }
  }

  // - check for unsigned operand of unary minus
  if (p && p->getKind()==TREE_EXPR
        && ((Expr*)p)->getExprKind()==EXPR_UOP
        && ((ExprUop*)p)->getOp()=='-') {
    if (!e->getType()->isSigned()) {
      // - e is unsigned operand of unary minus
      // - use signed temporary to do sign upgrade; widen *e by 1 bit
      if (sign)
	*sign=true;
      if (width) {
	if (!isConstWidth(e->getType(),width))		// - (sets *width)
	  assert(!"-everilog non-const width");
	(*width)++;
      }
      return true;
    }
  }

  // - check for operand of cast
  if (p && p->getKind()==TREE_EXPR
        && ((Expr*)p)->getExprKind()==EXPR_CAST) {
    if (sign)
      *sign = p->getType()->isSigned();
    if (width) {
      if (!isConstWidth(e->getType(),width))		// - (sets *width)
	assert(!"-everilog non-const width");
      if (!e->getType()->isSigned() && p->getType()->isSigned())
	// - use signed temporary to do sign upgrade; widen *e by 1 bit
	(*width)++;
    }
    return true;
  }

  return false;
}


/*
// - superceded by above version with sign, width
//
bool requiresTemporary (Expr *e, EVerilogInfo *info)
{
  // - return true iff expr *e should be assigned to a Verilog temporary reg
  //     to properly handle bit width / sign extension
  // - namely:  <<, >>, operand of cast

  if (requiresSignedTemporary(e,info))
    return true;

  if (e->getExprKind()==EXPR_BOP) {
    int bop=((ExprBop*)e)->getOp();
    if (bop==RIGHT_SHIFT || bop==LEFT_SHIFT)
      // - e is << or >>
      return true;
  }

  Tree *p = e->getParent();
  if ( p && p->getKind()==TREE_EXPR
         && ((Expr*)p)->getExprKind()==EXPR_CAST
         && ((ExprCast*)p)->getExpr()==e )
    // - e is operand of cast
    return true;

  return false;
}
*/


list<Symbol*> args_with_retsym_first (Operator *op)
{
  // - return list of op's arguments, with return symbol (if any) first
  list<Symbol*> args = *op->getArgs();
  Symbol *retsym = op->getRetSym();
  if (retsym && !retsym->equals(sym_none))
    args.push(op->getRetSym());
  return args;
}


bool tdfToVerilog_scanTdf_map (Tree *t, void *i)
{
  // - Scan TDF behavioral operator in preparation for Verilog emission
  // - Build up and return results in (EVerilogInfo*) i,
  //     including selecting verilog names and building verilog symbol table,
  //     NOT including info->assigned_syms (see tdfToVerilog_scanAssigns...)
  // - If called on a non-behavioral op (e.g. segment op),
  //     then build up and return results on formal arguments only

  EVerilogInfo *info = (EVerilogInfo*)i;

  static State     *state = NULL;     // - state we are mapping under
  static StateCase *scase = NULL;     // - state we are mapping under
  static int        stateFlagNum = 0; // - #of ifs (->flags) seen in this state

  switch (t->getKind())
  {
    case TREE_OPERATOR:	{
			  //assert(((Operator*)t)->getOpKind()==OP_BEHAVIORAL);
			  // - the immediately following code to get arg info
			  //     works on any op kind, not just behavioral
			  // - do the check for behav op afterwards, below
			  OperatorBehavioral *op=(OperatorBehavioral*)t;

			  // - create verilog inputs + outputs for streams
			  // - return stream, if any, is first verilog I/O
			  list<Symbol*> args = args_with_retsym_first(op);
			  Symbol *arg;
			  forall (arg, args) {
			    if (arg->isParam()) {
			      // - argument is param
			      if ( arg->isArray() &&
				  (arg->getAnnote(ANNOTE_IS_READ) ||
				   arg->getAnnote(ANNOTE_IS_WRITTEN)) ) {
				// - handle array param only if it is accessed
				if (!arg->getAnnote(ANNOTE_IS_WRITTEN)) {
				  // - ROM param array
				  string arg_a_= arg->getName()+"_a_";  // addr
				  string arg_d_= arg->getName()+"_d_";  // data
				  arg_a_ = info->vsymtab.insertRename(arg_a_,arg);
				  arg_d_ = info->vsymtab.insertRename(arg_d_,arg);
				  info->rom_addr[arg] = arg_a_;
				  info->rom_data[arg] = arg_d_;
				  info->always_assignable[arg_a_] = arg_a_;
				  info->always_assignable[arg_d_] = arg_d_;
				}
				else {
				  // - RAM param array
				  fatal(1,"-everilog cannot handle inline "
					  "writeable array "+arg->getName()+
					  ", try -xc",       arg->getToken());
				}
			      }
			      else {
				// - scalar params already bound, ignore
				// - will catch actual use in ExprLValue below
				continue;
			      }
			    }
			    else {
			      // - argument is stream
			      assert(arg->getSymKind()==SYMBOL_STREAM);
			      string arg_d  = arg->getName()+"_d";  // - I/O
			      string arg_e  = arg->getName()+"_e";
			      string arg_v  = arg->getName()+"_v";
			      string arg_b  = arg->getName()+"_b";
			      arg_d  = info->vsymtab.insertRename(arg_d,arg);
			      arg_e  = info->vsymtab.insertRename(arg_e,arg);
			      arg_v  = info->vsymtab.insertRename(arg_v,arg);
			      arg_b  = info->vsymtab.insertRename(arg_b,arg);
			      string arg_d_ = arg_d+"_";	// - always
			      string arg_e_ = arg_e+"_";	//   assignable
			      string arg_v_ = arg_v+"_";
			      string arg_b_ = arg_b+"_";
			      arg_d_ = info->vsymtab.insertRename(arg_d_,arg);
			      arg_e_ = info->vsymtab.insertRename(arg_e_,arg);
			      arg_v_ = info->vsymtab.insertRename(arg_v_,arg);
			      arg_b_ = info->vsymtab.insertRename(arg_b_,arg);
			      if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
				info->input_data [arg] = arg_d;
				info->input_eos  [arg] = arg_e;
				info->input_valid[arg] = arg_v;
				info->input_bp   [arg] = arg_b;
			      }
			      else {	// i.e. arg->getDir()==STREAM_OUT
				info->output_data [arg] = arg_d;
				info->output_eos  [arg] = arg_e;
				info->output_valid[arg] = arg_v;
				info->output_bp   [arg] = arg_b;
			      }
			      info->always_assignable[arg_d] = arg_d_;
			      info->always_assignable[arg_e] = arg_e_;
			      info->always_assignable[arg_v] = arg_v_;
			      info->always_assignable[arg_b] = arg_b_;
			    }
			  }

			  // - for non-behavioral ops, we only need arg info
			  // - the rest of this scan is for behavioral ops only
			  if (op->getOpKind()!=OP_BEHAVIORAL)
			    return false;

			  // - create verilog regs for register vars
			  list<Symbol*> *regs =op->getVars()->getSymbolOrder();
			  Symbol *reg;
			  forall (reg, *regs) {
			    assert(reg->isReg());
			    if (reg->isArray()) {
			      // - variable is array
			      if (!reg->getAnnote(ANNOTE_IS_WRITTEN)) {
				// - ROM array var
				string reg_a_= reg->getName()+"_a_";  // addr
				string reg_d_= reg->getName()+"_d_";  // data
				reg_a_ = info->vsymtab.insertRename(reg_a_,reg);
				reg_d_ = info->vsymtab.insertRename(reg_d_,reg);
				info->rom_addr[reg] = reg_a_;
				info->rom_data[reg] = reg_d_;
				info->always_assignable[reg_a_] = reg_a_;
				info->always_assignable[reg_d_] = reg_d_;
			      }
			      else {
				// - RAM array var
				fatal(1,"-everilog cannot handle inline "
					"writeable array "+reg->getName()+
					", try -xc",       reg->getToken());
			      }
			    }
			    else {
			      // - variable is register
			      string reg_reg = reg->getName();  // - reg
			      reg_reg = info->vsymtab.insertRename(reg_reg,reg);
			      string reg_ass = reg_reg+"_";	// - always
								//   assignable
			      reg_ass = info->vsymtab.insertRename(reg_ass,reg);
			      info->reg              [reg]     = reg_reg;
			      info->always_assignable[reg_reg] = reg_ass;
			    }
			  }

			  // - initialize max_statecases count
			  info->max_statecases = 0;

			  // - reset counter for naming temporaries
			  info->temp_count = 0;

			  return true;
			}  // case TREE_OPERATOR:

    case TREE_STATE:	{
			  State *s = (State*)t;
			  state        = s;
			  stateFlagNum = 0;

			  // - record greatest #state cases seen
			  int numCases = state->getCases()->size();
			  if (info->max_statecases < numCases)
			      info->max_statecases = numCases;

			  // - create state parameter
			  string state_par = "state_" + s->getName();
			  state_par = info->vsymtab.insertRename(state_par,s);
			  info->state[s] = state_par;

			  return true;
			}

    case TREE_STATECASE:{
			  scase        = (StateCase*)t;
			  return true;
			}

    case TREE_STMT:	{
			  switch (((Stmt*)t)->getStmtKind())
			  {
			    case STMT_BLOCK: {
			      // - create verilog reg for local vars
			      list<Symbol*> *locals =
				((StmtBlock*)t)->getSymtab()->getSymbolOrder();
			      Symbol *local;
			      forall (local, *locals) {
				if (local->isArray())
				  fatal(1,"-everilog cannot handle array access "
					  + local->toString() + ", try -xc",
					local->getToken());
			//	string local_reg = local->getName();	 // reg
			//	local_reg = info->vsymtab.
			//			insertRename(local_reg,local);
				string local_ass = local->getName()+"_"; // alw
				local_ass = info->vsymtab.
						insertRename(local_ass,local);
			//	info->local            [local]     = local_reg;
			//	info->always_assignable[local_reg] = local_ass;
				// - always assignable for local_ass is itself
				info->local            [local]     = local_ass;
				info->always_assignable[local_ass] = local_ass;
			      }
			      return true;
			    }
			    case STMT_IF: {
			      // - create verilog flag for if condition
			      Expr *cond=((StmtIf*)t)->getCond();
			      string flag_o   = "flag_" + state->getName() +
					          string("_%d",stateFlagNum);
			      flag_o   = info->vsymtab.insertRename(flag_o,
								    cond);
			      string flag_ass = flag_o + "_";
			      flag_ass = info->vsymtab.insertRename(flag_ass,
								    cond);
			      info->flag[cond]                = flag_o;
			      info->always_assignable[flag_o] = flag_ass;
			      stateFlagNum++;
			      return true;
			    }
			    case STMT_CALL: {
			      fatal(1,"-everilog cannot handle inlined call "+
				      t->toString() + ", try -xc",
				    t->getToken());
			      return false;	// - dummy
			    }
			    default: {
			      return true;
			    }
			  }
			}

    case TREE_EXPR:	{
			  // - create Verilog temporary, if necessary
			  bool tsign;
			  int  twidth;
			  if (requiresTemporary((Expr*)t,info,&tsign,&twidth))
			        createTemporary((Expr*)t,info, tsign, twidth);

			  switch (((Expr*)t)->getExprKind())
			  {
			    case EXPR_LVALUE: {
			      // - get input history depths
			      //     from r-value references to input streams
			      ExprLValue *lval=(ExprLValue*)t;
			      Symbol *sym=lval->getSymbol();
			      if (sym->isStream() &&
				  ((SymbolStream*)sym)->getDir()==STREAM_IN)
			      {
				// - found reference to input stream
				bool consumed = symConsumedHere(sym,scase);
				int historyDepth;
				Expr *retime=lval->getRetime();
				if (retime==NULL) {
				  historyDepth = (consumed ? 0 : 1);
				}
				else if (retime->getExprKind()==EXPR_VALUE
					 && retime->getType()
						  ->getTypeKind()==TYPE_INT) {
				  historyDepth = ((ExprValue*)retime)
								->getIntVal()
						 + (consumed ? 0 : 1);
				}
				else {
				  fatal (1,"cannot determine input history "
					   "depth because retiming expr of "+
					   retime->toString()+
					   " is not constant or not int",
					 retime->getToken());
				}
				// cerr << sym->getName() << " is"
				//      << (consumed?"":" not")
				//      << " consumed in " << state->getName()
				//      << " with history depth "
				//      << historyDepth << "\n";
				int &maxDepth = info->history_depth[sym];
				if  (maxDepth < historyDepth)
				     maxDepth = historyDepth;
			      }
//			      else if (sym->isArray()) {
//				fatal(1,"-everilog cannot handle array access "
//				        + t->toString() + ", try -xc",
//				      t->getToken());
//			      }
			      return true;
			    }
			    case EXPR_BOP: {
			      int bop = ((ExprBop*)t)->getOp();
			      if (bop=='/' || bop=='%') {
				// - '/' and '%' must divide by constant
				//     power of two, limitation of Synplify
				Expr  *eval = ((ExprBop*)t)->getExpr2();
				bool  isval = eval->getExprKind()==EXPR_VALUE;
				int     val = isval ?
					      ((ExprValue*)t)->getIntVal() : 0;
				if ( !isval ||
				     (val>0 && val!=(1<<(countBits(val)-1))) )
				  warn (  "-everilog cannot handle divisor "+
					  t->toString() + " that is "+
					  "not a constant power of two",
					  t->getToken());
			      }
			      return true;
			    }
			    case EXPR_CALL: {
			      fatal(1,"-everilog cannot handle inlined call "+
				      t->toString() + ", try -xc",
				    t->getToken());
			      return false;	// - dummy
			    }
			    case EXPR_BUILTIN: {
			      OperatorBuiltin *builtin =
				(OperatorBuiltin*)((ExprBuiltin*)t)->getOp();
			      if (builtin->getBuiltinKind()==BUILTIN_DONE) {
				info->needs_done = true;
				return false;
			      }
			      return true;
			    }
			    default: {
			      return true;
			    }
			  }
			}

    case TREE_TYPE:	{
			  return false;
			}

    default:		{
			  return true;
			}
  }
}


/*
// - NOT USED
bool tdfToVerilog_scanAssigns_preMap  (Tree *t, void *i);
bool tdfToVerilog_scanAssigns_postMap (Tree *t, void *i);
*/


/*
// - NOT USED
bool tdfToVerilog_scanAssigns_preMap (Tree *t, void *i)
{
  // - Scan TDF behavioral operator in preparation for Verilog emission
  // - build up and return results in (EVerilogInfo*) i
  //     for "assigned_syms" only

  EVerilogInfo *info = (EVerilogInfo*)i;

  switch (t->getKind())
  {
    case TREE_OPERATOR:
    case TREE_STATE:	{
			  return true;
			}

    case TREE_STATECASE:{
			  info->assign_block=t;
			  return true;
			}

    case TREE_STMT:	{
			  switch (((Stmt*)t)->getStmtKind())
			  {
			    case STMT_BLOCK: {
			      info->assign_block=t;
			      return true;
			    }
			    case STMT_IF: {
			      // - manually descend cond, left, right
			      StmtIf *s=((StmtIf*)t);
			      info->assign_block=s;
			      if (s->getCond())
				s->getCond()->map
					(tdfToVerilog_scanAssigns_preMap,
					 tdfToVerilog_scanAssigns_postMap,i);
			      info->assign_block=s->getThenPart();
			      if (s->getThenPart())
				s->getThenPart()->map
					(tdfToVerilog_scanAssigns_preMap,
					 tdfToVerilog_scanAssigns_postMap,i);
			      info->assign_block=s->getElsePart();
			      if (s->getElsePart())
				s->getElsePart()->map
					(tdfToVerilog_scanAssigns_preMap,
					 tdfToVerilog_scanAssigns_postMap,i);
			      info->assign_block=s;
			      return false;
			    }
			    case STMT_ASSIGN: {
			      Symbol *sym=((StmtAssign*)t)->getLValue()
							  ->getSymbol();
			      info->assigned_syms[info->assign_block].
								insert(sym);
			      return false;
			    }
			    case STMT_CALL: {
			      fatal(1,"-everiolog cannot handle inline call "
				      + t->toString() + ", try -xc",
				    t->getToken());
			      return false;	// - dummy
			    }
			    default: {
			      return true;
			    }
			  }
			}

      default:		{
			  return false;
			}
  }
}
*/


/*
// - NOT USED
bool tdfToVerilog_scanAssigns_postMap (Tree *t, void *i)
{
  // - Scan TDF behavioral operator in preparation for Verilog emission
  // - build up and return results in (EVerilogInfo*) i
  //     for "assigned_syms" only
  // - post-map for a parent block does:
  //     (1) add child block's assignment set into its parent's set
  //     (2) lift info->assign_block to point to parent

  EVerilogInfo *info = (EVerilogInfo*)i;

  Tree *p=t->getParent();
  if ( t->getKind()==TREE_STMT &&
      (p->getKind()==TREE_STMT || p->getKind()==TREE_STATECASE)) {

    // - add child block's assignment set into parent's assignment set
    info->assigned_syms[p] += info->assigned_syms[info->assign_block];

    // - reset present assign_block to point to parent
    info->assign_block=p;
  }
  return true;
}
*/


void tdfToVerilog_scanTdf (OperatorBehavioral *op, EVerilogInfo *info)
{
  // - Scan TDF behavioral operator in preparation for Verilog emission
  // - build up and return results in *info,
  //     including selecting verilog names and building verilog symbol table

  // - scan TDF code
  op->map(tdfToVerilog_scanTdf_map, (TreeMap)NULL, (void*)info);

  // - scan TDF code for assignments -- NOT USED
  // op->map(tdfToVerilog_scanAssigns_preMap,
  //         tdfToVerilog_scanAssigns_postMap, (void*)info);

  // - create input history registers
  Symbol *sym;
  forall_defined (sym, info->history_depth) {
    int depth = info->history_depth[sym];
    if (depth>0) {
      // - input "*sym" needs "depth" history regs
      array<string> &history_regs = info->history_regs[sym];
      history_regs = array<string>(depth);	// - initialize to size "depth"
      for (int d=0; d<depth; d++) {
	string history_reg = sym->getName() + string("_at_%d", d);
	       history_reg = info->vsymtab.insertRename(history_reg,NULL);
	string history_ass = history_reg+"_";
	       history_ass = info->vsymtab.insertRename(history_ass,NULL);
	history_regs[d] = history_reg;
	info->always_assignable[history_reg] = history_ass;
      }
    }
    // - if (depth==0) then info->history_depth[sym] is not defined,
    //          even though info->history_depth[sym] is defined as 0
  }

  // - create state reg
  string state_reg = "state_reg";			// - register
         state_reg = info->vsymtab.insertRename(state_reg,NULL);
  string state_ass = state_reg+"_";			// - always assignable
         state_ass = info->vsymtab.insertRename(state_ass,NULL);
  info->state_reg = state_reg;
  info->always_assignable[state_reg] = state_ass;

  // - create state output
  string state_out = "state";
  state_out = info->vsymtab.insertRename(state_out,NULL);
  info->state_out = state_out;
  // - do not need always assignable, have one from state_reg

  // - create state case output
  string statecase_out = "statecase";			// - output
         statecase_out = info->vsymtab.insertRename(statecase_out,NULL);
  string statecase_ass = statecase_out+"_";		// - always assignable
         statecase_ass = info->vsymtab.insertRename(statecase_ass,NULL);
  info->statecase_out = statecase_out;
  info->always_assignable[statecase_out] = statecase_ass;

  // - state parameters (except done state) were added in tdfToString_scanTdf

  // - create order of state parameters
  dictionary<string,State*> *dic = op->getStates();
  State *startState = op->getStartState();
  info->state_ordered.clear();
  info->state_ordered.append(info->state[startState]);
  dic_item it;
  forall_items (it, *dic) {
    State *s = dic->inf(it);
    if (s!=startState)
      info->state_ordered.append(info->state[s]);
  }

  // - create done state parameter, if necessary,
  //     and add to end of order of state parameters
  if (info->needs_done) {
    string state_par = "state_done";
           state_par = info->vsymtab.insertRename(state_par,NULL);
    // info->state[s] = state_par;	// - no associated TDF state for done
    info->state_ordered.append(state_par);
  }

  // - create state-case parameters
  info->statecase_ordered.clear();
  string par = "statecase_stall";			// - case 0 is stall
         par = info->vsymtab.insertRename(par,NULL);
  info->statecase_ordered.append(par);
  for (int i=1; i<=info->max_statecases; i++) {		// - cases 1+
    string scase_par = string("statecase_%d",i);
           scase_par = info->vsymtab.insertRename(scase_par,NULL);
    info->statecase_ordered.append(scase_par);
  }

  // - create goto (early exit) indicator
  string did_goto = "did_goto_";
         did_goto = info->vsymtab.insertRename(did_goto,NULL);
  info->did_goto = did_goto;
  info->always_assignable[did_goto] = did_goto;	// - always-ass is self
}


EVerilogInfo* tdfToVerilog_scanTdf_alloc (OperatorBehavioral *op)
{
  // - Data-abstract version of tdfToVerilog_scanTdf() [see above] + ALLOC
  // - Allocates and returns EVerilogInfo, which caller must deallocate later

  EVerilogInfo *info = new EVerilogInfo;
  tdfToVerilog_scanTdf(op,info);
  return info;
}


void tdfToVerilog_scanTdf_dealloc (OperatorBehavioral *op, EVerilogInfo *info)
{
  // - Data-abstract version of tdfToVerilog_scanTdf() [see above] + DEALLOC
  // - Deallocates EVerilogInfo created by tdfToVerilog_scanTdf_alloc()

  delete info;
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_blackbox  (emit black box)

string tdfToVerilog_fsm_dp_args_toString  (OperatorBehavioral *op,
					   EVerilogInfo *info);

string tdfToVerilog_fsm_dp_argTypes_toString  (OperatorBehavioral *op,
					       EVerilogInfo *info,
					       string indent);


string tdfToVerilog_blackbox_toString (Operator *op,
				       EVerilogInfo *info)
{
  // - emit Verilog black box module for *op
  // - black box Verilog specifies inputs and outputs but no internal code

  string ret;
  string indent;

  // - black box module declaration
  ret += indent + "module " + op->getName() + " ("
      +  CLOCK_NAME + ", " + RESET_NAME + ", ";

  // - black box module declaration:  stream I/O
  // - HACK:  cast op into behavioral op
  ret += tdfToVerilog_fsm_dp_args_toString((OperatorBehavioral*)op,info);

  // - black box module declaration:  finish
  ret    += ")  /* synthesis syn_black_box */ ;\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  // - black box module arg types:  clock, reset, stream I/O
  // - HACK:  cast op into behavioral op
  ret += tdfToVerilog_fsm_dp_argTypes_toString((OperatorBehavioral*)op,info,
					       indent);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "\n";

  return ret;
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_fsm_dp  (emit FSM+DP composition)

void addEmptyLine (string *s)
{
  // - add empty line to s, unless s is "" or already ends in an empty line
  if (s->length()>0 && s->tail(2)!="\n\n")
    *s += "\n";
}

string tdfToVerilog_fsm_dp_params_toString  (OperatorBehavioral *op,
					   EVerilogInfo *info)
{
  // - emit parameter list for Verilog behavioral op module:

  string ret;

  ret = "#(";

  // - module declaration:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      Expr* e_val = ((SymbolVar*)arg)->getValue();
      if(e_val!=NULL && e_val->getExprKind()==EXPR_VALUE) {
      	      int value = ((ExprValue*)e_val)->getIntVal();
	      ret += "." + ((SymbolVar*)arg)->toString() + " (" + string("%d",value) + ") ,";
      }
      continue;
    }
  }
  
  ret = ret(0,ret.length()-1-2);	// - drop last ", "
  ret += ") ";
  return ret;
}

string tdfToVerilog_fsm_dp_args_toString  (OperatorBehavioral *op,
					   EVerilogInfo *info)
{
  // - emit argument list for Verilog behavioral op module:
  //     clock, reset, stream data/eos/valid/bp    (no parentheses)

  string ret;

  // - module declaration:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
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
  ret = ret(0,ret.length()-1-2);	// - drop last ", "

  return ret;
}


string tdfToVerilog_fsm_dp_argTypes_toString  (OperatorBehavioral *op,
					       EVerilogInfo *info,
					       string indent)
{
  // - emit argument type declarations (input/output) for Verilog behav module:
  //     clock, reset, stream data/eos/valid/bp

  string ret;

  // - module arg types:  clock, reset
  ret += indent + "input  " + CLOCK_NAME + ";\n";
  ret += indent + "input  " + RESET_NAME + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - module arg types:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
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
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
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


string tdfToVerilog_fsm_dp_wires_toString  (OperatorBehavioral *op,
					    EVerilogInfo *info,
					    string indent)
{
  // - emit wire declarations for Verilog behavioral op module:
  //     state, state case, flags

  string ret;

  // - wires:  state
  int numStates = info->state_ordered.size();
  if (numStates>1) {
    int stateWidth = countBits(numStates - 1);
    ret += indent + "wire "
	+  (stateWidth>1 ? string("[%d:0] ",stateWidth-1) : string())
	+  info->state_out + ";\n";
  }

  // - wires:  state case
  int numCases = info->max_statecases + 1;	// - case 0 is stall
  if (numCases>1) {
    int caseWidth = countBits(numCases - 1);
    ret += indent + "wire "
	+  (caseWidth>1 ? string("[%d:0] ",caseWidth-1) : string())
        +  info->statecase_out + ";\n";
  }

  // - wires:  flags
  if (!info->flag.empty()) {
    Expr *e;
    forall_defined (e, info->flag)
      ret += indent + "wire " + info->flag[e] + ";\n";
  }

  return ret;
}


string tdfToVerilog_fsm_args_toString (OperatorBehavioral *op,
				       EVerilogInfo *info);

string tdfToVerilog_dp_args_toString  (OperatorBehavioral *op,
				       EVerilogInfo *info);


string tdfToVerilog_fsm_dp_toString (OperatorBehavioral *op,
				     EVerilogInfo *info)
{
  // - emit Verilog for FSM + DP composition for behavioral operator *op

  string ret;
  string indent;

  // - module declaration
  ret += indent + "module " + op->getName() + " ("
      +  CLOCK_NAME + ", " + RESET_NAME + ", ";

  // - module declaration:  stream I/O
  ret += tdfToVerilog_fsm_dp_args_toString(op,info);

  // - module declaration:  finish
  ret    += ");\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  // - module arg types:  clock, reset, stream I/O
  ret += tdfToVerilog_fsm_dp_argTypes_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  // - wires:  state, state case, flags
  ret += tdfToVerilog_fsm_dp_wires_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  // - instantiate FSM module
  ret += indent + op->getName() + "_fsm fsm ("
      +  tdfToVerilog_fsm_args_toString(op,info) + ");\n";

  // - instantiate DP module
  ret += indent + op->getName() + "_dp dp ("
      +  tdfToVerilog_dp_args_toString(op,info) + ");\n";

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "\n";

  return ret;
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_fsm  (emit FSM)

string tdfToVerilog_fsm_args_toString (OperatorBehavioral *op,
				       EVerilogInfo *info)
{
  // - emit argument list for Verilog FSM module:
  //     clock, reset, stream eos/valid/bp, state, flags    (no parentheses)

  string ret;

  ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";

  // - FSM module args:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
    //ret += info->input_data  [arg] + ", ";
      ret += info->input_eos   [arg] + ", ";
      ret += info->input_valid [arg] + ", ";
      ret += info->input_bp    [arg] + ", ";
    }
    else {  // arg is STREAM_OUT
    //ret += info->output_data [arg] + ", ";
      ret += info->output_eos  [arg] + ", ";
      ret += info->output_valid[arg] + ", ";
      ret += info->output_bp   [arg] + ", ";
    }    
  }

  // - FSM module args:  state
  int numStates = info->state_ordered.size();
  if (numStates>1)
    ret += info->state_out + ", ";

  // - FSM module args:  state case
  int numCases = info->max_statecases + 1;	// - case 0 is stall
  if (numCases>1)
    ret += info->statecase_out + ", ";

  // - FSM module args:  flags
  Expr *e;
  forall_defined (e, info->flag)
    ret += info->flag[e] + ", ";

  // - remove final ", "
  ret = ret(0, ret.length()-1-2);

  return ret;
}


string tdfToVerilog_fsm_argTypes_toString (OperatorBehavioral *op,
					   EVerilogInfo *info,
					   string indent)
{
  // - emit argument type declarations (input/output) for Verilog FSM module:
  //     clock, reset, stream eos/valid/bp, state, state-case, flags

  string ret;

  // - FSM module arg types:  clock, reset
  ret += indent + "input  " + CLOCK_NAME + ";\n";
  ret += indent + "input  " + RESET_NAME + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - FSM module arg types:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
    //ret += indent + "input  " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
    //					      : string())
    //				+ info->input_data  [arg] + ";\n";
      ret += indent + "input  " + info->input_eos   [arg] + ";\n";
      ret += indent + "input  " + info->input_valid [arg] + ";\n";
      ret += indent + "output " + info->input_bp    [arg] + ";\n";
    }
    else {  // arg is STREAM_OUT
    //ret += indent + "output " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
    //					      : string())
    //				+ info->output_data [arg] + ";\n";
      ret += indent + "output " + info->output_eos  [arg] + ";\n";
      ret += indent + "output " + info->output_valid[arg] + ";\n";
      ret += indent + "input  " + info->output_bp   [arg] + ";\n";
    }
  }

  addEmptyLine(&ret);		// ----------------

  // - module arg types:  state
  int numStates = info->state_ordered.size();
  if (numStates>1) {
    int stateWidth = countBits(numStates-1);
    ret += indent + "output "
	+  (stateWidth>1 ? string("[%d:0] ",stateWidth-1) : string())
	+  info->state_out + ";\n";
  }

  // - module arg types:  state-case
  int numCases = info->max_statecases + 1;	// - case 0 is stall
  if (numCases>1) {
    int caseWidth = countBits(numCases - 1);
    ret += indent + "output "
	+  (caseWidth>1 ? string("[%d:0] ",caseWidth-1) : string())
        +  info->statecase_out + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - module arg types:  flags
  if (!info->flag.empty()) {
    Expr *e;
    forall_defined (e, info->flag)
      ret += indent + "input  " + info->flag[e] + ";\n";
  }

  return ret;
}


string tdfToVerilog_fsm_stateParams_toString (OperatorBehavioral *op,
					      EVerilogInfo *info,
					      string indent)
{
  // - emit state parameter definitions for Verilog FSM module
  // -                             also for Verilog DP  module

  string ret;

  // - state parameters
  int numStates = info->state_ordered.size();
  if (numStates>1) {
    int stateWidth = countBits(numStates-1);
    string state_par;
    int code=0;
    forall (state_par, info->state_ordered)
      ret += indent + "parameter " + state_par
		    +  string(" = %d'd%d;\n", stateWidth, code++);
  }

  addEmptyLine(&ret);		// ----------------

  // - state-case parameters
  int numCases = info->max_statecases + 1;	// - case 0 is stall
  if (numCases>1) {
    int stateCaseWidth = countBits(numCases-1);
    string scase_par;
    int code=0;
    forall (scase_par, info->statecase_ordered)
      ret += indent + "parameter " + scase_par
                    + string(" = %d'd%d;\n", stateCaseWidth, code++);
  }

  return ret;
}


string tdfToVerilog_fsm_regs_toString (OperatorBehavioral *op,
				       EVerilogInfo *info,
				       string indent)
{
  // - emit regs and always-assignable regs for Verilog FSM module:
  //     state register, stream I/O always assignables

  string ret;

  // - reg:  state
  int numStates = info->state_ordered.size();
  if (numStates>1) {
    int stateWidth = countBits(numStates-1);
    ret += indent + "reg "
	+  (stateWidth>1 ? string("[%d:0] ",stateWidth-1) : string())
	+  info->state_reg + ", "
        +  info->always_assignable[info->state_reg] + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - reg: always assignable for state-case output
  int numCases = info->max_statecases + 1;	// - case 0 is stall
  if (numCases>1) {
    int caseWidth = countBits(numCases-1);
    ret += indent + "reg "
	+  (caseWidth>1 ? string("[%d:0] ",caseWidth-1) : string())
        +  info->always_assignable[ info->statecase_out ] + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - always assignable regs for stream I/O outputs
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
      ret += indent + "reg "
	  +  info->always_assignable[ info->input_bp    [arg] ] + ";\n";
    }
    else {  // arg is STREAM_OUT
      ret += indent + "reg "
	  +  info->always_assignable[ info->output_eos  [arg] ] + ";\n";
      ret += indent + "reg "
	  +  info->always_assignable[ info->output_valid[arg] ] + ";\n";
    }
  }

  addEmptyLine(&ret);		// ----------------

  // - goto (early exit) indicator
  ret += indent + "reg " + info->did_goto + ";\n";

  return ret;
}


string tdfToVerilog_fsm_assign_toString (OperatorBehavioral *op,
					 EVerilogInfo *info,
					 string indent)
{
  // - emit "assign" stmts for Verilog FSM module:
  //     stream I/O outputs

  string ret;

  // - assign:  I/O stream flow control
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
      ret += indent + "assign " + info->input_bp[arg]     + " = " +
	  +  info->always_assignable[ info->input_bp     [arg] ]  + ";\n";
    }
    else {  // arg is STREAM_OUT
      ret += indent + "assign " + info->output_eos[arg]   + " = " +
	  +  info->always_assignable[ info->output_eos   [arg] ]  + ";\n";
      ret += indent + "assign " + info->output_valid[arg] + " = " +
	  +  info->always_assignable[ info->output_valid [arg] ]  + ";\n";
    }
  }

  // - assign:  state output
  int numStates = info->state_ordered.size();
  if (numStates>1)
    ret += indent + "assign " + info->state_out
                  + " = "     + info->state_reg + ";\n";

  // - assign:  state-case output
  int numCases = info->max_statecases + 1;
  if (numCases>1)
    ret += indent + "assign " + info->statecase_out
                  + " = "     + info->always_assignable[ info->statecase_out ]
		  + ";\n";

  return ret;
}


string tdfToVerilog_fsm_alwaysSeq_toString (OperatorBehavioral *op,
					    EVerilogInfo *info,
					    string indent)
{
  // - emit "always @(posedge clock)" sequential block for Verilog FSM module

  string ret;

  int numStates = info->state_ordered.size();
  if (numStates<=1)
    // - supress always @(clock) if no state variable
    return ret;

  // - always block for clock edge
  ret += indent + "always @(posedge " + CLOCK_NAME
                +      " or negedge " + RESET_NAME + ")  begin\n";
  indent += "  ";
  ret += indent + "if (!" + RESET_NAME + ")\n";
  ret += indent + "  " + info->state_reg + "<="
                       + info->state[op->getStartState()] + ";\n";
  ret += indent + "else\n";
  ret += indent + "  " + info->state_reg + "<="
                       + info->always_assignable[info->state_reg] + ";\n";
  indent = indent(0,indent.length()-1-2);
  ret += indent + "end  // always @(posedge...)\n";

  return ret;
}


bool emittedSymbols_map (Tree *t, void *i)
{
  // - Find output streams emitted by code *t  (in behavioral operator)
  // - Add results to:  set<Symbol*> *i

  set<Symbol*> *emittedSymbols = (set<Symbol*>*)i;

  switch (t->getKind())
  {
    case TREE_OPERATOR:
    case TREE_STATE:
    case TREE_STATECASE:  { return true; }

    case TREE_STMT:	  {
			    switch (((Stmt*)t)->getStmtKind()) 
			    {
			      case STMT_BLOCK:
			      case STMT_IF: {
				return true;
			      }
			      case STMT_ASSIGN: {
				StmtAssign *s = (StmtAssign*)t;
				Symbol *sym = s->getLValue()->getSymbol();
				if (sym->getSymKind()==SYMBOL_STREAM &&
				    ((SymbolStream*)sym)->getDir()
						     ==STREAM_OUT)
				  emittedSymbols->insert(sym);
				return false;
			      }
			      case STMT_CALL: {
				fatal(1,"-everilog cannot handle inlined "
				        "call"+t->toString(),t->getToken());
			      }
			      default: {
				return false;
			      }
			    }
			  }

    default:		  { return false; }
  }

  return false;		// - dummy
}


void tdfToVerilog_fsm_stmt (OperatorBehavioral *op,
			    EVerilogInfo *info,
			    Stmt *s,
			    list<string> *stmts,
			    string indent,
			    bool *did_goto)
{
  // - emit Verilog for TDF statement *stmt,
  //     for FSM module of operator *op
  // - fill stmts  (prepend indent, append "\n")
  // - if did_goto is true,
  //     then condition assignments and gotos on "did_goto" reg
  //     (early exit indicator) -- (e.g.:  "if (!did_goto) x=y;")
  // - on exit, set did_goto if encountered a "goto" in some subtree

  // - suppress "state_reg = ..." if no state reg, i.e. if numStates<=1
  int numStates = info->state_ordered.size();

  switch (s->getStmtKind())
  {
    case STMT_BLOCK: {
      stmts->append(indent + "begin\n");
      Stmt *ss;
      forall (ss, *((StmtBlock*)s)->getStmts())
	tdfToVerilog_fsm_stmt(op,info,ss,stmts,indent+"  ",did_goto);
      stmts->append(indent + "end\n");
      break;
    }

    case STMT_IF: {
      Expr *econd = ((StmtIf*)s)->getCond();
      stmts->append(indent + "if (" + info->flag[econd] + ")  begin\n");
      bool then_did_goto=*did_goto, else_did_goto=*did_goto;
      Stmt *sthen = ((StmtIf*)s)->getThenPart();
      if (sthen) {
	tdfToVerilog_fsm_stmt(op,info,sthen,stmts,indent+"  ",&then_did_goto);
	stmts->append(indent + "end\n");
      }
      Stmt *selse = ((StmtIf*)s)->getElsePart();
      if (selse) {
	stmts->append(indent + "else  begin\n");
	tdfToVerilog_fsm_stmt(op,info,selse,stmts,indent+"  ",&else_did_goto);
	stmts->append(indent + "end\n");
      }
      *did_goto = then_did_goto || else_did_goto;
      break;
    }

    case STMT_ASSIGN: {
      Expr *elval = ((StmtAssign*)s)->getLValue();
      assert(elval->getExprKind()==EXPR_LVALUE);
      Symbol *sym = ((ExprLValue*)elval)->getSymbol();
      if (sym->getSymKind()==SYMBOL_STREAM &&
	  ((SymbolStream*)sym)->getDir()==STREAM_OUT) {
	// - output stream emission
	string pred = *did_goto ? ("if (!" + info->did_goto + ") ") : string();
	string valid = info->always_assignable[ info->output_valid[sym] ];
	string eos   = info->always_assignable[ info->output_eos  [sym] ];
	stmts->append(indent + pred + valid + " = 1;\n");
	stmts->append(indent + pred + eos   + " = 0;\n");
      }
      break;
    }

    case STMT_GOTO: {
      string pred = *did_goto ? ("if (!" + info->did_goto + ") ") : string();
      if (numStates>1)
	stmts->append(indent + pred
		       + info->always_assignable[ info->state_reg ] + " = "
		       + info->state[((StmtGoto*)s)->getState()]    + ";\n");
      stmts->append(indent + info->did_goto + " = 1;\n");
      *did_goto = true;
      break;
    }

    case STMT_BUILTIN:	{
      string pred = *did_goto ? ("if (!" + info->did_goto + ") ") : string();
      OperatorBuiltin *builtin = (OperatorBuiltin*)
				   ((StmtBuiltin*)s)->getBuiltin()->getOp();
      switch (builtin->getBuiltinKind())
      {
	case BUILTIN_CLOSE: {
	  Expr *elval = (ExprLValue*)
			((StmtBuiltin*)s)->getBuiltin()->getArgs()->head();
	  assert(elval->getExprKind()==EXPR_LVALUE);
	  Symbol *sym = ((ExprLValue*)elval)->getSymbol();
	  assert(sym->getSymKind()==SYMBOL_STREAM &&
		 ((SymbolStream*)sym)->getDir()==STREAM_OUT);
	  string valid = info->always_assignable[ info->output_valid[sym] ];
	  string eos   = info->always_assignable[ info->output_eos  [sym] ];
	  stmts->append(indent + pred + valid + " = 1;\n");
	  stmts->append(indent + pred + eos   + " = 1;\n");
	  break;
	}
	case BUILTIN_DONE: {
	  string done = info->state_ordered.tail();
	  if (numStates>1)
	    stmts->append(indent + pred
			   + info->always_assignable[ info->state_reg ] + " = "
			   + done + ";\n");
	  stmts->append(indent + info->did_goto + " = 1;\n");
	  *did_goto = true;
	  break;
	}
        case BUILTIN_PRINTF: {
	  break;
	}
        case BUILTIN_FRAMECLOSE: {
	  cout << "skipping frameclose during Verilog construction" << endl;
	  break;
	}
	default: {
	  cout << "StatementKind=" << stmtkindToString(s->getStmtKind()) << endl;
	  cout << "Statement=" << s->toString() << endl;
	  assert(!"-everilog unknown builtin stmt");
	  break;
	}
      }
      break;
    }

    default: {
      assert(!"-everilog unknown stmt kind");
      break;
    }
  }
}


string tdfToVerilog_fsm_alwaysCombi_toString (OperatorBehavioral *op,
					      EVerilogInfo *info,
					      string indent)
{
  // - emit "always @*" combinational block for Verilog FSM module

  string ret;

  // - always block for FSM combinational part
  ret += indent + "always @*  begin\n";
  indent += "  ";

  // - default assignments

  // - default assignments:  I/O stream flow control
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
      ret += indent + info->always_assignable[ info->input_bp[arg] ]
	            + " = 1;\n";
    }
    else {  // arg is STREAM_OUT
      ret += indent + info->always_assignable[ info->output_eos[arg] ]
	            + " = 0;\n";
      ret += indent + info->always_assignable[ info->output_valid[arg] ]
	            + " = 0;\n";
    }
  }

  addEmptyLine(&ret);		// ----------------

  // - default assignments:  state
  int numStates = info->state_ordered.size();
  if (numStates>1)
    ret += indent + info->always_assignable[ info->state_reg ] + " = "
                  + info->state_reg + ";\n";

  // - default assignments:  state-case
  int numCases = info->max_statecases + 1;
  if (numCases>1)
    ret += indent + info->always_assignable[ info->statecase_out ] + " = "
		  + info->statecase_ordered.head() + ";\n";

  // - default assignments:  goto (early exit) indicator
  ret += indent + info->did_goto + " = 0;\n";

  addEmptyLine(&ret);		// ----------------

  // - state actions

  // - suppress "case (state_reg)" if no state register
  bool suppress_case = (numStates<=1);

  // - state actions:  case stmt head
  if (!suppress_case) {
    ret += indent + "case (" + info->state_reg + ")\n";
    indent += "  ";
  }

  // - state actions:  case stmt cases
  list_item it;
  forall_items (it, info->state_ordered) {
    string state_par = info->state_ordered.inf(it);
    if (!suppress_case) {
      ret += indent + state_par + ":  begin\n";
      indent += "  ";
    }
    if (info->needs_done && it==info->state_ordered.last()) {
      // - done state has no firing condition or action
    }
    else {
      State *state = (State*)info->vsymtab.lookup(state_par);
      assert(state && state->getKind()==TREE_STATE);
      // - state actions:  a state-case  (i.e. a firing signature)
      int scase_num = 1;			// - case 0 is stall
      StateCase *scase;
      forall (scase, *state->getCases()) {
	// - state actions:  build firing predicate for firing sig #scase_num
	string firePred = "1";
	InputSpec *ispec;
	forall (ispec, *scase->getInputs()) {
	  Symbol *sym = ispec->getStream();
	  firePred += " && " + info->input_valid[sym];
	  if (!gIgnoreEos)
	    firePred += " && " + string(ispec->isEosCase() ? "" : "!")
	                       + info->input_eos[sym];
	}
	set<Symbol*> emittedSymbols;
	scase->map(emittedSymbols_map,(TreeMap)NULL,&emittedSymbols);
	Symbol *sym;
	forall (sym, emittedSymbols)
	  firePred += " && !" + info->output_bp[sym];
	firePred = firePred.del("1 && ",1);	// - remove leading "1 && "
	ret += indent + (scase_num>1 ? "else " : "")
	              + "if (" + firePred + ")  begin\n";
	indent += "  ";
	string scase_par =  info->statecase_ordered.inf
			   (info->statecase_ordered.get_item(scase_num));
	ret += indent + info->always_assignable[ info->statecase_out ]
		      + " = " + scase_par + ";\n";
	// - state actions:  consume for firing sig #scase_num
	forall (ispec, *scase->getInputs()) {
	  Symbol *sym = ispec->getStream();
	  if (!gIgnoreEos)
	    ret += indent + info->always_assignable[ info->input_bp[sym] ]
			  + " = 0;\n";
	}
	// - state actions:  action for firing signature #scase_num
	list<string> stmts;
	bool did_goto = false;
	Stmt *s;
	forall (s, *scase->getStmts())
	  tdfToVerilog_fsm_stmt(op,info,s,&stmts,indent,&did_goto);
	string stmt;
	forall (stmt, stmts)
	  ret += stmt;
	indent = indent(0,indent.length()-1-2);
	ret += indent + "end\n";
	scase_num++;
      }
    }
    if (!suppress_case) {
      indent = indent(0,indent.length()-1-2);
      ret += indent + "end\n";
    }
  }

  // - state actions:  finish
  if (!suppress_case) {
    indent = indent(0,indent.length()-1-2);
    ret += indent + "endcase  // case (" + info->state_reg + ")\n";
  }

  addEmptyLine(&ret);		// ----------------

  indent = indent(0,indent.length()-1-2);
  ret += indent + "end  // always @*\n";

  return ret;
}


string tdfToVerilog_fsm_toString (OperatorBehavioral *op, EVerilogInfo *info)
{
  // - emit Verilog for FSM for behavioral operator *op

  string ret;
  string indent;

  // - FSM module declaration
  ret += indent + "module " + op->getName() + "_fsm (";
  ret += tdfToVerilog_fsm_args_toString(op,info);
  ret += ");\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_argTypes_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_stateParams_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_regs_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_assign_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_alwaysSeq_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_alwaysCombi_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "_fsm\n";

  return ret;
}


string tdfToVerilog_fsmbb_toString (OperatorBehavioral *op, EVerilogInfo *info)
{
  // - emit Verilog for black-box FSM for behavioral operator *op

  string ret;
  string indent;

  // - FSM module declaration
  ret += indent + "module " + op->getName() + "_fsm (";
  ret += tdfToVerilog_fsm_args_toString(op,info);
  ret += ") /* synthesis syn_black_box */ ;\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_argTypes_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "_fsm\n";

  return ret;
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_dp  (emit datapath)


//  Notes on Datapath Synthesis:
//
//    - each if condition induces "assign flag = cond;"
//    - statements in a state induce verilog directly, in "always @*",
//        assigning to always-assignable versions of TDF variables
//    - rely on blocking assignment to handle multiply-assigned variables
//        (e.g. x=1; x=x+1;)
//    - begin "always @*" by assigning default value to each register's
//        always-assignable, so it has a value in non-assigned exec paths
//    - each local register induces a reg (i.e. name lifted to module level),
//        but it is live (defined, used) only within the begin-end block
//        corresponding to its TDF scope, inside "always @*",
//        so Synplify is smart enough to not create a latch
//        for execution paths in which the local is not defined
//    - Verilog 2001 features used:
//        * always @*
//        * signed numbers
//            (generate additional code for upgrading/casting,
//             since verilog defaults mixed binary operations to unsigned,
//             and verilog $signed probably does not expand width
//    - Synplify Pro 7.1 peculiarities
//        * width(a+b)  = max(width(a),width(b)) + 1
//        * width(a*b)  = width(a) + width(b)
//        * width(a<<b) = width of assignmt target  (input extends to it first)
//        * a<<<b not supported  (arithmetic shift left, shift right)
//        * bit subscripting (x[2:0]) only works on names, not exprs
//            --> explicit sign extension/upgrade require named argument
//        * width(a%b)  = width(b)
//            + requires:  a unsigned,  b = const power of 2
//            + is wires:  a%b = a[W(b)-1:0]
//        * width(a/b) = width(a)-width(b)
//            + requires:  a unsigned,  b = const power of 2
//            + is wires:  a/b = a[W(a)-1,W(a)-W(b)-1]
//        * no $signed()
//            --> upgrade unsigned to signed by assigning to signed temporary


bool isAssociative (Expr *e)
{
  // - return true if e yields an associative operator in Verilog
  //     (infix or otherwise)
  //     that may need to be surrounded by parentheses for proper evaluation

  ExprKind k = e->getExprKind();
  return k==EXPR_COND || k==EXPR_BOP || k==EXPR_UOP;
}


string tdfToVerilog_expr_toString (OperatorBehavioral *op,
				   EVerilogInfo *info,
				   Expr *e,
				   list<string> *tempDefs,
				   string indent)
{
  // - emit Verilog code for a TDF expression *e,
  //     for data-path module of operator *op
  // - if *e must be assigned to a Verilog temporary
  //     to properly handle bit width / sign extension,
  //     then create Verilog stmt to define the temporary, append to *tempDefs
  //     * tempDefs stmts are to be evaluated in Verilog before expr
  //     * temporaries were generated in tdfToVerilog_scanTDF()
  // - recursive, fills tempDefs for sub-trees
  // - indent is passed on for statement generation;
  //     returned expr is not itself indented

  string ret;

  Type *t = e->getType();
  if (t->getTypeKind()!=TYPE_BOOL &&
      t->getTypeKind()!=TYPE_DOUBLE && // Added by Nachiket on 16/8/2011
      t->getTypeKind()!=TYPE_INT)
    fatal(1,"-everilog cannot handle expression " + e->toString() +
	    " since its type is neither boolean nor integer", e->getToken());

  int width;
  if (!isConstWidth(t,&width))			// - (sets width)
    fatal(1, "-everilog cannot handle expression " + e->toString() +
	     " of non-constant width " + e->toString(), e->getToken());

  switch (e->getExprKind())
  {
    case EXPR_VALUE: {
      int val = ((ExprValue*)e)->getIntVal();
      switch (t->getTypeKind())
      {
	case TYPE_BOOL:		ret += string("1'b%d",(val!=0));
				break;

	case TYPE_INT:		if (val==0 && width==0)
				  // - unsigned[0] 0 --> 1'b0, not 0'b0
				  ret += t->isSigned() ? "1'sb0" : "1'b0";
				else {
				  if (t->isSigned()) {
				    if (val<0)
				      ret += string("-%d'sd%d",width,-val);
				    else
				      ret += string( "%d'sd%d",width, val);
				  }
				  else
				      ret += string( "%d'd%d", width, val);
				}
				break;

        default:		assert(!"-everilog bad type");
      }
      break;
    }

    case EXPR_LVALUE: {
      ExprLValue *lval = (ExprLValue*)e;
      Symbol     *sym  = lval->getSymbol();

      // - choose verilog reg to use  (input, input history, data reg, local)
      string reg;
      if (sym->getSymKind()==SYMBOL_STREAM &&
	  ((SymbolStream*)sym)->getDir()==STREAM_IN) {
	// - referenced symbol is input stream,
	//     check for input history retiming
	int retime;		// - set to retiming from TDF: "i@retime"
	Expr *e_retime = lval->getRetime();
	if (e_retime==NULL)
	  retime=0;
	else {
	  if (e_retime->getExprKind()!=EXPR_VALUE)
	    fatal(1, "-everilog cannot handle non-constant retiming of "+
		     e->toString(), e->getToken());
	  retime = ((ExprValue*)e_retime)->getIntVal();
	}
	// - actual history retiming depends on whether input stream is
	//     consumed in same state-case as it is referenced
	// - find state-case that encloses e
	StateCase *scase;
	for (Tree *tt=e; tt!=NULL; tt=tt->getParent())	// - find enclosing
	  if (tt->getKind()==TREE_STATECASE) {		//     statecase
	    scase = (StateCase*)tt;
	    break;
	  }
	if (scase==NULL)
	  assert(!"-everilog cannot find enclosing state-case");
	int actual_retime = -1 + retime + (symConsumedHere(sym,scase) ? 0 : 1);
	if (actual_retime==-1)		// - use input stream directly
	  reg = info->input_data[sym];
	else				// - use input history register
	  reg = info->history_regs[sym][actual_retime];
      }
      else if (sym->getSymKind()==SYMBOL_STREAM &&
	  ((SymbolStream*)sym)->getDir()==STREAM_OUT) {
	// - referenced symbol is output stream  (as assignment target)
	reg = info->always_assignable[ info->output_data[sym] ];
      }
      else if (sym->isArray()) {
	// - referenced symbol is TDF read-only array variable
	Expr  *eaddr   = lval->getArrayLoc();
	if (!eaddr)
	  fatal(1,"-everilog cannot reference entire array "+sym->getName()+
		  ", only elements thereof", lval->getToken());
	string saddr   = tdfToVerilog_expr_toString(op,info,eaddr,tempDefs,indent);
	string addrDef = indent + info->rom_addr[sym] + " = " + saddr + ";\n";
	tempDefs->append(addrDef);
	reg = info->always_assignable[ info->rom_data[sym] ];
      }
      else if (sym->isReg()) {
	// - referenced symbol is TDF register variable
	reg = info->always_assignable[ info->reg[sym] ];
      }
      else if (sym->isLocal()) {
	// - referenced symbol is TDF local variable
	reg = info->always_assignable[ info->local[sym] ];
      }
      else {
	fatal(1, "-everilog cannot handle symbol reference "+e->toString(),
	      e->getToken());
      }

      // - determine bit sub-range
      string bitrange;
      Expr *low=lval->getPosLow(), *hi=lval->getPosHigh();
      if (lval->usesAllBits())				// - no sub-range
	bitrange = "";
      else if (low && hi) {
	if (!low->equals(hi))				// - sub-range
	  bitrange = "[" + hi->toString() + ":" + low->toString() + "]";
	else						// - single bit
	  bitrange = "[" + low->toString() + "]";
      }
      else if (low && !hi)				// - single bit
	bitrange = "[" + low->toString() + "]";
      else
	assert(!"-everilog confused about bit sub-range");

      ret += reg + bitrange;
      break;
    }

    case EXPR_CALL: {
      fatal(1, "-everilog cannot handle inlined call " + e->toString() +
	       ", try -xc", e->getToken());
    }

    case EXPR_BUILTIN: {
      OperatorBuiltin *builtin = (OperatorBuiltin*) ((ExprBuiltin*)e)->getOp();
      switch (builtin->getBuiltinKind())
      {
	case BUILTIN_BITSOF: {
	  Expr *arg = ((ExprBuiltin*)e)->getArgs()->head();
	  ret += tdfToVerilog_expr_toString(op,info,arg,tempDefs,indent);
	  break;
	}
	case BUILTIN_CAT: {
	  ret += "{";
	  Expr *arg;
	  forall (arg, *((ExprBuiltin*)e)->getArgs())
	    ret += tdfToVerilog_expr_toString(op,info,arg,tempDefs,indent)
	        +  ", ";
	  ret = ret(0,ret.length()-1-2);	// - remove trailing ", "
	  ret += " }";
	  break;
	}
	case BUILTIN_WIDTHOF: {
	  Expr *arg = ((ExprBuiltin*)e)->getArgs()->head();
	  Type *argtype = arg->getType();
	  if (argtype->getTypeKind()!=TYPE_BOOL &&
	      argtype->getTypeKind()!=TYPE_INT)
	    fatal(1, "-everilog cannot get width of expr " + arg->toString() +
		     "since its type is neither boolean nor integer",
		  arg->getToken());
	  int argWidth;
	  if (!isConstWidth(arg->getType(),&argWidth))	// - (sets argWidth)
	    fatal(1, "-everilog cannot get width of expr " + arg->toString() +
		     " since it has non-constant width", arg->getToken());
	  ret += string("%d",argWidth);
	  break;
	}
	default: {
	  fatal(1, "-everilog cannot handle builtin "+e->toString(),
		e->getToken());
        }
      }
      break;
    }

    case EXPR_COND: {
      ExprCond *econd = (ExprCond*)e;
      string cond  = tdfToVerilog_expr_toString(op,info,econd->getCond(),
						tempDefs,indent);
      string e1    = tdfToVerilog_expr_toString(op,info,econd->getThenPart(),
						tempDefs,indent);
      string e2    = tdfToVerilog_expr_toString(op,info,econd->getElsePart(),
						tempDefs,indent);
      ret += cond + " ? " + e1 + " : " + e2;
      break;
    }

    case EXPR_BOP: {
      // - WARNING:  not sure if bit extension / width semantics are right
      ExprBop *ebop = (ExprBop*)e;
      Expr    *e1   = ebop->getExpr1(),  *e2 = ebop->getExpr2();
      string  se1   = tdfToVerilog_expr_toString(op,info,e1,tempDefs,indent);
      string  se2   = tdfToVerilog_expr_toString(op,info,e2,tempDefs,indent);
      int       bop = ebop->getOp();
      if (bop==LEFT_SHIFT || bop==RIGHT_SHIFT) {
	if (e1->getType()->isSigned()) {
	  // - WARNING:  Synplify does not support arithmetic shift <<< >>>
	  //             For now, fall through and emit logical shift
	}
      }
      ret += se1 + " " + opToString(bop) + " " + se2;
      break;
    }

    case EXPR_UOP: {
      // - WARNING:  not sure if bit extension / width semantics are right
      ExprUop *euop = (ExprUop*)e;
      string arg= tdfToVerilog_expr_toString(op,info,euop->getExpr(),
					     tempDefs,indent);
      ret += opToString(euop->getOp()) + " " + arg;
      break;
    }

    case EXPR_CAST: {
      Expr  *arg = ((ExprCast*)e)->getExpr();
      string tmp = tdfToVerilog_expr_toString(op,info,arg,tempDefs,indent);
      int argWidth;
      if (!isConstWidth(arg->getType(),&argWidth))	// - (sets argWidth)
	assert(!"non const width");			// - caught by tmp abov
      bool arg_signed = arg->getType()->isSigned();
      bool   e_signed =   e->getType()->isSigned();
      if (!arg_signed && !e_signed) {
	// - unsigned --> unsigned
	if (width>argWidth)			// - zero extend
	  ret += string("{%d'b0,",width-argWidth) + tmp + "}";
	else if (width<argWidth)		// - truncate high bits ILLEGAL
	  ret += tmp + string("[%d:0]",width-1);
	else					// - cast to same
	  ret += tmp;
      }
      else if (arg_signed && e_signed) {
	// - signed --> signed
	if (width>argWidth) {			// - sign extend
	  assert(argWidth>=1);
	  ret += "{" + string("{%d{",width-argWidth) + tmp
	             + string("[%d]}},",argWidth-1)  + tmp + "}";
	}
	else if (width<argWidth) {		// - signed width redcn ILLEGAL
	  assert(width>=1);
	  if (width>=2)
	    ret += "{" + tmp + string("[%d],",argWidth-1)
	               + tmp + string("[%d:0]",width-2)   + "}";
	  else  // width==1
	    ret += tmp + string("[%d]",argWidth-1);
	}
	else					// - cast to same
	  ret += tmp;
      }
      else if (!arg_signed && e_signed) {
	// - unsigned --> signed
	if (width>argWidth) {			// - sign upgrade + extend
	  assert(width>=1);
	  ret += string("{%d'sb0,",width-argWidth) + tmp + "}";
	}
	else if (width<=argWidth) {		// - sign upgrade+redcn ILLEGAL
	  assert(width>=1);
	  if (width>=2)
	    ret += "{1'sb0," + tmp + string("[%d:0]}",width-2) + "}";
	  else  // width==1
	    ret += "1'sb0";
	}
      }
      else {
	// - signed --> unsigned
	fatal(1, "cast "+e->toString()+" is an illegal sign downgrade, "
	         "try bitsof()", e->getToken());
      }
      break;
    }

    default: {
      fatal(1, "-everilog cannot handle expression " + e->toString(),
	    e->getToken());
    }
  }

  // - add surrounding parens if necessary
  if (isAssociative(e))
    ret = "(" + ret + ")";

  // - if expr requires a temporary then
  //     (1) return temporary, not expr
  //     (2) add stmt to *info->tempDefs to define temp:  temp = expr;
  //           NOTE:  intentionally omit goto (early exit) predication
  //                  from temp defs
  // - temporaries were generated in tdfToVerilog_scanTDF()
  bool tsign;
  int  twidth;
  if (requiresTemporary(e,info,&tsign,&twidth)) {
    string temp    = info->temp[e];
    string tempDef = indent + temp + " = " + ret + ";\n";
    tempDefs->append(tempDef);
    ret            = temp;
  }

  // - similarly, if expr is an "if" condition, then use flag output as temp
  //     (1) return flag output's always-assignable, not expr
  //     (2) add stmt to *info->tempDefs to define flag:  flag = expr;
  //           NOTE:  intentionally omit goto (early exit) predication
  //                  from temp defs
  Tree *p = e->getParent();
  if (             (p)->getKind()==TREE_STMT
       && ((Stmt*)  p)->getStmtKind()==STMT_IF
       && ((StmtIf*)p)->getCond()==e ) {
    string flag    = info->always_assignable[ info->flag[e] ];
    string flagDef = indent + flag + " = " + ret + ";\n";
    tempDefs->append(flagDef);
    ret            = flag;
  }

  return ret;
}


string tdfToVerilog_dp_args_toString (OperatorBehavioral *op,
				      EVerilogInfo *info)
{
  // - print argument list for DP module:  stream data,eos,bp, state, flags

  string ret;

  ret += string(CLOCK_NAME) + ", " + string(RESET_NAME) + ", ";

  // - DP module args:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
      ret += info->input_data  [arg] + ", ";
    //ret += info->input_eos   [arg] + ", ";
    //ret += info->input_valid [arg] + ", ";
    //ret += info->input_bp    [arg] + ", ";
    }
    else {  // arg is STREAM_OUT
      ret += info->output_data [arg] + ", ";
    //ret += info->output_eos  [arg] + ", ";
    //ret += info->output_valid[arg] + ", ";
    //ret += info->output_bp   [arg] + ", ";
    }    
  }

  // - DP module args:  state
  int numStates = info->state_ordered.size();
  if (numStates>1)
    ret += info->state_out + ", ";

  // - DP module args:  state-case
  int numCases = info->max_statecases + 1;	// - case 0 is stall
  if (numCases>1)
    ret += info->statecase_out + ", ";

  // - DP module args:  flags
  Expr *e;
  forall_defined (e, info->flag)
    ret += info->flag[e] + ", ";

  // - remove final ", "
  ret = ret(0, ret.length()-1-2);

  return ret;
}


string tdfToVerilog_dp_argTypes_toString (OperatorBehavioral *op,
					  EVerilogInfo *info,
					  string indent)
{
  // - emit argument type declarations (input/output) for Verilog DP module:
  //     clock, reset, stream data/bp, state, state-case, flags

  string ret;

  // - DP module arg types:  clock, reset
  ret += indent + "input  " + CLOCK_NAME + ";\n";
  ret += indent + "input  " + RESET_NAME + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - DP module arg types:  stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
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
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
      ret += indent + "input  " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
					      : string())
				+ info->input_data  [arg] + ";\n";
    //ret += indent + "input  " + info->input_eos   [arg] + ";\n";
    //ret += indent + "input  " + info->input_valid [arg] + ";\n";
    //ret += indent + "input  " + info->input_bp    [arg] + ";\n";
    }
    else {  // arg is STREAM_OUT
      ret += indent + "output " + (argWidth>1 ? string("[%d:0] ",argWidth-1)
					      : string())
				+ info->output_data [arg] + ";\n";
    //ret += indent + "output " + info->output_eos  [arg] + ";\n";
    //ret += indent + "output " + info->output_valid[arg] + ";\n";
    //ret += indent + "input  " + info->output_bp   [arg] + ";\n";
    }
  }

  addEmptyLine(&ret);		// ----------------

  // - DP module arg types:  state
  int numStates = info->state_ordered.size();
  if (numStates>1) {
    int stateWidth = countBits(numStates-1);
    ret += indent + "input  "
	+  (stateWidth>1 ? string("[%d:0] ",stateWidth-1) : string())
	+  info->state_out + ";\n";
  }

  // - DP module arg types:  state-case
  int numCases = info->max_statecases + 1;	// - case 0 is stall
  if (numCases>1) {
    int caseWidth = countBits(numCases-1);
    ret += indent + "input  "
	+  (caseWidth>1 ? string("[%d:0] ",caseWidth-1) : string())
        +  info->statecase_out + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - DP module arg types:  flags
  if (!info->flag.empty()) {
    Expr *e;
    forall_defined (e, info->flag)
      ret += indent + "output " + info->flag[e] + ";\n";
  }

  return ret;
}


string tdfToVerilog_dp_regs_toString (OperatorBehavioral *op,
				      EVerilogInfo *info,
				      string indent)
{
  // - emit regs and always-assignable regs for Verilog DP module:
  //     input history registers
  //     TDF data register variables
  //     TDF local variables
  //     TDF array variables' addr + data
  //     Verilog temporaries for expressions
  //     stream I/O  always assignables
  //     flag output always assignables

  string ret;

  // - input history registers
  Symbol *sym;
  forall_defined (sym, info->history_regs) {
    int width;
    if (!isConstWidth(sym->getType(),&width))		// - (sets width)
      assert(!"-everilog found history register with non-const width");
    string reg;
    forall (reg, info->history_regs[sym]) {
      ret += indent + "reg "
	  +  (sym->getType()->isSigned() ? "signed " : "")
	  +  (width>1 ? string("[%d:0] ",width-1) : string())
	  +  reg + ";\n";
      //  +  reg + ", "					// - don't need
      //  +  info->always_assignable[reg] + ";\n";	//   always assignable
    }
  }

  // - register variables (scalar)
  forall_defined (sym, info->reg) {
    int width;
    if (!isConstWidth(sym->getType(),&width))		// - (sets width)
      assert(!"-everilog found register variable with non-const width");
    string reg = info->reg[sym];
    ret += indent + "reg "
        +  (sym->getType()->isSigned() ? "signed " : "")
	+  (width>1 ? string("[%d:0] ",width-1) : string())
	+  reg + ", "
	+  info->always_assignable[reg] + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - local variables (scalar)
  forall_defined (sym, info->local) {
    int width;
    if (!isConstWidth(sym->getType(),&width))		// - (sets width)
      assert(!"-everilog found local variable with non-const width");
    string local = info->local[sym];
    ret += indent + "reg "
        +  (sym->getType()->isSigned() ? "signed " : "")
	+  (width>1 ? string("[%d:0] ",width-1) : string())
	+  info->always_assignable[local] + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - array variables' addr + data
  forall_defined (sym, info->rom_addr) {
    Type      *t  = sym->getType();
    TypeArray *ta = (TypeArray*)t;
    assert(t->getTypeKind()==TYPE_ARRAY);
    int width;
    if (!isConstWidth(ta->getElemType(),&width))	// - (sets width)
      assert(!"-everilog found array variable with non-const width");
    int nelems;
    if (!isConstNelems(ta,&nelems))			// - (sets nelems)
      assert(!"-everilog found array variable with non-const number of elements");
    int addrwidth = (nelems>1) ? countBits(nelems-1) : 1;
    string addr = info->rom_addr[sym];
    string data = info->rom_data[sym];
    ret += indent + "reg "
        +  (sym->getType()->isSigned() ? "signed " : "")
	+  (addrwidth>1 ? string("[%d:0] ",addrwidth-1) : string())
	+  info->always_assignable[addr] + ";\n";
    ret += indent + "reg "
        +  (sym->getType()->isSigned() ? "signed " : "")
	+  (width>1 ? string("[%d:0] ",width-1) : string())
	+  info->always_assignable[data] + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - Verilog temporaries for expressions
  Expr *e;
  forall_defined (e, info->temp) {
    string temp = info->temp[e];
    bool   sign = info->temp_sign[temp];
    int   width = info->temp_width[temp];
    ret += indent + "reg "
        +  (sign ? "signed " : "")
	+  (width>1 ? string("[%d:0] ",width-1) : string())
	+  info->always_assignable[temp] + ";\n";
  }

  addEmptyLine(&ret);		// ----------------

  // - always assignable regs for stream I/O
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    int argWidth;
    if (!isConstWidth(arg->getType(),&argWidth))	// - (sets argWidth)
      assert(!"non-const width");			// - caught above
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
    }
    else {  // arg is STREAM_OUT
      ret += indent + "reg "
	  +  (arg->getType()->isSigned() ? "signed " : "")
	  +  (argWidth>1 ? string("[%d:0] ",argWidth-1) : string())
	  +  info->always_assignable[ info->output_data [arg] ] + ";\n";
    }
  }

  addEmptyLine(&ret);		// ----------------

  // - always assignable regs for flag output
  string flag;
  forall (flag, info->flag)
    ret += indent + "reg " + info->always_assignable[flag] + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - goto (early exit) indicator
  ret += indent + "reg " + info->did_goto + ";\n";

  return ret;
}


string tdfToVerilog_dp_assign_toString (OperatorBehavioral *op,
					EVerilogInfo *info,
					string indent)
{
  // - emit "assign" stmts for Verilog DP module:
  //     output stream data, flags

  string ret;

  // - output stream data
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
    }
    else {  // arg is STREAM_OUT
      ret += indent + "assign " +     info->output_data[arg]    + " = " +
	  +  info->always_assignable[ info->output_data[arg] ]  + ";\n";
    }
  }

  addEmptyLine(&ret);		// ----------------

  // - flags
  //     * flag expressions may require temporaries, so cannot be in "assign"
  //     * use assign merely to copy always-assignable to flag output
  // Expr *e;
  // forall_defined (e, info->flag)
  string flag;
  forall (flag, info->flag)
    ret += indent + "assign " +    flag  + " = "
        +  info->always_assignable[flag] + ";\n";

  return ret;
}


string tdfToVerilog_dp_alwaysSeq_toString (OperatorBehavioral *op,
					   EVerilogInfo *info,
					   string indent)
{
  // - emit "always @(posedge clock)" sequential block for Verilog DP module

  string ret;

  // - always header
  ret += indent + "always @(posedge " + CLOCK_NAME
                +      " or negedge " + RESET_NAME + ")  begin\n";
  indent += "  ";

  // - reset case
  ret += indent + "if (!" + RESET_NAME + ")  begin\n";
  indent += "  ";

  // - reset case:  input history regs
  //                (initialization of history regs; assign don't care)
  Symbol *sym;
  forall_defined (sym, info->history_regs) {
    int width;
    if (!isConstWidth(sym->getType(),&width))		// - (sets width)
      assert(!"-everilog found history register with non-const width");
    string reg;
    forall (reg, info->history_regs[sym])
      ret += indent + reg + " <= " + string("%d'bx",width) + ";\n";
  }

  // - reset case:  TDF register variables
  forall_defined (sym, info->reg) {
    assert(sym->getSymKind()==SYMBOL_VAR);
    Expr *e_val = ((SymbolVar*)sym)->getValue();
    string reg  = info->reg[sym];
    if (e_val) {
      // - reg has initial value
      // - disallow complex initialization expressions,
      //     since tdfToVerilog_expr_toString(init) would incorrectly
      //     reference always-assignables and temps w/blocking assignment.
      //     Here, for the clock boundary, we should generate only
      //     non-blocking assignments and no temporaries.
      if (e_val->getExprKind()!=EXPR_VALUE)
	fatal(1,"-everilog cannot handle non-constant initialization " +
	        e_val->toString() + " for register variable " +sym->toString(),
	      e_val->getToken());
      list<string> tempDefs;
      string val = tdfToVerilog_expr_toString(op,info,e_val,&tempDefs,indent);
      assert(tempDefs.empty());
      // string tempDef;		// - no tempDefs w/blocking assignment
      // forall (tempDef, tempDefs)
      //   ret += tempDef;
      ret += indent + reg + " <= " + val + ";\n";
    }
    else {
      // - reg has NO initial value
      int width;
      if (!isConstWidth(sym->getType(),&width))		// - (sets width)
	assert(!"-everilog found register variable with non-const width");
      ret += indent + reg + string(" <= %d'bx;\n",width);
    }
  }

  // - reset case:  finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "end\n";

  // - non-reset case
  ret += indent + "else  begin\n";
  indent += "  ";

  // - non-reset case:  input history regs
  forall_defined (sym, info->history_regs) {
    array<string> &history_regs_of_sym = info->history_regs[sym];
    for (int d=0; d<history_regs_of_sym.size(); d++) {
      string reg =                                  history_regs_of_sym[d];
      string val = (d==0) ? info->input_data[sym] : history_regs_of_sym[d-1];
      ret += indent + reg + " <= " + val + ";\n";
    }
  }

  // - non-reset case:  TDF register variables
  forall_defined (sym, info->reg) {
    string reg = info->reg[sym];
    string val = info->always_assignable[reg];
    ret += indent + reg + " <= " + val + ";\n";
  }

  // - non-reset case:  finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "end\n";

  // - always finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "end  // always @(posedge...)\n";

  return ret;
}


void tdfToVerilog_dp_stmt (OperatorBehavioral *op,
			   EVerilogInfo *info,
			   Stmt *s,
			   list<string> *stmts,
			   string indent,
			   bool *did_goto)
{
  // - emit Verilog for TDF statement *stmt,
  //     for data-path module of operator *op
  // - fill stmts  (prepend indent, append "\n")
  // - if did_goto is true,
  //     then condition assignments and gotos on "did_goto" reg
  //     (early exit indicator) -- (e.g.:  "if (!did_goto) x=y;")
  // - on exit, set did_goto if encountered a "goto" in some subtree

  switch (s->getStmtKind())
  {
    case STMT_BLOCK: {
      stmts->append(indent + "begin\n");
      Stmt *ss;
      forall (ss, *((StmtBlock*)s)->getStmts())
	tdfToVerilog_dp_stmt(op,info,ss,stmts,indent+"  ",did_goto);
      stmts->append(indent + "end\n");
      break;
    }

    case STMT_IF: {
      Expr *econd = ((StmtIf*)s)->getCond();
      string cond = tdfToVerilog_expr_toString(op,info,econd,stmts,indent);
        // - expr generation creates flag definition statement, adds to *stmts
      stmts->append(indent + "if (" + cond + ")  begin\n");
      bool then_did_goto=*did_goto, else_did_goto=*did_goto;
      Stmt *sthen = ((StmtIf*)s)->getThenPart();
      if (sthen) {
	tdfToVerilog_dp_stmt(op,info,sthen,stmts,indent+"  ",&then_did_goto);
	stmts->append(indent + "end\n");
      }
      Stmt *selse = ((StmtIf*)s)->getElsePart();
      if (selse) {
	stmts->append(indent + "else  begin\n");
	tdfToVerilog_dp_stmt(op,info,selse,stmts,indent+"  ",&else_did_goto);
	stmts->append(indent + "end\n");
      }
      *did_goto = then_did_goto || else_did_goto;
      break;
    }

    case STMT_ASSIGN: {
      Expr *elval = ((StmtAssign*)s)->getLValue();
      string lval = tdfToVerilog_expr_toString(op,info,elval,stmts,indent);
      Expr  *erhs = ((StmtAssign*)s)->getRhs();
      string  rhs = tdfToVerilog_expr_toString(op,info,erhs,stmts,indent);
      string pred = *did_goto ? ("if (!" + info->did_goto + ") ") : string();
      stmts->append(indent + pred + lval + " = " + rhs + ";\n");
      break;
    }

    case STMT_GOTO: {
      stmts->append(indent + info->did_goto + " = 1;\n");
      *did_goto = true;
      break;
    }

    case STMT_BUILTIN:	{
      OperatorBuiltin *builtin = (OperatorBuiltin*)
				 ((StmtBuiltin*)s)->getBuiltin()->getOp();
      if (builtin->getBuiltinKind()==BUILTIN_DONE) {
	// - done()  -- set did_goto
	stmts->append(indent + info->did_goto + " = 1;\n");
	*did_goto = true;
      }
      // - close() -- nothing for data-path
      break;
    }

    default: {
      assert(!"everilog unknown stmt kind");
      break;
    }
  }
}


string tdfToVerilog_dp_alwaysCombi_toString (OperatorBehavioral *op,
					     EVerilogInfo *info,
					     string indent)
{
  // - emit "always @*" combinational block for Verilog DP module

  string ret;

  // - always block for DP combinational part
  ret += indent + "always @*  begin\n";
  indent += "  ";

  // - default assignments

  // - default assignments:  output stream data
  list<Symbol*> args = args_with_retsym_first(op);
  Symbol *arg;
  forall (arg, args) {
    if (arg->isParam()) {
      // - params should already be bound, ignore
      continue;
    }
    assert(arg->isStream());
    if (((SymbolStream*)arg)->getDir()==STREAM_IN) {
    }
    else {  // arg is STREAM_OUT
      int width;
      if (!isConstWidth(arg->getType(),&width))		// - (sets width)
	assert(!"-everilog found register variable with non-const width");
      ret += indent + info->always_assignable[ info->output_data[arg] ]
	            + string(" = %d'bx;\n",width);
    }
  }

  addEmptyLine(&ret);		// ----------------

  // - default assignments:  flag output
  string flag;
  forall (flag, info->flag)
    ret += indent + info->always_assignable[flag] + " = 1'bx;\n";

  addEmptyLine(&ret);		// ----------------

  // - default assignments:  TDF register variables (scalar)
  string reg;
  forall (reg, info->reg)
    ret += indent + info->always_assignable[reg] + " = " + reg + ";\n";

  addEmptyLine(&ret);		// ----------------

  // - default assignments:  TDF local variables (scalar)
  Symbol *sym;
  forall_defined (sym, info->local) {
    assert(sym->getSymKind()==SYMBOL_VAR);
    Expr *e_val = ((SymbolVar*)sym)->getValue();
    string local = info->local[sym];
    if (e_val) {
      // - local has initial value
      list<string> tempDefs;
      string val = tdfToVerilog_expr_toString(op,info,e_val,&tempDefs,indent);
      ret += indent + local + " = " + val + ";\n";
    }
    else {
      // - local has NO initial value
      int width;
      if (!isConstWidth(sym->getType(),&width))		// - (sets width)
	assert(!"-everilog found local variable with non-const width");
      ret += indent + local + string(" = %d'bx;\n",width);
    }
  }

  // - default assignments:  TDF array variables' addr
  forall_defined (sym, info->rom_addr) {
    Type      *t  = sym->getType();
    TypeArray *ta = (TypeArray*)t;
    assert(t->getTypeKind()==TYPE_ARRAY);
//  int width;
//  if (!isConstWidth(ta->getElemType(),&width))	// - (sets width)
//    assert(!"-everilog found array variable with non-const width");
    int nelems;
    if (!isConstNelems(ta,&nelems))			// - (sets nelems)
      assert(!"-everilog found array variable with non-const number of elements");
    int addrwidth = (nelems>1) ? countBits(nelems-1) : 1;
    string addr = info->rom_addr[sym];
//  string data = info->rom_data[sym];
    ret += indent + addr + string(" = %d'bx;\n",addrwidth);
//  ret += indent + data + string(" = %d'bx;\n",    width);
  }

  addEmptyLine(&ret);		// ----------------

  // - default assignments:  goto (early exit) indicator
  ret += indent + info->did_goto + " = 0;\n";

  addEmptyLine(&ret);		// ----------------

  // - state actions

  // - suppress "case (state_reg)" if no state register
  int numStates = info->state_ordered.size();
  bool suppress_case = (numStates<=1);

  // - state actions:  case stmt head
  if (!suppress_case) {
    ret += indent + "case (" + info->state_out + ")\n";
    indent += "  ";
  }

  // - state actions:  case stmt cases
  list_item it;
  forall_items (it, info->state_ordered) {
    string state_par = info->state_ordered.inf(it);
    if (!suppress_case) {
      ret += indent + state_par + ":  begin\n";
      indent += "  ";
    }
    if (info->needs_done && it==info->state_ordered.last()) {
      // - done state has no firing condition or action
    }
    else {
      State *state = (State*)info->vsymtab.lookup(state_par);
      assert(state && state->getKind()==TREE_STATE);
      // - state actions:  a state-case  (i.e. a firing signature)
      int scase_num = 1;			// - case 0 is stall
      StateCase *scase;
      forall (scase, *state->getCases()) {
	string scase_par =  info->statecase_ordered.inf
			   (info->statecase_ordered.get_item(scase_num));
	ret += indent + (scase_num>1 ? "else " : "")
		      + "if (" + info->statecase_out
		      + " == "   + scase_par + ")  begin\n";
	indent += "  ";
	// - state actions:  action for firing signature #scase_num
	list<string> stmts;
	bool did_goto = false;
	Stmt *s;
	forall (s, *scase->getStmts()) {
//	  cout << "\t Kapre is generating Verilog for state=" << s->toString() <<  endl;
	  tdfToVerilog_dp_stmt(op,info,s,&stmts,indent,&did_goto);
	}
	string stmt;
	forall (stmt, stmts)
	  ret += stmt;
	indent = indent(0,indent.length()-1-2);
	ret += indent + "end\n";
	scase_num++;
      }
    }
    if (!suppress_case) {
      indent = indent(0,indent.length()-1-2);
      ret += indent + "end\n";
    }
  }

  // - state actions:  finish
  if (!suppress_case) {
    indent = indent(0,indent.length()-1-2);
    ret += indent + "endcase  // case (" + info->state_reg + ")\n";
  }

  indent = indent(0,indent.length()-1-2);
  ret += indent + "end  // always @*\n";

  return ret;
}


string tdfToVerilog_dp_alwaysROM_toString (OperatorBehavioral *op,
					   EVerilogInfo *info,
					   Symbol *sym,
					   string indent)
{
  // - emit "always @*" combinational block for one ROM in Verilog DP module
  // - sym is a read-only array

  string ret;

  // - get array value, chase array params up call stack
  ExprArray *arrayValue = chaseArrayValue(sym);
  if (!arrayValue)
    warn("array " + sym->getName() + " has no value, "
	 "so -everilog will generate don't-cares instead of ROM",
	 sym->getToken());

  Type      *t  = sym->getType();
  TypeArray *ta = (TypeArray*)t;
  assert(t->getTypeKind()==TYPE_ARRAY);
  int width;
  if (!isConstWidth(ta->getElemType(),&width))		// - (sets width)
    assert(!"-everilog found array variable with non-const width");
  int nelems;
  if (!isConstNelems(ta,&nelems))			// - (sets nelems)
    assert(!"-everilog found array variable with non-const number of elements");
  int addrwidth = (nelems>1) ? countBits(nelems-1) : 1;
  string addr = info->rom_addr[sym];
  string data = info->rom_data[sym];

  // - always block for ROM, head
  ret += indent + "always @*  begin\n";
  indent += "  ";

  // - ROM values:  case stmt head
  ret += indent + "case (" + addr + ")\n";
  indent += "  ";

  // - ROM values:  case stmt cases
  list<Expr*> noValues;
  list<Expr*>  *values = arrayValue ? arrayValue->getExprs() : &noValues;
  Expr *e_val;
  int i=0;
  forall (e_val, *values) {
    if (e_val->getExprKind()!=EXPR_VALUE)
      fatal(1,"-everilog cannot handle non-constant initialization " +
	      e_val->toString() + " for array " + sym->toString() +
	      string(" position %d",i),
	    e_val->getToken());
    list<string> tempDefs;
    string s_val = tdfToVerilog_expr_toString(op,info,e_val,&tempDefs,indent);
    // NOTE:  each s_val is printed with minimum width, not width of array
    assert(tempDefs.size()==0);
    ret += indent + string("%d'd%d : ",addrwidth,i)
		  + data + " = " + s_val + " ;\n";
    i++;
  }
  ret += indent + "default : " + data + string(" = %d'bx ;\n",width);
   
  // - ROM values:  case stmt end
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endcase  // case (" + info->state_reg + ")\n";

  // - always block for ROM, end
  indent = indent(0,indent.length()-1-2);
  ret += indent + "end  // always @*\n";

  return ret;
}


string tdfToVerilog_dp_toString (OperatorBehavioral *op, EVerilogInfo *info)
{
  string ret;
  string indent;

  // - module declaration
  ret += indent + "module " + op->getName() + "_dp (";
  ret += tdfToVerilog_dp_args_toString(op,info);
  ret += ");\n";
  indent += "  ";

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_dp_argTypes_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_fsm_stateParams_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_dp_regs_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_dp_assign_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_dp_alwaysSeq_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  ret += tdfToVerilog_dp_alwaysCombi_toString(op,info,indent);

  addEmptyLine(&ret);		// ----------------

  Symbol *rom_sym;
  forall_defined (rom_sym, info->rom_addr) {
    ret += tdfToVerilog_dp_alwaysROM_toString(op,info,rom_sym,indent);
    addEmptyLine(&ret);		// ----------------
  }

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret += indent + "endmodule  // " + op->getName() + "_dp\n";

  return ret;
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_toFile  (emit to files)

void tdfToVerilog_blackbox_toFile (Operator *op)
{
  // - emit Verilog black box module for *op to file <opname>.v
  // - black box Verilog specifies inputs and outputs but no internal code

  EVerilogInfo info;
  op->map(tdfToVerilog_scanTdf_map, (TreeMap)NULL, (void*)&info);

  string fileName_blackbox = op->getName() + ".v";

  ofstream fout(fileName_blackbox);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_blackbox);

  // - comment
  string comment = "// Verilog black box module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - Verilog module
  fout << tdfToVerilog_blackbox_toString(op,&info);

  fout.close();
}


void tdfToVerilog_fsm_toFile (OperatorBehavioral *op, EVerilogInfo *info)
{
  // - emit Verilog FSM module for *op to file <opname>_fsm.v

  string fileName_fsm = op->getName() + "_fsm.v";

  ofstream fout(fileName_fsm);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_fsm);

  // - comment
  string comment = "// Verilog FSM module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - Verilog module
  fout << tdfToVerilog_fsm_toString(op,info);

  fout.close();
}


void tdfToVerilog_dp_toFile (OperatorBehavioral *op, EVerilogInfo *info)
{
  // - emit Verilog data-path module for *op to file <opname>_fsm.v

  string fileName_dp = op->getName() + "_dp.v";

  ofstream fout(fileName_dp);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_dp);

  // - comment
  string comment = "// Verilog data-path module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - Verilog module
  fout << tdfToVerilog_dp_toString(op,info);

  fout.close();
}


void tdfToVerilog_fsm_dp_toFile (OperatorBehavioral *op, EVerilogInfo *info)
{
  // - emit Verilog top level (FSM + DP) for *op to file <opname>.v

  string fileName_top = op->getName() + ".v";
  string fileName_fsm = op->getName() + "_fsm.v";
  string fileName_dp  = op->getName() + "_dp.v";

  ofstream fout(fileName_top);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_top);

  // - comment
  string comment = "// Verilog top module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;

  // - includes
// Removed includes for keeping XST happy - 3rd September 2011 - Nachiket 
//  fout << "`include \"" + fileName_fsm + "\"\n";
//  fout << "`include \"" + fileName_dp  + "\"\n\n";

  // - Verilog module
  fout << tdfToVerilog_fsm_dp_toString(op,info);

  fout.close();
}


// 3rd September 2011: Nachiket
void tdfToUCF(OperatorBehavioral *op, EVerilogInfo *info) 
{
  // - emit UCF constraints for file

  string fileName_ucf = op->getName() + ".ucf";
  
  ofstream fout(fileName_ucf);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_ucf);
  
  fout << "NET \"clock\" PERIOD=2.5ns;" << endl;
  fout << "INST \"" << op->getName() << "\" AREA_GROUP = \"pblock_1\";" << endl;
  fout << "AREA_GROUP \"pblock_1\" RANGE=SLICE_X0Y0:SLICE_X?Y?;" << endl;

  fout.close();
}

void tdfToVerilog_toFile (OperatorBehavioral *op)
{
  // - emit Verilog for behavioral *op to ".v" files
  // - creates files in current working directory:
  //     <op>.v  <op>_fsm.v  <op>_dp.v

  EVerilogInfo info;
  tdfToVerilog_scanTdf(op,&info);

  tdfToVerilog_fsm_toFile   (op,&info);
  tdfToVerilog_dp_toFile    (op,&info);
  tdfToVerilog_fsm_dp_toFile(op,&info);

  // 3rd September 2011: Nachiket writing out dummy best-guess UCF constraints
  tdfToUCF(op, &info);
}

void tdfToVerilog_seg_toFile (Operator *op)
{
  // - emit Verilog for behavioral *op to ".v" files
  // - creates files in current working directory:
  //     <op>.v  <op>_fsm.v  <op>_dp.v

  EVerilogInfo info;
  op->map(tdfToVerilog_scanTdf_map, (TreeMap)NULL, (void*)&info);
  
  string fileName_blackbox = op->getName() + ".v";

  ofstream fout(fileName_blackbox);
  if (!fout)
    fatal(1,"-everilog could not open output file "+fileName_blackbox);

  // - emit Verilog wrapper module for *op
  // - wrapper Verilog specifies inputs and outputs binding 
  //   but no internal code

  string ret;
  string indent;
  
  string comment = "// Verilog top (segment) module for " + op->getName() + "\n"
                 + "// " + tdfcComment() + "\n";
  fout << comment;


  // - segrw module declaration
  ret = indent + "module " + op->getName() + " ("
      +  CLOCK_NAME + ", " + RESET_NAME + ", ";
  fout << ret;

  fout << endl;

  // - segrw module declaration:  stream I/O
  // - HACK:  cast op into behavioral op
  fout << tdfToVerilog_fsm_dp_args_toString((OperatorBehavioral*)op,&info);

  // -  segrw module declaration:  finish
  fout   << ")  ;\n";
  indent += "  ";

  fout << endl;

  // - segrw module arg types:  clock, reset, stream I/O
  // - HACK:  cast op into behavioral op
  fout << tdfToVerilog_fsm_dp_argTypes_toString((OperatorBehavioral*)op,&info,
					       indent);

  fout << endl;

  // TODO: Add instantiation of SEG_<type>..
  string inner_name="";
  if(((OperatorSegment *)op)->getSegmentKind()==SEGMENT_RW) {
  	inner_name="SEG_rw ";
  } else if(((OperatorSegment*)op)->getSegmentKind()==SEGMENT_R) {
  	inner_name="SEG_r ";
  
  } else if(((OperatorSegment*)op)->getSegmentKind()==SEGMENT_SEQ_R) {
  	inner_name="SEG_seq_r ";
  	
  } else if(((OperatorSegment*)op)->getSegmentKind()==SEGMENT_SEQ_CYCLIC_R) {
  	inner_name="SEG_seq_cyclic_r ";
}
  fout << indent + inner_name;
  fout << tdfToVerilog_fsm_dp_params_toString((OperatorBehavioral*)op,&info);
  
  fout << "(" << CLOCK_NAME << ", " << RESET_NAME << ", ";
  fout << tdfToVerilog_fsm_dp_args_toString((OperatorBehavioral*)op,&info);	
  fout << indent << ");";
  
  fout << endl;

  // - finish
  indent = indent(0,indent.length()-1-2);
  ret = indent + "endmodule  // " + op->getName() + "\n";
  fout << ret;
  fout << endl;

  fout.close();
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog_toCerr  (emit to cerr)

void tdfToVerilog_toCerr (OperatorBehavioral *op)
{
  // - emit Verilog for *op to cerr, for debugging

  cerr << "// -everilog " << op->getName() << '\n';

  EVerilogInfo info;
  tdfToVerilog_scanTdf(op,&info);

  // cerr << printEVerilogInfo(&info);

  cerr << "\n";
  cerr << tdfToVerilog_fsm_toString   (op,&info);
  cerr << "\n";
  cerr << tdfToVerilog_dp_toString    (op,&info);
  cerr << "\n";
  cerr << tdfToVerilog_fsm_dp_toString(op,&info);
}


////////////////////////////////////////////////////////////////
//  tdfToVerilog

void tdfToVerilog (OperatorBehavioral *op)
{
  // - emit Verilog for behavioral operator *op
  warn("Emitting Verilog for behavioral " + op->getName());
  tdfToVerilog_toFile(op);
}


////////////////////////////////////////////////////////////////

void tdfToVerilog_instance_OLD (Operator *iop,
				list<OperatorBehavioral*> *instances)
{
  // - emit individual behavioral ops only
  if (iop->getOpKind()==OP_COMPOSE) {
    OperatorBehavioral *bop;
    forall (bop, *instances)
      tdfToVerilog(bop);
  }
  else {
    // - iop is single behavioral operator
    tdfToVerilog((OperatorBehavioral*)iop);
  }
}


void tdfToVerilog_instance (Operator *iop,
			    list<OperatorBehavioral*> *instances)
{
  if (iop->getOpKind()==OP_COMPOSE) {
    set_values(iop,true);				// - bind vals
    resolve_bound_values(&iop);
    tdfToVerilog_compose((OperatorCompose*)iop);    // - emit page + contents
  } else {
    set_values(iop,true);				// - bind vals
    resolve_bound_values(&iop);
    tdfToVerilog((OperatorBehavioral*)iop);	    // - emit indiv behav op
  }
}
