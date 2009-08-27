// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - 7->8 bit compression instead of 3->4

module zle_xcB_dp  (clock, reset,  i_d,  o_d,  state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_127);
   
   input  clock, reset;
   input  [6:0] i_d;				// - input  stream i
   output [7:0] o_d;				// - output stream o
   input  [3:0] state;				// - state from FSM
   output f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_t_cnt_eq_127;			// - flags to FSM
   
   parameter state_start	= 4'd0;
   parameter state_start_t	= 4'd1;
   parameter state_start_e	= 4'd2;
   parameter state_zeros	= 4'd3;
   parameter state_zeros_t	= 4'd4;
   parameter state_zeros_t_t	= 4'd5;
   parameter state_zeros_t_e	= 4'd6;
   parameter state_zeros_e	= 4'd7;
   parameter state_pending	= 4'd8;

   reg [6:0] cnt, next_cnt;
   reg [6:0] i_at_0, i_at_0_;			// - i@0 history register
   reg [7:0] o_d_;				// - always assignable

   assign    o_d = o_d_;
   assign    f_start_i_eq_0       = (i_d==0);
   assign    f_zeros_i_eq_0       = (i_d==0);
   assign    f_zeros_t_cnt_eq_127 = (cnt==127);
   
   always @(posedge clock or negedge reset)
     if (!reset)	begin	cnt<=0;		i_at_0<=7'bx;     end
     else		begin	cnt<=next_cnt;	i_at_0<=i_at_0_;  end
   
   always @(cnt or state or i_at_0 or i_d) begin
      case (state)
	state_start:
		begin  i_at_0_<=i_d;	 o_d_<=8'bx;	next_cnt<=cnt;	  end
	state_start_t:
		begin  i_at_0_<=i_at_0;	 o_d_<=8'bx;	next_cnt<=1;	  end
	state_start_e:
		begin  i_at_0_<=i_at_0;	 o_d_<=i_at_0;	next_cnt<=cnt;	  end
	state_zeros:
		begin  i_at_0_<=i_d;	 o_d_<=8'bx;	next_cnt<=cnt;	  end
	state_zeros_t:
		begin  i_at_0_<=i_at_0;	 o_d_<=8'bx;	next_cnt<=cnt;	  end
	state_zeros_t_t:
		begin  i_at_0_<=i_at_0;	 o_d_<=128|cnt;	next_cnt<=0;	  end
	state_zeros_t_e:
		begin  i_at_0_<=i_at_0;	 o_d_<=8'bx;	next_cnt<=cnt+1;  end
	state_zeros_e:
		begin  i_at_0_<=i_at_0;	 o_d_<=128|cnt;	next_cnt<=0;	  end
	state_pending:
		begin  i_at_0_<=i_at_0;	 o_d_<=i_at_0;	next_cnt<=cnt;	  end
	default:
		begin  i_at_0_<=i_at_0;	 o_d_<=8'bx;	next_cnt<=cnt;	  end
      endcase // case(state)
   end // always @ (cnt or state or i_at_0 or i_d)

endmodule // zle_xcB_dp
