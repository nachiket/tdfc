module localassign (clock, reset, state, a, b, o);

  input clock, reset, a, b;
  input [2:0] state;
  output o;

  parameter state_0 = 3'd0;
  parameter state_1 = 3'd1;
  parameter state_2 = 3'd2;
  parameter state_3 = 3'd3;
  parameter state_4 = 3'd4;
  parameter state_5 = 3'd5;
  parameter state_6 = 3'd6;
  parameter state_7 = 3'd7;

  reg tmp_, o_;

  assign o = o_;

  always @(a or b or tmp_) begin
    case (state)
      state_0:	begin
		  if (a==b) tmp_=0; else tmp_=1;
		  o_<=tmp_^a;
		end		  
      state_1:	begin	tmp_=1'bx;  o_<=0;	end
      state_2:	begin	tmp_=1'bx;  o_<=1;	end
      state_3:	begin	tmp_=1'bx;  o_<=1;	end
      state_4:	begin	tmp_=1'bx;  o_<=1'bx;	end
      state_5:	begin	tmp_=1'bx;  o_<=1'bx;	end
      state_6:	begin	tmp_=1'bx;  o_<=1'bx;	end
      state_7:	begin	tmp_=1'bx;  o_<=1'bx;	end
    endcase
  end

endmodule