// Q_srl_test.v
//
//  - Wrapper to synthesize Q_srl.v with different parameters
//  - Used by Q_srl_test.cmd
//
//  - Synplify 7.1
//  - Eylon Caspi,  9/18/03

`include "Q_srl.v"

module Q_srl_test (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth = 2;
   parameter width = 1;
   
   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;
   input              i_v;
   output             i_b;

   output [width-1:0] o_d;
   output             o_v;
   input              o_b;

   Q_srl #(depth, width) q (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

endmodule // Q_srl_test
