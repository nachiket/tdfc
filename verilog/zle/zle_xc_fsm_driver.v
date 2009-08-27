`include "zle_xc_fsm.v"

module top;
   wire       clock, reset;
   wire       i_v, i_b_, o_v, o_b;
   wire [3:0] state;
   wire       f1,f2,f3;

   zle_xc_fsm fsm_(clock,reset, i_v,i_b_, o_v,o_b, state, f1,f2,f3);

   initial begin
      clock = 1;
      forever begin
	 #1000 clock = !clock;
	 $monitor("%d  r=%d i_v=%d i_b_=%d o_v=%d o_b=%d state=%d\n",
		  $time, reset, i_v, i_b_, o_v, o_b);
      end
   end

   initial begin
      reset=0;
      i_v=0;
      o_b=1;
      state=0;
   end
      
endmodule // top

