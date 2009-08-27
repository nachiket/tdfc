// - Cascadable queue stage, depth 2
// - To be active on input and output,
//     stage accepts input (deasserts backpressure) only if empty

module Q2 (clock, reset, X_d, X_e, X_v, X_b, Y_d, Y_e, Y_v, Y_b);

   parameter       W = 8;	// - data width (bits)

   input 	   clock;
   input 	   reset;
   
   input  [W-1:0]  X_d;		// - input  stream:  data
   input 	   X_e;		//		     eos
   input 	   X_v;		//		     valid
   output 	   X_b;		//		     backpressure

   output [W-1:0]  Y_d;		// - output stream:  data
   output 	   Y_e;		//		     eos
   output 	   Y_v;		//		     valid
   input 	   Y_b;		//		     backpressure

   reg    [W-1:0]  r_d, s_d;	// - queue regs:     data
   reg 		   r_e, s_e;	//		     eos
   reg 		   r_v, s_v;	//		     valid

   reg    [W-1:0]  r_d_, s_d_;	// - always assignables
   reg  	   r_e_, s_e_;
   reg  	   r_v_, s_v_;
   reg    [W-1:0]  Y_d_;
   reg 		   Y_e_;
   reg 		   Y_v_;
   reg 		   X_b_;

   assign 	   Y_d = Y_d_;
   assign 	   Y_e = Y_e_;
   assign 	   Y_v = Y_v_;
   assign 	   X_b = X_b_;

   always @(posedge clock or negedge reset) begin
      if (!reset) begin
	 r_d<=0;     r_e<=0;     r_v<=0;     
	 s_d<=0;     s_e<=0;     s_v<=0;     
      end
      else begin
	 r_d<=r_d_;  r_e<=r_e_;  r_v<=r_v_;
	 s_d<=s_d_;  s_e<=s_e_;  s_v<=s_v_;
      end
   end

   always @(X_v or X_d or X_e or Y_b or
	    r_v or r_d or r_e or
	    s_v or s_d or s_e)
   begin
      case ({r_v,s_v})
	2'b00:	begin				// - empty
		  Y_v_ <= 0;
		  Y_d_ <= {W{1'bx}};
		  Y_e_ <= 1'bx;
		  X_b_ <= 0;
		  // consume = X_v;  emit = 0;
		  r_v_ <= X_v ? 1   : 0;
		  r_d_ <= X_v ? X_d : r_d;
		  r_e_ <= X_v ? X_e : r_e;
		  s_v_ <= 0;
		  s_d_ <= {W{1'bx}};
		  s_e_ <= 1'bx;
		end // case: 2'b00

	2'b10:	begin				// - r full, s empty
		  Y_v_ <= 1;
		  Y_d_ <= r_d;
		  Y_e_ <= r_e;
		  X_b_ <= 0;
		  // consume = X_v;  emit = !Y_b;
		  case ({X_v,!Y_b})
		    2'b00:	begin		// -- no I/O
				  r_v_<=1; r_d_<=r_d;       r_e_<=r_e;
				  s_v_<=0; s_d_<={W{1'bx}}; s_e_<=1'bx;
				end
		    2'b10:	begin		// -- consume s (spill)
				  r_v_<=1; r_d_<=r_d; r_e_<=r_e;
				  s_v_<=1; s_d_<=X_d; s_e_<=X_e;
				end
		    2'b01:	begin		// -- emit r
				  r_v_<=0; r_d_<={W{1'bx}}; r_e_<=1'bx;
				  s_v_<=0; s_d_<={W{1'bx}}; s_e_<=1'bx;
				end
		    2'b11:	begin		// -- consume + emit r
				  r_v_<=1; r_d_<=X_d;       r_e_<=X_e;
				  s_v_<=0; s_d_<={W{1'bx}}; s_e_<=1'bx;
				end
		  endcase // case({X_v,!Y_b})
		end // case: 2'b10

	2'b11:	begin				// - r & s full
		  Y_v_ <= 1;
		  Y_d_ <= r_d;
		  Y_e_ <= r_e;
		  X_b_ <= 1;
		  // consume = 0;  emit = !Y_b;
		  r_v_ <= (!Y_b) ? 1         : 1;
		  r_d_ <= (!Y_b) ? s_d       : r_d;
		  r_e_ <= (!Y_b) ? s_e       : r_e;
		  s_v_ <= (!Y_b) ? 0         : 1;
		  s_d_ <= (!Y_b) ? {W{1'bx}} : s_d;
		  s_e_ <= (!Y_b) ? 1'bx      : s_e;
		end // case: 2'b11

	2'b01:	begin				// - impossible
		  Y_v_ <= 1'bx;
		  Y_d_ <= {W{1'bx}};
		  Y_e_ <= 1'bx;
		  X_b_ <= 1'bx;
		  // consume = 1'bx;  emit = 1'bx
		  r_v_ <= 1'bx;
		  r_d_ <= {W{1'bx}};
		  r_e_ <= 1'bx;
		  s_v_ <= 1'bx;
		  s_d_ <= {W{1'bx}};
		  s_e_ <= 1'bx;
		end // case: 2'b01

      endcase // case({r_v,s_v})

   end // always @ (X_v or X_d or X_e or r_d or r_e or s_d or s_e)
   
endmodule // Q2
