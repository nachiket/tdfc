// Verilog data-path module for bl_d1_ScOrEtMp49
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module bl_d1_ScOrEtMp49_dp (clock, reset, a_d, b_d, c_d, d_d, e_d, f_d, g_d, h_d, s_d, t_d, u_d, v_d, w_d, x_d, y_d, z_d, statecase);

  input  clock;
  input  reset;

  input  [8:0] a_d;
  input  [8:0] b_d;
  input  [8:0] c_d;
  input  [8:0] d_d;
  input  [8:0] e_d;
  input  [8:0] f_d;
  input  [8:0] g_d;
  input  [8:0] h_d;
  output [8:0] s_d;
  output [8:0] t_d;
  output [8:0] u_d;
  output [8:0] v_d;
  output [8:0] w_d;
  output [8:0] x_d;
  output [8:0] y_d;
  output [8:0] z_d;

  input  statecase;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg signed [8:0] A_;
  reg signed [8:0] H_;
  reg signed [8:0] C_;
  reg signed [8:0] B_;
  reg signed [8:0] D_;
  reg signed [8:0] E_;
  reg signed [8:0] F_;
  reg signed [8:0] G_;

  reg signed tmp0_;
  reg signed [8:0] tmp15_;
  reg signed tmp6_;
  reg signed [8:0] tmp5_;
  reg signed tmp4_;
  reg signed [8:0] tmp3_;
  reg signed [8:0] tmp1_;
  reg signed tmp2_;
  reg signed [8:0] tmp7_;
  reg signed tmp8_;
  reg signed [8:0] tmp9_;
  reg signed tmp10_;
  reg signed [8:0] tmp11_;
  reg signed tmp12_;
  reg signed [8:0] tmp13_;
  reg signed tmp14_;

  reg signed [8:0] s_d_;
  reg signed [8:0] t_d_;
  reg signed [8:0] u_d_;
  reg signed [8:0] v_d_;
  reg signed [8:0] w_d_;
  reg signed [8:0] x_d_;
  reg signed [8:0] y_d_;
  reg signed [8:0] z_d_;

  reg did_goto_;

  assign s_d = s_d_;
  assign t_d = t_d_;
  assign u_d = u_d_;
  assign v_d = v_d_;
  assign w_d = w_d_;
  assign x_d = x_d_;
  assign y_d = y_d_;
  assign z_d = z_d_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
    end
    else  begin
    end
  end  // always @(posedge...)

  always @*  begin
    s_d_ = 9'bx;
    t_d_ = 9'bx;
    u_d_ = 9'bx;
    v_d_ = 9'bx;
    w_d_ = 9'bx;
    x_d_ = 9'bx;
    y_d_ = 9'bx;
    z_d_ = 9'bx;

    A_ = 9'bx;
    H_ = 9'bx;
    C_ = 9'bx;
    B_ = 9'bx;
    D_ = 9'bx;
    E_ = 9'bx;
    F_ = 9'bx;
    G_ = 9'bx;

    did_goto_ = 0;

    if (statecase == statecase_1)  begin
      begin
        tmp0_ = 1'b0;
        A_ = ((a_d < tmp0_) ? 1'b0 : a_d);
        tmp1_ = 8'd255;
        s_d_ = ((A_ > tmp1_) ? 8'd255 : A_);
        tmp2_ = 1'b0;
        B_ = ((b_d < tmp2_) ? 1'b0 : b_d);
        tmp3_ = 8'd255;
        t_d_ = ((B_ > tmp3_) ? 8'd255 : B_);
        tmp4_ = 1'b0;
        C_ = ((c_d < tmp4_) ? 1'b0 : c_d);
        tmp5_ = 8'd255;
        u_d_ = ((C_ > tmp5_) ? 8'd255 : C_);
        tmp6_ = 1'b0;
        D_ = ((d_d < tmp6_) ? 1'b0 : d_d);
        tmp7_ = 8'd255;
        v_d_ = ((D_ > tmp7_) ? 8'd255 : D_);
        tmp8_ = 1'b0;
        E_ = ((e_d < tmp8_) ? 1'b0 : e_d);
        tmp9_ = 8'd255;
        w_d_ = ((E_ > tmp9_) ? 8'd255 : E_);
        tmp10_ = 1'b0;
        F_ = ((f_d < tmp10_) ? 1'b0 : f_d);
        tmp11_ = 8'd255;
        x_d_ = ((F_ > tmp11_) ? 8'd255 : F_);
        tmp12_ = 1'b0;
        G_ = ((g_d < tmp12_) ? 1'b0 : g_d);
        tmp13_ = 8'd255;
        y_d_ = ((G_ > tmp13_) ? 8'd255 : G_);
        tmp14_ = 1'b0;
        H_ = ((h_d < tmp14_) ? 1'b0 : h_d);
        tmp15_ = 8'd255;
        z_d_ = ((H_ > tmp15_) ? 8'd255 : H_);
      end
    end
  end  // always @*

endmodule  // bl_d1_ScOrEtMp49_dp
