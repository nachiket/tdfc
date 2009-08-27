// Q_srl_reserve_prefull_preresv.v
//
//  - In-page queue with parameterizable depth, bit width, reserve capacity
//  - Stream I/O is triple (data, valid, back-pressure),
//      with EOS concatenated into the data
//  - Flow control for input & output is combinationally decoupled
//  - Emit input back-pressure if number of items remaining empty
//      reaches or falls below reserve capacity,
//      but continue consuming  (useful for pipelined input)
//  - 2 <= depth <= 256
//      * (depth >= 2)  is required to decouple I/O flow control,
//          where empty => no produce,  full => no consume,
//          and depth 1 would ping-pong between the two at half rate
//      * (depth <= 256) can be modified
//           by changing ''synthesis loop_limit X'' below
//          and changing ''addrwidth'' or its log computation
//  - 1 <= width
//  - 0 <= reserve <= depth-1
//      * (reserve == 0)       is equivalent to no reserve
//      * (reserve <= depth-1) is required because reserve=depth would have
//                             back-pressure on all the time,
//                             low bp only from initial value of bp pipe regs,
//                             and hence not full throughput (bp hickups)
//  - Queue storage is in SRL16E, up to depth 16 per LUT per bit-slice
//  - Queue addressing is done by ''addr'' up-down counter
//  - Queue fullness is checked by comparator (addr==depth)
//  - Queue fullness and reserve mode are pre-computed for next cycle
//  - FSM has only 2 states (empty, non-empty)
//  - When empty, continue to emit most recently emitted value (for debugging)
//
//  - queue slots used      = (state=state_empty ? 0 : addr+1),  used<=depth
//  - queue slots remaining =  depth - used
//                          = (state=state_empty ? depth : depth-addr-1)
//  - raise back-pressure (but continue consuming) whenever remaining<=reserve
//          i.e. whenever (state!=state_empty && addr>=depth-1-reserve)
//
//  - Synplify 7.1
//  - Eylon Caspi,  9/11/03, 7/21/04


`ifdef  Q_srl_reserve
`else
`define Q_srl_reserve


module Q_srl_reserve (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth   = 16;   // - greatest #items in queue  (2 <= depth <= 256)
   parameter width   = 16;   // - width of data (i_d, o_d)
   parameter reserve =  1;   // - remaining capacity at/below which emit bp

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

// parameter addrwidth = log2(depth);		// - width of queue addr
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
   reg    [width-1:0] 	  srl [depth-1:0];		// - SRL16 memory

   parameter state_empty    = 1'b0;   // - state empty   : o_v=0 o_d=UNDEFINED
   parameter state_nonempty = 1'b1;   // - state nonempty: o_v=1 o_d=srl[addr]
				      //     #items in srl = addr+1

   reg 	     state, state_;	      // - state register

   reg       addr_full; 	      // - true iff addr==depth-1
   reg       addr_full_;	      // - true iff addr==depth-1 on NEXT cycle
   wire      addr_zero_;	      // - true iff addr==0
   reg       addr_resv; 	      // - true iff addr>=depth-1-reserve &&
				      //            state==state_nonempty
   reg       addr_resv_;	      // - true iff ...           on NEXT cycle

// assign addr_full_ = (addr_==depth-1);	// - queue full   on NEXT cycle
   assign addr_zero_ = (addr==0);		// - queue contains 1
// assign addr_resv_ = (addr_>=depth-1-reserve) && (state_==state_nonempty);
						// - queue remaining<=reserve
						//                on NEXT cycle
   assign o_d = srl[addr];			// - output data from queue
   assign o_v = (state==state_empty) ? 0 : 1;	// - output valid if non-empty
// assign i_b = addr_full;			// - input bp if full
   assign i_b = (reserve==0) ? addr_full : addr_resv;
						// - input bp if reserving

   // - ''always'' block with both FFs and SRL16 does not work,
   //      since FFs need reset but SRL16 does not

   always @(posedge clock or negedge reset) begin	// - seq always: FFs
      if (!reset) begin
	 state     <= state_empty;
	 addr      <= 0;
	 addr_full <= 0;
	 addr_resv <= 0;
      end
      else begin
	 state     <= state_;
	 addr      <= addr_;
	 addr_full <= addr_full_;
	 addr_resv <= addr_resv_;
      end
   end // always @ (posedge clock or negedge reset)

   always @(posedge clock) begin			// - seq always: SRL16
      // - infer enabled SRL16 from shifting srl array
      // - no reset capability;  srl[] contents undefined on reset
      if (shift_en_) begin
	 // synthesis loop_limit 256
	 for (a_=depth-1; a_>0; a_=a_-1) begin
	    srl[a_] <= srl[a_-1];
	 end
	 srl[0] <= i_d;
      end
   end // always @ (posedge clock or negedge reset)

   always @* begin					// - combi always
      case (state)

	state_empty: begin		// - (empty, will not produce)
	   if (i_v) begin		// - empty & i_v => consume
	      shift_en_  <= 1;
	      addr_      <= 0;
	      addr_full_ <= 0;
	      addr_resv_ <= (reserve >= depth-1);  // (0>=d-1-r)
	      state_     <= state_nonempty;
	   end
	   else	begin			// - empty & !i_v => idle
	      shift_en_  <= 0;
	      addr_      <= 0;
	      addr_full_ <= 0;
	      addr_resv_ <= 0;
	      state_     <= state_empty;
	   end
	end

	state_nonempty: begin
	   if (addr_full) begin 	    // - (full, will not consume)
	      if (o_b) begin		    // - full & o_b => idle
		 shift_en_  <= 0;
		 addr_      <= addr;
		 addr_full_ <= 1;
		 addr_resv_ <= 1;
		 state_     <= state_nonempty;
	      end
	      else begin		    // - full & !o_b => produce
		 shift_en_  <= 0;
		 addr_      <= addr-1;
		 addr_full_ <= 0;
		 addr_resv_ <= (addr >= depth-reserve);  // (a-1>=d-1-r)
		 state_     <= state_nonempty;
//		 addr_      <= addr_zero_ ? 0           : addr-1;
//		 state_     <= addr_zero_ ? state_empty : state_nonempty;
	      end
	   end
	   else begin			    // - (mid: neither empty nor full)
	      if (i_v && o_b) begin	    // - mid & i_v & o_b => consume
		 shift_en_  <= 1;
		 addr_      <= addr+1;
		 addr_full_ <= (addr==depth-2);
		 addr_resv_ <= (addr >= ((depth-2>=reserve) ?
					 (depth-2- reserve) : 0));
			    // (a+1>=d-1-r)       (a>=d-2-r)
			    // (a>=max{d-2-r,0})  (a>=((d-2-r>=0)?(d-2-r):(0)))
		 state_     <= state_nonempty;
	      end
	      else if (i_v && !o_b) begin   // - mid & i_v & !o_b => cons+prod
		 shift_en_  <= 1;
		 addr_      <= addr;
		 addr_full_ <= 0;
		 addr_resv_ <= addr_resv;
		 state_     <= state_nonempty;
	      end
	      else if (!i_v && o_b) begin   // - mid & !i_v & o_b => idle
		 shift_en_  <= 0;
		 addr_      <= addr;
		 addr_full_ <= 0;
		 addr_resv_ <= addr_resv;
		 state_     <= state_nonempty;
	      end
	      else if (!i_v && !o_b) begin  // - mid & !i_v & !o_b => produce
		 shift_en_  <= 0;
		 addr_      <= addr_zero_ ? 0           : addr-1;
		 addr_full_ <= 0;
		 addr_resv_ <= addr_zero_ ? 0 : (addr >= depth-reserve);  // (a-1>=d-1-r)
		 state_     <= addr_zero_ ? state_empty : state_nonempty;
	      end
	   end // else: !if(addr_full)
	end // case: state_nonempty

      endcase // case(state)
   end // always @ *
   
endmodule // Q_srl_reserve


`endif  // `ifdef  Q_srl_reserve
