// Depth 3 queue, cascaded as { Q Q Q1 }


module Q3 (clk,reset, Xd,Xv,Xb, Yd,Yv,Yb);

   parameter W = 8;		// - data width
   
   input  clk;
   input  reset;
   input  [W-1:0] Xd;		// - input stream X  - data
   input 	  Xv;		//                   - valid
   output 	  Xb;		//                   - backpressure
   output [W-1:0] Yd;		// - output stream Y - data
   output 	  Yv;		//                   - valid
   input 	  Yb;		//                   - backpressure

   wire   [W-1:0] Ad, Bd;		// - intermediate streams A, B
   wire           Av, Bv, Ab, Bb;

   Q  #(W) qa_ (clk,reset, Xd,Xv,Xb, Ad,Av,Ab);
   Q  #(W) qb_ (clk,reset, Ad,Av,Ab, Bd,Bv,Bb);
   Q1 #(W) qc_ (clk,reset, Bd,Bv,Bb, Yd,Yv,Yb);

   //  --(X)--> qa_ --(A)--> qb_ --(B)--> qc_ --(Y)-->

endmodule // Q3

