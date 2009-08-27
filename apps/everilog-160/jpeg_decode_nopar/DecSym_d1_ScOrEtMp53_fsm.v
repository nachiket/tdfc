// Verilog FSM module for DecSym_d1_ScOrEtMp53
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module DecSym_d1_ScOrEtMp53_fsm (clock, reset, parsToken_e, parsToken_v, parsToken_b, setIncr_e, setIncr_v, setIncr_b, getReq_e, getReq_v, getReq_b, decToken_e, decToken_v, decToken_b, ftabind_e, ftabind_v, ftabind_b, ftabval_e, ftabval_v, ftabval_b, state, statecase, flag_DecAC1_0, flag_spitZero_3, flag_spitZero_2, flag_spitZero_1, flag_spitZero_0, flag_DecRS__0, flag_DecRS_1, flag_DecRS_0, flag_DecACLen_0, flag_DecAC2_0, flag_DecDC_0, flag_DecDC_1, flag_DecDC_2, flag_DecDCLen_0);

  input  clock;
  input  reset;

  input  parsToken_e;
  input  parsToken_v;
  output parsToken_b;
  output setIncr_e;
  output setIncr_v;
  input  setIncr_b;
  output getReq_e;
  output getReq_v;
  input  getReq_b;
  output decToken_e;
  output decToken_v;
  input  decToken_b;
  output ftabind_e;
  output ftabind_v;
  input  ftabind_b;
  input  ftabval_e;
  input  ftabval_v;
  output ftabval_b;

  output [3:0] state;
  output statecase;

  input  flag_DecAC1_0;
  input  flag_spitZero_3;
  input  flag_spitZero_2;
  input  flag_spitZero_1;
  input  flag_spitZero_0;
  input  flag_DecRS__0;
  input  flag_DecRS_1;
  input  flag_DecRS_0;
  input  flag_DecACLen_0;
  input  flag_DecAC2_0;
  input  flag_DecDC_0;
  input  flag_DecDC_1;
  input  flag_DecDC_2;
  input  flag_DecDCLen_0;

  parameter state_DecDCp = 4'd0;
  parameter state_DecAC1 = 4'd1;
  parameter state_DecAC2 = 4'd2;
  parameter state_DecACLen = 4'd3;
  parameter state_DecDC = 4'd4;
  parameter state_DecDCLen = 4'd5;
  parameter state_DecRS = 4'd6;
  parameter state_DecRS_ = 4'd7;
  parameter state_spitZero = 4'd8;
  parameter state_done = 4'd9;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [3:0] state_reg, state_reg_;

  reg statecase_;

  reg parsToken_b_;
  reg setIncr_e_;
  reg setIncr_v_;
  reg getReq_e_;
  reg getReq_v_;
  reg decToken_e_;
  reg decToken_v_;
  reg ftabind_e_;
  reg ftabind_v_;
  reg ftabval_b_;

  reg did_goto_;

  assign parsToken_b = parsToken_b_;
  assign setIncr_e = setIncr_e_;
  assign setIncr_v = setIncr_v_;
  assign getReq_e = getReq_e_;
  assign getReq_v = getReq_v_;
  assign decToken_e = decToken_e_;
  assign decToken_v = decToken_v_;
  assign ftabind_e = ftabind_e_;
  assign ftabind_v = ftabind_v_;
  assign ftabval_b = ftabval_b_;
  assign state = state_reg;
  assign statecase = statecase_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)
      state_reg<=state_DecDCp;
    else
      state_reg<=state_reg_;
  end  // always @(posedge...)

  always @*  begin
    parsToken_b_ = 1;
    setIncr_e_ = 0;
    setIncr_v_ = 0;
    getReq_e_ = 0;
    getReq_v_ = 0;
    decToken_e_ = 0;
    decToken_v_ = 0;
    ftabind_e_ = 0;
    ftabind_v_ = 0;
    ftabval_b_ = 1;

    state_reg_ = state_reg;
    statecase_ = statecase_stall;
    did_goto_ = 0;

    case (state_reg)
      state_DecDCp:  begin
        if (!getReq_b)  begin
          statecase_ = statecase_1;
          begin
            getReq_v_ = 1;
            getReq_e_ = 0;
            state_reg_ = state_DecDC;
            did_goto_ = 1;
          end
        end
      end
      state_DecAC1:  begin
        if (parsToken_v && !parsToken_e && !setIncr_b && !getReq_b)  begin
          statecase_ = statecase_1;
          parsToken_b_ = 0;
          begin
            if (flag_DecAC1_0)  begin
              begin
                setIncr_v_ = 1;
                setIncr_e_ = 0;
                getReq_v_ = 1;
                getReq_e_ = 0;
                state_reg_ = state_DecAC2;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                setIncr_v_ = 1;
                setIncr_e_ = 0;
                state_reg_ = state_DecRS;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecAC2:  begin
        if (parsToken_v && !parsToken_e && !setIncr_b && !ftabind_b)  begin
          statecase_ = statecase_1;
          parsToken_b_ = 0;
          begin
            if (flag_DecAC2_0)  begin
              begin
                setIncr_v_ = 1;
                setIncr_e_ = 0;
                ftabind_v_ = 1;
                ftabind_e_ = 0;
                state_reg_ = state_DecRS_;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                setIncr_v_ = 1;
                setIncr_e_ = 0;
                state_reg_ = state_DecRS;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecACLen:  begin
        if (parsToken_v && !parsToken_e && !setIncr_b && !getReq_b && !decToken_b)  begin
          statecase_ = statecase_1;
          parsToken_b_ = 0;
          begin
            decToken_v_ = 1;
            decToken_e_ = 0;
            setIncr_v_ = 1;
            setIncr_e_ = 0;
            if (flag_DecACLen_0)  begin
              begin
                getReq_v_ = 1;
                getReq_e_ = 0;
                state_reg_ = state_DecDC;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                getReq_v_ = 1;
                getReq_e_ = 0;
                state_reg_ = state_DecAC1;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecDC:  begin
        if (parsToken_v && !parsToken_e && !setIncr_b && !getReq_b && !decToken_b)  begin
          statecase_ = statecase_1;
          parsToken_b_ = 0;
          begin
            if (flag_DecDC_0)  begin
              begin
                state_reg_ = state_done;
                did_goto_ = 1;
              end
            end
            if (flag_DecDC_1)  begin
              begin
                if (!did_goto_) decToken_v_ = 1;
                if (!did_goto_) decToken_e_ = 0;
                if (!did_goto_) setIncr_v_ = 1;
                if (!did_goto_) setIncr_e_ = 0;
                if (!did_goto_) getReq_v_ = 1;
                if (!did_goto_) getReq_e_ = 0;
                if (!did_goto_) state_reg_ = state_DecAC1;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (flag_DecDC_2)  begin
                  begin
                    if (!did_goto_) setIncr_v_ = 1;
                    if (!did_goto_) setIncr_e_ = 0;
                  end
                end
                else  begin
                  begin
                    if (!did_goto_) setIncr_v_ = 1;
                    if (!did_goto_) setIncr_e_ = 0;
                  end
                end
                if (!did_goto_) getReq_v_ = 1;
                if (!did_goto_) getReq_e_ = 0;
                if (!did_goto_) state_reg_ = state_DecDCLen;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecDCLen:  begin
        if (parsToken_v && !parsToken_e && !setIncr_b && !getReq_b && !decToken_b)  begin
          statecase_ = statecase_1;
          parsToken_b_ = 0;
          begin
            if (flag_DecDCLen_0)  begin
              begin
              end
            end
            else  begin
              begin
              end
            end
            decToken_v_ = 1;
            decToken_e_ = 0;
            setIncr_v_ = 1;
            setIncr_e_ = 0;
            getReq_v_ = 1;
            getReq_e_ = 0;
            state_reg_ = state_DecAC1;
            did_goto_ = 1;
          end
        end
      end
      state_DecRS:  begin
        if (!getReq_b)  begin
          statecase_ = statecase_1;
          begin
            if (flag_DecRS_0)  begin
              begin
                state_reg_ = state_spitZero;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (flag_DecRS_1)  begin
                  begin
                    getReq_v_ = 1;
                    getReq_e_ = 0;
                    state_reg_ = state_DecACLen;
                    did_goto_ = 1;
                  end
                end
                else  begin
                  begin
                    state_reg_ = state_spitZero;
                    did_goto_ = 1;
                  end
                end
              end
            end
          end
        end
      end
      state_DecRS_:  begin
        if (ftabval_v && !ftabval_e && !getReq_b)  begin
          statecase_ = statecase_1;
          ftabval_b_ = 0;
          begin
            if (flag_DecRS__0)  begin
              begin
                getReq_v_ = 1;
                getReq_e_ = 0;
                state_reg_ = state_DecACLen;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                state_reg_ = state_spitZero;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_spitZero:  begin
        if (!getReq_b && !decToken_b)  begin
          statecase_ = statecase_1;
          begin
            if (flag_spitZero_0)  begin
              begin
                if (flag_spitZero_1)  begin
                  begin
                    if (flag_spitZero_2)  begin
                      begin
                        if (flag_spitZero_3)  begin
                          begin
                            state_reg_ = state_done;
                            did_goto_ = 1;
                          end
                        end
                        if (!did_goto_) getReq_v_ = 1;
                        if (!did_goto_) getReq_e_ = 0;
                        if (!did_goto_) state_reg_ = state_DecDC;
                        did_goto_ = 1;
                      end
                    end
                    else  begin
                      begin
                        getReq_v_ = 1;
                        getReq_e_ = 0;
                        state_reg_ = state_DecAC1;
                        did_goto_ = 1;
                      end
                    end
                  end
                end
                else  begin
                  begin
                    getReq_v_ = 1;
                    getReq_e_ = 0;
                    state_reg_ = state_DecACLen;
                    did_goto_ = 1;
                  end
                end
              end
            end
            else  begin
              begin
                decToken_v_ = 1;
                decToken_e_ = 0;
              end
            end
          end
        end
      end
      state_done:  begin
      end
    endcase  // case (state_reg)

  end  // always @*

endmodule  // DecSym_d1_ScOrEtMp53_fsm
