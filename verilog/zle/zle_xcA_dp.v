// - Hand coded verilog for ZLE datapath (DP)
// - Zero run-length encoder, no EOS cases, compiled "tdfc -xc"
// - Datapath resource sharing + flag wire sharing
// - No "fire" variable;  firing implicit in verilog "if" stmts

module zle_xcA_dp  (clock, reset, i_d, o_d, sel_o_d, sel_cnt, i_b, sel_f, f);
   
   input  clock, reset;
   input  [2:0] i_d;				// - input  stream i
   output [3:0] o_d;				// - output stream o
   input        sel_o_d;			// - o_d mux select from FSM
   input  [1:0] sel_cnt;			// - cnt mux select from FSM
   input        i_b;				// - i !consume     from FSM
						//     (i.e. i mux select)
   input        sel_f;				// - f   mux select from FSM
   output 	f;				// - flag to FSM
   
   parameter sel_o_d_start_e	= 1'd0;
   parameter sel_o_d_zeros_t_t	= 1'd1;

   parameter sel_cnt_start	= 2'd0;
   parameter sel_cnt_start_t	= 2'd1;
   parameter sel_cnt_zeros_t_t	= 2'd2;
   parameter sel_cnt_zeros_t_e	= 2'd3;

   parameter sel_f_start	= 1'd0;
   parameter sel_f_zeros_t	= 1'd1;

   reg [3:0]  cnt, next_cnt;
   reg [2:0]  i_at_0, i_at_0_;			// - i@0 history register
   reg [3:0]  o_d_;				// - always assignable
   reg        f_;				// - always assignable

   wire       o_d_start_e, o_d_zeros_t_t;	// - datapath choices
   wire [3:0] cnt_start, cnt_start_t, cnt_zeros_t_t, cnt_zeros_t_e;
   wire       i_at_0_consume, i_at_0_noconsume;
   wire       f_start_i_eq_0, f_zeros_t_cnt_eq_15;
   
   assign    o_d = o_d_;
   assign    f   = f_;

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

   // - i_at_0_ : 2 unique datapaths
   assign i_at_0_consume   = i_d;
   assign i_at_0_noconsume = i_at_0;

   // - f : 2 unique datapaths
   assign f_start_i_eq_0      = (i_at_0==0);
// assign f_zeros_i_eq_0      = (i_at_0==0);
   assign f_zeros_t_cnt_eq_15 = (cnt==15);

   always @(posedge clock or negedge reset)
     if (!reset)	begin	cnt<=0;		i_at_0<=2'bx;     end
     else		begin	cnt<=next_cnt;	i_at_0<=i_at_0_;  end
   
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

   always @(sel_f or
	    f_start_i_eq_0 or f_zeros_t_cnt_eq_15) begin
      case (sel_f)
	sel_f_start:		f_<=f_start_i_eq_0;
	sel_f_zeros_t:		f_<=f_zeros_t_cnt_eq_15;
      endcase // case(sel_f)
   end

   always @(i_b or i_at_0_consume or i_at_0_noconsume) begin
      case (i_b)
	0:			i_at_0_<=i_at_0_consume;
	1:			i_at_0_<=i_at_0_noconsume;
      endcase // case(i_b)
   end
   
endmodule // zle_xc_dp9
