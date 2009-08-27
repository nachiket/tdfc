module memory (clock, reset, addr, data_in, data_out);

   input  clock;
   input  reset;
   input  [3:0] addr;
   input  [7:0] data_in;
   output [7:0] data_out;
   
   /****************************************************************
    ** ROM
    */
   
   reg    [7:0] data_out;		// - redundant with output data_out
   always @* begin
      case (addr)
	 0 : data_out =   0;
	 1 : data_out =  10;
	 2 : data_out =  20;
	 3 : data_out =  30;
	 4 : data_out =  40;
	 5 : data_out =  50;
	 6 : data_out =  60;
	 7 : data_out =  70;
	 8 : data_out =  80;
	 9 : data_out =  90;
	10 : data_out = 100;
	11 : data_out = 110;
	12 : data_out = 120;
	13 : data_out = 130;
	14 : data_out = 140;
	15 : data_out = 150;
      endcase
   end

endmodule // memory

