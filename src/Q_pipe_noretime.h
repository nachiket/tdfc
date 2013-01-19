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
// SCORE TDF compiler:  Verilog pipelined interconnct queue "Q_pipe_noretime.h"
// $Revision: 1.2 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_Q_PIPE_NORETIME_
#define _TDF_Q_PIPE_NORETIME_


const char* Q_pipe_noretime = \n\
// Q_pipe_noretime.v\n\
//\n\
//  - Pipelined interconnect stage(s),\n\
//      registering data/valid    in the forward  direction,\n\
//      registering back-pressure in the backward direction,\n\
//      with parameterizable depth, bit width\n\
//        d --> d --> d --> d\n\
//        v --> v --> v --> v\n\
//        b <-- b <-- b <-- b\n\
//  - Registers are NOT retimable\n\
//  - Stream I/O is triple (data, valid, back-pressure),\n\
//      with EOS concatenated into the data\n\
//  - Initialization values:  valid         all 0  (invalid data),\n\
//                            back-pressure all 1  (do not send),\n\
//                            data          all undefined\n\
//  - Back-pressure initialization to 1 is in case network had been\n\
//      drained s.t. downstream queue is already full on startup\n\
//  - 1 <= depth <= 256\n\
//      * (depth <= 256) can be modified\n\
//           by changing ''// synthesis loop_limit X'' below\n\
//          and changing ''addrwidth'' or its log computation\n\
//  - 1 <= width\n\
//\n\
//  - Synplify 7.1\n\
//  - Eylon Caspi,  10/27/03\n\
\n\
\n\
`ifdef  Q_pipe_noretime\n\
`else\n\
`define Q_pipe_noretime\n\
\n\
\n\
module Q_pipe_noretime (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);\n\
\n\
   parameter depth =  1;    // - greatest #items in queue  (1 <= depth <= 256)\n\
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
   reg 	  signed [addrwidth:0] i_, j_, k_;	// - stage indexes\n\
   reg			       b [depth-1:0]	// - stage backpressure\n\
			         /* synthesis syn_srlstyle=\"registers\" */\n\
			         /* synthesis syn_allow_retiming=0     */ ;\n\
   reg 			       v [depth-1:0]	// - stage valid bit\n\
			         /* synthesis syn_srlstyle=\"registers\" */\n\
			         /* synthesis syn_allow_retiming=0     */ ;\n\
   reg           [width-1:0]   d [depth-1:0]	// - stage data\n\
			         /* synthesis syn_srlstyle=\"registers\" */\n\
			         /* synthesis syn_allow_retiming=0     */ ;\n\
\n\
   assign o_d = d[depth-1];			// - output data,  last  stage\n\
   assign o_v = v[depth-1];			// - output valid, last  stage\n\
   assign i_b = b[0];				// - input  bp,    first stage\n\
\n\
   always @(posedge clock or negedge reset) begin	// - seq always: v, b\n\
      if (!reset) begin\n\
	 // synthesis loop_limit 256\n\
	 for (i_=depth-1; i_>=0; i_=i_-1) begin\n\
	    v[i_] <= 0;\n\
	 end\n\
	 for (j_=0; j_<=depth-1; j_=j_+1) begin\n\
	    b[j_] <= 1;\n\
	 end\n\
      end\n\
      else begin\n\
	 // synthesis loop_limit 256\n\
	 for (i_=depth-1; i_>0; i_=i_-1) begin\n\
	    v[i_] <= v[i_-1];\n\
	 end\n\
	 v[0] <= i_v;\n\
	 // synthesis loop_limit 256\n\
	 for (j_=0; j_<depth-1; j_=j_+1) begin\n\
	    b[j_] <= b[j_+1];\n\
	 end\n\
	 b[depth-1] <= o_b;\n\
      end\n\
   end // always @ (posedge clock or negedge reset)\n\
\n\
   always @(posedge clock) begin			// - seq always: d\n\
      // - no reset for d[] or b[] -- undefined on reset\n\
      // synthesis loop_limit 256\n\
      for (k_=depth-1; k_>0; k_=k_-1) begin\n\
	 d[k_] <= d[k_-1];\n\
      end\n\
      d[0] <= i_d;\n\
   end // always @ (posedge clock)\n\
      \n\
endmodule // Q_pipe_noretime\n\
\n\
\n\
`endif  // `ifdef  Q_pipe_noretime\n\
";


#endif	// #ifndef _TDF_Q_PIPE_NORETIME_
