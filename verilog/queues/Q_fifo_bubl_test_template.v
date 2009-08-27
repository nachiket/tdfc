// Q_srl_test.v
//
//  - Wrapper to synthesize Q_fifo_bubl.v with different parameters
//  - Used by Q_fifo_bubl_test.cmd
//
//  - Synplify 7.1
//  - Eylon Caspi,  9/18/03

`include "Q_fifo_bubl.v"

module Q_fifo_bubl_test (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth = 1;
   parameter width = 1;
   
   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;
   input              i_v;
   output             i_b;

   output [width-1:0] o_d;
   output             o_v;
   input              o_b;

   Q_fifo_bubl #(depth, width) q (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

endmodule // Q_fifo_bubl_test
