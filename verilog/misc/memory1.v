module memory (clock, reset, addr, data_in, data_out);

   input  clock;
   input  reset;
   input  [3:0] addr;
   input  [7:0] data_in;
   output [7:0] data_out;

   /****************************************************************
    ** RESETABLE MEMORY
    */

   reg    [7:0] mem [15:0]  /* synthesis syn_ramstyle="select_ram" */ ;
   assign 	data_out = mem[addr];
   integer 	a_;

   always @(posedge clock, negedge reset)  begin
      if (!reset) begin
	 for (a_=0; a_<=15; a_=a_+1) begin
	    mem[a_] <= 0;
	 end
	 /*
	 mem[ 0] <=   0;
	 mem[ 1] <=  10;
	 mem[ 2] <=  20;
	 mem[ 3] <=  30;
	 mem[ 4] <=  40;
	 mem[ 5] <=  50;
	 mem[ 6] <=  60;
	 mem[ 7] <=  70;
	 mem[ 8] <=  80;
	 mem[ 9] <=  90;
	 mem[10] <= 100;
	 mem[11] <= 110;
	 mem[12] <= 120;
	 mem[13] <= 130;
	 mem[14] <= 140;
	 mem[15] <= 150;
	 */
      end
      else begin
	 mem[addr] <= data_in;
      end
   end

endmodule // memory

