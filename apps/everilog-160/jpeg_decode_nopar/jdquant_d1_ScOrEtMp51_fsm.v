// Verilog FSM module for jdquant_d1_ScOrEtMp51
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module jdquant_d1_ScOrEtMp51_fsm (clock, reset, inStream_e, inStream_v, inStream_b, outStream_e, outStream_v, outStream_b, statecase, flag_only_0);

  input  clock;
  input  reset;

  input  inStream_e;
  input  inStream_v;
  output inStream_b;
  output outStream_e;
  output outStream_v;
  input  outStream_b;

  output statecase;

  input  flag_only_0;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg statecase_;

  reg inStream_b_;
  reg outStream_e_;
  reg outStream_v_;

  reg did_goto_;

  assign inStream_b = inStream_b_;
  assign outStream_e = outStream_e_;
  assign outStream_v = outStream_v_;
  assign statecase = statecase_;

  always @*  begin
    inStream_b_ = 1;
    outStream_e_ = 0;
    outStream_v_ = 0;

    statecase_ = statecase_stall;
    did_goto_ = 0;

    if (inStream_v && !inStream_e && !outStream_b)  begin
      statecase_ = statecase_1;
      inStream_b_ = 0;
      begin
        outStream_v_ = 1;
        outStream_e_ = 0;
        if (flag_only_0)  begin
        end
        else  begin
        end
      end
    end

  end  // always @*

endmodule  // jdquant_d1_ScOrEtMp51_fsm
