
#ifndef _TDF_SEG_SEQCYCLICR_DP_
#define _TDF_SEG_SEQCYCLICR_DP_

const char* SEG_seq_cyclic_r_dp = "\n\
// SEG_seq_cyclic_r_dp.v\n\
// //\n\
// //  - A sequential cyclic read-only segment datapath (BlockRAM)\n\
// //\n\
// //  - Nachiket Kapre,  22/Oct/11 Amended by Abid Rafique 23/Oct/11\n\
// \n\
// \n\
 `ifdef  SEG_seq_cyclic_r_dp\n\
 `else\n\
 `define SEG_seq_cyclic_r_dp\n\
// \n\
// \n\
module SEG_seq_cyclic_r_dp (clock, reset, dataR_d, state, statecase, flag_steady_0, flag_steady_1);\n\
\n\
  parameter nelems=0; \n\
  parameter dwidth=127;\n\
  parameter awidth=7;\n\
\n\
  input  clock;\n\
  input  reset;\n\
\n\
  output [dwidth-1:0] dataR_d;\n\
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
\n\
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
      addrreg <= 'b0;\n\
      datareg <= 'b0;\n\
    end\n\
    else  begin\n\
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
    addrreg_ = addrreg;\n\
    datareg_ = datareg;\n\
\n\
    did_goto_ = 0;\n\
\n\
    case (state)\n\
      state_steady:  begin\n\
        if (statecase == statecase_1)  begin\n\
	       	if(addrreg_ < nelems -1) begin\n\
			 addrreg_ = addrreg_+1; // TODO: Abid please add addrreg_<=N-1 check here.. \n\
		end \n\
		else begin\n\
			addrreg_ = 0; \n\
		end \n\
            dataR_d_ = contents[addrreg];		\n\
        //  end\n\
          did_goto_ = 1;\n\
        end\n\
      end\n\
    endcase  // case (state_reg)\n\
  end  // always @*\n\
\n\
\n\
endmodule // SEG_seq_cyclic_r_dp\n\
\n\
\n\
`endif  // `ifdef  SEG_seq_cyclic_r_dp\n\
";


#endif  // #ifndef _TDF_SEG_SEQR_DP_
