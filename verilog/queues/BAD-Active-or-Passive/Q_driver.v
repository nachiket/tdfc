// Test for Q.v


`include "Q.v"

module top;

   parameter W=8;
   wire clk, rst;
   wire Xv, Xb, Yv, Yb;
   wire [W-1:0] Xd, Yd;

   reg 		done;

   Q #(8) q_ (clk,rst, Xd,Xv,Xb, Yd,Yv,Yb);

   initial begin
      clk=1;
      forever begin
	 #1000 clk=!clk;
	 $write("\n");
	 $monitor("%6d   rst=%d   Xd=%d Xv=%d Xb=%d   Yd=%d Yv=%d Yb=%d\n",
		  $time,rst,Xd,Xv,Xb,Yd,Yv,Yb);
      end
   end

   initial begin
      #1000  rst=1;  Xd=100;  Xv=0;  Yb=1;
      #2000  rst=0;  Xd=101;  Xv=0;  Yb=1;
      #2000  rst=0;  Xd=102;  Xv=0;  Yb=1;
      #2000  rst=0;  Xd=103;  Xv=1;  Yb=1;	// - Q fires
      #2000  rst=0;  Xd=104;  Xv=1;  Yb=1;
      #2000  rst=0;  Xd=105;  Xv=1;  Yb=1;
      #2000  rst=0;  Xd=106;  Xv=1;  Yb=0;	// - Q emits + fires
      #2000  rst=0;  Xd=107;  Xv=1;  Yb=1;
      #2000  rst=0;  Xd=108;  Xv=1;  Yb=1;
      $finish;
   end
   
endmodule // top
