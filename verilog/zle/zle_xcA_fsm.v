// - Hand coded verilog for ZLE FSM
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - Datapath resource sharing + flag wire sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts

module zle_xcA_fsm (clock, reset,  i_v, i_b,  o_v, o_b,  sel_o_d, sel_cnt,
		    sel_f, f);

   input  clock, reset;
   input  i_v;	output i_b;			// - input  stream i
   output o_v;	input  o_b;			// - output stream o
   output       sel_o_d;			// - o_d mux select to datapath
   output [1:0] sel_cnt;			// - cnt mux select to datapath
   output       sel_f;				// - f   mux select to datapath
   input 	f;				// - flags from datapath
   
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

   parameter sel_f_start	= 1'd0;
   parameter sel_f_zeros_t	= 1'd1;
   
   reg [3:0] state, next_state;
   reg 	     i_b_, o_v_;				// - always assignable
   reg       sel_o_d_;
   reg [1:0] sel_cnt_;
   reg       sel_f_;

   assign    i_b     = i_b_;
   assign    o_v     = o_v_;
   assign    sel_o_d = sel_o_d_;
   assign    sel_cnt = sel_cnt_;
   assign    sel_f = sel_f_;

   always @(posedge clock or negedge reset)
     if (!reset) state<=state_start;
     else        state<=next_state;
   
   always @(state or i_v or o_b or f) begin
      case (state)
	state_start:		if (i_v) begin
				   i_b_<=0;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=sel_f_start;
				   if (f)
				      next_state<=state_start_t;
				   else
				      next_state<=state_start_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state;
				end
	  
	state_start_t:		begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start_t;
				   sel_f_<=1'bx;
				   next_state<=state_zeros;
				end

	state_start_e:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_start_e;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state;
				end

	state_zeros:		if (i_v) begin
				   i_b_<=0;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
   				   sel_f_<=sel_f_start;
				   if (f)
				      next_state<=state_zeros_t;
				   else
				      next_state<=state_zeros_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state;
				end

	state_zeros_t:		begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=sel_f_zeros_t;
				   if (f)
				      next_state<=state_zeros_t_t;
				   else
				      next_state<=state_zeros_t_e;
				end

	state_zeros_t_t:	if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_zeros_t_t;
				   sel_cnt_<=sel_cnt_zeros_t_t;
				   sel_f_<=1'bx;
				   next_state<=state_zeros;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state;
				end

	state_zeros_t_e:	begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_zeros_t_e;
				   sel_f_<=1'bx;
				   next_state<=state_zeros;
				end

	state_zeros_e:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_zeros_t_t;
				   sel_cnt_<=sel_cnt_zeros_t_t;
				   sel_f_<=1'bx;
				   next_state<=state_pending;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state;
				end

	state_pending:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   sel_o_d_<=sel_o_d_start_e;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=state;
				end

	default:		begin
				   i_b_<=1;
				   o_v_<=0;
				   sel_o_d_<=1'bx;
				   sel_cnt_<=sel_cnt_start;
				   sel_f_<=1'bx;
				   next_state<=4'bx;
				end

      endcase // case(state)
   end // always @ (state or i_v or o_b or f)
   
endmodule // zle_xcA_fsm
