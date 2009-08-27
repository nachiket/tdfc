// - Hand coded verilog for ZLE FSM
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - No resource sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts
// - Unified behavioral FSM contains FSM + DP stmts

module zle_xc6 (clock, reset,  i_d, i_v, i_b,  o_d, o_v, o_b);
   
   input  clock, reset;
   input  [2:0] i_d;	input  i_v;	output i_b;	// - input  stream i
   output [3:0] o_d;	output o_v;	input  o_b;	// - output stream o

   parameter state_start	= 4'd0;
   parameter state_start_t	= 4'd1;
   parameter state_start_e	= 4'd2;
   parameter state_zeros	= 4'd3;
   parameter state_zeros_t	= 4'd4;
   parameter state_zeros_t_t	= 4'd5;
   parameter state_zeros_t_e	= 4'd6;
   parameter state_zeros_e	= 4'd7;
   parameter state_pending	= 4'd8;

   reg [3:0] state, next_state;		// - register: state
   reg [3:0] cnt,   next_cnt;		// - register: zero count

   reg 	     i_b_, o_v_;		// - always assignables
   reg [3:0] o_d_;
   assign    i_b = i_b_;
   assign    o_v = o_v_;
   assign    o_d = o_d_;

   always @(posedge clock or negedge reset) begin
     if (!reset) begin
	state<=state_start;
	cnt<=0;
     end
     else begin
        state<=next_state;
	cnt<=next_cnt;
     end
   end

   always @(state or cnt or i_d or i_v or o_b) begin
      case (state)
	state_start:		if (i_v) begin
				   i_b_<=0;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   if (i_d==0)
				      next_state<=state_start_t;
				   else
				      next_state<=state_start_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   next_state<=state;
				end
	  
	state_start_t:		begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=1;
				   next_state<=state_zeros;
				end

	state_start_e:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   o_d_<=i_d;
				   next_cnt<=cnt;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   next_state<=state;
				end

	state_zeros:		if (i_v) begin
				   i_b_<=0;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   if (i_d==0)
				      next_state<=state_zeros_t;
				   else
				      next_state<=state_zeros_e;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   next_state<=state;
				end

	state_zeros_t:		begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   if (cnt==15)
				      next_state<=state_zeros_t_t;
				   else
				      next_state<=state_zeros_t_e;
				end

	state_zeros_t_t:	if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   o_d_<=16|cnt;
				   next_cnt<=0;
				   next_state<=state_zeros;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   next_state<=state;
				end

	state_zeros_t_e:	begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt+1;
				   next_state<=state_zeros;
				end

	state_zeros_e:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   o_d_<=16|cnt;
				   next_cnt<=cnt;
				   next_state<=state_pending;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   next_state<=state;
				end

	state_pending:		if (!o_b) begin
				   i_b_<=1;
				   o_v_<=1;
				   o_d_<=i_d;
				   next_cnt<=cnt;
				   next_state<=state_start;
				end
				else begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   next_state<=state;
				end

	default:		begin
				   i_b_<=1;
				   o_v_<=0;
				   o_d_<=4'bx;
				   next_cnt<=cnt;
				   next_state<=4'bx;
				end

      endcase // case(state)
   end // always @ (state or cnt or i_d or i_v or o_b)

endmodule // zle_xc6
