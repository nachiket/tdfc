// Verilog data-path module for ftabmod_noinline_d1_ScOrEtMp57
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module ftabmod_noinline_d1_ScOrEtMp57_dp (clock, reset, ind_d, oval_d, segment_r_addr_d, segment_r_data_d, state, statecase);

  input  clock;
  input  reset;

  input  [7:0] ind_d;
  output [7:0] oval_d;
  output [31:0] segment_r_addr_d;
  input  [63:0] segment_r_data_d;

  input  state;
  input  statecase;

  parameter state_only = 1'd0;
  parameter state_only_1 = 1'd1;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [63:0] intermed, intermed_;

  reg [7:0] oval_d_;
  reg [31:0] segment_r_addr_d_;

  reg did_goto_;

  assign oval_d = oval_d_;
  assign segment_r_addr_d = segment_r_addr_d_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
      intermed <= 64'bx;
    end
    else  begin
      intermed <= intermed_;
    end
  end  // always @(posedge...)

  always @*  begin
    oval_d_ = 8'bx;
    segment_r_addr_d_ = 32'bx;

    intermed_ = intermed;

    did_goto_ = 0;

    case (state)
      state_only:  begin
        if (statecase == statecase_1)  begin
          begin
            segment_r_addr_d_ = ind_d;
            did_goto_ = 1;
          end
        end
      end
      state_only_1:  begin
        if (statecase == statecase_1)  begin
          begin
            intermed_ = segment_r_data_d;
            oval_d_ = intermed_[7:0];
            did_goto_ = 1;
          end
        end
      end
    endcase  // case (state_reg)
  end  // always @*

endmodule  // ftabmod_noinline_d1_ScOrEtMp57_dp
