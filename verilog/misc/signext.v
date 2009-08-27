function signext (i);
  parameter Wo = 32;	// - width of output
  parameter Wi = 16;	// - width of input     [ assert(Wo>Wi) ]
  input [Wi-1:0] i;
  assign signext = {(Wo-Wi){i[Wi-1]},i};
endfunction

module top;
  wire [4:0] s;
  wire [8:0] l;
  assign l=#(8,4)signext(s);
endmodule
