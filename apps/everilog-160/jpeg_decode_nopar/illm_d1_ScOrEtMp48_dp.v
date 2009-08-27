// Verilog data-path module for illm_d1_ScOrEtMp48
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module illm_d1_ScOrEtMp48_dp (clock, reset, a0_d, a1_d, a2_d, a3_d, a4_d, a5_d, a6_d, a7_d, b0_d, b1_d, b2_d, b3_d, b4_d, b5_d, b6_d, b7_d, statecase);

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
  output [8:0] b0_d;
  output [8:0] b1_d;
  output [8:0] b2_d;
  output [8:0] b3_d;
  output [8:0] b4_d;
  output [8:0] b5_d;
  output [8:0] b6_d;
  output [8:0] b7_d;

  input  statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg signed [17:0] tmp11__;
  reg signed [21:0] tmp4_;
  reg signed [19:0] tmp6_;
  reg signed [17:0] tmp10__;
  reg signed [17:0] tmp3_;
  reg signed [18:0] tmp12_;
  reg signed [16:0] tmp13_;
  reg signed [19:0] tmp2_;
  reg signed [16:0] z10_;
  reg signed [16:0] z12_;
  reg signed [16:0] tmp10_;
  reg signed [20:0] tmp5_;
  reg signed [18:0] tmp12__;
  reg signed [19:0] tmp1_;
  reg signed [17:0] tmp0_;
  reg signed [16:0] tmp11_;
  reg signed [16:0] z13_;
  reg signed [16:0] z11_;
  reg signed [17:0] tmp7_;
  reg signed [17:0] z5_;

  reg signed [25:0] tmp0__1;
  reg signed [22:0] tmp16_;
  reg signed [9:0] tmp1__1;
  reg signed [9:0] tmp7__1;
  reg signed [26:0] tmp8_;
  reg signed [18:0] tmp10__1;
  reg signed [20:0] tmp12__1;
  reg signed [21:0] tmp14_;
  reg signed [20:0] tmp13__1;
  reg signed [22:0] tmp17_;
  reg signed [21:0] tmp15_;
  reg signed [25:0] tmp6__1;
  reg signed [9:0] tmp5__1;
  reg signed [26:0] tmp4__1;
  reg signed [9:0] tmp3__1;
  reg signed [26:0] tmp2__1;
  reg signed [10:0] tmp9_;
  reg signed [18:0] tmp11__1;

  reg signed [8:0] b0_d_;
  reg signed [8:0] b1_d_;
  reg signed [8:0] b2_d_;
  reg signed [8:0] b3_d_;
  reg signed [8:0] b4_d_;
  reg signed [8:0] b5_d_;
  reg signed [8:0] b6_d_;
  reg signed [8:0] b7_d_;

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
    b0_d_ = 9'bx;
    b1_d_ = 9'bx;
    b2_d_ = 9'bx;
    b3_d_ = 9'bx;
    b4_d_ = 9'bx;
    b5_d_ = 9'bx;
    b6_d_ = 9'bx;
    b7_d_ = 9'bx;

    tmp11__ = 18'bx;
    tmp4_ = 22'bx;
    tmp6_ = 20'bx;
    tmp10__ = 18'bx;
    tmp3_ = 18'bx;
    tmp12_ = 19'bx;
    tmp13_ = 17'bx;
    tmp2_ = 20'bx;
    z10_ = 17'bx;
    z12_ = 17'bx;
    tmp10_ = 17'bx;
    tmp5_ = 21'bx;
    tmp12__ = 19'bx;
    tmp1_ = 20'bx;
    tmp0_ = 18'bx;
    tmp11_ = 17'bx;
    z13_ = 17'bx;
    z11_ = 17'bx;
    tmp7_ = 18'bx;
    z5_ = 18'bx;

    did_goto_ = 0;

    if (statecase == statecase_1)  begin
      begin
        tmp10_ = (a0_d + a4_d);
        tmp13_ = (a2_d + a6_d);
        tmp11_ = (a0_d - a4_d);
        tmp1__1 = 9'd362;
        tmp0__1 = (((a2_d - a6_d) * tmp1__1) >> 4'd8);
        tmp12_ = (tmp0__1 - tmp13_);
        tmp0_ = (tmp10_ + tmp13_);
        tmp3_ = (tmp10_ - tmp13_);
        tmp1_ = (tmp11_ + tmp12_);
        tmp2_ = (tmp11_ - tmp12_);
        z13_ = (a5_d + a3_d);
        z10_ = (a5_d - a3_d);
        z11_ = (a1_d + a7_d);
        z12_ = (a1_d - a7_d);
        tmp7_ = (z11_ + z13_);
        tmp3__1 = 9'd362;
        tmp2__1 = (((z11_ - z13_) * tmp3__1) >> 4'd8);
        tmp11__ = tmp2__1;
        tmp5__1 = 9'd473;
        tmp4__1 = (((z10_ + z12_) * tmp5__1) >> 4'd8);
        z5_ = tmp4__1;
        tmp7__1 = 9'd277;
        tmp6__1 = ((z12_ * tmp7__1) >> 4'd8);
        tmp10__ = (tmp6__1 - z5_);
        tmp9_ = 10'd669;
        tmp8_ = ((z10_ * tmp9_) >> 4'd8);
        tmp12__ = (z5_ - tmp8_);
        tmp6_ = (tmp12__ - tmp7_);
        tmp5_ = (tmp11__ - tmp6_);
        tmp4_ = (tmp10__ + tmp5_);
        tmp10__1 = ((tmp0_ + tmp7_) >> 8'd4);
        b0_d_ = tmp10__1;
        tmp11__1 = ((tmp0_ - tmp7_) >> 8'd4);
        b7_d_ = tmp11__1;
        tmp12__1 = ((tmp1_ + tmp6_) >> 8'd4);
        b1_d_ = tmp12__1;
        tmp13__1 = ((tmp1_ - tmp6_) >> 8'd4);
        b6_d_ = tmp13__1;
        tmp14_ = ((tmp2_ + tmp5_) >> 8'd4);
        b2_d_ = tmp14_;
        tmp15_ = ((tmp2_ - tmp5_) >> 8'd4);
        b5_d_ = tmp15_;
        tmp16_ = ((tmp3_ + tmp4_) >> 8'd4);
        b4_d_ = tmp16_;
        tmp17_ = ((tmp3_ - tmp4_) >> 8'd4);
        b3_d_ = tmp17_;
        did_goto_ = 1;
      end
    end
  end  // always @*

endmodule  // illm_d1_ScOrEtMp48_dp
