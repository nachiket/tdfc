// Verilog base queue Q_lowqli_in_p_p_srl_r.v
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

\n// - Q_lowqli_in_p_p_srl_r.v
//
//  - Input side (second half) of...
//  - Stream with parameterizable bit width, consisting of
//      (1) logic output-side pipelining of parameterizable depth,
//      (2) interconnect      pipelining of parameterizable depth,
//      (3) SRL queue with reserve       of parameterizable depth,
//      (4) logic input-side  relaying   of parameterizable depth
//  -                                        [ Q_srl_reserve --> Q_fifo_bubl ]
//    of: [ Q_fwd_pipe --> Q_pipe_noretime --> Q_srl_reserve --> Q_fifo_bubl ]
//  - Stream I/O is triple (data, valid, back-pressure),
//      with EOS concatenated into the data
//  - Flow control for input & output is combinationally decoupled
//  - Emit input back-pressure early to manage queue reserve capacity
//      in the presence of pipelined (i.e. stale) flow control,
//      (queue reserve capacity) =     (logic out pipe depth)
//                               + 2 * (interconnect pipe depth)
//  - 2 <= depth <= 256
//      * (depth >= 2)  is required to decouple I/O flow control,
//          where empty => no produce,  full => no consume,
//          and depth 1 would ping-pong between the two at half rate
//      * (depth <= 256) can be modified [in Q_srl_reserve.v]
//           by changing ''synthesis loop_limit X'' below
//          and changing ''addrwidth'' or its log computation
//  - 1 <= width
//  - 0 <= lipipe    (logic  input-side pipelining depth)
//  - 0 <= lopipe    (logic output-side pipelining depth)
//  - 0 <=  wpipe    (interconnect/wire pipelining depth)
//  - depth >= (lopipe + 2*wpipe) + 2
//      * required because pipelining allocates queue reserve capacity,
//          which must be strictly less than queue total capacity
//      * depth = ... + 1 would be half throughput, oscillate back-pressure
//  - Queue storage is in SRL16E, up to depth 16 per LUT per bit-slice
//  - When empty, continue to emit most recently emitted value (for debugging)
//
//  - Synplify 7.1 / 8.0
//  - Eylon Caspi, 4/9/05


`ifdef  Q_lowqli_in
`else
`define Q_lowqli_in


`include "Q_fwd_pipe.v"
`include "Q_pipe_noretime.v"
`include "Q_srl_reserve.v"
`include "Q_fifo_bubl.v"

module Q_lowqli_in (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter  depth  = 16;   // - greatest #items in queue  (2 <= depth <= 256)
   parameter  width  = 16;   // - width of data (i_d, o_d)
   parameter lopipe  =  0;   // - logic output-side pipelining depth
   parameter  wpipe  =  0;   // - interconnect      pipelining depth
   parameter lipipe  =  0;   // - logic  input-side pipelining depth

   input     clock;
   input     reset;

   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure

   parameter           d0 = ( depth==0);
   parameter          lo0 = (lopipe==0);
   parameter           w0 = ( wpipe==0);
   parameter          li0 = (lipipe==0);

   parameter           depth_ = ( d0 ? 2 :  depth);	// - ``safe''  depth
   parameter          lopipe_ = (lo0 ? 1 : lopipe);	// - ``safe'' lopipe
   parameter           wpipe_ = ( w0 ? 1 :  wpipe);	// - ``safe''  wpipe
   parameter          lipipe_ = (li0 ? 1 : lipipe);	// - ``safe'' lipipe

   // - the following ``wire'' stmts are I/O for pipe + q components
   // - *i_[dvb] refers to the side closer to the input  stream
   // - *o_[dvb] refers to the side closer to the output stream

   wire [width-1:0]   loi_d, loo_d, wi_d, wo_d, qi_d, qo_d, lii_d, lio_d;
   wire               loi_v, loo_v, wi_v, wo_v, qi_v, qo_v, lii_v, lio_v;
   wire               loi_b, loo_b, wi_b, wo_b, qi_b, qo_b, lii_b, lio_b;

   // - the following ``wire'' stmts connect or bypass components of depth 0
   //
   //     i_d --[lopipe]-> l_d --[wpipe]-> w_d --[Q]-> q_d --[lipipe]-> o_d
   //     i_v --[      ]-> l_v --[     ]-> w_v --[ ]-> q_v --[      ]-> o_v
   //     i_b <-[      ]-- l_b <-[     ]-- w_b <-[ ]-- q_b <-[      ]-- o_b

   wire [width-1:0]   l_d, w_d, q_d;
   wire               l_v, w_v, q_v;
   wire               l_b, w_b, q_b;

   // - logic output-side pipelining (lopipe)
   assign             loi_d = i_d ;
   assign             loi_v = i_v ;
   assign             loo_b = l_b ;
   assign             l_d = /* lo0 */ 1 ? loi_d : loo_d ;
   assign             l_v = /* lo0 */ 1 ? loi_v : loo_v ;
   assign             i_b = /* lo0 */ 1 ? loo_b : loi_b ;
   // Q_fwd_pipe #(lopipe_,width) q_lopipe (clock,reset,loi_d,loi_v,loi_b,loo_d,loo_v,loo_b);

   // - interconnect pipelining (wpipe)
   assign             wi_d = l_d ;
   assign             wi_v = l_v ;
   assign             wo_b = w_b ;
   assign             w_d =  /* w0 */ 1 ? wi_d : wo_d ;
   assign             w_v =  /* w0 */ 1 ? wi_v : wo_v ;
   assign             l_b =  /* w0 */ 1 ? wo_b : wi_b ;
   // Q_pipe_noretime #(wpipe_,width) q_wpipe (clock,reset,wi_d,wi_v,wi_b,wo_d,wo_v,wo_b);

   // - queue
   assign             qi_d = w_d ;
   assign             qi_v = w_v ;
   assign             qo_b = q_b ;
   assign             q_d =  d0 ? qi_d : qo_d ;
   assign             q_v =  d0 ? qi_v : qo_v ;
   assign             w_b =  d0 ? qo_b : qi_b ;
   Q_srl_reserve #(depth_,width,lopipe+wpipe*2) q (clock,reset,qi_d,qi_v,qi_b,qo_d,qo_v,qo_b);

   // logic input-side pipelining (lipipe)
   assign             lii_d = q_d ;
   assign             lii_v = q_v ;
   assign             lio_b = o_b ;
   assign             o_d = li0 ? lii_d : lio_d ;
   assign             o_v = li0 ? lii_v : lio_v ;
   assign             q_b = li0 ? lio_b : lii_b ;
   Q_fifo_bubl #(lipipe_,width) q_lipipe (clock,reset,lii_d,lii_v,lii_b,lio_d,lio_v,lio_b);

endmodule  // Q_lowqli_in


`endif  // `ifdef  Q_lowqli_in
