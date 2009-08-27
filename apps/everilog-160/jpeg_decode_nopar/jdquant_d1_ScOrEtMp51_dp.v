// Verilog data-path module for jdquant_d1_ScOrEtMp51
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module jdquant_d1_ScOrEtMp51_dp (clock, reset, inStream_d, outStream_d, statecase, flag_only_0);

  input  clock;
  input  reset;

  input  [15:0] inStream_d;
  output [15:0] outStream_d;

  input  statecase;

  output flag_only_0;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [7:0] index, index_;
  reg [7:0] tmpindex, tmpindex_;
  reg signed [8:0] newstep, newstep_;
  reg [63:0] step, step_;

  reg [5:0] quantSteps_a_;
  reg [63:0] quantSteps_d_;

  reg signed [8:0] tmp0_;

  reg signed [15:0] outStream_d_;

  reg flag_only_0_;

  reg did_goto_;

  assign outStream_d = outStream_d_;

  assign flag_only_0 = flag_only_0_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
      index <= 1'b0;
      tmpindex <= 8'bx;
      newstep <= 9'bx;
      step <= 64'bx;
    end
    else  begin
      index <= index_;
      tmpindex <= tmpindex_;
      newstep <= newstep_;
      step <= step_;
    end
  end  // always @(posedge...)

  always @*  begin
    outStream_d_ = 16'bx;

    flag_only_0_ = 1'bx;

    index_ = index;
    tmpindex_ = tmpindex;
    newstep_ = newstep;
    step_ = step;

    quantSteps_a_ = 6'bx;

    did_goto_ = 0;

    if (statecase == statecase_1)  begin
      begin
        quantSteps_a_ = index_;
        step_ = quantSteps_d_;
        tmp0_ = step_[7:0];
        newstep_ = {1'sb0,tmp0_};
        outStream_d_ = (inStream_d * newstep_);
        tmpindex_ = (index_ + 1'd1);
        flag_only_0_ = (tmpindex_ == 7'd64);
        if (flag_only_0_)  begin
          index_ = 1'b0;
        end
        else  begin
          index_ = (index_ + 1'd1);
        end
      end
    end
  end  // always @*

  always @*  begin
    case (quantSteps_a_)
      6'd0 : quantSteps_d_ = 5'd16 ;
      6'd1 : quantSteps_d_ = 5'd17 ;
      6'd2 : quantSteps_d_ = 5'd17 ;
      6'd3 : quantSteps_d_ = 5'd18 ;
      6'd4 : quantSteps_d_ = 5'd23 ;
      6'd5 : quantSteps_d_ = 4'd13 ;
      6'd6 : quantSteps_d_ = 5'd19 ;
      6'd7 : quantSteps_d_ = 5'd25 ;
      6'd8 : quantSteps_d_ = 5'd25 ;
      6'd9 : quantSteps_d_ = 5'd16 ;
      6'd10 : quantSteps_d_ = 5'd18 ;
      6'd11 : quantSteps_d_ = 5'd29 ;
      6'd12 : quantSteps_d_ = 5'd27 ;
      6'd13 : quantSteps_d_ = 6'd33 ;
      6'd14 : quantSteps_d_ = 5'd24 ;
      6'd15 : quantSteps_d_ = 5'd31 ;
      6'd16 : quantSteps_d_ = 6'd36 ;
      6'd17 : quantSteps_d_ = 6'd37 ;
      6'd18 : quantSteps_d_ = 6'd34 ;
      6'd19 : quantSteps_d_ = 5'd31 ;
      6'd20 : quantSteps_d_ = 5'd19 ;
      6'd21 : quantSteps_d_ = 5'd27 ;
      6'd22 : quantSteps_d_ = 6'd39 ;
      6'd23 : quantSteps_d_ = 6'd50 ;
      6'd24 : quantSteps_d_ = 6'd41 ;
      6'd25 : quantSteps_d_ = 6'd52 ;
      6'd26 : quantSteps_d_ = 6'd63 ;
      6'd27 : quantSteps_d_ = 5'd28 ;
      6'd28 : quantSteps_d_ = 5'd17 ;
      6'd29 : quantSteps_d_ = 6'd45 ;
      6'd30 : quantSteps_d_ = 6'd60 ;
      6'd31 : quantSteps_d_ = 6'd61 ;
      6'd32 : quantSteps_d_ = 7'd66 ;
      6'd33 : quantSteps_d_ = 6'd57 ;
      6'd34 : quantSteps_d_ = 6'd48 ;
      6'd35 : quantSteps_d_ = 5'd20 ;
      6'd36 : quantSteps_d_ = 6'd33 ;
      6'd37 : quantSteps_d_ = 6'd55 ;
      6'd38 : quantSteps_d_ = 6'd59 ;
      6'd39 : quantSteps_d_ = 7'd68 ;
      6'd40 : quantSteps_d_ = 7'd81 ;
      6'd41 : quantSteps_d_ = 6'd49 ;
      6'd42 : quantSteps_d_ = 5'd21 ;
      6'd43 : quantSteps_d_ = 5'd20 ;
      6'd44 : quantSteps_d_ = 6'd51 ;
      6'd45 : quantSteps_d_ = 7'd86 ;
      6'd46 : quantSteps_d_ = 7'd64 ;
      6'd47 : quantSteps_d_ = 6'd56 ;
      6'd48 : quantSteps_d_ = 6'd35 ;
      6'd49 : quantSteps_d_ = 6'd32 ;
      6'd50 : quantSteps_d_ = 6'd56 ;
      6'd51 : quantSteps_d_ = 7'd64 ;
      6'd52 : quantSteps_d_ = 6'd56 ;
      6'd53 : quantSteps_d_ = 5'd20 ;
      6'd54 : quantSteps_d_ = 5'd22 ;
      6'd55 : quantSteps_d_ = 6'd48 ;
      6'd56 : quantSteps_d_ = 6'd52 ;
      6'd57 : quantSteps_d_ = 5'd31 ;
      6'd58 : quantSteps_d_ = 5'd22 ;
      6'd59 : quantSteps_d_ = 6'd35 ;
      6'd60 : quantSteps_d_ = 5'd20 ;
      6'd61 : quantSteps_d_ = 4'd15 ;
      6'd62 : quantSteps_d_ = 5'd16 ;
      6'd63 : quantSteps_d_ = 4'd8 ;
      default : quantSteps_d_ = 64'bx ;
    endcase  // case (state_reg)
  end  // always @*

endmodule  // jdquant_d1_ScOrEtMp51_dp
