#ifndef _TDF_SEG_RW_FSM_
#define _TDF_SEG_RW_FSM_

const char* SEG_rw_fsm = "\n\
// SEG_rw_fsm.v\n\
// //\n\
// //  - A read-write segment FSM\n\
// //\n\
// //  - Nachiket Kapre,  16/Oct/11\n\
// \n\
// \n\
// `ifdef  SEG_rw_fsm\n\
// `else\n\
// `define SEG_rw_fsm\n\
// \n\
// \n\
module SEG_rw_fsm (clock, reset, addr_e, addr_v, addr_b, dataR_e, dataR_v, dataR_b, dataW_e, dataW_v, dataW_b, write_e, write_v, write_b, state, statecase, flag_steady_0, flag_steady_1);\n\
\n\
  input  clock;\n\
  input  reset;\n\
\n\
  input  addr_e;\n\
  input  addr_v;\n\
  output addr_b;\n\
  output dataR_e;\n\
  output dataR_v;\n\
  input  dataR_b;\n\
  input  dataW_e;\n\
  input  dataW_v;\n\
  output dataW_b;\n\
  input  write_e;\n\
  input  write_v;\n\
  output write_b;\n\
\n\
  output state;\n\
  output [1:0] statecase;\n\
\n\
  input  flag_steady_0;\n\
  input  flag_steady_1;\n\
\n\
  parameter state_start = 1'd0;\n\
  parameter state_steady = 1'd1;\n\
\n\
  parameter statecase_stall = 2'd0;\n\
  parameter statecase_1 = 2'd1;\n\
  parameter statecase_2 = 2'd2;\n\
\n\
  reg state_reg, state_reg_;\n\
\n\
  reg [1:0] statecase_;\n\
\n\
  reg addr_b_;\n\
  reg dataR_e_;\n\
  reg dataR_v_;\n\
  reg dataW_b_;\n\
  reg write_b_;\n\
\n\
  reg did_goto_;\n\
\n\
  assign addr_b = addr_b_;\n\
  assign dataR_e = dataR_e_;\n\
  assign dataR_v = dataR_v_;\n\
  assign dataW_b = dataW_b_;\n\
  assign write_b = write_b_;\n\
  assign state = state_reg;\n\
  assign statecase = statecase_;\n\
\n\
  always @(posedge clock or negedge reset)  begin\n\
    if (!reset)\n\
      state_reg<=state_start;\n\
    else\n\
      state_reg<=state_reg_;\n\
  end  // always @(posedge...)\n\
\n\
  always @*  begin\n\
    addr_b_ = 1;\n\
    dataR_e_ = 0;\n\
    dataR_v_ = 0;\n\
    dataW_b_ = 1;\n\
    write_b_ = 1;\n\
\n\
    state_reg_ = state_reg;\n\
    statecase_ = statecase_stall;\n\
    did_goto_ = 0;\n\
\n\
    case (state_reg)\n\
      state_start:  begin\n\
        if (addr_v && !addr_e && dataW_v && !dataW_e && write_v && !write_e)  begin\n\
          statecase_ = statecase_1;\n\
          addr_b_ = 0;\n\
          dataW_b_ = 0;\n\
          write_b_ = 0;\n\
          state_reg_ = state_steady;\n\
          did_goto_ = 1;\n\
        end\n\
      end\n\
      state_steady:  begin\n\
        if (addr_v && !addr_e && dataW_v && !dataW_e && write_v && !write_e && !dataR_b)  begin\n\
          statecase_ = statecase_1;\n\
          addr_b_ = 0;\n\
          dataW_b_ = 0;\n\
          write_b_ = 0;\n\
          if (flag_steady_0)  begin\n\
          end\n\
          else  begin\n\
            dataR_v_ = 1;\n\
            dataR_e_ = 0;\n\
          end\n\
          state_reg_ = state_steady;\n\
          did_goto_ = 1;\n\
          if (!did_goto_) state_reg_ = state_steady;\n\
          did_goto_ = 1;\n\
        end\n\
        else if (!dataR_b)  begin\n\
          statecase_ = statecase_2;\n\
          if (flag_steady_1)  begin\n\
          end\n\
          else  begin\n\
            dataR_v_ = 1;\n\
            dataR_e_ = 0;\n\
          end\n\
          state_reg_ = state_start;\n\
          did_goto_ = 1;\n\
        end\n\
      end\n\
    endcase  // case (state_reg)\n\
\n\
  end  // always @*\n\
\n\
endmodule  // SEG_rw_fsm\n\
\n\
\n\
`endif   // `ifdef SEG_rw_fsm\n\
";

#endif  // #ifdef _TDF_SEG_RW_FSM_
