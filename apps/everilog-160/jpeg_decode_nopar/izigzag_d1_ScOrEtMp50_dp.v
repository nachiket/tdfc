// Verilog data-path module for izigzag_d1_ScOrEtMp50
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:43 2009

module izigzag_d1_ScOrEtMp50_dp (clock, reset, chuA_d, chuB_d, chuC_d, chuD_d, chuE_d, chuF_d, chuG_d, chuH_d, ruS_d, state, statecase, flag__1_0, flag__8_1, flag__8_0, flag__7_1, flag__7_0, flag__6_0, flag__5_1, flag__2_0, flag__1_1, flag__3_0, flag__3_1, flag__4_0, flag__5_0);

  input  clock;
  input  reset;

  output [15:0] chuA_d;
  output [15:0] chuB_d;
  output [15:0] chuC_d;
  output [15:0] chuD_d;
  output [15:0] chuE_d;
  output [15:0] chuF_d;
  output [15:0] chuG_d;
  output [15:0] chuH_d;
  input  [15:0] ruS_d;

  input  [2:0] state;
  input  statecase;

  output flag__1_0;
  output flag__8_1;
  output flag__8_0;
  output flag__7_1;
  output flag__7_0;
  output flag__6_0;
  output flag__5_1;
  output flag__2_0;
  output flag__1_1;
  output flag__3_0;
  output flag__3_1;
  output flag__4_0;
  output flag__5_0;

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

  reg [5:0] index, index_;
  reg phase8, phase8_;
  reg phase7, phase7_;
  reg phase2, phase2_;
  reg phase1, phase1_;
  reg phase3, phase3_;
  reg phase4, phase4_;
  reg phase5, phase5_;
  reg phase6, phase6_;

  reg signed [15:0] chuA_d_;
  reg signed [15:0] chuB_d_;
  reg signed [15:0] chuC_d_;
  reg signed [15:0] chuD_d_;
  reg signed [15:0] chuE_d_;
  reg signed [15:0] chuF_d_;
  reg signed [15:0] chuG_d_;
  reg signed [15:0] chuH_d_;

  reg flag__1_0_;
  reg flag__8_1_;
  reg flag__8_0_;
  reg flag__7_1_;
  reg flag__7_0_;
  reg flag__6_0_;
  reg flag__5_1_;
  reg flag__2_0_;
  reg flag__1_1_;
  reg flag__3_0_;
  reg flag__3_1_;
  reg flag__4_0_;
  reg flag__5_0_;

  reg did_goto_;

  assign chuA_d = chuA_d_;
  assign chuB_d = chuB_d_;
  assign chuC_d = chuC_d_;
  assign chuD_d = chuD_d_;
  assign chuE_d = chuE_d_;
  assign chuF_d = chuF_d_;
  assign chuG_d = chuG_d_;
  assign chuH_d = chuH_d_;

  assign flag__1_0 = flag__1_0_;
  assign flag__8_1 = flag__8_1_;
  assign flag__8_0 = flag__8_0_;
  assign flag__7_1 = flag__7_1_;
  assign flag__7_0 = flag__7_0_;
  assign flag__6_0 = flag__6_0_;
  assign flag__5_1 = flag__5_1_;
  assign flag__2_0 = flag__2_0_;
  assign flag__1_1 = flag__1_1_;
  assign flag__3_0 = flag__3_0_;
  assign flag__3_1 = flag__3_1_;
  assign flag__4_0 = flag__4_0_;
  assign flag__5_0 = flag__5_0_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
      index <= 1'b0;
      phase8 <= 1'b1;
      phase7 <= 1'b1;
      phase2 <= 1'b1;
      phase1 <= 1'b1;
      phase3 <= 1'b1;
      phase4 <= 1'b1;
      phase5 <= 1'b1;
      phase6 <= 1'b1;
    end
    else  begin
      index <= index_;
      phase8 <= phase8_;
      phase7 <= phase7_;
      phase2 <= phase2_;
      phase1 <= phase1_;
      phase3 <= phase3_;
      phase4 <= phase4_;
      phase5 <= phase5_;
      phase6 <= phase6_;
    end
  end  // always @(posedge...)

  always @*  begin
    chuA_d_ = 16'bx;
    chuB_d_ = 16'bx;
    chuC_d_ = 16'bx;
    chuD_d_ = 16'bx;
    chuE_d_ = 16'bx;
    chuF_d_ = 16'bx;
    chuG_d_ = 16'bx;
    chuH_d_ = 16'bx;

    flag__1_0_ = 1'bx;
    flag__8_1_ = 1'bx;
    flag__8_0_ = 1'bx;
    flag__7_1_ = 1'bx;
    flag__7_0_ = 1'bx;
    flag__6_0_ = 1'bx;
    flag__5_1_ = 1'bx;
    flag__2_0_ = 1'bx;
    flag__1_1_ = 1'bx;
    flag__3_0_ = 1'bx;
    flag__3_1_ = 1'bx;
    flag__4_0_ = 1'bx;
    flag__5_0_ = 1'bx;

    index_ = index;
    phase8_ = phase8;
    phase7_ = phase7;
    phase2_ = phase2;
    phase1_ = phase1;
    phase3_ = phase3;
    phase4_ = phase4;
    phase5_ = phase5;
    phase6_ = phase6;

    did_goto_ = 0;

    case (state)
      state__1:  begin
        if (statecase == statecase_1)  begin
          begin
            chuA_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__1_0_ = phase1_;
            if (flag__1_0_)  begin
              begin
                phase1_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase1_ = 1'b1;
                flag__1_1_ = (index_ == 6'd36);
                if (flag__1_1_)  begin
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__2:  begin
        if (statecase == statecase_1)  begin
          begin
            chuB_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__2_0_ = phase2_;
            if (flag__2_0_)  begin
              begin
                phase2_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase2_ = 1'b1;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state__3:  begin
        if (statecase == statecase_1)  begin
          begin
            chuC_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__3_0_ = phase3_;
            if (flag__3_0_)  begin
              begin
                phase3_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase3_ = 1'b1;
                flag__3_1_ = (index_ == 6'd49);
                if (flag__3_1_)  begin
                  did_goto_ = 1;
                end
                else  begin
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__4:  begin
        if (statecase == statecase_1)  begin
          begin
            chuD_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__4_0_ = phase4_;
            if (flag__4_0_)  begin
              begin
                phase4_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase4_ = 1'b1;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state__5:  begin
        if (statecase == statecase_1)  begin
          begin
            chuE_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__5_0_ = phase5_;
            if (flag__5_0_)  begin
              begin
                phase5_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase5_ = 1'b1;
                flag__5_1_ = (index_ == 6'd58);
                if (flag__5_1_)  begin
                  did_goto_ = 1;
                end
                else  begin
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__6:  begin
        if (statecase == statecase_1)  begin
          begin
            chuF_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__6_0_ = phase6_;
            if (flag__6_0_)  begin
              begin
                phase6_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase6_ = 1'b1;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state__7:  begin
        if (statecase == statecase_1)  begin
          begin
            chuG_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__7_0_ = phase7_;
            if (flag__7_0_)  begin
              begin
                phase7_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase7_ = 1'b1;
                flag__7_1_ = (index_ == 6'd63);
                if (flag__7_1_)  begin
                  did_goto_ = 1;
                end
                else  begin
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
      state__8:  begin
        if (statecase == statecase_1)  begin
          begin
            chuH_d_ = ruS_d;
            index_ = (index_ + 1'd1);
            flag__8_0_ = phase8_;
            if (flag__8_0_)  begin
              begin
                phase8_ = 1'b0;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                phase8_ = 1'b1;
                flag__8_1_ = (index_ == 7'd64);
                if (flag__8_1_)  begin
                  begin
                    index_ = 1'b0;
                    did_goto_ = 1;
                  end
                end
                else  begin
                  did_goto_ = 1;
                end
              end
            end
          end
        end
      end
    endcase  // case (state_reg)
  end  // always @*

endmodule  // izigzag_d1_ScOrEtMp50_dp
