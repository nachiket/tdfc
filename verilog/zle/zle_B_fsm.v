// - Hand coded verilog for ZLE FSM
// - Zero run-length encoder, no EOS cases
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - Blocking assignment to overwrite state, control flow, regs, etc.
// - 7->8 bit compression instead of 3->4

module zle_B_fsm (clock, reset,  i_v, i_b,  o_v, o_b,  stateo, fireo,
		  f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_cnt_eq_127);
   
   input  clock, reset;
   input  i_v;	output i_b;			// - input  stream i
   output o_v;	input  o_b;			// - output stream o
   output [1:0] stateo;				// - state to datapath
   output       fireo;				// - fire  to datapath
   input  f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_cnt_eq_127;			// - flags from datapath
   
   parameter state_start	= 2'd0;
   parameter state_zeros	= 2'd1;
   parameter state_pending	= 2'd2;

   reg [1:0] state, next_state;
   reg 	     i_b_, o_v_;			// - always assignable
   reg       fireo_;

   assign    stateo = state;
   assign    fireo  = fireo_;
   assign    i_b    = i_b_;
   assign    o_v    = o_v_;
   
   always @(posedge clock or negedge reset)
     if (!reset) state<=state_start;
     else        state<=next_state;
   
   always @(state or i_v or o_b or
	    f_start_i_eq_0 or f_zeros_i_eq_0 or f_zeros_cnt_eq_127) begin

      // - defaults
      fireo_=0;
      i_b_=1;
      o_v_=0;
      next_state=state;

      // - translated from TDF
      case (state)
	state_start:		if (i_v && !o_b) begin
				   fireo_=1;
				   i_b_=0;
				   if (f_start_i_eq_0) begin
				      next_state=state_zeros;
				   end
				   else begin
				      o_v_=1;
				   end
				end

	state_zeros:		if (i_v && !o_b) begin
				   fireo_=1;
				   i_b_=0;
				   if (f_zeros_i_eq_0) begin
				      if (f_zeros_cnt_eq_127) begin
				         o_v_=1;
				      end
				      else begin
				      end
				   end
				   else begin
				      o_v_=1;
				      next_state=state_pending;
				   end
				end
	  
	state_pending:		if (!o_b) begin
				   fireo_=1;
				   o_v_=1;
				   next_state=state_start;
				end

        default:		begin			// - never happens
				   fireo_=1'bx;
				   i_b_=1'bx;
				   o_v_=1'bx;
				   next_state=2'bx;
				end

      endcase // case(state)
   end // always @ (state or i_v or o_b or...
   
endmodule // zle_B_fsm
