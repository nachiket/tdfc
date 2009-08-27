// Verilog data-path module for DecHuff_d1_ScOrEtMp52
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module DecHuff_d1_ScOrEtMp52_dp (clock, reset, filebyte_d, reqSize_d, advance_d, parsedToken_d, state, statecase, flag_getData_0, flag_waitZero_0, flag_getData_1, flag_req_0);

  input  clock;
  input  reset;

  input  [7:0] filebyte_d;
  input  [7:0] reqSize_d;
  input  [7:0] advance_d;
  output [15:0] parsedToken_d;

  input  [2:0] state;
  input  statecase;

  output flag_getData_0;
  output flag_waitZero_0;
  output flag_getData_1;
  output flag_req_0;

  parameter state_initreq = 3'd0;
  parameter state_advancePtr = 3'd1;
  parameter state_getData = 3'd2;
  parameter state_putData = 3'd3;
  parameter state_req = 3'd4;
  parameter state_waitZero = 3'd5;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [7:0] reqSize_at_0;
  reg [7:0] currReq, currReq_;
  reg [31:0] intermed, intermed_;
  reg [7:0] marker, marker_;

  reg [31:0] tmp0_;
  reg tmp2_;
  reg [31:0] tmp1_;

  reg [15:0] parsedToken_d_;

  reg flag_getData_0_;
  reg flag_waitZero_0_;
  reg flag_getData_1_;
  reg flag_req_0_;

  reg did_goto_;

  assign parsedToken_d = parsedToken_d_;

  assign flag_getData_0 = flag_getData_0_;
  assign flag_waitZero_0 = flag_waitZero_0_;
  assign flag_getData_1 = flag_getData_1_;
  assign flag_req_0 = flag_req_0_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
      reqSize_at_0 <= 8'bx;
      currReq <= 8'bx;
      intermed <= 1'b0;
      marker <= 1'b0;
    end
    else  begin
      reqSize_at_0 <= reqSize_d;
      currReq <= currReq_;
      intermed <= intermed_;
      marker <= marker_;
    end
  end  // always @(posedge...)

  always @*  begin
    parsedToken_d_ = 16'bx;

    flag_getData_0_ = 1'bx;
    flag_waitZero_0_ = 1'bx;
    flag_getData_1_ = 1'bx;
    flag_req_0_ = 1'bx;

    currReq_ = currReq;
    intermed_ = intermed;
    marker_ = marker;

    did_goto_ = 0;

    case (state)
      state_initreq:  begin
        if (statecase == statecase_1)  begin
          begin
            currReq_ = reqSize_d;
            did_goto_ = 1;
          end
        end
      end
      state_advancePtr:  begin
        if (statecase == statecase_1)  begin
          begin
            marker_ = (marker_ - advance_d);
            did_goto_ = 1;
          end
        end
      end
      state_getData:  begin
        if (statecase == statecase_1)  begin
          begin
            tmp0_ = (intermed_ << 8'd8);
            intermed_ = (tmp0_ + filebyte_d);
            marker_ = (marker_ + 8'd8);
            flag_getData_0_ = (filebyte_d != 8'd255);
            if (flag_getData_0_)  begin
              begin
                flag_getData_1_ = (marker_ >= reqSize_at_0);
                if (flag_getData_1_)  begin
                  begin
                    did_goto_ = 1;
                  end
                end
              end
            end
            else  begin
              begin
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_putData:  begin
        if (statecase == statecase_1)  begin
          begin
            tmp1_ = (intermed_ >> (marker_ - currReq_));
            tmp2_ = (1'd1 << currReq_);
            parsedToken_d_ = (tmp1_ & (tmp2_ - 1'd1));
            did_goto_ = 1;
          end
        end
      end
      state_req:  begin
        if (statecase == statecase_1)  begin
          begin
            currReq_ = reqSize_d;
            flag_req_0_ = (marker_ >= reqSize_d);
            if (flag_req_0_)  begin
              begin
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_waitZero:  begin
        if (statecase == statecase_1)  begin
          begin
            flag_waitZero_0_ = (marker_ >= reqSize_at_0);
            if (flag_waitZero_0_)  begin
              begin
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                did_goto_ = 1;
              end
            end
          end
        end
      end
    endcase  // case (state_reg)
  end  // always @*

endmodule  // DecHuff_d1_ScOrEtMp52_dp
