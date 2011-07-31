// Verilog data-path module for tdfc_vadd_d1_ScOrEtMp0
// Emitted by /home/cody/Internship/tdfc/tdfc version 1.160, Fri May 27 18:31:12 2011

module tdfc_vadd_d1_ScOrEtMp0_dp (clock, reset, a_d, x_d, y_d, z_d, statecase);

  input  clock;
  input  reset;

  input  [7:0] a_d;
  input  [7:0] x_d;
  input  [7:0] y_d;
  output [7:0] z_d;

  input  statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [7:0] a_at_0;

  reg [7:0] z_d_;

  reg did_goto_;

  assign z_d = z_d_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
      a_at_0 <= 8'bx;
    end
    else  begin
      a_at_0 <= a_d;
    end
  end  // always @(posedge...)

  always @*  begin
    z_d_ = 8'bx;

    did_goto_ = 0;

    if (statecase == statecase_1)  begin
      begin
        z_d_ = ((a_at_0 * x_d) + y_d);
      end
    end
  end  // always @*

endmodule  // tdfc_vadd_d1_ScOrEtMp0_dp
