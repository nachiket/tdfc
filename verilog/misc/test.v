module kaka;

wire clock, x, y, z;

always @(x or y) begin
  z = x+y;
end

always @(posedge clock) begin
  x = y;
end

endmodule
