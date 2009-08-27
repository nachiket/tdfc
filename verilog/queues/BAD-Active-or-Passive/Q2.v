// Depth 2 queue, cascaded as { Q Q1 }


module Q2 (clk,reset, Xd,Xv,Xb, Yd,Yv,Yb);

   parameter W = 8;		// - data width
   
   input  clk;
   input  reset;
   input  [W-1:0] Xd;		// - input stream X  - data
   input 	  Xv;		//                   - valid
   output 	  Xb;		//                   - backpressure
   output [W-1:0] Yd;		// - output stream Y - data
   output 	  Yv;		//                   - valid
   input 	  Yb;		//                   - backpressure

   wire   [W-1:0] Ad;		// - intermediate stream X
   wire           Av, Ab;

   Q  #(W) qa_ (clk,reset, Xd,Xv,Xb, Ad,Av,Ab);
   Q1 #(W) qb_ (clk,reset, Ad,Av,Ab, Yd,Yv,Yb);

   //  --(X)--> qa_ --(A)--> qb_ --(Y)-->

endmodule // Q2

