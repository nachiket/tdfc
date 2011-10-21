// Verilog data-path module for segrw_d1_ScOrEtMp0
// Emitted by ../../tdfc version 1.158, Tue Dec 14 12:18:04 2004
// revised by Nachiket on 16/Oct/2011

module SEG_rw_dp (clock, reset, addr_d, dataR_d, dataW_d, write_d, state, statecase, flag_steady_0, flag_steady_1);

  parameter nelems=127;
  parameter awidth=7;
  parameter dwidth=7;

  input  clock;
  input  reset;

  input  [awidth-1:0] addr_d;
  output [dwidth-1:0] dataR_d;
  input  [dwidth-1:0] dataW_d;
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
  reg [nelems-1:0] contents_at_addrreg_;			// +EC
  reg [nelems-1:0] contents [dwidth-1:0] /* synthesis syn_ramstyle="block_ram" */ ;	// +EC
  reg 	    en_;					// +EC
   
  reg writereg, writereg_;
  reg [awidth-1:0] addrreg, addrreg_;
  reg [dwidth-1:0] datareg, datareg_;

  reg [dwidth-1:0] dataR_d_;

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
      addrreg <= awidth'bx;
      datareg <= dwidth'bx;
    end
    else  begin
//    contents_at_addrreg <= contents_at_addrreg_;			// -EC
      writereg <= writereg_;
      addrreg <= addrreg_;
      datareg <= datareg_;
    end
  end  // always @(posedge...)

  always @(posedge clock)  begin					// +EC
    if (en_) begin contents[addrreg] <= contents_at_addrreg_; end;	// +EC
  end									// +EC

  always @*  begin
    dataR_d_ = dwidth'bx;

    flag_steady_0_ = 1'bx;
    flag_steady_1_ = 1'bx;

    en_ = 0;						// +EC
    contents_at_addrreg_ = dwidth'bx;			// +EC

//  contents_at_addrreg_ = contents_at_addrreg;		// -EC
    writereg_ = writereg;
    addrreg_ = addrreg;
    datareg_ = datareg;

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

endmodule  // SEG_rw_dp
