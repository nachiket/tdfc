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
// SCORE TDF compiler:  Verilog SRL queue w/reserve capacity "Q_srl_reserve"
// $Revision: 1.3 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_Q_SRL_RESERVE_
#define _TDF_Q_SRL_RESERVE_


const char* Q_srl_reserve = "\n\
// Q_srl_reserve_oreg3_prefull_preresv_SIMPLE.v\n\
//\n\
//  - In-page queue with parameterizable depth, bit width, reserve capacity\n\
//  - Stream I/O is triple (data, valid, back-pressure),\n\
//      with EOS concatenated into the data\n\
//  - Flow control for input & output is combinationally decoupled\n\
//  - Emit input back-pressure if number of items remaining empty\n\
//      reaches or falls below reserve capacity,\n\
//      but continue consuming  (useful for pipelined input)\n\
//  - 2 <= depth <= 256\n\
//      * (depth >= 2)  is required to decouple I/O flow control,\n\
//          where empty => no produce,  full => no consume,\n\
//          and depth 1 would ping-pong between the two at half rate\n\
//      * (depth <= 256) can be modified\n\
//           by changing ''synthesis loop_limit X'' below\n\
//          and changing ''addrwidth'' or its log computation\n\
//  - 1 <= width\n\
//  - 0 <= reserve <= depth-1\n\
//      * (reserve == 0)       is equivalent to no reserve\n\
//      * (reserve <= depth-1) is required because reserve=depth would have\n\
//                             back-pressure on all the time,\n\
//                             low bp only from initial value of bp pipe regs,\n\
//                             and hence not full throughput (bp hickups)\n\
//  - Queue storage is in SRL16E, up to depth 16 per LUT per bit-slice,\n\
//      plus output register (for fast output)\n\
//  - Queue addressing is done by ''addr'' up-down counter\n\
//  - Queue fullness is checked by comparator (addr==depth)\n\
//  - Queue fullness and reserve mode         are pre-computed for next cycle\n\
//  - Queue input back-pressure                is pre-computed for next cycle\n\
//  - Queue output valid (state!=state__empty) is pre-computed for next cycle\n\
//      (necessary since SRL data output reg requires non-boolean state)\n\
//  - FSM has 3 states (empty, one, more)\n\
//  - When empty, continue to emit most recently emitted value (for debugging)\n\
//\n\
//  - Queue slots used      = / (state==state_empty) ? 0\n\
//                            | (state==state_one)   ? 1\n\
//                            \\ (state==state_more)  ? addr+2\n\
//  - Queue slots used     <=  depth\n\
//  - Queue slots remaining =  depth - used\n\
//                          = / (state==state_empty) ? depth\n\
//                            | (state==state_one)   ? depth-1\n\
//                            \\ (state==state_more)  ? depth-2-addr\n\
//  - raise back-pressure (but continue consuming) whenever remaining<=reserve\n\
//          i.e. whenever (reserve==depth-1) ? (state!=state_empty) :\n\
//                        (state==state_more  && addr>=depth-2-reserve)\n\
//\n\
//\n\
//  - Synplify 7.1 / 8.0\n\
//  - Eylon Caspi,  9/11/03, 8/18/04, 3/29/05\n\
\n\
\n\
`ifdef  Q_srl_reserve\n\
`else\n\
`define Q_srl_reserve\n\
\n\
\n\
module Q_srl_reserve (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);\n\
\n\
   parameter depth   = 16;   // - greatest #items in queue  (2 <= depth <= 256)\n\
   parameter width   = 16;   // - width of data (i_d, o_d)\n\
   parameter reserve =  1;   // - remaining capacity at/below which emit bp\n\
   \n\
   `define LOG2 (  (((depth))     ==0) ? 0	/* - depth==0   LOG2=0 */ \n\
		 : (((depth-1)>>0)==0) ? 0	/* - depth<=1   LOG2=0 */ \n\
		 : (((depth-1)>>1)==0) ? 1	/* - depth<=2   LOG2=1 */ \n\
		 : (((depth-1)>>2)==0) ? 2	/* - depth<=4   LOG2=2 */ \n\
		 : (((depth-1)>>3)==0) ? 3	/* - depth<=8   LOG2=3 */ \n\
		 : (((depth-1)>>4)==0) ? 4	/* - depth<=16  LOG2=4 */ \n\
		 : (((depth-1)>>5)==0) ? 5	/* - depth<=32  LOG2=5 */ \n\
		 : (((depth-1)>>6)==0) ? 6	/* - depth<=64  LOG2=6 */ \n\
		 : (((depth-1)>>7)==0) ? 7	/* - depth<=128 LOG2=7 */ \n\
		 :                       8)	/* - depth<=256 LOG2=8 */   \n\
\n\
// parameter addrwidth = LOG2;			// - width of queue addr\n\
\n\
   parameter addrwidth =\n\
		(  (((depth))     ==0) ? 0	// - depth==0   LOG2=0\n\
		 : (((depth-1)>>0)==0) ? 0	// - depth<=1   LOG2=0\n\
		 : (((depth-1)>>1)==0) ? 1	// - depth<=2   LOG2=1\n\
		 : (((depth-1)>>2)==0) ? 2	// - depth<=4   LOG2=2\n\
		 : (((depth-1)>>3)==0) ? 3	// - depth<=8   LOG2=3\n\
		 : (((depth-1)>>4)==0) ? 4	// - depth<=16  LOG2=4\n\
		 : (((depth-1)>>5)==0) ? 5	// - depth<=32  LOG2=5\n\
		 : (((depth-1)>>6)==0) ? 6	// - depth<=64  LOG2=6\n\
		 : (((depth-1)>>7)==0) ? 7	// - depth<=128 LOG2=7\n\
		 :                       8)	// - depth<=256 LOG2=8\n\
		 ;\n\
\n\
   input     clock;\n\
   input     reset;\n\
   \n\
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)\n\
   input              i_v;	// - input  stream valid\n\
   output             i_b;	// - input  stream back-pressure\n\
\n\
   output [width-1:0] o_d;	// - output stream data (concat data + eos)\n\
   output             o_v;	// - output stream valid\n\
   input              o_b;	// - output stream back-pressure\n\
\n\
   reg    [addrwidth-1:0] addr, addr_, a_;		// - SRL16 address\n\
							//     for data output\n\
   reg 			  shift_en_;			// - SRL16 shift enable\n\
   reg    [width-1:0] 	  srl [depth-2:0];		// - SRL16 memory\n\
   reg 			  shift_en_o_;			// - SRLO  shift enable\n\
   reg    [width-1:0] 	  srlo_, srlo			// - SRLO  output reg\n\
			  /* synthesis syn_allow_retiming=0 */ ;\n\
\n\
   parameter state_empty = 2'd0;    // - state empty : o_v=0 o_d=UNDEFINED\n\
   parameter state_one   = 2'd1;    // - state one   : o_v=1 o_d=srlo\n\
   parameter state_more  = 2'd2;    // - state more  : o_v=1 o_d=srlo\n\
				    //     #items in srl = addr+2\n\
\n\
   reg [1:0] state, state_;	    // - state register\n\
\n\
   reg       addr_full_;	    // - true iff addr==depth-2 on NEXT cycle\n\
   reg       addr_full; 	    // - true iff addr==depth-2\n\
   wire      addr_zero_;	    // - true iff addr==0\n\
   wire      addr_resv_;	    // - true iff ...           on NEXT cycle\n\
   reg       addr_resv; 	    // - true iff addr>=depth-2-reserve &&\n\
				    //            state==state_more  (depth>2)\n\
   wire      o_v_reg_;		    // - true iff state_empty   on NEXT cycle\n\
   reg       o_v_reg  		    // - true iff state_empty\n\
	     /* synthesis syn_allow_retiming=0 */ ;\n\
   wire      i_b_reg_;		    // - true iff !full/!resv   on NEXT cycle\n\
   reg       i_b_reg  		    // - true iff !full/!resv\n\
	     /* synthesis syn_allow_retiming=0 */ ;\n\
\n\
// assign addr_full_ = (state_==state_more) && (addr_==depth-2);\n\
						// - queue full\n\
   assign addr_zero_ = (addr==0);		// - queue contains 2 (or 1,0)\n\
   assign addr_resv_ = (reserve==depth-1) ?      (state_!=state_empty) :\n\
		       (addr_>=depth-2-reserve && state_==state_more);\n\
						// - queue remaining<=reserve\n\
   assign o_v_reg_   = (state_!=state_empty);	// - output valid if non-empty\n\
   assign i_b_reg_   = (reserve==0) ? addr_full_ : addr_resv_;\n\
						// - input bp if full/reserving\n\
   assign o_d = srlo;				// - output data from queue\n\
   assign o_v = o_v_reg;			// - output valid if non-empty\n\
   assign i_b = i_b_reg;			// - input bp if full/reserving\n\
\n\
   // - ''always'' block with both FFs and SRL16 does not work,\n\
   //      since FFs need reset but SRL16 does not\n\
\n\
   always @(posedge clock or negedge reset) begin	// - seq always: FFs\n\
      if (!reset) begin\n\
	 state     <= state_empty;\n\
	 addr      <= 0;\n\
         addr_full <= 0;\n\
	 addr_resv <= 0;\n\
	 o_v_reg   <= 0;\n\
	 i_b_reg   <= 1;\n\
      end\n\
      else begin\n\
	 state     <= state_;\n\
	 addr      <= addr_;\n\
         addr_full <= addr_full_;\n\
	 addr_resv <= addr_resv_;\n\
	 o_v_reg   <= o_v_reg_;\n\
	 i_b_reg   <= i_b_reg_;\n\
      end\n\
   end // always @ (posedge clock or negedge reset)\n\
\n\
   always @(posedge clock or negedge reset) begin	// - seq always: srlo\n\
      // - infer enabled output reg at end of shift chain\n\
      // - input first element from i_d, all subsequent elements from SRL16\n\
      if (!reset) begin\n\
	 srlo <= 0;\n\
      end\n\
      else begin\n\
	 if (shift_en_o_) begin\n\
	    srlo <= srlo_;\n\
	 end\n\
      end\n\
   end // always @ (posedge clock or negedge reset)\n\
\n\
   always @(posedge clock) begin			// - seq always: srl\n\
      // - infer enabled SRL16E from shifting srl array\n\
      // - no reset capability;  srl[] contents undefined on reset\n\
      if (shift_en_) begin\n\
	 // synthesis loop_limit 256\n\
	 for (a_=depth-2; a_>0; a_=a_-1) begin\n\
	    srl[a_] <= srl[a_-1];\n\
	 end\n\
	 srl[0] <= i_d;\n\
      end\n\
   end // always @ (posedge clock or negedge reset)\n\
\n\
   always @* begin					// - combi always\n\
      case (state)\n\
\n\
	state_empty: begin		    // - (empty, will not produce)\n\
	      if (i_v) begin		    // - empty & i_v => consume\n\
		 srlo_       <= i_d;\n\
		 shift_en_o_ <= 1;\n\
		 shift_en_   <= 1'bx;\n\
		 addr_       <= 0;\n\
		 addr_full_  <= 0;\n\
		 state_      <= state_one;\n\
	      end\n\
	      else	begin		    // - empty & !i_v => idle\n\
		 srlo_       <= 'bx;\n\
		 shift_en_o_ <= 0;\n\
		 shift_en_   <= 1'bx;\n\
		 addr_       <= 0;\n\
		 addr_full_  <= 0;\n\
		 state_      <= state_empty;\n\
	      end\n\
	end\n\
\n\
	state_one: begin		    // - (contains one)\n\
	      if (i_v && o_b) begin	    // - one & i_v & o_b => consume\n\
		 srlo_       <= 'bx;\n\
		 shift_en_o_ <= 0;\n\
		 shift_en_   <= 1;\n\
		 addr_       <= 0;\n\
		 addr_full_  <= (depth==2) ? 1 : 0;\n\
		 state_      <= state_more;\n\
	      end\n\
	      else if (i_v && !o_b) begin   // - one & i_v & !o_b => cons+prod\n\
		 srlo_       <= i_d;\n\
		 shift_en_o_ <= 1;\n\
		 shift_en_   <= 1;\n\
		 addr_       <= 0;\n\
		 addr_full_  <= 0;\n\
		 state_      <= state_one;\n\
	      end\n\
	      else if (!i_v && o_b) begin   // - one & !i_v & o_b => idle\n\
		 srlo_       <= 'bx;\n\
		 shift_en_o_ <= 0;\n\
		 shift_en_   <= 1'bx;\n\
		 addr_       <= 0;\n\
		 addr_full_  <= 0;\n\
		 state_      <= state_one;\n\
	      end\n\
	      else if (!i_v && !o_b) begin  // - one & !i_v & !o_b => produce\n\
		 srlo_       <= 'bx;\n\
		 shift_en_o_ <= 0;\n\
		 shift_en_   <= 1'bx;\n\
		 addr_       <= 0;\n\
		 addr_full_  <= 0;\n\
		 state_      <= state_empty;\n\
	      end\n\
	end // case: state_one\n\
	\n\
	state_more: begin		    // - (contains more than one)\n\
	   if (addr_full || (depth==2)) begin\n\
					    // - (full, will not consume) \n\
					    // - (full here if depth==2)\n\
	      if (o_b) begin		    // - full & o_b => idle\n\
		 srlo_       <= 'bx;\n\
		 shift_en_o_ <= 0;\n\
		 shift_en_   <= 0;\n\
		 addr_       <= addr;\n\
		 addr_full_  <= 1;\n\
		 state_      <= state_more;\n\
	      end\n\
	      else begin		    // - full & !o_b => produce\n\
		 srlo_       <= srl[addr];\n\
		 shift_en_o_ <= 1;\n\
		 shift_en_   <= 0;\n\
//		 addr_       <= addr-1;\n\
//		 state_      <= state_more;\n\
		 addr_       <= addr_zero_ ? 0         : addr-1;\n\
		 addr_full_  <= 0;\n\
		 state_      <= addr_zero_ ? state_one : state_more;\n\
	      end\n\
	   end\n\
	   else begin			    // - (mid: neither empty nor full)\n\
	      if (i_v && o_b) begin	    // - mid & i_v & o_b => consume\n\
		 srlo_       <= 'bx;\n\
		 shift_en_o_ <= 0;\n\
		 shift_en_   <= 1;\n\
		 addr_       <= addr+1;\n\
		 addr_full_  <= (addr==depth-3);\n\
		 state_      <= state_more;\n\
	      end\n\
	      else if (i_v && !o_b) begin   // - mid & i_v & !o_b => cons+prod\n\
		 srlo_       <= srl[addr];\n\
		 shift_en_o_ <= 1;\n\
		 shift_en_   <= 1;\n\
		 addr_       <= addr;\n\
		 addr_full_  <= 0;\n\
		 state_      <= state_more;\n\
	      end\n\
	      else if (!i_v && o_b) begin   // - mid & !i_v & o_b => idle\n\
		 srlo_       <= 'bx;\n\
		 shift_en_o_ <= 0;\n\
		 shift_en_   <= 0;\n\
		 addr_       <= addr;\n\
		 addr_full_  <= 0;\n\
		 state_      <= state_more;\n\
	      end\n\
	      else if (!i_v && !o_b) begin  // - mid & !i_v & !o_b => produce\n\
		 srlo_       <= srl[addr];\n\
		 shift_en_o_ <= 1;\n\
		 shift_en_   <= 0;\n\
		 addr_       <= addr_zero_ ? 0         : addr-1;\n\
		 addr_full_  <= 0;\n\
		 state_      <= addr_zero_ ? state_one : state_more;\n\
	      end\n\
	   end // else: !if(addr_full)\n\
	end // case: state_more\n\
\n\
	default: begin\n\
		 srlo_       <=  'bx;\n\
		 shift_en_o_ <= 1'bx;\n\
		 shift_en_   <= 1'bx;\n\
		 addr_       <=  'bx;\n\
		 addr_full_  <= 1'bx;\n\
		 state_      <= 2'bx;\n\
	end // case: default\n\
\n\
      endcase // case(state)\n\
   end // always @ *\n\
   \n\
endmodule // Q_srl_reserve\n\
\n\
\n\
`endif  // `ifdef  Q_srl_reserve\n\
";


#endif	// #ifndef _TDF_Q_SRL_RESERVE_
