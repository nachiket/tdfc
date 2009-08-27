// Verilog FSM module for izigzag_d1_ScOrEtMp50
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module izigzag_d1_ScOrEtMp50_fsm (clock, reset, chuA_e, chuA_v, chuA_b, chuB_e, chuB_v, chuB_b, chuC_e, chuC_v, chuC_b, chuD_e, chuD_v, chuD_b, chuE_e, chuE_v, chuE_b, chuF_e, chuF_v, chuF_b, chuG_e, chuG_v, chuG_b, chuH_e, chuH_v, chuH_b, ruS_e, ruS_v, ruS_b, state, statecase, flag__1_0, flag__8_1, flag__8_0, flag__7_1, flag__7_0, flag__6_0, flag__5_1, flag__2_0, flag__1_1, flag__3_0, flag__3_1, flag__4_0, flag__5_0);

  input  clock;
  input  reset;

  output chuA_e;
  output chuA_v;
  input  chuA_b;
  output chuB_e;
  output chuB_v;
  input  chuB_b;
  output chuC_e;
  output chuC_v;
  input  chuC_b;
  output chuD_e;
  output chuD_v;
  input  chuD_b;
  output chuE_e;
  output chuE_v;
  input  chuE_b;
  output chuF_e;
  output chuF_v;
  input  chuF_b;
  output chuG_e;
  output chuG_v;
  input  chuG_b;
  output chuH_e;
  output chuH_v;
  input  chuH_b;
  input  ruS_e;
  input  ruS_v;
  output ruS_b;

  output [2:0] state;
  output statecase;

  input  flag__1_0;
  input  flag__8_1;
  input  flag__8_0;
  input  flag__7_1;
  input  flag__7_0;
  input  flag__6_0;
  input  flag__5_1;
  input  flag__2_0;
  input  flag__1_1;
  input  flag__3_0;
  input  flag__3_1;
  input  flag__4_0;
  input  flag__5_0;

  parameter state__1 = 3'd0;
  parameter state__2 = 3'd1;
  parameter state__3 = 3'd2;
  parameter state__4 = 3'd3;
  parameter state__5 = 3'd4;
  parameter state__6 = 3'd5;
  parameter state__7 = 3'd6;
  parameter state__8 = 3'd7;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [2:0] state_reg, state_reg_;

  reg statecase_;

  reg chuA_e_;
  reg chuA_v_;
  reg chuB_e_;
  reg chuB_v_;
  reg chuC_e_;
  reg chuC_v_;
  reg chuD_e_;
  reg chuD_v_;
  reg chuE_e_;
  reg chuE_v_;
  reg chuF_e_;
  reg chuF_v_;
  reg chuG_e_;
  reg chuG_v_;
  reg chuH_e_;
  reg chuH_v_;
  reg ruS_b_;

  reg did_goto_;

  assign chuA_e = chuA_e_;
  assign chuA_v = chuA_v_;
  assign chuB_e = chuB_e_;
  assign chuB_v = chuB_v_;
  assign chuC_e = chuC_e_;
  assign chuC_v = chuC_v_;
  assign chuD_e = chuD_e_;
  assign chuD_v = chuD_v_;
  assign chuE_e = chuE_e_;
  assign chuE_v = chuE_v_;
  assign chuF_e = chuF_e_;
  assign chuF_v = chuF_v_;
  assign chuG_e = chuG_e_;
  assign chuG_v = chuG_v_;
  assign chuH_e = chuH_e_;
  assign chuH_v = chuH_v_;
  assign ruS_b = ruS_b_;
  assign state = state_reg;
  assign statecase = statecase_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)
      state_reg<=state__1;
    else
      state_reg<=state_reg_;
  end  // always @(posedge...)

  always @*  begin
    chuA_e_ = 0;
    chuA_v_ = 0;
    chuB_e_ = 0;
    chuB_v_ = 0;
    chuC_e_ = 0;
    chuC_v_ = 0;
    chuD_e_ = 0;
    chuD_v_ = 0;
    chuE_e_ = 0;
    chuE_v_ = 0;
    chuF_e_ = 0;
    chuF_v_ = 0;
    chuG_e_ = 0;
    chuG_v_ = 0;
    chuH_e_ = 0;
    chuH_v_ = 0;
    ruS_b_ = 1;

    state_reg_ = state_reg;
    statecase_ = statecase_stall;
    did_goto_ = 0;

    case (state_reg)
      state__1:  begin
        if (ruS_v && !ruS_e && !chuA_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuA_v_ = 1;
            chuA_e_ = 0;
            if (flag__1_0)  begin
              begin
                state_reg_ = state__2;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (flag__1_1)  begin
                  state_reg_ = state__2;
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__2:  begin
        if (ruS_v && !ruS_e && !chuB_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuB_v_ = 1;
            chuB_e_ = 0;
            if (flag__2_0)  begin
              begin
                state_reg_ = state__1;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                state_reg_ = state__3;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state__3:  begin
        if (ruS_v && !ruS_e && !chuC_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuC_v_ = 1;
            chuC_e_ = 0;
            if (flag__3_0)  begin
              begin
                state_reg_ = state__4;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (flag__3_1)  begin
                  state_reg_ = state__4;
                  did_goto_ = 1;
                end
                else  begin
                  state_reg_ = state__2;
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__4:  begin
        if (ruS_v && !ruS_e && !chuD_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuD_v_ = 1;
            chuD_e_ = 0;
            if (flag__4_0)  begin
              begin
                state_reg_ = state__3;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                state_reg_ = state__5;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state__5:  begin
        if (ruS_v && !ruS_e && !chuE_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuE_v_ = 1;
            chuE_e_ = 0;
            if (flag__5_0)  begin
              begin
                state_reg_ = state__6;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (flag__5_1)  begin
                  state_reg_ = state__6;
                  did_goto_ = 1;
                end
                else  begin
                  state_reg_ = state__4;
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__6:  begin
        if (ruS_v && !ruS_e && !chuF_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuF_v_ = 1;
            chuF_e_ = 0;
            if (flag__6_0)  begin
              begin
                state_reg_ = state__5;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                state_reg_ = state__7;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state__7:  begin
        if (ruS_v && !ruS_e && !chuG_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuG_v_ = 1;
            chuG_e_ = 0;
            if (flag__7_0)  begin
              begin
                state_reg_ = state__8;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (flag__7_1)  begin
                  state_reg_ = state__8;
                  did_goto_ = 1;
                end
                else  begin
                  state_reg_ = state__6;
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__8:  begin
        if (ruS_v && !ruS_e && !chuH_b)  begin
          statecase_ = statecase_1;
          ruS_b_ = 0;
          begin
            chuH_v_ = 1;
            chuH_e_ = 0;
            if (flag__8_0)  begin
              begin
                state_reg_ = state__7;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (flag__8_1)  begin
                  begin
                    state_reg_ = state__1;
                    did_goto_ = 1;
                  end
                end
                else  begin
                  state_reg_ = state__8;
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
    endcase  // case (state_reg)

  end  // always @*

endmodule  // izigzag_d1_ScOrEtMp50_fsm
