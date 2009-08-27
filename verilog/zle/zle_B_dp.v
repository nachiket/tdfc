// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases
// - No resource sharing
// - Blocking assignment to overwrite state, control flow, regs, etc.
// - 7->8 bit compression instead of 3->4

module zle_B_dp  (clock, reset,  i_d,  o_d,  state, fire,
		  f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_cnt_eq_127);
   
   input  clock, reset;
   input  [6:0] i_d;				// - input  stream i
   output [7:0] o_d;				// - output stream o
   input  [1:0] state;				// - state from FSM
   input        fire;				// - fire  from FSM
   output f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_cnt_eq_127;			// - flags to FSM
   
   parameter state_start	= 2'd0;
   parameter state_zeros	= 2'd1;
   parameter state_pending	= 2'd2;

   reg [7:0] cnt, next_cnt;
   reg [6:0] i_at_0, i_at_0_;			// - i@0 history register
   reg [7:0] o_d_;				// - always assignable

   assign    o_d = o_d_;
   assign    f_start_i_eq_0     = (i_d==0);
   assign    f_zeros_i_eq_0     = (i_d==0);
   assign    f_zeros_cnt_eq_127 = (cnt==127);
   
   always @(posedge clock or negedge reset)
     if (!reset)	begin	cnt<=0;		i_at_0<=7'bx;     end
     else		begin	cnt<=next_cnt;	i_at_0<=i_at_0_;  end
   
   always @(cnt or state or fire or i_at_0 or i_d or
	    f_start_i_eq_0 or f_zeros_i_eq_0 or f_zeros_cnt_eq_127) begin

      // - defaults
      next_cnt=cnt;
      i_at_0_=i_at_0;
      o_d_=8'bx;
      
      // - translated from TDF
      if (fire)
      case (state)

	state_start:	begin
			   i_at_0_=i_d;
			   if (f_start_i_eq_0) begin
			      next_cnt=1;
			   end
			   else begin
			      o_d_=i_d;
			   end
			end

	state_zeros:	begin
			   i_at_0_=i_d;
			   if (f_zeros_i_eq_0) begin
			      if (f_zeros_cnt_eq_127) begin
			         o_d_=16|cnt;
				 next_cnt=0;
			      end
			      else begin
				 next_cnt=cnt+1;
			      end
			   end
			end

	state_pending:	begin
			   o_d_=i_at_0;
			end

	default:	begin
			   next_cnt=8'bx;
			   i_at_0_=7'bx;
			   o_d_=8'bx;
			end

      endcase // case(state)
   end // always @ (cnt or state or fire or i_at_0 or i_d)

endmodule // zle_B_dp
