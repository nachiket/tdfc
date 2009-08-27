// Verilog data-path module for tpose_d1_ScOrEtMp47
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module tpose_d1_ScOrEtMp47_dp (clock, reset, a0_d, a1_d, a2_d, a3_d, a4_d, a5_d, a6_d, a7_d, b0_d, b1_d, b2_d, b3_d, b4_d, b5_d, b6_d, b7_d, state, statecase);

  input  clock;
  input  reset;

  input  [15:0] a0_d;
  input  [15:0] a1_d;
  input  [15:0] a2_d;
  input  [15:0] a3_d;
  input  [15:0] a4_d;
  input  [15:0] a5_d;
  input  [15:0] a6_d;
  input  [15:0] a7_d;
  output [15:0] b0_d;
  output [15:0] b1_d;
  output [15:0] b2_d;
  output [15:0] b3_d;
  output [15:0] b4_d;
  output [15:0] b5_d;
  output [15:0] b6_d;
  output [15:0] b7_d;

  input  [4:0] state;
  input  [1:0] statecase;

  parameter state_init_0 = 5'd0;
  parameter state_end_1 = 5'd1;
  parameter state_end_2 = 5'd2;
  parameter state_end_3 = 5'd3;
  parameter state_end_4 = 5'd4;
  parameter state_end_5 = 5'd5;
  parameter state_end_6 = 5'd6;
  parameter state_init_1 = 5'd7;
  parameter state_init_2 = 5'd8;
  parameter state_init_3 = 5'd9;
  parameter state_init_4 = 5'd10;
  parameter state_init_5 = 5'd11;
  parameter state_init_6 = 5'd12;
  parameter state_st_0 = 5'd13;
  parameter state_st_1 = 5'd14;
  parameter state_st_2 = 5'd15;
  parameter state_st_3 = 5'd16;
  parameter state_st_4 = 5'd17;
  parameter state_st_5 = 5'd18;
  parameter state_st_6 = 5'd19;
  parameter state_st_7 = 5'd20;

  parameter statecase_stall = 2'd0;
  parameter statecase_1 = 2'd1;
  parameter statecase_2 = 2'd2;

  reg signed [15:0] b0_d_;
  reg signed [15:0] b1_d_;
  reg signed [15:0] b2_d_;
  reg signed [15:0] b3_d_;
  reg signed [15:0] b4_d_;
  reg signed [15:0] b5_d_;
  reg signed [15:0] b6_d_;
  reg signed [15:0] b7_d_;

  reg did_goto_;

  assign b0_d = b0_d_;
  assign b1_d = b1_d_;
  assign b2_d = b2_d_;
  assign b3_d = b3_d_;
  assign b4_d = b4_d_;
  assign b5_d = b5_d_;
  assign b6_d = b6_d_;
  assign b7_d = b7_d_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
    end
    else  begin
    end
  end  // always @(posedge...)

  always @*  begin
    b0_d_ = 16'bx;
    b1_d_ = 16'bx;
    b2_d_ = 16'bx;
    b3_d_ = 16'bx;
    b4_d_ = 16'bx;
    b5_d_ = 16'bx;
    b6_d_ = 16'bx;
    b7_d_ = 16'bx;

    did_goto_ = 0;

    case (state)
      state_init_0:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a0_d);
            did_goto_ = 1;
          end
        end
      end
      state_end_1:  begin
        if (statecase == statecase_1)  begin
          begin
            b2_d_ = a7_d;
            b3_d_ = a6_d;
            b4_d_ = a5_d;
            b5_d_ = a4_d;
            b6_d_ = a3_d;
            b7_d_ = a2_d;
            did_goto_ = 1;
          end
        end
      end
      state_end_2:  begin
        if (statecase == statecase_1)  begin
          begin
            b3_d_ = a7_d;
            b4_d_ = a6_d;
            b5_d_ = a5_d;
            b6_d_ = a4_d;
            b7_d_ = a3_d;
            did_goto_ = 1;
          end
        end
      end
      state_end_3:  begin
        if (statecase == statecase_1)  begin
          begin
            b4_d_ = a7_d;
            b5_d_ = a6_d;
            b6_d_ = a5_d;
            b7_d_ = a4_d;
            did_goto_ = 1;
          end
        end
      end
      state_end_4:  begin
        if (statecase == statecase_1)  begin
          begin
            b5_d_ = a7_d;
            b6_d_ = a6_d;
            b7_d_ = a5_d;
            did_goto_ = 1;
          end
        end
      end
      state_end_5:  begin
        if (statecase == statecase_1)  begin
          begin
            b6_d_ = a7_d;
            b7_d_ = a6_d;
            did_goto_ = 1;
          end
        end
      end
      state_end_6:  begin
        if (statecase == statecase_1)  begin
          begin
            b7_d_ = a7_d;
          end
        end
      end
      state_init_1:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a1_d);
            b1_d_ = a0_d;
            did_goto_ = 1;
          end
        end
      end
      state_init_2:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a2_d);
            b1_d_ = a1_d;
            b2_d_ = a0_d;
            did_goto_ = 1;
          end
        end
      end
      state_init_3:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a3_d);
            b1_d_ = a2_d;
            b2_d_ = a1_d;
            b3_d_ = a0_d;
            did_goto_ = 1;
          end
        end
      end
      state_init_4:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a4_d);
            b1_d_ = a3_d;
            b2_d_ = a2_d;
            b3_d_ = a1_d;
            b4_d_ = a0_d;
            did_goto_ = 1;
          end
        end
      end
      state_init_5:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a5_d);
            b1_d_ = a4_d;
            b2_d_ = a3_d;
            b3_d_ = a2_d;
            b4_d_ = a1_d;
            b5_d_ = a0_d;
            did_goto_ = 1;
          end
        end
      end
      state_init_6:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a6_d);
            b1_d_ = a5_d;
            b2_d_ = a4_d;
            b3_d_ = a3_d;
            b4_d_ = a2_d;
            b5_d_ = a1_d;
            b6_d_ = a0_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_0:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a0_d);
            b1_d_ = a7_d;
            b2_d_ = a6_d;
            b3_d_ = a5_d;
            b4_d_ = a4_d;
            b5_d_ = a3_d;
            b6_d_ = a2_d;
            b7_d_ = a1_d;
            did_goto_ = 1;
          end
        end
        else if (statecase == statecase_2)  begin
          begin
            b1_d_ = a7_d;
            b2_d_ = a6_d;
            b3_d_ = a5_d;
            b4_d_ = a4_d;
            b5_d_ = a3_d;
            b6_d_ = a2_d;
            b7_d_ = a1_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_1:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a1_d);
            b1_d_ = a0_d;
            b2_d_ = a7_d;
            b3_d_ = a6_d;
            b4_d_ = a5_d;
            b5_d_ = a4_d;
            b6_d_ = a3_d;
            b7_d_ = a2_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_2:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a2_d);
            b1_d_ = a1_d;
            b2_d_ = a0_d;
            b3_d_ = a7_d;
            b4_d_ = a6_d;
            b5_d_ = a5_d;
            b6_d_ = a4_d;
            b7_d_ = a3_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_3:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a3_d);
            b1_d_ = a2_d;
            b2_d_ = a1_d;
            b3_d_ = a0_d;
            b4_d_ = a7_d;
            b5_d_ = a6_d;
            b6_d_ = a5_d;
            b7_d_ = a4_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_4:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a4_d);
            b1_d_ = a3_d;
            b2_d_ = a2_d;
            b3_d_ = a1_d;
            b4_d_ = a0_d;
            b5_d_ = a7_d;
            b6_d_ = a6_d;
            b7_d_ = a5_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_5:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a5_d);
            b1_d_ = a4_d;
            b2_d_ = a3_d;
            b3_d_ = a2_d;
            b4_d_ = a1_d;
            b5_d_ = a0_d;
            b6_d_ = a7_d;
            b7_d_ = a6_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_6:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a6_d);
            b1_d_ = a5_d;
            b2_d_ = a4_d;
            b3_d_ = a3_d;
            b4_d_ = a2_d;
            b5_d_ = a1_d;
            b6_d_ = a0_d;
            b7_d_ = a7_d;
            did_goto_ = 1;
          end
        end
      end
      state_st_7:  begin
        if (statecase == statecase_1)  begin
          begin
            b0_d_ = (16'sd2048 + a7_d);
            b1_d_ = a6_d;
            b2_d_ = a5_d;
            b3_d_ = a4_d;
            b4_d_ = a3_d;
            b5_d_ = a2_d;
            b6_d_ = a1_d;
            b7_d_ = a0_d;
            did_goto_ = 1;
          end
        end
      end
    endcase  // case (state_reg)
  end  // always @*

endmodule  // tpose_d1_ScOrEtMp47_dp
