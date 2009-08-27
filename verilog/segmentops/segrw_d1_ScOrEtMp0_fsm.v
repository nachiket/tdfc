// Verilog FSM module for segrw_d1_ScOrEtMp0
// Emitted by ../../tdfc version 1.158, Tue Dec 14 12:18:04 2004

module segrw_d1_ScOrEtMp0_fsm (clock, reset, addr_e, addr_v, addr_b, dataR_e, dataR_v, dataR_b, dataW_e, dataW_v, dataW_b, write_e, write_v, write_b, state, statecase, flag_steady_0, flag_steady_1);

  input  clock;
  input  reset;

  input  addr_e;
  input  addr_v;
  output addr_b;
  output dataR_e;
  output dataR_v;
  input  dataR_b;
  input  dataW_e;
  input  dataW_v;
  output dataW_b;
  input  write_e;
  input  write_v;
  output write_b;

  output state;
  output [1:0] statecase;

  input  flag_steady_0;
  input  flag_steady_1;

  parameter state_start = 1'd0;
  parameter state_steady = 1'd1;

  parameter statecase_stall = 2'd0;
  parameter statecase_1 = 2'd1;
  parameter statecase_2 = 2'd2;

  reg state_reg, state_reg_;

  reg [1:0] statecase_;

  reg addr_b_;
  reg dataR_e_;
  reg dataR_v_;
  reg dataW_b_;
  reg write_b_;

  reg did_goto_;

  assign addr_b = addr_b_;
  assign dataR_e = dataR_e_;
  assign dataR_v = dataR_v_;
  assign dataW_b = dataW_b_;
  assign write_b = write_b_;
  assign state = state_reg;
  assign statecase = statecase_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)
      state_reg<=state_start;
    else
      state_reg<=state_reg_;
  end  // always @(posedge...)

  always @*  begin
    addr_b_ = 1;
    dataR_e_ = 0;
    dataR_v_ = 0;
    dataW_b_ = 1;
    write_b_ = 1;

    state_reg_ = state_reg;
    statecase_ = statecase_stall;
    did_goto_ = 0;

    case (state_reg)
      state_start:  begin
        if (addr_v && !addr_e && dataW_v && !dataW_e && write_v && !write_e)  begin
          statecase_ = statecase_1;
          addr_b_ = 0;
          dataW_b_ = 0;
          write_b_ = 0;
          state_reg_ = state_steady;
          did_goto_ = 1;
        end
      end
      state_steady:  begin
        if (addr_v && !addr_e && dataW_v && !dataW_e && write_v && !write_e && !dataR_b)  begin
          statecase_ = statecase_1;
          addr_b_ = 0;
          dataW_b_ = 0;
          write_b_ = 0;
          if (flag_steady_0)  begin
          end
          else  begin
            dataR_v_ = 1;
            dataR_e_ = 0;
          end
          state_reg_ = state_steady;
          did_goto_ = 1;
          if (!did_goto_) state_reg_ = state_steady;
          did_goto_ = 1;
        end
        else if (!dataR_b)  begin
          statecase_ = statecase_2;
          if (flag_steady_1)  begin
          end
          else  begin
            dataR_v_ = 1;
            dataR_e_ = 0;
          end
          state_reg_ = state_start;
          did_goto_ = 1;
        end
      end
    endcase  // case (state_reg)

  end  // always @*

endmodule  // segrw_d1_ScOrEtMp0_fsm
