// Verilog data-path module for add_d1_ScOrEtMp1
// Emitted by ../tdfc version 1.160, Thu Aug 27 19:13:22 2009

module add_d1_ScOrEtMp1_dp (clock, reset, add_d1_ScOrEtMp1_d, a_d, b_d, statecase);

  input  clock;
  input  reset;

  output add_d1_ScOrEtMp1_d;
  input  a_d;
  input  b_d;

  input  statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg add_d1_ScOrEtMp1_d_;

  reg did_goto_;

  assign add_d1_ScOrEtMp1_d = add_d1_ScOrEtMp1_d_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
    end
    else  begin
    end
  end  // always @(posedge...)

  always @*  begin
    add_d1_ScOrEtMp1_d_ = 0'bx;

    did_goto_ = 0;

    if (statecase == statecase_1)  begin
      add_d1_ScOrEtMp1_d_ = (a_d + b_d);
    end
  end  // always @*

endmodule  // add_d1_ScOrEtMp1_dp
