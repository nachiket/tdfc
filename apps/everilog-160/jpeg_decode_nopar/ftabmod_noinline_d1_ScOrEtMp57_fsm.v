// Verilog FSM module for ftabmod_noinline_d1_ScOrEtMp57
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module ftabmod_noinline_d1_ScOrEtMp57_fsm (clock, reset, ind_e, ind_v, ind_b, oval_e, oval_v, oval_b, segment_r_addr_e, segment_r_addr_v, segment_r_addr_b, segment_r_data_e, segment_r_data_v, segment_r_data_b, state, statecase);

  input  clock;
  input  reset;

  input  ind_e;
  input  ind_v;
  output ind_b;
  output oval_e;
  output oval_v;
  input  oval_b;
  output segment_r_addr_e;
  output segment_r_addr_v;
  input  segment_r_addr_b;
  input  segment_r_data_e;
  input  segment_r_data_v;
  output segment_r_data_b;

  output state;
  output statecase;

  parameter state_only = 1'd0;
  parameter state_only_1 = 1'd1;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg state_reg, state_reg_;

  reg statecase_;

  reg ind_b_;
  reg oval_e_;
  reg oval_v_;
  reg segment_r_addr_e_;
  reg segment_r_addr_v_;
  reg segment_r_data_b_;

  reg did_goto_;

  assign ind_b = ind_b_;
  assign oval_e = oval_e_;
  assign oval_v = oval_v_;
  assign segment_r_addr_e = segment_r_addr_e_;
  assign segment_r_addr_v = segment_r_addr_v_;
  assign segment_r_data_b = segment_r_data_b_;
  assign state = state_reg;
  assign statecase = statecase_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)
      state_reg<=state_only;
    else
      state_reg<=state_reg_;
  end  // always @(posedge...)

  always @*  begin
    ind_b_ = 1;
    oval_e_ = 0;
    oval_v_ = 0;
    segment_r_addr_e_ = 0;
    segment_r_addr_v_ = 0;
    segment_r_data_b_ = 1;

    state_reg_ = state_reg;
    statecase_ = statecase_stall;
    did_goto_ = 0;

    case (state_reg)
      state_only:  begin
        if (ind_v && !ind_e && !segment_r_addr_b)  begin
          statecase_ = statecase_1;
          ind_b_ = 0;
          begin
            segment_r_addr_v_ = 1;
            segment_r_addr_e_ = 0;
            state_reg_ = state_only_1;
            did_goto_ = 1;
          end
        end
      end
      state_only_1:  begin
        if (segment_r_data_v && !segment_r_data_e && !oval_b)  begin
          statecase_ = statecase_1;
          segment_r_data_b_ = 0;
          begin
            oval_v_ = 1;
            oval_e_ = 0;
            state_reg_ = state_only;
            did_goto_ = 1;
          end
        end
      end
    endcase  // case (state_reg)

  end  // always @*

endmodule  // ftabmod_noinline_d1_ScOrEtMp57_fsm
