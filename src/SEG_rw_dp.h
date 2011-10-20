
#ifndef _TDF_SEG_RW_DP_
#define _TDF_SEG_RW_DP_

const char* SEG_rw_dp = "\n\
\n\
\n\
module SEG_rw_dp (clock, reset, addr_d, dataR_d, dataW_d, write_d, state, statecase, flag_steady_0, flag_steady_1);\n\
\n\
  parameter nelems=0; (unused?)\n\
  parameter dwidth=127;\n\
  parameter awidth=7;\n\
\n\
  input  clock;\n\
  input  reset;\n\
\n\
  input  [awidth-1:0] addr_d;\n\
  output [dwidth-1:0] dataR_d;\n\
  input  [dwidth-1:0] dataW_d;\n\
  input  write_d;\n\
\n\
  input  state;\n\
  input  [1:0] statecase;\n\
\n\
  output flag_steady_0;\n\
  output flag_steady_1;\n\
\n\
  parameter state_start = 1'd0;\n\
  parameter state_steady = 1'd1;\n\
\n\
  parameter statecase_stall = 2'd0;\n\
  parameter statecase_1 = 2'd1;\n\
  parameter statecase_2 = 2'd2;\n\
\n\
  reg [dwidth-1:0] contents_at_addrreg_;\n\
  reg [dwidth-1:0] contents [nelems-1:0];\n\
  reg 	    en_;\n\
\n\
  reg writereg, writereg_;\n\
  reg [awidth-1:0] addrreg, addrreg_;\n\
  reg [dwidth-1:0] datareg, datareg_;\n\
\n\
  reg [dwidth-1:0] dataR_d_;\n\
\n\
  reg flag_steady_0_;\n\
  reg flag_steady_1_;\n\
\n\
  reg did_goto_;\n\
\n\
  assign dataR_d = dataR_d_;\n\
\n\
  assign flag_steady_0 = flag_steady_0_;\n\
  assign flag_steady_1 = flag_steady_1_;\n\
\n\
  always @(posedge clock or negedge reset)  begin\n\
    if (!reset)  begin\n\
      writereg <= 'b0;\n\
      addrreg <= 'b0;\n\
      datareg <= 'b0;\n\
    end\n\
    else  begin\n\
      if (en_) begin contents[addrreg] <= contents_at_addrreg_; end;	// +EC\n\
      writereg <= writereg_;\n\
      addrreg <= addrreg_;\n\
      datareg <= datareg_;\n\
    end\n\
  end  // always @(posedge...)\n\
\n\
  always @*  begin\n\
    dataR_d_ = 'b0;\n\
\n\
    flag_steady_0_ = 1'bx;\n\
    flag_steady_1_ = 1'bx;\n\
\n\
    writereg_ = writereg;\n\
    addrreg_ = addrreg;\n\
    datareg_ = datareg;\n\
\n\
    en_       = 0;\n\
\n\
    did_goto_ = 0;\n\
\n\
    case (state)\n\
      state_start:  begin\n\
        if (statecase == statecase_1)  begin\n\
          addrreg_ = addr_d;\n\
          datareg_ = dataW_d;\n\
          writereg_ = write_d;\n\
          did_goto_ = 1;\n\
        end\n\
      end\n\
      state_steady:  begin\n\
        if (statecase == statecase_1)  begin\n\
          flag_steady_0_ = writereg_;\n\
          if (flag_steady_0_)  begin\n\
            contents_at_addrreg_ = datareg_;\n\
	    en_ = 1;					\n\
          end\n\
          else  begin\n\
            dataR_d_ = contents[addrreg];		\n\
          end\n\
          addrreg_ = addr_d;\n\
          datareg_ = dataW_d;\n\
          writereg_ = write_d;\n\
          did_goto_ = 1;\n\
          did_goto_ = 1;\n\
        end\n\
        else if (statecase == statecase_2)  begin\n\
          flag_steady_1_ = writereg_;\n\
          if (flag_steady_1_)  begin\n\
            contents_at_addrreg_ = datareg_;\n\
	    en_ = 1;					\n\
          end\n\
          else  begin\n\
            dataR_d_ = contents[addrreg];		\n\
          end\n\\n\
          did_goto_ = 1;\n\
        end\n\
      end\n\
    endcase  // case (state_reg)\n\
  end  // always @*\n\
\n\
\n\
endmodule // SEG_rw_dp\n\
\n\
\n\
`endif  // `ifdef  SEG_rw_dp\n\
";


#endif  // #ifndef _TDF_SEG_RW_DP_
