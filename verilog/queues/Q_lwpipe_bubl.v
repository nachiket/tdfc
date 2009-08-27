// Q_lwpipe_bubl.v
//
//  - Stream with parameterizable queue depth, bit width,
//      logic pipelining depth, and interconnect (wire) pipelining depth,
//      but WITHOUT THE QUEUE
//      (this is the first half of ``Q_srl_reserve_lwpipe_bubl.v'')
//       this is: [ Q_fifo_bubl --> Q_pipe_noretime ]
//       out  of: [ Q_fifo_bubl --> Q_pipe_noretime --> Q_srl_reserve ]
//  - Stream I/O is triple (data, valid, back-pressure),
//      with EOS concatenated into the data
//  - 2 <= depth <= 256
//  - 1 <= width
//  - 0 <= lpipe    (logic             pipelining depth)
//  - 0 <= wpipe    (interconnect/wire pipelining depth)
//
//  - Synplify 7.1
//  - Eylon Caspi,  8/2/04 3/25/05


`ifdef  Q_lwpipe_bubl
`else
`define Q_lwpipe_bubl


`include "Q_fifo_bubl.v"
`include "Q_pipe_noretime.v"
// `include "Q_srl_reserve.v"


module Q_lwpipe_bubl (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth   = 16;   // - greatest #items in queue (IGNORED)
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
   
   parameter 	      l0     = (lpipe==0);
   parameter 	        w0   = (wpipe==0);
   parameter 	      l0w0   = (l0 && w0);
   
   parameter 	      lpipe_ = (l0 ? 1 : lpipe); // - safe lpipe for Q_fifo_bubl
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

   assign 	      qod = qid ;		// - bypass queue
   assign 	      qov = qiv ;
   assign 	      qib = qob ; 	      
   
   // - logic pipelining
   Q_fifo_bubl     #(lpipe_,width) q_lpipe (clock,reset,lid,liv,lib,lod,lov,lob);

   // - interconnect pipelining
   Q_pipe_noretime #(wpipe_,width) q_wpipe (clock,reset,wid,wiv,wib,wod,wov,wob);

   // - NO queue with reserve capacity
   // Q_srl_reserve #(depth,width,wpipe*2) q (clock,reset,qid,qiv,qib,qod,qov,qob);
   
endmodule // Q_lwpipe_bubl


`endif  // `ifdef  Q_lwpipe_bubl
