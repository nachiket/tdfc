// Q_wire.v
//
//  - Pass-through stream queue, wire in to out
//  - depth parameter is ignored
//  - 1 <= width
//
//  - Synplify 7.1
//  - Eylon Caspi,  11/18/03


`ifdef  Q_wire
`else
`define Q_wire


module Q_wire (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

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

   assign 	      o_d = i_d;
   assign 	      o_v = i_v;
   assign 	      i_b = o_b;
   
endmodule // Q_wire


`endif	// `ifdef  Q_wire
