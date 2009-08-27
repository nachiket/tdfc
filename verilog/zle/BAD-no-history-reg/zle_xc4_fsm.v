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
