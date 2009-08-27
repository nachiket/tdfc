// Cascadable queue stage, combinational back-pressure feed-through


module Q1 (clk,reset, Xd,Xv,Xb, Yd,Yv,Yb);

   parameter W = 8;		// - data width
   
   input  clk;
   input  reset;
   input  [W-1:0] Xd;		// - input stream X  - data
   input 	  Xv;		//                   - valid
   output 	  Xb;		//                   - backpressure
   output [W-1:0] Yd;		// - output stream Y - data
   output 	  Yv;		//                   - valid
   input 	  Yb;		//                   - backpressure

   reg    [W-1:0] d;		// - queued token    - data
   reg 		  v;		//                   - valid

   wire 	  fire, next_v, Xb, Yv;
   wire   [W-1:0] Yd;

   assign 	  fire   = Xv && (!v || !Yb);
   assign 	  next_v = fire || (v && Yb);
   assign 	  Xb     = !fire;
   assign	  Yv     = v;
   assign	  Yd     = d;
  
   always @(posedge clk)
     begin
	if (reset) begin
	   v <= 0;
	   // d <= 0;
	   // - d does not need reset
	end
	else begin
	  v <= next_v;
	  d <= fire ? Xd : d;
	end
     end

endmodule // Q1
