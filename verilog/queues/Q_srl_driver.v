// Q_srl_driver.v
//
//  - Test driver for Q_srl.v
//
//  - Synplify 7.1,  ModelSim 5.7d
//  - Eylon Caspi,  9/15/03


// `include "Q_srl.v"


module Q_srl_driver;

   reg  [15:0] X_d;		// - input stream of Q_srl
   reg        X_v;
   wire       X_b;

   wire [15:0] Y_d;		// - output stream of Q_srl
   wire       Y_v;
   reg        Y_b;

   reg       clock, reset;

   Q_srl #(2, 16) q (clock, reset, X_d, X_v, X_b, Y_d, Y_v, Y_b); // 2 x 16 bit

   initial begin
      clock=1;
      forever begin
	 #1000 clock=!clock;
	 $write("\n");
         $monitor("%6d   reset=%d   X_d=%d X_v=%d X_b=%d   Y_d=%d Y_v=%d Y_b=%d\n",
                  $time, reset, X_d, X_v, X_b, Y_d, Y_v, Y_b);
      end
   end
   
   initial begin
      #1000  reset=0;  X_d=1000;  X_v=0;  Y_b=1;	// (0)
      #2000  reset=1;  X_d=1001;  X_v=0;  Y_b=1;	// (0)
      #2000  reset=1;  X_d=1002;  X_v=0;  Y_b=1;	// (0)
      #2000  reset=1;  X_d=1003;  X_v=1;  Y_b=1;	// (0) consume 1003 (1)
      #2000  reset=1;  X_d=1004;  X_v=0;  Y_b=0;	// (1) produce 1003 (0)
      #2000  reset=1;  X_d=1005;  X_v=0;  Y_b=0;	// (0) underflow
      #2000  reset=1;  X_d=1006;  X_v=1;  Y_b=0;	// (0) consume 1006
							//     underflow    (1)
      #2000  reset=1;  X_d=1007;  X_v=1;  Y_b=0;	// (1) consume 1007
							//     produce 1006 (1)
      #2000  reset=1;  X_d=1008;  X_v=1;  Y_b=1;	// (1) consume 1008 (2)
      #2000  reset=1;  X_d=1009;  X_v=1;  Y_b=1;	// (2) overflow     (2)
      #2000  reset=1;  X_d=1010;  X_v=0;  Y_b=0;	// (2) produce 1007 (1)
      #2000  reset=1;  X_d=1011;  X_v=1;  Y_b=0;	// (1) consume 1011
							//     produce 1008 (1)
      #2000  reset=1;  X_d=1012;  X_v=0;  Y_b=0;	// (1) produce 1011 (0)
      #2000  reset=1;  X_d=1013;  X_v=0;  Y_b=0;	// (0) underflow    (0)
      #2000  reset=1;  X_d=1014;  X_v=0;  Y_b=1;	// (0)
      #2000  reset=1;  X_d=1015;  X_v=1;  Y_b=1;	// (0) consume 1015 (1)
      #2000  reset=1;  X_d=1016;  X_v=0;  Y_b=1;	// (1)
      #2000  reset=1;  X_d=1017;  X_v=0;  Y_b=0;	// (1) produce 1015 (0)
      #2000  reset=1;  X_d=1018;  X_v=0;  Y_b=1;	// (0)
      #2000  reset=1;  X_d=1019;  X_v=0;  Y_b=1;	// (0)
      #2000  $stop;
   end // initial begin
   
endmodule // Q_srl_driver
