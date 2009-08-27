module blockingassign (clock, reset, i, o);

  input  clock, reset, i;
  output o;

  reg  r, r_,  s, s_,  t, t_;
  reg  tmp1_,  tmp2_;

  assign o = r&s&t;

  always @(posedge clock or negedge reset)
    if (!reset)  begin  r<=0;  s<=0;  t<=0;   end
    else         begin  r<=r_; s<=s_; t<=t_;  end

  always @* begin

    tmp1_ = 1;
    tmp1_ = i ^ tmp1_;
    s_    = tmp1_;

    r_ = 0;
    r_ = i ^ r_;

    if (i) begin
      tmp2_ = s;		// - tmp2_ is used as a local variable
      t_    = r ^ tmp2_;	//     in this block; Synplify is smart
    end				//     enough to not generate a latch
    else begin
      t_    = r;
    end

  end

endmodule
