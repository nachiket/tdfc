module memory (clock, reset, addr, data_in, data_out);

   input  clock;
   input  reset;
   input  [3:0] addr;
   input  [7:0] data_in;
   output [7:0] data_out;

   /****************************************************************
    ** NON-RESETABLE MEMORY
    */

   reg    [7:0] mem [15:0]  /* synthesis syn_ramstyle="select_ram" */ ;
   assign 	data_out = mem[addr];

   always @(posedge clock)  begin
      mem[addr] <= data_in;
   end


endmodule // memory

