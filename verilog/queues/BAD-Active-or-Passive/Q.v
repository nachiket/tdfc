// Cascadable queue stage, no combinational feed-through
//
// - bug:  the only way to conservatively guarantee low backpressure
//         for next cycle (b=0) is if empty next cycle (v=0),
//         in which case will alternate cycles of input, no input
//         (i.e. equation b=... is bogus).
//         Instead, want to allow inputting even if full next cycle,
//         by spilling into another stage.  Maybe {Q,Q1} is good stage...

module Q (clk,reset, Xd,Xv,Xb, Yd,Yv,Yb);

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
   reg 		  b;		//                   - backpressure

   wire 	  fire, next_v, next_b, Xb, Yv;
   wire   [W-1:0] Yd;

   assign 	  fire   = Xv && (!v || !Yb);
   assign 	  next_v = fire || (v && Yb);
   assign 	  next_b =         (v || Yb);	// - *** FIX PPT
   assign 	  Xb     = b;
   assign	  Yv     = v;
   assign	  Yd     = d;
  
   always @(posedge clk)
     begin
	if (reset) begin
	   b <= 1;
	   v <= 0;
	   // d <= 0;
	   // - d does not need reset
	end
	else begin
	  b <= next_b;
	  v <= next_v;
	  d <= fire ? Xd : d;
	end
     end

endmodule // Q
