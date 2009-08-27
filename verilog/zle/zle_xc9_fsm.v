// - Hand coded verilog for ZLE FSM
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - Datapath resource sharing for outputs, registers, but NOT flags
// - No "fire" variable;  firing implicit in verilog "if" stmts

module zle_xc9_fsm (clock, reset,  i_v, i_b,  o_v, o_b,  sel_o_d, sel_cnt,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   input  clock, reset;
   input  i_v;	output i_b;			// - input  stream i
   output o_v;	input  o_b;			// - output stream o
   output       sel_o_d;			// - o_d mux select to datapath
   output [1:0] sel_cnt;			// - cnt mux select to datapath
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

   parameter sel_o_d_start_e	= 1'd0;
   parameter sel_o_d_zeros_t_t	= 1'd1;

   parameter sel_cnt_start	= 2'd0;		// - this one holds value
   parameter sel_cnt_start_t	= 2'd1;
   parameter sel_cnt_zeros_t_t	= 2'd2;
   parameter sel_cnt_zeros_t_e	= 2'd3;
   
   reg [3:0] state, next_state;
   reg 	     i_b_, o_v_;				// - always assignable
   reg       sel_o_d_;
   reg [1:0] sel_cnt_;

   assign    i_b     = i_b_;
   assign    o_v     = o_v_;
   assign    sel_o_d = sel_o_d_;
   assign    sel_cnt = sel_cnt_;

   always @(posedge clock or negedge reset)
     if (!reset) state<=state_start;
     else        state<=next_state;
   
   always @(state or i_v or o_b or
	    f_start_i_eq_0 or f_zeros_i_eq_0 or f_zeros_t_cnt_eq_15) begin
      case (state)
	state_start:		if (i_v) begin
				   i_b_<=0;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   if (f_start_i_eq_0)
				      next_state<=state_start_t;
				   else
				      next_state<=state_start_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state;
				end
	  
	state_start_t:		begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start_t;
				   next_state<=state_zeros;
				end

	state_start_e:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_start_e;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state;
				end

	state_zeros:		if (i_v) begin
				   i_b_<=0;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   if (f_zeros_i_eq_0)
				      next_state<=state_zeros_t;
				   else
				      next_state<=state_zeros_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state;
				end

	state_zeros_t:		begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   if (f_zeros_t_cnt_eq_15)
				      next_state<=state_zeros_t_t;
				   else
				      next_state<=state_zeros_t_e;
				end

	state_zeros_t_t:	if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_zeros_t_t;
				   sel_cnt_<=sel_cnt_zeros_t_t;
				   next_state<=state_zeros;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state;
				end

	state_zeros_t_e:	begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_zeros_t_e;
				   next_state<=state_zeros;
				end

	state_zeros_e:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_zeros_t_t;
				   sel_cnt_<=sel_cnt_zeros_t_t;
				   next_state<=state_pending;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state;
				end

	state_pending:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_start_e;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=state;
				end

	default:		begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   next_state<=4'bx;
				end

      endcase // case(state)
   end // always @ (state or i_v or o_b or...
   
endmodule // zle_xc9_fsm
