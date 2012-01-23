#ifndef _TDF_SEG_SEQCYCLICR_FSM_
#define _TDF_SEG_SEQCYCLICR_FSM_

const char* SEG_seq_cyclic_r_fsm = "\n\
// SEG_seqr_fsm.v\n\
// //\n\
// //  - A read-only segment FSM\n\
// //\n\
// //  - Nachiket Kapre,  22/Oct/11 Amended by Abid on 23/Oct/2011\n\
// \n\
// \n\
 `ifdef  SEG_seq_cyclic_r_fsm\n\
 `else\n\
 `define SEG_seq_cyclic_r_fsm\n\
// \n\
// \n\
module SEG_seq_cyclic_r_fsm (clock, reset, dataR_e, dataR_v, dataR_b, state, statecase, flag_steady_0, flag_steady_1);\n\
\n\
  input  clock;\n\
  input  reset;\n\
\n\
  output dataR_e;\n\
  output dataR_v;\n\
  input  dataR_b;\n\
\n\
  output state;\n\
  output [1:0] statecase;\n\
\n\
  input  flag_steady_0;\n\
  input  flag_steady_1;\n\
\n\
  parameter state_steady = 1'd0;\n\
  parameter state_done = 1'd1;\n\
\n\
  parameter statecase_stall = 2'd0;\n\
  parameter statecase_1 = 2'd1;\n\
  parameter statecase_2 = 2'd2;\n\
\n\
  reg state_reg, state_reg_;\n\
\n\
  reg [1:0] statecase_;\n\
\n\
  reg dataR_e_;\n\
  reg dataR_v_;\n\
\n\
  reg did_goto_;\n\
\n\
  assign dataR_e = dataR_e_;\n\
  assign dataR_v = dataR_v_;\n\
  assign state = state_reg;\n\
  assign statecase = statecase_;\n\
\n\
  always @(posedge clock or negedge reset)  begin\n\
    if (!reset)\n\
      state_reg<=state_steady;\n\
    else\n\
      state_reg<=state_reg_;\n\
  end  // always @(posedge...)\n\
\n\
  always @*  begin\n\
    dataR_e_ = 0;\n\
    dataR_v_ = 0;\n\
\n\
    state_reg_ = state_reg;\n\
    statecase_ = statecase_stall;\n\
    did_goto_ = 0;\n\
\n\
    case (state_reg)\n\
      state_steady:  begin\n\
        if (!dataR_b)  begin\n\
          statecase_ = statecase_1;\n\
          if (flag_steady_0)  begin\n\
            state_reg_ = state_done;\n\
          end\n\
          else  begin\n\
            dataR_v_ = 1;\n\
            dataR_e_ = 0;\n\
          end\n\
          state_reg_ = state_steady;\n\
        end\n\
      end\n\
      state_done:  begin\n\
        dataR_v_ = 0; // nothing to send anymore.. \n\
        dataR_e_ = 0;\n\
      end\n\
    endcase  // case (state_reg)\n\
\n\
  end  // always @*\n\
\n\
endmodule  // SEG_seq_cyclic_r_fsm\n\
\n\
\n\
`endif   // `ifdef SEG_seq_cyclic_r_fsm\n\
";

#endif  // #ifdef _TDF_SEG_SEQCYCLICR_FSM_
