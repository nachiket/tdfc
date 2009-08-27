// Q_srl_reserve_lwpipe.v
//
//  - Stream with parameterizable queue depth, bit width,
//      logic pipelining depth, and interconnect (wire) pipelining depth
//      [ Q_fwd_pipe --> Q_pipe_noretime --> Q_srl_reserve ]
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
//  - (lpipe + 2*wpipe) < depth
//      * required because pipelining allocates queue reserve capacity,
//          which must be strictly less than queue total capacity
//  - Queue storage is in SRL16E, up to depth 16 per LUT per bit-slice
//  - When empty, continue to emit most recently emitted value (for debugging)
//
//  - Synplify 7.1
//  - Eylon Caspi,  8/2/04


`ifdef  Q_srl_reserve_lwpipe
`else
`define Q_srl_reserve_lwpipe


`include "Q_fwd_pipe.v"
`include "Q_pipe_noretime.v"
`include "Q_srl_reserve.v"


module Q_srl_reserve_lwpipe (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth   = 16;   // - greatest #items in queue  (2 <= depth <= 256)
   parameter width   = 16;   // - width of data (i_d, o_d)
   parameter lpipe   =  0;   // - logic pipelining depth (retimable fwd regs)
   parameter wpipe   =  0;   // - wire  pipelining depth (nonretim bidir regs)

   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure
   
   parameter 	      l0     = (lpipe==0);
   parameter 	        w0   = (wpipe==0);
   parameter 	      l0w0   = (l0 && w0);
   
   parameter 	      lpipe_ = (l0 ? 1 : lpipe); // - safe lpipe for Q_fwd_pipe
   parameter 	      wpipe_ = (w0 ? 1 : wpipe); // - safe wpipe for Q_pipe_noretime

   // - the following ``wire'' and ``assign'' stmts are used
   //     to bypass the logic pipelining module if lpipe==0 and
   //     to bypass the wire  pipelining module if wpipe==0
   
   wire [width-1:0]   lid,lod,wid,wod,qid,qod;	// - q_lpipe connections
   wire 	      liv,lov,wiv,wov,qiv,qov;	// - q_wpipe connections
   wire 	      lib,lob,wib,wob,qib,qob;	// - q       connections

						// - input stream connections
   assign 	      i_b = l0w0 ? qib : w0 ? lib : l0 ? wib : lib ;
   
   assign 	      lid = i_d;		// - q_lpipe connections
   assign 	      liv = i_v;
   assign 	      lob = w0 ? qib : wib ;

   assign 	      wid = l0 ? i_d : lod ;	// - q_wpipe connections
   assign 	      wiv = l0 ? i_v : lov ;
   assign 	      wob = qib ;

						// - q connections
   assign 	      qid = l0w0 ? i_d : w0 ? lod : l0 ? wod : wod ;
   assign 	      qiv = l0w0 ? i_v : w0 ? lov : l0 ? wov : wov ;
   assign 	      qob = o_b ;

   assign 	      o_d = qod ;		// - output stream connections
   assign 	      o_v = qov ;

   // - logic pipelining
   Q_fwd_pipe      #(lpipe_,width) q_lpipe (clock,reset,lid,liv,lib,lod,lov,lob);

   // - interconnect pipelining
   Q_pipe_noretime #(wpipe_,width) q_wpipe (clock,reset,wid,wiv,wib,wod,wov,wob);

   // - queue with reserve capacity
   Q_srl_reserve   #(depth,width,lpipe+wpipe*2) q (clock,reset,qid,qiv,qib,qod,qov,qob);
   
endmodule // Q_srl_reserve_lwpipe


`endif  // `ifdef  Q_srl_reserve_lwpipe
