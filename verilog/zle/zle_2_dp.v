// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases
// - No resource sharing
// - Blocking assignment to overwrite state, control flow, regs, etc.
// - DP case stmt has identical "if (fire)" check in every case

module zle_2_dp  (clock, reset,  i_d,  o_d,  state, fire,
		  f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_cnt_eq_15);
   
   input  clock, reset;
   input  [2:0] i_d;				// - input  stream i
   output [3:0] o_d;				// - output stream o
   input  [1:0] state;				// - state from FSM
   input        fire;				// - fire  from FSM
   output f_start_i_eq_0,
          f_zeros_i_eq_0,
          f_zeros_cnt_eq_15;			// - flags to FSM
   
   parameter state_start	= 2'd0;
   parameter state_zeros	= 2'd1;
   parameter state_pending	= 2'd2;

   reg [3:0] cnt, next_cnt;
   reg [2:0] i_at_0, i_at_0_;			// - i@0 history register
   reg [3:0] o_d_;				// - always assignable

   assign    o_d = o_d_;
   assign    f_start_i_eq_0    = (i_d==0);
   assign    f_zeros_i_eq_0    = (i_d==0);
   assign    f_zeros_cnt_eq_15 = (cnt==15);
   
   always @(posedge clock or negedge reset)
     if (!reset)	begin	cnt<=0;		i_at_0<=3'bx;     end
     else		begin	cnt<=next_cnt;	i_at_0<=i_at_0_;  end
   
   always @(cnt or state or fire or i_at_0 or i_d or
	    f_start_i_eq_0 or f_zeros_i_eq_0 or f_zeros_cnt_eq_15) begin

      // - defaults
      next_cnt=cnt;
      i_at_0_=i_at_0;
      o_d_=4'bx;
      
      // - translated from TDF
      case (state)

	state_start:	if (fire) begin
			   i_at_0_=i_d;
			   if (f_start_i_eq_0) begin
			      next_cnt=1;
			   end
			   else begin
			      o_d_=i_d;
			   end
			end

	state_zeros:	if (fire) begin
			   i_at_0_=i_d;
			   if (f_zeros_i_eq_0) begin
			      if (f_zeros_cnt_eq_15) begin
			         o_d_=16|cnt;
				 next_cnt=0;
			      end
			      else begin
				 next_cnt=cnt+1;
			      end
			   end
			end

	state_pending:	if (fire) begin
			   o_d_=i_at_0;
			end

	default:	begin
			   next_cnt=4'bx;
			   i_at_0_=3'bx;
			   o_d_=4'bx;
			end

      endcase // case(state)
   end // always @ (cnt or state or fire or i_at_0 or i_d)

endmodule // zle_2_dp
