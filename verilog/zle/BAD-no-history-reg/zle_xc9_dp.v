// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - Datapath resource sharing for outputs, registers, but NOT flags

module zle_xc9_dp  (clock, reset,  i_d,  o_d,  sel_o_d, sel_cnt,
		    f_start_i_eq_0, f_zeros_i_eq_0, f_zeros_t_cnt_eq_15);
   
   input  clock, reset;
   input  [2:0] i_d;				// - input  stream i
   output [3:0] o_d;				// - output stream o
   input        sel_o_d;			// - o_d mux select from FSM
   input  [1:0] sel_cnt;			// - cnt mux select from FSM
   output       f_start_i_eq_0,
		f_zeros_i_eq_0,
		f_zeros_t_cnt_eq_15;		// - flags to FSM

   parameter sel_o_d_start_e	= 1'd0;
   parameter sel_o_d_zeros_t_t	= 1'd1;

   parameter sel_cnt_start	= 2'd0;
   parameter sel_cnt_start_t	= 2'd1;
   parameter sel_cnt_zeros_t_t	= 2'd2;
   parameter sel_cnt_zeros_t_e	= 2'd3;

   reg  [3:0]  cnt, next_cnt;
   reg  [3:0]  o_d_;				// - always assignable

   wire       o_d_start_e, o_d_zeros_t_t;	// - datapath choices
   wire [3:0] cnt_start, cnt_start_t, cnt_zeros_t_t, cnt_zeros_t_e;
   
   assign o_d = o_d_;
   assign f_start_i_eq_0      = (i_d==0);
   assign f_zeros_i_eq_0      = (i_d==0);
   assign f_zeros_t_cnt_eq_15 = (cnt==15);

   // - o_d : 2 unique datapaths
// assign o_d_start	= 4'bx;
// assign o_d_start_t	= 4'bx;
   assign o_d_start_e	= i_d;
// assign o_d_zeros	= 4'bx;
// assign o_d_zeros_t	= 4'bx;
   assign o_d_zeros_t_t	= 16|cnt;
// assign o_d_zeros_t_e	= 4'bx;
// assign o_d_zeros_e	= 16|cnt;
// assign o_d_pending	= i_d;

   // - next_cnt : 4 unique datapaths
   assign cnt_start	= cnt;
   assign cnt_start_t	= 1;
// assign cnt_start_e	= cnt;
// assign cnt_zeros	= cnt;
// assign cnt_zeros_t	= cnt;
   assign cnt_zeros_t_t	= 0;
   assign cnt_zeros_t_e	= cnt+1;
// assign cnt_zeros_e	= 0;
// assign cnt_pending	= cnt;

   always @(posedge clock or negedge reset)
     if (!reset)	cnt<=0;
     else		cnt<=next_cnt;
   
   always @(sel_o_d or i_d or
	    o_d_start_e or o_d_zeros_t_t) begin
      case (sel_o_d)
	sel_o_d_start_e:	o_d_<=o_d_start_e;
	sel_o_d_zeros_t_t:	o_d_<=o_d_zeros_t_t;
	default:		o_d_<=1'bx;
      endcase; // case(o_d_sel)
   end

   always @(sel_cnt or cnt or
	    cnt_start or cnt_start_t or cnt_zeros_t_t or cnt_zeros_t_e)
   begin
      case (sel_cnt)
	sel_cnt_start:		next_cnt<=cnt_start;
	sel_cnt_start_t:	next_cnt<=cnt_start_t;
	sel_cnt_zeros_t_t:	next_cnt<=cnt_zeros_t_t;
	sel_cnt_zeros_t_e:	next_cnt<=cnt_zeros_t_e;
	default:		next_cnt<=cnt;
      endcase // case(sel_cnt)
   end

endmodule // zle_xc9_dp
