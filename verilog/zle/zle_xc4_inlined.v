// - Hand coded verilog for ZLE top level
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - state parameters at top level only  (otherwise same as zle_xc)

`define state_start	= 4'd0;
`define state_start_t	= 4'd1;
`define state_start_e	= 4'd2;
`define state_zeros	= 4'd3;
`define state_zeros_t	= 4'd4;
`define state_zeros_t_t	= 4'd5;
`define state_zeros_t_e	= 4'd6;
`define state_zeros_e	= 4'd7;
`define state_pending	= 4'd8;


// - Hand coded verilog for ZLE FSM
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - state parameters at top level only  (otherwise same as zle_xc)

module zle_xc4_fsm (clock, reset,  i_v, i_b_,  o_v_, o_b,  state_,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   input  clock, reset;
   input  i_v;	output i_b_;			// - input  stream i
   output o_v_;	input  o_b;			// - output stream o
   output [3:0] state_;				// - state to datapath
   input  f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_t_cnt_eq_15;			// - flags from datapath

   reg [3:0] state, next_state;
   reg 	     i_b, o_v;
      
   assign    state_ = state;
   assign    i_b_   = i_b;
   assign    o_v_   = o_v;
   
   always @(posedge clock or negedge reset)
     if (!reset) state<=state_start;
     else        state<=next_state;
   
   always @(state or i_v or o_b or
	    f_start_i_eq_0 or f_zeros_i_eq_0 or f_zeros_t_cnt_eq_15) begin
      case (state)
	state_start:		if (i_v) begin
				   i_b<=0;
				   o_v<=0;
				   if (f_start_i_eq_0)
				      next_state<=state_start_t;
				   else
				      next_state<=state_start_e;
				end
				else begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state;
				end
	  
	state_start_t:		begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state_zeros;
				end

	state_start_e:		if (!o_b) begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_start;
				end
				else begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state;
				end

	state_zeros:		if (i_v) begin
				   i_b<=0;
				   o_v<=0;
				   if (f_zeros_i_eq_0)
				      next_state<=state_zeros_t;
				   else
				      next_state<=state_zeros_e;
				end
				else begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state;
				end

	state_zeros_t:		begin
				   i_b<=1;
				   o_v<=0;
				   if (f_zeros_t_cnt_eq_15)
				      next_state<=state_zeros_t_t;
				   else
				      next_state<=state_zeros_t_e;
				end

	state_zeros_t_t:	if (!o_b) begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_zeros;
				end
				else begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state;
				end

	state_zeros_t_e:	begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state_zeros;
				end

	state_zeros_e:		if (!o_b) begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_pending;
				end
				else begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state;
				end

	state_pending:		if (!o_b) begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_start;
				end
				else begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=state;
				end

	default:		begin
				   i_b<=1;
				   o_v<=0;
				   next_state<=4'bx;
				end

      endcase // case(state)
   end // always @ (state or i_v or o_b or...
   
endmodule // zle_xc4_fsm


// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing

module zle_xc_dp  (clock, reset,  i_d,  o_d,  state,
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


module zle_xc4_inlined (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [2:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [3:0] o_d;	output o_v;	input  o_b;	// - output stream o

   wire [3:0] 	state;				// - state to datapath
   wire 	f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_t_cnt_eq_15;		// - flags from datapath
      
   zle_xc4_fsm fsm (clock, reset,  i_v, i_b,  o_v, o_b,  state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   zle_xc4_dp  dp  (clock, reset, i_d, o_d, state,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);

endmodule // zle_xc4_inlined
