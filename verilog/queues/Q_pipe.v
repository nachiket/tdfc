// Q_pipe.v
//
//  - Pipelined interconnect stage(s),
//      registering data/valid    in the forward  direction,
//      registering back-pressure in the backward direction,
//      with parameterizable depth, bit width
//        d --> d --> d --> d
//        v --> v --> v --> v
//        b <-- b <-- b <-- b
//  - Registers are retimable
//  - Stream I/O is triple (data, valid, back-pressure),
//      with EOS concatenated into the data
//  - Initialization values:  valid         all 0  (invalid data),
//                            back-pressure all 1  (do not send),
//                            data          all undefined
//  - Back-pressure initialization to 1 is in case network had been
//      drained s.t. downstream queue is already full on startup
//  - 1 <= depth <= 256
//      * (depth <= 256) can be modified
//           by changing ''// synthesis loop_limit X'' below
//          and changing ''addrwidth'' or its log computation
//  - 1 <= width
//
//  - Synplify 7.1
//  - Eylon Caspi,  10/27/03


`ifdef  Q_pipe
`else
`define Q_pipe


module Q_pipe (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);

   parameter depth =  1;    // - greatest #items in queue  (1 <= depth <= 256)
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

   reg 	  signed [addrwidth:0] i_, j_, k_;	// - stage indexes
   reg			       b [depth-1:0]	// - stage backpressure
			         /* synthesis syn_srlstyle="registers" */
			         /* synthesis syn_allow_retiming=1     */ ;
   reg 			       v [depth-1:0]	// - stage valid bit
			         /* synthesis syn_srlstyle="registers" */
			         /* synthesis syn_allow_retiming=1     */ ;
   reg           [width-1:0]   d [depth-1:0]	// - stage data
			         /* synthesis syn_srlstyle="registers" */
			         /* synthesis syn_allow_retiming=1     */ ;

   assign o_d = d[depth-1];			// - output data,  last  stage
   assign o_v = v[depth-1];			// - output valid, last  stage
   assign i_b = b[0];				// - input  bp,    first stage

   always @(posedge clock or negedge reset) begin	// - seq always: v, b
      if (!reset) begin
	 // synthesis loop_limit 256
	 for (i_=depth-1; i_>=0; i_=i_-1) begin
	    v[i_] <= 0;
	 end
	 for (j_=0; j_<=depth-1; j_=j_+1) begin
	    b[j_] <= 1;
	 end
      end
      else begin
	 // synthesis loop_limit 256
	 for (i_=depth-1; i_>0; i_=i_-1) begin
	    v[i_] <= v[i_-1];
	 end
	 v[0] <= i_v;
	 // synthesis loop_limit 256
	 for (j_=0; j_<depth-1; j_=j_+1) begin
	    b[j_] <= b[j_+1];
	 end
	 b[depth-1] <= o_b;
      end
   end // always @ (posedge clock or negedge reset)

   always @(posedge clock) begin			// - seq always: d
      // - no reset for d[] or b[] -- undefined on reset
      // synthesis loop_limit 256
      for (k_=depth-1; k_>0; k_=k_-1) begin
	 d[k_] <= d[k_-1];
      end
      d[0] <= i_d;
   end // always @ (posedge clock)
      
endmodule // Q_pipe


`endif  // `ifdef  Q_pipe
