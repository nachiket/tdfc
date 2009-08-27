// Q_blackbox.v
//
//  - Black box stream queue
//  - depth parameter is ignored
//  - 1 <= width
//
//  - Synplify 7.1
//  - Eylon Caspi,  11/18/03


`ifdef  Q_blackbox
`else
`define Q_blackbox


module Q_blackbox (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b)  /* synthesis syn_black_box */ ;

   parameter depth =  0;    // - greatest #items in queue  (unused)
   parameter width = 16;    // - width of data (i_d, o_d)

   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure

endmodule // Q_blackbox


`endif	// `ifdef  Q_blackbox
