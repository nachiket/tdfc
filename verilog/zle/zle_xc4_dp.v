// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - state parameters at top level only  (otherwise same as zle_xc)

module zle_xc4_dp  (clock, reset,  i_d,  o_d,  state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   input  clock, reset;
   input  [2:0] i_d;				// - input  stream i
   output [3:0] o_d;				// - output stream o
   input  [3:0] state;				// - state from FSM
   output f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_t_cnt_eq_15;			// - flags to FSM

   reg [3:0] cnt, next_cnt;
   reg [2:0] i_at_0, i_at_0_;			// - i@0 history register
   reg [3:0] o_d_;				// - always assignable

   assign    o_d = o_d_;
   assign    f_start_i_eq_0      = (i_d==0);
   assign    f_zeros_i_eq_0      = (i_d==0);
   assign    f_zeros_t_cnt_eq_15 = (cnt==15);
   
   always @(posedge clock or negedge reset)
     if (!reset)	begin	cnt<=0;		i_at_0<=2'bx;     end
     else		begin	cnt<=next_cnt;	i_at_0<=i_at_0_;  end
   
   always @(cnt or state or i_at_0 or i_d) begin
      case (state)
	state_start:
		begin  i_at_0_<=i_d;	 o_d_<=4'bx;	next_cnt<=cnt;	  end
	state_start_t:
		begin  i_at_0_<=i_at_0;	 o_d_<=4'bx;	next_cnt<=1;	  end
	state_start_e:
		begin  i_at_0_<=i_at_0;	 o_d_<=i_at_0;	next_cnt<=cnt;	  end
	state_zeros:
		begin  i_at_0_<=i_d;	 o_d_<=4'bx;	next_cnt<=cnt;	  end
	state_zeros_t:
		begin  i_at_0_<=i_at_0;	 o_d_<=4'bx;	next_cnt<=cnt;	  end
	state_zeros_t_t:
		begin  i_at_0_<=i_at_0;	 o_d_<=16|cnt;	next_cnt<=0;	  end
	state_zeros_t_e:
		begin  i_at_0_<=i_at_0;	 o_d_<=4'bx;	next_cnt<=cnt+1;  end
	state_zeros_e:
		begin  i_at_0_<=i_at_0;	 o_d_<=16|cnt;	next_cnt<=0;	  end
	state_pending:
		begin  i_at_0_<=i_at_0;	 o_d_<=i_at_0;	next_cnt<=cnt;	  end
	default:
		begin  i_at_0_<=i_at_0;	 o_d_<=4'bx;	next_cnt<=cnt;	  end
      endcase // case(state)
   end // always @ (cnt or state or i_at_0 or i_d)

endmodule // zle_xc4_dp
