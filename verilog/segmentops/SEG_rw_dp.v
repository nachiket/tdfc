// Verilog data-path module for segrw_d1_ScOrEtMp0
// Emitted by ../../tdfc version 1.158, Tue Dec 14 12:06:25 2004

module segrw_d1_ScOrEtMp0_dp (clock, reset, addr_d, dataR_d, dataW_d, write_d, state, statecase, flag_steady_0, flag_steady_1);

  input  clock;
  input  reset;

  input  [3:0] addr_d;
  output [7:0] dataR_d;
  input  [7:0] dataW_d;
  input  write_d;

  input  state;
  input  [1:0] statecase;

  output flag_steady_0;
  output flag_steady_1;

  parameter state_start = 1'd0;
  parameter state_steady = 1'd1;

  parameter statecase_stall = 2'd0;
  parameter statecase_1 = 2'd1;
  parameter statecase_2 = 2'd2;

//reg [7:0] contents_at_addrreg, contents_at_addrreg_;	// -EC
  reg [7:0] contents_at_addrreg_;			// +EC
  reg [7:0] contents [15:0];				// +EC
  reg 	    en_;					// +EC
   
  reg writereg, writereg_;
  reg [3:0] addrreg, addrreg_;
  reg [7:0] datareg, datareg_;

  reg [7:0] dataR_d_;

  reg flag_steady_0_;
  reg flag_steady_1_;

  reg did_goto_;

  assign dataR_d = dataR_d_;

  assign flag_steady_0 = flag_steady_0_;
  assign flag_steady_1 = flag_steady_1_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
//    contents_at_addrreg <= 8'bx;					// -EC
      writereg <= 1'bx;
      addrreg <= 4'bx;
      datareg <= 8'bx;
    end
    else  begin
//    contents_at_addrreg <= contents_at_addrreg_;			// -EC
      if (en_) begin contents[addrreg] <= contents_at_addrreg_; end;	// +EC
      writereg <= writereg_;
      addrreg <= addrreg_;
      datareg <= datareg_;
    end
  end  // always @(posedge...)

  always @*  begin
    dataR_d_ = 8'bx;

    flag_steady_0_ = 1'bx;
    flag_steady_1_ = 1'bx;

    contents_at_addrreg_ = contents_at_addrreg;
    writereg_ = writereg;
    addrreg_ = addrreg;
    datareg_ = datareg;

    en_       = 0;					// +EC

    did_goto_ = 0;

    case (state)
      state_start:  begin
        if (statecase == statecase_1)  begin
          addrreg_ = addr_d;
          datareg_ = dataW_d;
          writereg_ = write_d;
          did_goto_ = 1;
        end
      end
      state_steady:  begin
        if (statecase == statecase_1)  begin
          flag_steady_0_ = writereg_;
          if (flag_steady_0_)  begin
            contents_at_addrreg_ = datareg_;
	    en_ = 1;					// +EC
          end
          else  begin
            // dataR_d_ = contents_at_addrreg_;		// -EC
            dataR_d_ = contents[addrreg];		// +EC
          end
          addrreg_ = addr_d;
          datareg_ = dataW_d;
          writereg_ = write_d;
          did_goto_ = 1;
          did_goto_ = 1;
        end
        else if (statecase == statecase_2)  begin
          flag_steady_1_ = writereg_;
          if (flag_steady_1_)  begin
            contents_at_addrreg_ = datareg_;
	    en_ = 1;					// +EC
          end
          else  begin
            // dataR_d_ = contents_at_addrreg_;		// -EC
            dataR_d_ = contents[addrreg];		// +EC
          end
          did_goto_ = 1;
        end
      end
    endcase  // case (state_reg)
  end  // always @*

endmodule  // segrw_d1_ScOrEtMp0_dp
