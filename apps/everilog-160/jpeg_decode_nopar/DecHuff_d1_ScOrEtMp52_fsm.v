// Verilog FSM module for DecHuff_d1_ScOrEtMp52
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module DecHuff_d1_ScOrEtMp52_fsm (clock, reset, filebyte_e, filebyte_v, filebyte_b, reqSize_e, reqSize_v, reqSize_b, advance_e, advance_v, advance_b, parsedToken_e, parsedToken_v, parsedToken_b, state, statecase, flag_getData_0, flag_waitZero_0, flag_getData_1, flag_req_0);

  input  clock;
  input  reset;

  input  filebyte_e;
  input  filebyte_v;
  output filebyte_b;
  input  reqSize_e;
  input  reqSize_v;
  output reqSize_b;
  input  advance_e;
  input  advance_v;
  output advance_b;
  output parsedToken_e;
  output parsedToken_v;
  input  parsedToken_b;

  output [2:0] state;
  output statecase;

  input  flag_getData_0;
  input  flag_waitZero_0;
  input  flag_getData_1;
  input  flag_req_0;

  parameter state_initreq = 3'd0;
  parameter state_advancePtr = 3'd1;
  parameter state_getData = 3'd2;
  parameter state_putData = 3'd3;
  parameter state_req = 3'd4;
  parameter state_waitZero = 3'd5;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [2:0] state_reg, state_reg_;

  reg statecase_;

  reg filebyte_b_;
  reg reqSize_b_;
  reg advance_b_;
  reg parsedToken_e_;
  reg parsedToken_v_;

  reg did_goto_;

  assign filebyte_b = filebyte_b_;
  assign reqSize_b = reqSize_b_;
  assign advance_b = advance_b_;
  assign parsedToken_e = parsedToken_e_;
  assign parsedToken_v = parsedToken_v_;
  assign state = state_reg;
  assign statecase = statecase_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)
      state_reg<=state_initreq;
    else
      state_reg<=state_reg_;
  end  // always @(posedge...)

  always @*  begin
    filebyte_b_ = 1;
    reqSize_b_ = 1;
    advance_b_ = 1;
    parsedToken_e_ = 0;
    parsedToken_v_ = 0;

    state_reg_ = state_reg;
    statecase_ = statecase_stall;
    did_goto_ = 0;

    case (state_reg)
      state_initreq:  begin
        if (reqSize_v && !reqSize_e)  begin
          statecase_ = statecase_1;
          reqSize_b_ = 0;
          begin
            state_reg_ = state_getData;
            did_goto_ = 1;
          end
        end
      end
      state_advancePtr:  begin
        if (advance_v && !advance_e)  begin
          statecase_ = statecase_1;
          advance_b_ = 0;
          begin
            state_reg_ = state_req;
            did_goto_ = 1;
          end
        end
      end
      state_getData:  begin
        if (filebyte_v && !filebyte_e)  begin
          statecase_ = statecase_1;
          filebyte_b_ = 0;
          begin
            if (flag_getData_0)  begin
              begin
                if (flag_getData_1)  begin
                  begin
                    state_reg_ = state_putData;
                    did_goto_ = 1;
                  end
                end
              end
            end
            else  begin
              begin
                state_reg_ = state_waitZero;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_putData:  begin
        if (!parsedToken_b)  begin
          statecase_ = statecase_1;
          begin
            parsedToken_v_ = 1;
            parsedToken_e_ = 0;
            state_reg_ = state_advancePtr;
            did_goto_ = 1;
          end
        end
      end
      state_req:  begin
        if (reqSize_v && !reqSize_e)  begin
          statecase_ = statecase_1;
          reqSize_b_ = 0;
          begin
            if (flag_req_0)  begin
              begin
                state_reg_ = state_putData;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                state_reg_ = state_getData;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_waitZero:  begin
        if (filebyte_v && !filebyte_e)  begin
          statecase_ = statecase_1;
          filebyte_b_ = 0;
          begin
            if (flag_waitZero_0)  begin
              begin
                state_reg_ = state_putData;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                state_reg_ = state_getData;
                did_goto_ = 1;
              end
            end
          end
        end
      end
    endcase  // case (state_reg)

  end  // always @*

endmodule  // DecHuff_d1_ScOrEtMp52_fsm
