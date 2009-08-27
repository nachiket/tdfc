/*
 module misc (a, b, o);
   input  [1:0] a;
   input  signed [2:0] b;
   output signed [4:0] o;

   assign o = a+b;
// assign o = a-b;
// assign o = a*b;
// assign o = b<<a;
// assign o = b>>a;
// assign o = b<<<a;	// syntax error in Synplify Pro 7.1
// assign o = b>>>a;	// syntax error in Synplify Pro 7.1
// assign o = b%a;
// assign o = b%4;
// assign o = b/a;
// assign o = b/4;
   
// wire   signed[4:0] t = b<<a;
// assign o = {6'd0, t[3:0]};

endmodule // misc
*/

/*
module misc (clock,reset,i,o);
   input        clock, reset;
   input  [7:0] i;
   output [7:0] o;
   reg    [7:0] r, r_;
   assign 	o=r;
   always @(posedge clock or negedge reset)
      if (!reset) r<=0; else r<=r_;
   always @(*) begin
      r_=r;
      if (r<10) r_=r+i;
   end
endmodule // misc
*/


/*
module misc (clock,reset,i,o);
   input        clock, reset;
   input  [7:0] i;
   output [7:0] o;
   reg    [7:0] r, r_, t;
   assign 	o=r;
   always @(posedge clock or negedge reset)
      if (!reset) begin
	 t=1<<3;
	 r=t;
      end
      else begin
	 r=r_;
      end
   always @(*) begin
      r_=r;
      if (r<10) r_=r+i;
   end
endmodule // misc
*/


module misc (a,b,o) ;
   input         [3:0] a;
   input  signed [7:0] b;
   output signed [7:0] o;
// output        [7:0] o;

// wire   signed [4:0] as;
// assign as = {1'sb1,a};
// assign o  = (as > b);

// -   signed shift ">>>" does not exist
// - unsigned shift ">>"  is unsigned regardless of operand signs
// - divide         "/"   is defined only for unsigned operands
// - hence, shift of signed must be implemented manually
// assign o = b << 2;
// assign o = {3{b[7]},b[6:2]};			// o = b >>> 2;
// assign o = b >> 2;
   assign o = {b[7],b[4:0],2{1'b0}};		// o = b <<< 2;
   
// assign o = ($signed({1'b0,a}) +  b);
// assign o = ($signed({1'b0,a}) -  b);
// assign o = ($signed({1'b0,a}) *  b);
// assign o = ($signed({1'b0,a}) /  b);
// assign o = ($signed({1'b0,a}) %  b);
// assign o = ($signed({1'b0,a}) << b);
// assign o = ($signed({1'b0,a}) >> b);
// assign o = (b << $signed({1'b0,a}));
// assign o = (b >> $signed({1'b0,a}));
// assign o = ($signed({1'b0,a}) <  b);
// assign o = ($signed({1'b0,a}) >  b);
// assign o = ($signed({1'b0,a}) <= b);
// assign o = ($signed({1'b0,a}) >= b);
// assign o = ($signed({1'b0,a}) == b);
// assign o = ($signed({1'b0,a}) != b);

endmodule // misc
