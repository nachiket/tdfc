// - Hand coded verilog for ZLE FSM
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - Explicit "fire" signal
// - FSM has "no fire" code + case stmt for "fire" code  (+2 LUTs vs zle_xc2)

module zle_xc3_fsm (clock, reset,  i_v, i_b_,  o_v_, o_b,  state_,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   input  clock, reset;
   input  i_v;	output i_b_;			// - input  stream i
   output o_v_;	input  o_b;			// - output stream o
   output [3:0] state_;				// - state to datapath
   input  f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_t_cnt_eq_15;			// - flags from datapath
   
   parameter state_start	= 4'd0;
   parameter state_start_t	= 4'd1;
   parameter state_start_e	= 4'd2;
   parameter state_zeros	= 4'd3;
   parameter state_zeros_t	= 4'd4;
   parameter state_zeros_t_t	= 4'd5;
   parameter state_zeros_t_e	= 4'd6;
   parameter state_zeros_e	= 4'd7;
   parameter state_pending	= 4'd8;

   reg [3:0] state, next_state;
   reg 	     i_b, o_v;
   reg 	     fire;
      
   assign    state_ = state;
   assign    i_b_   = i_b;
   assign    o_v_   = o_v;
   
   always @(posedge clock or negedge reset)
     if (!reset) state<=state_start;
     else        state<=next_state;
   
   always @(state or i_v or o_b) begin
      case (state)
	state_start:		fire<=i_v;
	state_start_t:		fire<=1;
	state_start_e:		fire<=!o_b;
	state_zeros:		fire<=i_v;
	state_zeros_t:		fire<=1;
	state_zeros_t_t:	fire<=!o_b;
	state_zeros_t_e:	fire<=1;
	state_zeros_e:		fire<=!o_b;
	state_pending:		fire<=!o_b;
	default:		fire<=0;
      endcase // case(state)
   end // always @ (state or i_v or o_b)

   always @(state or i_v or o_b or fire or
	    f_start_i_eq_0 or f_zeros_i_eq_0 or f_zeros_t_cnt_eq_15) begin
      if (!fire) begin
	 i_b<=1;
	 o_v<=0;
	 next_state<=state;
      end
      else case (state)
	state_start:		begin
				   i_b<=0;
				   o_v<=0;
				   if (f_start_i_eq_0)
				      next_state<=state_start_t;
				   else
				      next_state<=state_start_e;
				end
	  
	state_start_t:		begin			// - unconditional fire
				   i_b<=1;
				   o_v<=0;
				   next_state<=state_zeros;
				end

	state_start_e:		begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_start;
				end

	state_zeros:		begin
				   i_b<=0;
				   o_v<=0;
				   if (f_zeros_i_eq_0)
				      next_state<=state_zeros_t;
				   else
				      next_state<=state_zeros_e;
				end

	state_zeros_t:		begin			// - unconditional fire
				   i_b<=1;
				   o_v<=0;
				   if (f_zeros_t_cnt_eq_15)
				      next_state<=state_zeros_t_t;
				   else
				      next_state<=state_zeros_t_e;
				end

	state_zeros_t_t:	begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_zeros;
				end

	state_zeros_t_e:	begin			// - unconditional fire
				   i_b<=1;
				   o_v<=0;
				   next_state<=state_zeros;
				end

	state_zeros_e:		begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_pending;
				end

	state_pending:		begin
				   i_b<=1;
				   o_v<=1;
				   next_state<=state_start;
				end

	default:		begin			// - dummy no fire
				   i_b<=1;
				   o_v<=0;
				   next_state<=4'bx;
				end

      endcase // case(state)
   end // always @ (state or i_v or o_b or fire or...
   
endmodule // zle_xc3_fsm
