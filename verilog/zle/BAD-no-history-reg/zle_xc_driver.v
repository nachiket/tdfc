`include "zle_xc.v"

module top;
   wire clock, reset;
   wire [2:0] i_d;	wire i_v, i_b;
   wire [3:0] o_d;	wire o_v, o_b;

   zle_xc zle_xc_(clock,reset, i_d,i_v,i_b, o_d,o_v,o_b);

   assign     i_v=1;
   assign     o_b=0;
   assign     clock=1;
   assign     reset=0;
     
endmodule // top

