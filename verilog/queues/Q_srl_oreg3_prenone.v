// Q_srl_oreg3_prenone.v
//
//  - In-page queue with parameterizable depth, bit width
//  - Stream I/O is triple (data, valid, back-pressure),
//      with EOS concatenated into the data
//  - Flow control for input & output is combinationally decoupled
//  - 2 <= depth <= 256
//      * (depth >= 2)  is required to decouple I/O flow control,
//          where empty => no produce,  full => no consume,
//          and depth 1 would ping-pong between the two at half rate
//      * (depth <= 256) can be modified
//           by changing ''synthesis loop_limit X'' below
//          and changing ''addrwidth'' or its log computation
//  - 1 <= width
//  - Queue storage is in SRL16E, up to depth 16 per LUT per bit-slice,
//      plus output register (for fast output)
//  - Queue addressing is done by ''addr'' up-down counter
//  - Queue fullness is checked by comparator (addr==depth)
//  - FSM has 3 states (empty, one, more)
//  - When empty, continue to emit most recently emitted value (for debugging)
//
//  - Queue slots used      = / (state==state_empty) ? 0
//                            | (state==state_one)   ? 1
//                            \ (state==state_more)  ? addr+2
//  - Queue slots used     <=  depth
//  - Queue slots remaining =  depth - used
//                          = / (state==state_empty) ? depth
//                            | (state==state_one)   ? depth-1
//                            \ (state==state_more)  ? depth-2-addr
//
//  - Synplify 7.1 / 8.0
//  - Eylon Caspi,  9/11/03, 8/18/04, 3/29/05


`ifdef  Q_srl
`else
`define Q_srl


module Q_srl (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth = 16;   // - greatest #items in queue  (2 <= depth <= 256)
   parameter width = 16;   // - width of data (i_d, o_d)
   
   `define LOG2 (  (((depth))     ==0) ? 0	/* - depth==0   LOG2=0 */ \
		 : (((depth-1)>>0)==0) ? 0	/* - depth<=1   LOG2=0 */ \
		 : (((depth-1)>>1)==0) ? 1	/* - depth<=2   LOG2=1 */ \
		 : (((depth-1)>>2)==0) ? 2	/* - depth<=4   LOG2=2 */ \
		 : (((depth-1)>>3)==0) ? 3	/* - depth<=8   LOG2=3 */ \
		 : (((depth-1)>>4)==0) ? 4	/* - depth<=16  LOG2=4 */ \
		 : (((depth-1)>>5)==0) ? 5	/* - depth<=32  LOG2=5 */ \
		 : (((depth-1)>>6)==0) ? 6	/* - depth<=64  LOG2=6 */ \
		 : (((depth-1)>>7)==0) ? 7	/* - depth<=128 LOG2=7 */ \
		 :                       8)	/* - depth<=256 LOG2=8 */   

// parameter addrwidth = LOG2;			// - width of queue addr

   parameter addrwidth =
		(  (((depth))     ==0) ? 0	// - depth==0   LOG2=0
		 : (((depth-1)>>0)==0) ? 0	// - depth<=1   LOG2=0
		 : (((depth-1)>>1)==0) ? 1	// - depth<=2   LOG2=1
		 : (((depth-1)>>2)==0) ? 2	// - depth<=4   LOG2=2
		 : (((depth-1)>>3)==0) ? 3	// - depth<=8   LOG2=3
		 : (((depth-1)>>4)==0) ? 4	// - depth<=16  LOG2=4
		 : (((depth-1)>>5)==0) ? 5	// - depth<=32  LOG2=5
		 : (((depth-1)>>6)==0) ? 6	// - depth<=64  LOG2=6
		 : (((depth-1)>>7)==0) ? 7	// - depth<=128 LOG2=7
		 :                       8)	// - depth<=256 LOG2=8
		 ;

   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure

   reg    [addrwidth-1:0] addr, addr_, a_;		// - SRL16 address
							//     for data output
   reg 			  shift_en_;			// - SRL16 shift enable
   reg    [width-1:0] 	  srl [depth-2:0];		// - SRL16 memory
   reg 			  shift_en_o_;			// - SRLO  shift enable
   reg    [width-1:0] 	  srlo_, srlo			// - SRLO  output reg
			  /* synthesis syn_allow_retiming=0 */ ;

   parameter state_empty = 2'd0;    // - state empty : o_v=0 o_d=UNDEFINED
   parameter state_one   = 2'd1;    // - state one   : o_v=1 o_d=srlo
   parameter state_more  = 2'd2;    // - state more  : o_v=1 o_d=srlo
				    //     #items in srl = addr+2

   reg [1:0] state, state_;	    // - state register

   wire      addr_full_;	    // - true iff addr==depth-2
   wire      addr_zero_;	    // - true iff addr==0

   assign addr_full_ = (state==state_more) && (addr==depth-2);
						// - queue full
   assign addr_zero_ = (addr==0);		// - queue contains 2 (or 1,0)
   assign o_d = srlo;				// - output data from queue
   assign o_v = (state!=state_empty);		// - output valid if non-empty
   assign i_b = addr_full_;			// - input bp if full

   // - ''always'' block with both FFs and SRL16 does not work,
   //      since FFs need reset but SRL16 does not

   always @(posedge clock or negedge reset) begin	// - seq always: FFs
      if (!reset) begin
	 state     <= state_empty;
	 addr      <= 0;
      end
      else begin
	 state     <= state_;
	 addr      <= addr_;
      end
   end // always @ (posedge clock or negedge reset)

   always @(posedge clock or negedge reset) begin	// - seq always: srlo
      // - infer enabled output reg at end of shift chain
      // - input first element from i_d, all subsequent elements from SRL16
      if (!reset) begin
	 srlo <= 0;
      end
      else begin
	 if (shift_en_o_) begin
	    srlo <= srlo_;
	 end
      end
   end // always @ (posedge clock or negedge reset)

   always @(posedge clock) begin			// - seq always: srl
      // - infer enabled SRL16E from shifting srl array
      // - no reset capability;  srl[] contents undefined on reset
      if (shift_en_) begin
	 // synthesis loop_limit 256
	 for (a_=depth-2; a_>0; a_=a_-1) begin
	    srl[a_] <= srl[a_-1];
	 end
	 srl[0] <= i_d;
      end
   end // always @ (posedge clock or negedge reset)

   always @* begin					// - combi always
      case (state)

	state_empty: begin		    // - (empty, will not produce)
	      if (i_v) begin		    // - empty & i_v => consume
		 srlo_       <= i_d;
		 shift_en_o_ <= 1;
		 shift_en_   <= 1'bx;
		 addr_       <= 0;
		 state_      <= state_one;
	      end
	      else	begin		    // - empty & !i_v => idle
		 srlo_       <= 'bx;
		 shift_en_o_ <= 0;
		 shift_en_   <= 1'bx;
		 addr_       <= 0;
		 state_      <= state_empty;
	      end
	end

	state_one: begin		    // - (contains one)
	      if (i_v && o_b) begin	    // - one & i_v & o_b => consume
		 srlo_       <= 'bx;
		 shift_en_o_ <= 0;
		 shift_en_   <= 1;
		 addr_       <= 0;
		 state_      <= state_more;
	      end
	      else if (i_v && !o_b) begin   // - one & i_v & !o_b => cons+prod
		 srlo_       <= i_d;
		 shift_en_o_ <= 1;
		 shift_en_   <= 1;
		 addr_       <= 0;
		 state_      <= state_one;
	      end
	      else if (!i_v && o_b) begin   // - one & !i_v & o_b => idle
		 srlo_       <= 'bx;
		 shift_en_o_ <= 0;
		 shift_en_   <= 1'bx;
		 addr_       <= 0;
		 state_      <= state_one;
	      end
	      else if (!i_v && !o_b) begin  // - one & !i_v & !o_b => produce
		 srlo_       <= 'bx;
		 shift_en_o_ <= 0;
		 shift_en_   <= 1'bx;
		 addr_       <= 0;
		 state_      <= state_empty;
	      end
	end // case: state_one
	
	state_more: begin		    // - (contains more than one)
	   if (addr_full_ || (depth==2)) begin
					    // - (full, will not consume) 
					    // - (full here if depth==2)
	      if (o_b) begin		    // - full & o_b => idle
		 srlo_       <= 'bx;
		 shift_en_o_ <= 0;
		 shift_en_   <= 0;
		 addr_       <= addr;
		 state_      <= state_more;
	      end
	      else begin		    // - full & !o_b => produce
		 srlo_       <= srl[addr];
		 shift_en_o_ <= 1;
		 shift_en_   <= 0;
//		 addr_       <= addr-1;
//		 state_      <= state_more;
		 addr_       <= addr_zero_ ? 0         : addr-1;
		 state_      <= addr_zero_ ? state_one : state_more;
	      end
	   end
	   else begin			    // - (mid: neither empty nor full)
	      if (i_v && o_b) begin	    // - mid & i_v & o_b => consume
		 srlo_       <= 'bx;
		 shift_en_o_ <= 0;
		 shift_en_   <= 1;
		 addr_       <= addr+1;
		 state_      <= state_more;
	      end
	      else if (i_v && !o_b) begin   // - mid & i_v & !o_b => cons+prod
		 srlo_       <= srl[addr];
		 shift_en_o_ <= 1;
		 shift_en_   <= 1;
		 addr_       <= addr;
		 state_      <= state_more;
	      end
	      else if (!i_v && o_b) begin   // - mid & !i_v & o_b => idle
		 srlo_       <= 'bx;
		 shift_en_o_ <= 0;
		 shift_en_   <= 0;
		 addr_       <= addr;
		 state_      <= state_more;
	      end
	      else if (!i_v && !o_b) begin  // - mid & !i_v & !o_b => produce
		 srlo_       <= srl[addr];
		 shift_en_o_ <= 1;
		 shift_en_   <= 0;
		 addr_       <= addr_zero_ ? 0         : addr-1;
		 state_      <= addr_zero_ ? state_one : state_more;
	      end
	   end // else: !if(addr_full_)
	end // case: state_more

	default: begin
		 srlo_       <=  'bx;
		 shift_en_o_ <= 1'bx;
		 shift_en_   <= 1'bx;
		 addr_       <=  'bx;
		 state_      <= 2'bx;
	end // case: default

      endcase // case(state)
   end // always @ *
   
endmodule // Q_srl


`endif  // `ifdef  Q_srl
