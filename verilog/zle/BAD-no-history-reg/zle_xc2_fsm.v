// - Hand coded verilog for ZLE FSM
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - Explicit "fire" signal  -->  (17% slower, +1 LUT vs. implicit fire)
// - FSM case stmt has identical "no fire" code in every case

module zle_xc2_fsm (clock, reset,  i_v, i_b,  o_v, o_b,  stateo,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   input  clock, reset;
   input  i_v;	output i_b;			// - input  stream i
   output o_v;	input  o_b;			// - output stream o
   output [3:0] stateo;				// - state to datapath
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
   reg 	     i_b_, o_v_;			// - always assignable
   reg 	     fire;				// - always assignable
      
   assign    stateo = state;
   assign    i_b    = i_b_;
   assign    o_v    = o_v_;
   
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
      case (state)

	// - Note:  The "no fire" else clause is repeated verbatim
	//          in every state case, for proper Synplicity FSM style.
	//          It would be nice to lift it out of the case stmt.
	
	state_start:		if (fire) begin
				   i_b_<=0;
				   o_v_<=0;
				   if (f_start_i_eq_0)
				      next_state<=state_start_t;
				   else
				      next_state<=state_start_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end
	  
	state_start_t:		if (fire) begin		// - unconditional
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state_zeros;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	state_start_e:		if (fire) begin
				   i_b_<=1;
				   o_v_<=1;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	state_zeros:		if (fire) begin
				   i_b_<=0;
				   o_v_<=0;
				   if (f_zeros_i_eq_0)
				      next_state<=state_zeros_t;
				   else
				      next_state<=state_zeros_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	state_zeros_t:		if (fire) begin		// - unconditional
				   i_b_<=1;
				   o_v_<=0;
				   if (f_zeros_t_cnt_eq_15)
				      next_state<=state_zeros_t_t;
				   else
				      next_state<=state_zeros_t_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	state_zeros_t_t:	if (fire) begin
				   i_b_<=1;
				   o_v_<=1;
				   next_state<=state_zeros;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	state_zeros_t_e:	if (fire) begin		// - unconditional
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state_zeros;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	state_zeros_e:		if (fire) begin
				   i_b_<=1;
				   o_v_<=1;
				   next_state<=state_pending;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	state_pending:		if (fire) begin
				   i_b_<=1;
				   o_v_<=1;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=state;
				end

	default:		begin		// - unconditional no fire
				   i_b_<=1;
				   o_v_<=0;
				   next_state<=4'bx;
				end

      endcase // case(state)
   end // always @ (state or i_v or o_b or fire or...
   
endmodule // zle_xc2_fsm
