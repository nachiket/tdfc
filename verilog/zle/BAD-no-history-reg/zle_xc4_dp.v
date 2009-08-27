// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - state parameters at top level only  (otherwise same as zle_xc)

module zle_xc4_dp (clock, reset,  i_d,  o_d_,  state,
		   f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   input  clock, reset;
   input  [2:0] i_d;				// - input  stream i
   output [3:0] o_d_;				// - output stream o
   input  [3:0] state;				// - state from FSM
   output f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_t_cnt_eq_15;			// - flags to FSM

   reg [3:0] cnt, next_cnt;
   reg [3:0] o_d;

   assign    o_d_ = o_d;
   assign    f_start_i_eq_0      = (i_d==0);
   assign    f_zeros_i_eq_0      = (i_d==0);
   assign    f_zeros_t_cnt_eq_15 = (cnt==15);
   
   always @(posedge clock or negedge reset)
     if (!reset)	cnt<=0;
     else		cnt<=next_cnt;
   
   always @(cnt or state or i_d) begin
      case (state)
	state_start:		begin	o_d<=4'bx;	next_cnt<=cnt;	  end
	state_start_t:		begin	o_d<=4'bx;	next_cnt<=1;	  end
	state_start_e:		begin	o_d<=i_d;	next_cnt<=cnt;	  end
	state_zeros:		begin	o_d<=4'bx;	next_cnt<=cnt;	  end
	state_zeros_t:		begin	o_d<=4'bx;	next_cnt<=cnt;	  end
	state_zeros_t_t:	begin	o_d<=16|cnt;	next_cnt<=0;	  end
	state_zeros_t_e:	begin	o_d<=4'bx;	next_cnt<=cnt+1;  end
	state_zeros_e:		begin	o_d<=16|cnt;	next_cnt<=0;	  end
	state_pending:		begin	o_d<=i_d;	next_cnt<=cnt;	  end
	default:		begin	o_d<=4'bx;	next_cnt<=cnt;	  end
      endcase // case(state)
   end // always @ (posedge clock)

endmodule // zle_xc4_dp
