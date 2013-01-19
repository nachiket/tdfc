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
// SCORE TDF compiler:  Verilog bubbling FIFO queue "Q_fifo_bubl"
// $Revision: 1.2 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_Q_FIFO_BUBL_
#define _TDF_Q_FIFO_BUBL_


const char* Q_fifo_bubl = \n\
// Q_fifo_bubl.v\n\
//\n\
//  - FIFO with back-pressure and bubbling,\n\
//      with parameterizable depth, bit width\n\
//  - Stream I/O is triple (data, valid, back-pressure),\n\
//      with EOS concatenated into the data\n\
//  - Queue storage is shift-chain of independently enabled data/valid regs\n\
//  - Data/valid pair bubbles from input towards output\n\
//      until it reaches a back-pressure stalled stage\n\
//  - Flow control for input & output is\n\
//      * combinationally decoupled for valid  (v bubbles down w/data)\n\
//      * combinationally   coupled for back-pressure  (cascade of AND gates)\n\
//  - 1 <= depth <= 256\n\
//      * (depth <= 256) can be modified\n\
//           by changing ''// synthesis loop_limit X'' below\n\
//          and changing ''addrwidth'' or its log computation\n\
//      * depth  = 1    is implemented in Q_fifo_bubl_1  (scalar reg syntax)\n\
//      * depth >= 2    is implemented in Q_fifo_bubl_2  (array  reg syntax)\n\
//      * general depth is implemented in Q_fifo_bubl    (chooses _1 or _2)\n\
//  - 1 <= width\n\
//\n\
//  - Synplify 7.1\n\
//  - Eylon Caspi,  10/22/03\n\
\n\
\n\
`ifdef  Q_fifo_bubl\n\
`else\n\
`define Q_fifo_bubl\n\
\n\
\n\
module Q_fifo_bubl_2 (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);\n\
\n\
   parameter depth = 16;    // - greatest #items in queue  (2 <= depth <= 256)\n\
   parameter width = 16;    // - width of data (i_d, o_d)\n\
\n\
   parameter addrwidth = 8;\n\
\n\
   /*   \n\
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
   */\n\
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
   reg 	  signed [addrwidth:0] i_, j_, k_, l_;		// - stage indexes\n\
   reg			       bp_ [depth-1:0];		// - stage backpressure\n\
   reg 			       en_ [depth-1:0];		// - stage enable = ~bp\n\
   reg 			       v   [depth-1:0];		// - stage valid bit\n\
   reg    [width-1:0]	       d   [depth-1:0];		// - stage data\n\
\n\
   assign o_d = d[depth-1];			// - output data,  last  stage\n\
   assign o_v = v[depth-1];			// - output valid, last  stage\n\
   assign i_b = bp_[0];				// - input  bp,    first stage\n\
\n\
   always @(posedge clock or negedge reset) begin	// - seq always: v\n\
      if (!reset) begin\n\
	 // synthesis loop_limit 256\n\
	 for (i_=depth-1; i_>=0; i_=i_-1) begin\n\
	    v[i_] <= 0;\n\
	 end\n\
      end\n\
      else begin\n\
	 // synthesis loop_limit 256\n\
	 for (i_=depth-1; i_>0; i_=i_-1) begin\n\
	    if (en_[i_]) begin\n\
	       v[i_] <= v[i_-1];\n\
	    end\n\
	 end\n\
	 if (en_[0]) begin\n\
	    v[0] <= i_v;\n\
	 end\n\
      end\n\
   end // always @ (posedge clock or negedge reset)\n\
\n\
   always @(posedge clock) begin			// - seq always: d\n\
      // - no reset for d[] -- undefined on reset\n\
      // synthesis loop_limit 256\n\
      for (j_=depth-1; j_>0; j_=j_-1) begin\n\
	 if (en_[j_]) begin\n\
	    d[j_] <= d[j_-1];\n\
	 end\n\
      end\n\
      if (en_[0]) begin\n\
	 d[0] <= i_d;\n\
      end\n\
   end // always @ (posedge clock)\n\
\n\
   always @* begin				// - combi always: bp_, en_\n\
      bp_[depth-1] <= o_b & v[depth-1];\n\
      // synthesis loop_limit 256\n\
      for (k_=depth-2; k_>=0; k_=k_-1) begin\n\
	 bp_[k_] <= bp_[k_+1] & v[k_];\n\
      end\n\
      // synthesis loop_limit 256\n\
      for (l_=depth-1; l_>=0; l_=l_-1) begin	// - en[] = ~bp[]\n\
	 en_[l_] <= ~bp_[l_];\n\
      end\n\
   end\n\
      \n\
endmodule // Q_fifo_bubl_2\n\
\n\
\n\
module Q_fifo_bubl_1 (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);\n\
\n\
// parameter depth = 1;     // - greatest #items in queue  (depth = 1)\n\
   parameter width = 16;    // - width of data (i_d, o_d)\n\
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
   reg			       bp_;			// - stage backpressure\n\
   reg 			       en_;			// - stage enable = ~bp\n\
   reg 			       v;			// - stage valid bit\n\
   reg    [width-1:0]	       d;			// - stage data\n\
\n\
   assign o_d = d;				// - output data,  last  stage\n\
   assign o_v = v;				// - output valid, last  stage\n\
   assign i_b = bp_;				// - input  bp,    first stage\n\
\n\
   always @(posedge clock or negedge reset) begin	// - seq always: v\n\
      if (!reset) begin\n\
	 v <= 0;\n\
      end\n\
      else begin\n\
	 if (en_) begin\n\
	    v <= i_v;\n\
	 end\n\
      end\n\
   end // always @ (posedge clock or negedge reset)\n\
\n\
   always @(posedge clock) begin			// - seq always: d\n\
      // - no reset for d -- undefined on reset\n\
      if (en_) begin\n\
	 d <= i_d;\n\
      end\n\
   end // always @ (posedge clock)\n\
\n\
   always @* begin				// - combi always: bp_, en_\n\
      bp_ <= o_b & v;\n\
      en_ <= ~bp_;\n\
   end\n\
      \n\
endmodule // Q_fifo_bubl_1\n\
\n\
\n\
// - Q_fifo_bubl := (depth==1) ? Q_fifo_bubl_1 : Q_fifo_bubl_2\n\
// - Q_fifo_bubl implementation below is a convoluted way to choose\n\
//     Q_fifo_bubl_1 or Q_fifo_bubl_2 based on whether depth is 1\n\
// - We instantiate BOTH Q's, multiplex their outputs on (depth==1),\n\
//     then let constant folding eliminate the unused Q\n\
\n\
\n\
module Q_fifo_bubl (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);\n\
\n\
   parameter depth = 1;     // - greatest #items in queue  (1 <= depth <= 256)\n\
   parameter width = 1;     // - width of data (i_d, o_d)\n\
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
   wire   [width-1:0] o_d_1, o_d_2;\n\
   wire 	      o_v_1, o_v_2;\n\
   wire 	      i_b_1, i_b_2;\n\
\n\
   assign o_d = (depth==1) ? o_d_1 : o_d_2;\n\
   assign o_v = (depth==1) ? o_v_1 : o_v_2;\n\
   assign i_b = (depth==1) ? i_b_1 : i_b_2;\n\
\n\
   parameter depth2 = (depth==1) ? 2 : depth;	// safe depth for Q_fifo_bubl_2\n\
   \n\
   Q_fifo_bubl_1 #(       width) q1(clock,reset,i_d,i_v,i_b_1,o_d_1,o_v_1,o_b);\n\
   Q_fifo_bubl_2 #(depth2,width) q2(clock,reset,i_d,i_v,i_b_2,o_d_2,o_v_2,o_b);\n\
\n\
endmodule // Q_fifo_bubl\n\
\n\
\n\
`endif  // `ifdef  Q_fifo_bubl\n\
";


#endif	// #ifndef _TDF_Q_FIFO_BUBL_
