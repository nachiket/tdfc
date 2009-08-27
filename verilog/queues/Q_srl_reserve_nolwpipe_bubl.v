// Q_srl_reserve_nolwpipe_bubl.v
//
//  - Stream with parameterizable queue depth, bit width,
//      logic pipelining depth, and interconnect (wire) pipelining depth,
//      but WITHOUT THE PIPELINE REGISTERS
//      (this is the second half of ``Q_srl_reserve_lwpipe.v'')
//       this is:                                    [ Q_srl_reserve ]
//       out  of: [ Q_fifo_bubl --> Q_pipe_noretime --> Q_srl_reserve ]
//  - Stream I/O is triple (data, valid, back-pressure),
//      with EOS concatenated into the data
//  - Flow control for input & output is combinationally decoupled
//  - Emit input back-pressure early to manage queue reserve capacity
//      in the presence of pipelined (i.e. stale) flow control,
//      (queue reserve capacity) = (logic pipe depth) + 2 * (interx pipe depth)
//  - 2 <= depth <= 256
//      * (depth >= 2)  is required to decouple I/O flow control,
//          where empty => no produce,  full => no consume,
//          and depth 1 would ping-pong between the two at half rate
//      * (depth <= 256) can be modified [in Q_srl_reserve.v]
//           by changing ''synthesis loop_limit X'' below
//          and changing ''addrwidth'' or its log computation
//  - 1 <= width
//  - 0 <= lpipe    (logic             pipelining depth)
//  - 0 <= wpipe    (interconnect/wire pipelining depth)
//  - (2*wpipe) < depth
//      * required because pipelining allocates queue reserve capacity,
//          which must be strictly less than queue total capacity
//  - Queue storage is in SRL16E, up to depth 16 per LUT per bit-slice
//  - When empty, continue to emit most recently emitted value (for debugging)
//
//  - Synplify 7.1
//  - Eylon Caspi,  8/2/04 3/25/05


`ifdef  Q_srl_reserve_nolwpipe_bubl
`else
`define Q_srl_reserve_nolwpipe_bubl


// `include "Q_fifo_bubl.v"
// `include "Q_pipe_noretime.v"
`include "Q_srl_reserve.v"


module Q_srl_reserve_nolwpipe_bubl (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth   = 16;   // - greatest #items in queue  (2 <= depth <= 256)
   parameter width   = 16;   // - width of data (i_d, o_d)
   parameter lpipe   =  0;   // - logic pipelining depth (retimabl Q_fifo_bubl)
   parameter wpipe   =  0;   // - wire  pipelining depth (nonretim bidir regs)

   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure

   // - [ wiring from ``Q_srl_reserve_lwpipe.v'' DELETED ]

   // - NO logic pipelining
   // Q_fifo_bubl     #(lpipe_,width) q_lpipe (clock,reset,lid,liv,lib,lod,lov,lob);

   // - NO interconnect pipelining
   // Q_pipe_noretime #(wpipe_,width) q_wpipe (clock,reset,wid,wiv,wib,wod,wov,wob);

   // - queue with reserve capacity
   // Q_srl_reserve #(depth,width,wpipe*2) q (clock,reset,qid,qiv,qib,qod,qov,qob);
   Q_srl_reserve    #(depth,width,wpipe*2) q (clock,reset,i_d,i_v,i_b,o_d,o_v,o_b);
   
endmodule // Q_srl_reserve_nolwpipe_bubl


`endif  // `ifdef  Q_srl_reserve_nolwpipe_bubl
