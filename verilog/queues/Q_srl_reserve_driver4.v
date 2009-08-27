// Q_srl_reserve_driver4.v
//
//  - Test driver for Q_srl_reserve.v
//
//  - Synplify 7.1,  ModelSim 5.7d
//  - Eylon Caspi,  9/15/03, 7/21/04


// `include "Q_srl_reserve.v"


module Q_srl_reserve_driver;

   reg  [15:0] X_d;		// - input stream of Q_srl_reserve
   reg        X_v;
   wire       X_b;

   wire [15:0] Y_d;		// - output stream of Q_srl_reserve
   wire       Y_v;
   reg        Y_b;

   reg       clock, reset;

   Q_srl_reserve #(3, 16, 1) q (clock, reset, X_d, X_v, X_b, Y_d, Y_v, Y_b);
   // 3 deep x 16 bit x 1 reserve

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
      #1000  reset=0;  X_d=1000;  X_v=0;  Y_b=1;	// idle			--> cap=0;  X_b=0;  Y_v=0;  Y_d=X;
      #2000  reset=1;  X_d=1001;  X_v=0;  Y_b=1;	// idle			--> cap=0;  X_b=0;  Y_v=0;  Y_d=X;
      #2000  reset=1;  X_d=1002;  X_v=0;  Y_b=1;	// idle			--> cap=0;  X_b=0;  Y_v=0;  Y_d=X;
      #2000  reset=1;  X_d=1003;  X_v=1;  Y_b=1;	// cons 1003		--> cap=1;  X_b=0;  Y_v=1;  Y_d=1003;
      #2000  reset=1;  X_d=1004;  X_v=0;  Y_b=0;	//            prod 1003	--> cap=0;  X_b=0;  Y_v=0;  Y_d=X; (1003)
      #2000  reset=1;  X_d=1005;  X_v=0;  Y_b=0;	// underflow		--> cap=0;  X_b=0;  Y_v=0;  Y_d=X; (1003)
      #2000  reset=1;  X_d=1006;  X_v=1;  Y_b=0;	// cons 1006, underflow	--> cap=1;  X_b=0;  Y_v=1;  Y_d=1006;
      #2000  reset=1;  X_d=1007;  X_v=1;  Y_b=0;	// cons 1007, prod 1006	--> cap=1;  X_b=0;  Y_v=1;  Y_d=1007;
      #2000  reset=1;  X_d=1008;  X_v=1;  Y_b=1;	// cons 1008		--> cap=2;  X_b=1;  Y_v=1;  Y_d=1007;
      #2000  reset=1;  X_d=1009;  X_v=1;  Y_b=1;	// cons 1009		--> cap=3;  X_b=1;  Y_v=1;  Y_d=1007;
      #2000  reset=1;  X_d=1010;  X_v=0;  Y_b=0;	//            prod 1007	--> cap=2;  X_b=1;  Y_v=1;  Y_d=1008;
      #2000  reset=1;  X_d=1011;  X_v=1;  Y_b=0;	// cons 1011, prod 1008	--> cap=2;  X_b=1;  Y_v=1;  Y_d=1009;
      #2000  reset=1;  X_d=1012;  X_v=0;  Y_b=0;	//            prod 1009	--> cap=1;  X_b=0;  Y_v=1;  Y_d=1011;
      #2000  reset=1;  X_d=1013;  X_v=0;  Y_b=0;	//     	      prod 1011	--> cap=0;  X_b=0;  Y_v=0;  Y_d=X; (1011)
      #2000  reset=1;  X_d=1014;  X_v=0;  Y_b=1;	// idle			--> cap=0;  X_b=0;  Y_v=0;  Y_d=X; (1011)
      #2000  reset=1;  X_d=1015;  X_v=1;  Y_b=1;	// cons 1015		--> cap=1;  X_b=0;  Y_v=1;  Y_d=1015;
      #2000  reset=1;  X_d=1016;  X_v=0;  Y_b=1;	// idle			--> cap=1;  X_b=0;  Y_v=1;  Y_d=1015;
      #2000  reset=1;  X_d=1017;  X_v=0;  Y_b=0;	//            prod 1015	--> cap=0;  X_b=0;  Y_v=0;  Y_d=X; (1015)
      #2000  reset=1;  X_d=1018;  X_v=0;  Y_b=1;	// idle			--> cap=0;  X_b=0;  Y_v=0;  Y_d=X; (1015)
      #2000  reset=1;  X_d=1019;  X_v=0;  Y_b=1;	// idle			--> cap=0;  X_b=0;  Y_v=0;  Y_d=X; (1015)
      #2000  $stop;
   end // initial begin
   
endmodule // Q_srl_driver
