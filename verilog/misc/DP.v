// sample datapath

module DP (clock, reset, state, i1, i2, o1, o2, f1, f2);

  input  clock, reset;
  input  state,  i1, i2;
  output o1, o2, f1, f2;

  reg  r1, r2;			// r1 & r2

  wire r1v1, r1v2, r1v3, r1v4;	// r1 values from each state DP
  wire r2v1, r2v2, r2v3, r2v4;	// r2 values from each state DP
  wire o1v1, o1v2, o1v3, o1v4;	// o1 values from each state DP
  wire o2v1, o2v2, o2v3, o2v4;	// o2 values from each state DP
  wire f1v1, f1v2, f1v3, f1v4;	// f1 values from each state DP
  wire f2v1, f2v2, f2v3, f2v4;	// f2 values from each state DP

  assign r1v1 = i1;		// DP for state 1
  assign r2v1 = i2;
  assign o1v1 = r1;
  assign o2v1 = r2;
  assign f1v1 = 0;
  assign f2v1 = 0;

  assign r1v2 = i1+1;		// DP for state 2
  assign r2v2 = i2+1;
  assign o1v2 = r1+1;
  assign o2v2 = r2+1;
  assign f1v2 = i1>0;
  assign f2v2 = 0;

  assign r1v3 = i1+i2;		// DP for state 3
  assign r2v3 = i2+i1;
  assign o1v3 = r1+r2;
  assign o2v3 = r2+r1;
  assign f1v3 = 0;
  assign f2v3 = i2>0;

  assign r1v4 = i1+r1;		// DP for state 4
  assign r2v4 = i2+r2;
  assign o1v4 = r1+i1;
  assign o2v4 = r2+i2;
  assign f1v4 = r1v1>0;
  assign f2v4 = r2v4>0;

  assign o1   = MUX(state,o1v1,o1v2,o1v3,o1v4);
  assign o2   = MUX(state,o2v1,o2v2,o2v3,o2v4);
  assign f1   = MUX(state,f1v1,f1v2,f1v3,f1v4);
  assign f2   = MUX(state,f2v1,f2v2,f2v3,f2v4);
//assign r1in = MUX(state,r1v1,r1v2,r1v3,r1v4);
//assign r2in = MUX(state,r2v1,r2v2,r2v3,r2v4);

  // - Problem:  How to shrink MUXes with don't-care inputs?
  //             + For regs r1, r2, use "case" in sequential part,
  //               with default case r1<=r1
  //             + For outputs o1, o2, f1, f2 ???
   
  always @(posedge clock) begin

    // - behavioral version of r1, r2 muxes:
    case (state)
      state1  :  r1<=r1v1; r2<=r1v1; // o1<=o1v1; o2<=o2v1; f1<=f1v1; f2<=f2v1;
      state2  :  r1<=r1v2; r2<=r1v2; // o1<=o1v2; o2<=o2v2; f1<=f1v2; f2<=f2v2;
      state3  :  r1<=r1v3; r2<=r1v3; // o1<=o1v3; o2<=o2v3; f1<=f1v3; f2<=f2v3;
      state4  :  r1<=r1v4; r2<=r1v4; // o1<=o1v4; o2<=o2v4; f1<=f1v4; f2<=f2v4;
      default :  r1<=r1;   r2<=r2;
    endcase // case(state)

    // - structural version of r1, r2 muxes:
    r1=r2in;
    r2=r2in;

  end

endmodule;
