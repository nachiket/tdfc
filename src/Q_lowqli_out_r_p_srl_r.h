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
// SCORE TDF compiler:  Verilog stream queue "Q_lowqli_out_r_p_srl_r"
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_Q_LOWQLI_OUT_R_P_SRL_R_
#define _TDF_Q_LOWQLI_OUT_R_P_SRL_R_


const char* Q_lowqli_out_r_p_srl_r = "\n\
// - Q_lowqli_out_r_p_srl_r.v\n\
//\n\
//  - Output side (first half) of...\n\
//  - Stream with parameterizable bit width, consisting of\n\
//      (1) logic output-side relaying   of parameterizable depth,\n\
//      (2) interconnect      pipelining of parameterizable depth,\n\
//      (3) SRL queue with reserve       of parameterizable depth,\n\
//      (4) logic input-side  relaying   of parameterizable depth\n\
//  -     [ Q_fifo_bubl --> Q_pipe_noretime ]\n\
//    of: [ Q_fifo_bubl --> Q_pipe_noretime --> Q_srl_reserve --> Q_fifo_bubl ]\n\
//  - Stream I/O is triple (data, valid, back-pressure),\n\
//      with EOS concatenated into the data\n\
//  - Flow control for input & output is combinationally decoupled\n\
//  - Emit input back-pressure early to manage queue reserve capacity\n\
//      in the presence of pipelined (i.e. stale) flow control,\n\
//      (queue reserve capacity) = 2 * (interconnect pipe depth)\n\
//  - 2 <= depth <= 256\n\
//      * (depth >= 2)  is required to decouple I/O flow control,\n\
//          where empty => no produce,  full => no consume,\n\
//          and depth 1 would ping-pong between the two at half rate\n\
//      * (depth <= 256) can be modified [in Q_srl_reserve.v]\n\
//           by changing ''synthesis loop_limit X'' below\n\
//          and changing ''addrwidth'' or its log computation\n\
//  - 1 <= width\n\
//  - 0 <= lipipe    (logic  input-side pipelining depth)\n\
//  - 0 <= lopipe    (logic output-side pipelining depth)\n\
//  - 0 <=  wpipe    (interconnect/wire pipelining depth)\n\
//  - depth >= (2*wpipe) + 2\n\
//      * required because pipelining allocates queue reserve capacity,\n\
//          which must be strictly less than queue total capacity\n\
//      * depth = ... + 1 would be half throughput, oscillate back-pressure\n\
//  - Queue storage is in SRL16E, up to depth 16 per LUT per bit-slice\n\
//  - When empty, continue to emit most recently emitted value (for debugging)\n\
//\n\
//  - Synplify 7.1 / 8.0\n\
//  - Eylon Caspi, 4/9/05\n\
\n\
\n\
`ifdef  Q_lowqli_out\n\
`else\n\
`define Q_lowqli_out\n\
\n\
\n\
`include \"Q_pipe_noretime.v\"\n\
`include \"Q_srl_reserve.v\"\n\
`include \"Q_fifo_bubl.v\"\n\
\n\
module Q_lowqli_out (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);\n\
\n\
   parameter  depth  = 16;   // - greatest #items in queue  (2 <= depth <= 256)\n\
   parameter  width  = 16;   // - width of data (i_d, o_d)\n\
   parameter lopipe  =  0;   // - logic output-side pipelining depth\n\
   parameter  wpipe  =  0;   // - interconnect      pipelining depth\n\
   parameter lipipe  =  0;   // - logic  input-side pipelining depth\n\
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
   parameter           d0 = ( depth==0);\n\
   parameter          lo0 = (lopipe==0);\n\
   parameter           w0 = ( wpipe==0);\n\
   parameter          li0 = (lipipe==0);\n\
\n\
   parameter           depth_ = ( d0 ? 2 :  depth);	// - ``safe''  depth\n\
   parameter          lopipe_ = (lo0 ? 1 : lopipe);	// - ``safe'' lopipe\n\
   parameter           wpipe_ = ( w0 ? 1 :  wpipe);	// - ``safe''  wpipe\n\
   parameter          lipipe_ = (li0 ? 1 : lipipe);	// - ``safe'' lipipe\n\
\n\
   // - the following ``wire'' stmts are I/O for pipe + q components\n\
   // - *i_[dvb] refers to the side closer to the input  stream\n\
   // - *o_[dvb] refers to the side closer to the output stream\n\
\n\
   wire [width-1:0]   loi_d, loo_d, wi_d, wo_d, qi_d, qo_d, lii_d, lio_d;\n\
   wire               loi_v, loo_v, wi_v, wo_v, qi_v, qo_v, lii_v, lio_v;\n\
   wire               loi_b, loo_b, wi_b, wo_b, qi_b, qo_b, lii_b, lio_b;\n\
\n\
   // - the following ``wire'' stmts connect or bypass components of depth 0\n\
   //\n\
   //     i_d --[lopipe]-> l_d --[wpipe]-> w_d --[Q]-> q_d --[lipipe]-> o_d\n\
   //     i_v --[      ]-> l_v --[     ]-> w_v --[ ]-> q_v --[      ]-> o_v\n\
   //     i_b <-[      ]-- l_b <-[     ]-- w_b <-[ ]-- q_b <-[      ]-- o_b\n\
\n\
   wire [width-1:0]   l_d, w_d, q_d;\n\
   wire               l_v, w_v, q_v;\n\
   wire               l_b, w_b, q_b;\n\
\n\
   // - logic output-side relaying (lopipe)\n\
   assign             loi_d = i_d ;\n\
   assign             loi_v = i_v ;\n\
   assign             loo_b = l_b ;\n\
   assign             l_d = lo0 ? loi_d : loo_d ;\n\
   assign             l_v = lo0 ? loi_v : loo_v ;\n\
   assign             i_b = lo0 ? loo_b : loi_b ;\n\
   Q_fifo_bubl #(lopipe_,width) q_lopipe (clock,reset,loi_d,loi_v,loi_b,loo_d,loo_v,loo_b);\n\
\n\
   // - interconnect pipelining (wpipe)\n\
   assign             wi_d = l_d ;\n\
   assign             wi_v = l_v ;\n\
   assign             wo_b = w_b ;\n\
   assign             w_d =  w0 ? wi_d : wo_d ;\n\
   assign             w_v =  w0 ? wi_v : wo_v ;\n\
   assign             l_b =  w0 ? wo_b : wi_b ;\n\
   Q_pipe_noretime #(wpipe_,width) q_wpipe (clock,reset,wi_d,wi_v,wi_b,wo_d,wo_v,wo_b);\n\
\n\
   // - queue\n\
   assign             qi_d = w_d ;\n\
   assign             qi_v = w_v ;\n\
   assign             qo_b = q_b ;\n\
   assign             q_d =  /* d0 */ 1 ? qi_d : qo_d ;\n\
   assign             q_v =  /* d0 */ 1 ? qi_v : qo_v ;\n\
   assign             w_b =  /* d0 */ 1 ? qo_b : qi_b ;\n\
   // Q_srl_reserve #(depth_,width,wpipe*2) q (clock,reset,qi_d,qi_v,qi_b,qo_d,qo_v,qo_b);\n\
\n\
   // logic input-side pipelining (lipipe)\n\
   assign             lii_d = q_d ;\n\
   assign             lii_v = q_v ;\n\
   assign             lio_b = o_b ;\n\
   assign             o_d = /* li0 */ 1 ? lii_d : lio_d ;\n\
   assign             o_v = /* li0 */ 1 ? lii_v : lio_v ;\n\
   assign             q_b = /* li0 */ 1 ? lio_b : lii_b ;\n\
   // Q_fifo_bubl #(lipipe_,width) q_lipipe (clock,reset,lii_d,lii_v,lii_b,lio_d,lio_v,lio_b);\n\
\n\
endmodule  // Q_lowqli_out\n\
\n\
\n\
`endif  // `ifdef  Q_lowqli_out\n\
";


#endif	// #ifndef _TDF_Q_LOWQLI_OUT_R_P_SRL_R_
