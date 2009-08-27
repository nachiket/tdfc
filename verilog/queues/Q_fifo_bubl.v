// Q_fifo_bubl.v
//
//  - FIFO with back-pressure and bubbling,
//      with parameterizable depth, bit width
//  - Stream I/O is triple (data, valid, back-pressure),
//      with EOS concatenated into the data
//  - Queue storage is shift-chain of independently enabled data/valid regs
//  - Data/valid pair bubbles from input towards output
//      until it reaches a back-pressure stalled stage
//  - Flow control for input & output is
//      * combinationally decoupled for valid  (v bubbles down w/data)
//      * combinationally   coupled for back-pressure  (cascade of AND gates)
//  - 1 <= depth <= 256
//      * (depth <= 256) can be modified
//           by changing ''// synthesis loop_limit X'' below
//          and changing ''addrwidth'' or its log computation
//      * depth  = 1    is implemented in Q_fifo_bubl_1  (scalar reg syntax)
//      * depth >= 2    is implemented in Q_fifo_bubl_2  (array  reg syntax)
//      * general depth is implemented in Q_fifo_bubl    (chooses _1 or _2)
//  - 1 <= width
//
//  - Synplify 7.1
//  - Eylon Caspi,  10/22/03


`ifdef  Q_fifo_bubl
`else
`define Q_fifo_bubl


module Q_fifo_bubl_2 (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth = 16;    // - greatest #items in queue  (2 <= depth <= 256)
   parameter width = 16;    // - width of data (i_d, o_d)

   parameter addrwidth = 8;

   /*   
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
   */
   
   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure

   reg 	  signed [addrwidth:0] i_, j_, k_, l_;		// - stage indexes
   reg			       bp_ [depth-1:0];		// - stage backpressure
   reg 			       en_ [depth-1:0];		// - stage enable = ~bp
   reg 			       v   [depth-1:0];		// - stage valid bit
   reg    [width-1:0]	       d   [depth-1:0];		// - stage data

   assign o_d = d[depth-1];			// - output data,  last  stage
   assign o_v = v[depth-1];			// - output valid, last  stage
   assign i_b = bp_[0];				// - input  bp,    first stage

   always @(posedge clock or negedge reset) begin	// - seq always: v
      if (!reset) begin
	 // synthesis loop_limit 256
	 for (i_=depth-1; i_>=0; i_=i_-1) begin
	    v[i_] <= 0;
	 end
      end
      else begin
	 // synthesis loop_limit 256
	 for (i_=depth-1; i_>0; i_=i_-1) begin
	    if (en_[i_]) begin
	       v[i_] <= v[i_-1];
	    end
	 end
	 if (en_[0]) begin
	    v[0] <= i_v;
	 end
      end
   end // always @ (posedge clock or negedge reset)

   always @(posedge clock) begin			// - seq always: d
      // - no reset for d[] -- undefined on reset
      // synthesis loop_limit 256
      for (j_=depth-1; j_>0; j_=j_-1) begin
	 if (en_[j_]) begin
	    d[j_] <= d[j_-1];
	 end
      end
      if (en_[0]) begin
	 d[0] <= i_d;
      end
   end // always @ (posedge clock)

   always @* begin				// - combi always: bp_, en_
      bp_[depth-1] <= o_b & v[depth-1];
      // synthesis loop_limit 256
      for (k_=depth-2; k_>=0; k_=k_-1) begin
	 bp_[k_] <= bp_[k_+1] & v[k_];
      end
      // synthesis loop_limit 256
      for (l_=depth-1; l_>=0; l_=l_-1) begin	// - en[] = ~bp[]
	 en_[l_] <= ~bp_[l_];
      end
   end
      
endmodule // Q_fifo_bubl_2


module Q_fifo_bubl_1 (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

// parameter depth = 1;     // - greatest #items in queue  (depth = 1)
   parameter width = 16;    // - width of data (i_d, o_d)

   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure

   reg			       bp_;			// - stage backpressure
   reg 			       en_;			// - stage enable = ~bp
   reg 			       v;			// - stage valid bit
   reg    [width-1:0]	       d;			// - stage data

   assign o_d = d;				// - output data,  last  stage
   assign o_v = v;				// - output valid, last  stage
   assign i_b = bp_;				// - input  bp,    first stage

   always @(posedge clock or negedge reset) begin	// - seq always: v
      if (!reset) begin
	 v <= 0;
      end
      else begin
	 if (en_) begin
	    v <= i_v;
	 end
      end
   end // always @ (posedge clock or negedge reset)

   always @(posedge clock) begin			// - seq always: d
      // - no reset for d -- undefined on reset
      if (en_) begin
	 d <= i_d;
      end
   end // always @ (posedge clock)

   always @* begin				// - combi always: bp_, en_
      bp_ <= o_b & v;
      en_ <= ~bp_;
   end
      
endmodule // Q_fifo_bubl_1


// - Q_fifo_bubl := (depth==1) ? Q_fifo_bubl_1 : Q_fifo_bubl_2
// - Q_fifo_bubl implementation below is a convoluted way to choose
//     Q_fifo_bubl_1 or Q_fifo_bubl_2 based on whether depth is 1
// - We instantiate BOTH Q's, multiplex their outputs on (depth==1),
//     then let constant folding eliminate the unused Q


module Q_fifo_bubl (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth = 1;     // - greatest #items in queue  (1 <= depth <= 256)
   parameter width = 1;     // - width of data (i_d, o_d)

   input     clock;
   input     reset;
   
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)
   input              i_v;	// - input  stream valid
   output             i_b;	// - input  stream back-pressure

   output [width-1:0] o_d;	// - output stream data (concat data + eos)
   output             o_v;	// - output stream valid
   input              o_b;	// - output stream back-pressure

   wire   [width-1:0] o_d_1, o_d_2;
   wire 	      o_v_1, o_v_2;
   wire 	      i_b_1, i_b_2;

   assign o_d = (depth==1) ? o_d_1 : o_d_2;
   assign o_v = (depth==1) ? o_v_1 : o_v_2;
   assign i_b = (depth==1) ? i_b_1 : i_b_2;

   parameter depth2 = (depth==1) ? 2 : depth;	// safe depth for Q_fifo_bubl_2
   
   Q_fifo_bubl_1 #(       width) q1(clock,reset,i_d,i_v,i_b_1,o_d_1,o_v_1,o_b);
   Q_fifo_bubl_2 #(depth2,width) q2(clock,reset,i_d,i_v,i_b_2,o_d_2,o_v_2,o_b);

endmodule // Q_fifo_bubl


`endif  // `ifdef  Q_fifo_bubl
