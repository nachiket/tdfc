module memory (clock, reset, addr, data_in, data_out);

   input  clock;
   input  reset;
   input  [3:0] addr;
   input  [7:0] data_in;
   output [7:0] data_out;

   /****************************************************************
    ** NON-RESETABLE MEMORY WITH REGISTERED ADDRESS + DATA-IN
    ** (registered address is required for block-ram)
    */

// reg    [7:0] mem [15:0]  /* synthesis syn_ramstyle="select_ram" */ ;
   reg    [7:0] mem [15:0]  /* synthesis syn_ramstyle="block_ram" */ ;
   reg    [7:0]	data_in_reg;
   reg    [3:0]	addr_reg;
   assign 	data_out = mem[addr_reg];

   always @(posedge clock)  begin
      mem[addr_reg] <= data_in_reg;
      data_in_reg   <= data_in;
      addr_reg      <= addr;
   end


endmodule // memory

