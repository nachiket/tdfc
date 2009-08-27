// Verilog data-path module for DecSym_d1_ScOrEtMp53
// Emitted by ../../../tdfc version 1.160, Mon Aug 24 17:52:44 2009

module DecSym_d1_ScOrEtMp53_dp (clock, reset, parsToken_d, setIncr_d, getReq_d, decToken_d, ftabind_d, ftabval_d, state, statecase, flag_DecAC1_0, flag_spitZero_3, flag_spitZero_2, flag_spitZero_1, flag_spitZero_0, flag_DecRS__0, flag_DecRS_1, flag_DecRS_0, flag_DecACLen_0, flag_DecAC2_0, flag_DecDC_0, flag_DecDC_1, flag_DecDC_2, flag_DecDCLen_0);

  input  clock;
  input  reset;

  input  [15:0] parsToken_d;
  output [7:0] setIncr_d;
  output [7:0] getReq_d;
  output [15:0] decToken_d;
  output [7:0] ftabind_d;
  input  [7:0] ftabval_d;

  input  [3:0] state;
  input  statecase;

  output flag_DecAC1_0;
  output flag_spitZero_3;
  output flag_spitZero_2;
  output flag_spitZero_1;
  output flag_spitZero_0;
  output flag_DecRS__0;
  output flag_DecRS_1;
  output flag_DecRS_0;
  output flag_DecACLen_0;
  output flag_DecAC2_0;
  output flag_DecDC_0;
  output flag_DecDC_1;
  output flag_DecDC_2;
  output flag_DecDCLen_0;

  parameter state_DecDCp = 4'd0;
  parameter state_DecAC1 = 4'd1;
  parameter state_DecAC2 = 4'd2;
  parameter state_DecACLen = 4'd3;
  parameter state_DecDC = 4'd4;
  parameter state_DecDCLen = 4'd5;
  parameter state_DecRS = 4'd6;
  parameter state_DecRS_ = 4'd7;
  parameter state_spitZero = 4'd8;
  parameter state_done = 4'd9;

  parameter statecase_stall = 1'd0;
  parameter statecase_1 = 1'd1;

  reg [7:0] marker, marker_;
  reg end_1, end_1_;
  reg nolevel, nolevel_;
  reg signed [15:0] lastDC, lastDC_;
  reg ONE, ONE_;
  reg [3:0] dcLen, dcLen_;
  reg [7:0] coeffCnt, coeffCnt_;
  reg [19:0] macInd, macInd_;
  reg [3:0] run, run_;
  reg [7:0] rs, rs_;
  reg [3:0] ssss, ssss_;
  reg [31:0] intermed, intermed_;
  reg [31:0] curraddr, curraddr_;
  reg [4:0] switcher, switcher_;

  reg [14:0] temp_;
  reg [11:0] temp__1;
  reg signed [16:0] currToken_;
  reg [15:0] val_;
  reg [3:0] dcLentmp_;
  reg bo4_;
  reg bterm11_;
  reg bterm10_;
  reg bterm09_;
  reg bterm08_;
  reg bterm07_;
  reg bterm06_;
  reg bterm05_;
  reg bterm04_;
  reg bterm03_;
  reg bterm02_;
  reg bterm01_;
  reg bterm00_;
  reg [15:0] val__2;
  reg [7:0] incr__1;
  reg [19:0] tabInd__1;
  reg bool82_;
  reg bool72_;
  reg bool62_;
  reg bool33_;
  reg bool24_;
  reg boolf0_;
  reg boold1_;
  reg bool52_;
  reg bool15_;
  reg boolc1_;
  reg bool51_;
  reg bool13_;
  reg bool06_;
  reg bool41_;
  reg bool31_;
  reg bool21_;
  reg bool12_;
  reg bool05_;
  reg bool02_;
  reg bool01_;
  reg bool03_;
  reg bool00_;
  reg bool04_;
  reg bool11_;
  reg bool61_;
  reg bool07_;
  reg bool22_;
  reg bool71_;
  reg [19:0] tabInd_;
  reg [7:0] incr_;
  reg [15:0] val__1;
  reg bool14_;
  reg bool32_;
  reg bool81_;
  reg bool91_;
  reg boola1_;
  reg bool08_;
  reg bool23_;
  reg bool42_;
  reg boolb1_;

  reg tmp0_;
  reg tmp9_;
  reg signed [2:0] tmp8_;
  reg signed [16:0] tmp7_;
  reg signed [16:0] tmp2_;
  reg signed [16:0] tmp1_;
  reg signed [2:0] tmp3_;
  reg tmp4_;
  reg tmp5_;
  reg signed [16:0] tmp6_;

  reg [7:0] setIncr_d_;
  reg [7:0] getReq_d_;
  reg signed [15:0] decToken_d_;
  reg [7:0] ftabind_d_;

  reg flag_DecAC1_0_;
  reg flag_spitZero_3_;
  reg flag_spitZero_2_;
  reg flag_spitZero_1_;
  reg flag_spitZero_0_;
  reg flag_DecRS__0_;
  reg flag_DecRS_1_;
  reg flag_DecRS_0_;
  reg flag_DecACLen_0_;
  reg flag_DecAC2_0_;
  reg flag_DecDC_0_;
  reg flag_DecDC_1_;
  reg flag_DecDC_2_;
  reg flag_DecDCLen_0_;

  reg did_goto_;

  assign setIncr_d = setIncr_d_;
  assign getReq_d = getReq_d_;
  assign decToken_d = decToken_d_;
  assign ftabind_d = ftabind_d_;

  assign flag_DecAC1_0 = flag_DecAC1_0_;
  assign flag_spitZero_3 = flag_spitZero_3_;
  assign flag_spitZero_2 = flag_spitZero_2_;
  assign flag_spitZero_1 = flag_spitZero_1_;
  assign flag_spitZero_0 = flag_spitZero_0_;
  assign flag_DecRS__0 = flag_DecRS__0_;
  assign flag_DecRS_1 = flag_DecRS_1_;
  assign flag_DecRS_0 = flag_DecRS_0_;
  assign flag_DecACLen_0 = flag_DecACLen_0_;
  assign flag_DecAC2_0 = flag_DecAC2_0_;
  assign flag_DecDC_0 = flag_DecDC_0_;
  assign flag_DecDC_1 = flag_DecDC_1_;
  assign flag_DecDC_2 = flag_DecDC_2_;
  assign flag_DecDCLen_0 = flag_DecDCLen_0_;

  always @(posedge clock or negedge reset)  begin
    if (!reset)  begin
      marker <= 1'b0;
      end_1 <= 1'b0;
      nolevel <= 1'bx;
      lastDC <= 1'b0;
      ONE <= 1'd1;
      dcLen <= 4'bx;
      coeffCnt <= 8'bx;
      macInd <= 1'b0;
      run <= 4'bx;
      rs <= 8'bx;
      ssss <= 4'bx;
      intermed <= 1'b0;
      curraddr <= 9'd328;
      switcher <= 1'b0;
    end
    else  begin
      marker <= marker_;
      end_1 <= end_1_;
      nolevel <= nolevel_;
      lastDC <= lastDC_;
      ONE <= ONE_;
      dcLen <= dcLen_;
      coeffCnt <= coeffCnt_;
      macInd <= macInd_;
      run <= run_;
      rs <= rs_;
      ssss <= ssss_;
      intermed <= intermed_;
      curraddr <= curraddr_;
      switcher <= switcher_;
    end
  end  // always @(posedge...)

  always @*  begin
    setIncr_d_ = 8'bx;
    getReq_d_ = 8'bx;
    decToken_d_ = 16'bx;
    ftabind_d_ = 8'bx;

    flag_DecAC1_0_ = 1'bx;
    flag_spitZero_3_ = 1'bx;
    flag_spitZero_2_ = 1'bx;
    flag_spitZero_1_ = 1'bx;
    flag_spitZero_0_ = 1'bx;
    flag_DecRS__0_ = 1'bx;
    flag_DecRS_1_ = 1'bx;
    flag_DecRS_0_ = 1'bx;
    flag_DecACLen_0_ = 1'bx;
    flag_DecAC2_0_ = 1'bx;
    flag_DecDC_0_ = 1'bx;
    flag_DecDC_1_ = 1'bx;
    flag_DecDC_2_ = 1'bx;
    flag_DecDCLen_0_ = 1'bx;

    marker_ = marker;
    end_1_ = end_1;
    nolevel_ = nolevel;
    lastDC_ = lastDC;
    ONE_ = ONE;
    dcLen_ = dcLen;
    coeffCnt_ = coeffCnt;
    macInd_ = macInd;
    run_ = run;
    rs_ = rs;
    ssss_ = ssss;
    intermed_ = intermed;
    curraddr_ = curraddr;
    switcher_ = switcher;

    temp_ = 15'bx;
    temp__1 = 12'bx;
    currToken_ = 17'bx;
    val_ = 16'bx;
    dcLentmp_ = 4'bx;
    bo4_ = 1'bx;
    bterm11_ = 1'bx;
    bterm10_ = 1'bx;
    bterm09_ = 1'bx;
    bterm08_ = 1'bx;
    bterm07_ = 1'bx;
    bterm06_ = 1'bx;
    bterm05_ = 1'bx;
    bterm04_ = 1'bx;
    bterm03_ = 1'bx;
    bterm02_ = 1'bx;
    bterm01_ = 1'bx;
    bterm00_ = 1'bx;
    val__2 = 16'bx;
    incr__1 = 1'b0;
    tabInd__1 = 20'bx;
    bool82_ = 1'bx;
    bool72_ = 1'bx;
    bool62_ = 1'bx;
    bool33_ = 1'bx;
    bool24_ = 1'bx;
    boolf0_ = 1'bx;
    boold1_ = 1'bx;
    bool52_ = 1'bx;
    bool15_ = 1'bx;
    boolc1_ = 1'bx;
    bool51_ = 1'bx;
    bool13_ = 1'bx;
    bool06_ = 1'bx;
    bool41_ = 1'bx;
    bool31_ = 1'bx;
    bool21_ = 1'bx;
    bool12_ = 1'bx;
    bool05_ = 1'bx;
    bool02_ = 1'bx;
    bool01_ = 1'bx;
    bool03_ = 1'bx;
    bool00_ = 1'bx;
    bool04_ = 1'bx;
    bool11_ = 1'bx;
    bool61_ = 1'bx;
    bool07_ = 1'bx;
    bool22_ = 1'bx;
    bool71_ = 1'bx;
    tabInd_ = 20'bx;
    incr_ = 1'b0;
    val__1 = 16'bx;
    bool14_ = 1'bx;
    bool32_ = 1'bx;
    bool81_ = 1'bx;
    bool91_ = 1'bx;
    boola1_ = 1'bx;
    bool08_ = 1'bx;
    bool23_ = 1'bx;
    bool42_ = 1'bx;
    boolb1_ = 1'bx;

    did_goto_ = 0;

    case (state)
      state_DecDCp:  begin
        if (statecase == statecase_1)  begin
          begin
            getReq_d_ = 4'd9;
            did_goto_ = 1;
          end
        end
      end
      state_DecAC1:  begin
        if (statecase == statecase_1)  begin
          begin
            switcher_ = 2'd2;
            val_ = parsToken_d;
            bool01_ = (val_[7:6] == 1'b0);
            bool02_ = (val_[7:6] == 1'd1);
            bool03_ = (val_[7:5] == 3'd4);
            bool00_ = (val_[7:4] == 4'd10);
            bool04_ = (val_[7:4] == 4'd11);
            bool11_ = (val_[7:4] == 4'd12);
            bool05_ = (val_[7:3] == 5'd26);
            bool12_ = (val_[7:3] == 5'd27);
            bool21_ = (val_[7:3] == 5'd28);
            bool31_ = (val_[7:2] == 6'd58);
            bool41_ = (val_[7:2] == 6'd59);
            bool06_ = (val_[7:1] == 7'd120);
            bool13_ = (val_[7:1] == 7'd121);
            bool51_ = (val_[7:1] == 7'd122);
            bool61_ = (val_[7:1] == 7'd123);
            bool07_ = (val_[7:0] == 8'd248);
            bool22_ = (val_[7:0] == 8'd249);
            bool71_ = (val_[7:0] == 8'd250);
            flag_DecAC1_0_ = ((val_[7:3] == 5'd31) && (val_[2:0] > 2'd2));
            if (flag_DecAC1_0_)  begin
              begin
                setIncr_d_ = 1'b0;
                getReq_d_ = 5'd16;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                rs_ = 1'b0;
                rs_ = (bool01_ ? 1'd1 : rs_);
                rs_ = (bool02_ ? 2'd2 : rs_);
                rs_ = (bool03_ ? 2'd3 : rs_);
                rs_ = (bool04_ ? 3'd4 : rs_);
                rs_ = (bool11_ ? 5'd17 : rs_);
                rs_ = (bool05_ ? 3'd5 : rs_);
                rs_ = (bool12_ ? 5'd18 : rs_);
                rs_ = (bool21_ ? 6'd33 : rs_);
                rs_ = (bool31_ ? 6'd49 : rs_);
                rs_ = (bool41_ ? 7'd65 : rs_);
                rs_ = (bool06_ ? 3'd6 : rs_);
                rs_ = (bool13_ ? 5'd19 : rs_);
                rs_ = (bool51_ ? 7'd81 : rs_);
                rs_ = (bool61_ ? 7'd97 : rs_);
                rs_ = (bool07_ ? 3'd7 : rs_);
                rs_ = (bool22_ ? 6'd34 : rs_);
                rs_ = (bool71_ ? 7'd113 : rs_);
                incr_[0] = ((((((((bool03_ || bool05_) || bool12_) || bool21_) || bool06_) || bool13_) || bool51_) || bool61_) ? 1'd1 : 1'b0);
                incr_[1] = (((((((((bool01_ || bool02_) || bool03_) || bool31_) || bool41_) || bool06_) || bool13_) || bool51_) || bool61_) ? 1'd1 : 1'b0);
                incr_[2] = ((((((((((((bool00_ || bool04_) || bool11_) || bool05_) || bool12_) || bool21_) || bool31_) || bool41_) || bool06_) || bool13_) || bool51_) || bool61_) ? 1'd1 : 1'b0);
                incr_[3] = (((bool07_ || bool22_) || bool71_) ? 1'd1 : 1'b0);
                setIncr_d_ = incr_;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecAC2:  begin
        if (statecase == statecase_1)  begin
          begin
            switcher_ = 3'd4;
            val__1 = parsToken_d;
            flag_DecAC2_0_ = ((((val__1[10] == 1'd1) || (val__1[9] == 1'b0)) || (val__1[8] == 1'b0)) && ((val__1[10] == 1'b0) || ((val__1[9:7] == 3'd7) && (val__1[6:1] > 1'b0))));
            if (flag_DecAC2_0_)  begin
              begin
                tabInd__1 = ((val__1[10:0] - 17'd130946) + 18'd129024);
                setIncr_d_ = 5'd16;
                ftabind_d_ = tabInd__1;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                bool14_ = (val__1[10:7] == 3'd6);
                bool32_ = (val__1[10:7] == 3'd7);
                bool81_ = (val__1[10:7] == 4'd8);
                bool91_ = (val__1[10:7] == 4'd9);
                boola1_ = (val__1[10:7] == 4'd10);
                bool08_ = (val__1[10:6] == 5'd22);
                bool23_ = (val__1[10:6] == 5'd23);
                bool42_ = (val__1[10:6] == 5'd24);
                boolb1_ = (val__1[10:6] == 5'd25);
                boolc1_ = (val__1[10:6] == 5'd26);
                bool15_ = (val__1[10:5] == 6'd54);
                bool52_ = (val__1[10:5] == 6'd55);
                boold1_ = (val__1[10:5] == 6'd56);
                boolf0_ = (val__1[10:5] == 6'd57);
                bool24_ = (val__1[10:4] == 7'd116);
                bool33_ = (val__1[10:4] == 7'd117);
                bool62_ = (val__1[10:4] == 7'd118);
                bool72_ = (val__1[10:4] == 7'd119);
                bool82_ = (val__1[10:1] == 10'd960);
                rs_[0] = (((((((((bool81_ || bool91_) || boola1_) || bool23_) || boolb1_) || boolc1_) || bool15_) || boold1_) || bool33_) ? 1'd1 : 1'b0);
                rs_[1] = ((((((((bool32_ || bool23_) || bool42_) || bool52_) || bool33_) || bool62_) || bool72_) || bool82_) ? 1'd1 : 1'b0);
                rs_[2] = (((bool14_ || bool15_) || bool24_) ? 1'd1 : 1'b0);
                rs_[3] = (bool08_ ? 1'd1 : 1'b0);
                rs_[4] = ((((((((((bool14_ || bool32_) || bool91_) || boolb1_) || bool15_) || bool52_) || boold1_) || boolf0_) || bool33_) || bool72_) ? 1'd1 : 1'b0);
                rs_[5] = (((((((((bool32_ || boola1_) || bool23_) || boolb1_) || boolf0_) || bool24_) || bool33_) || bool62_) || bool72_) ? 1'd1 : 1'b0);
                rs_[6] = (((((((bool42_ || boolc1_) || bool52_) || boold1_) || boolf0_) || bool62_) || bool72_) ? 1'd1 : 1'b0);
                rs_[7] = ((((((((bool81_ || bool91_) || boola1_) || boolb1_) || boolc1_) || boold1_) || boolf0_) || bool82_) ? 1'd1 : 1'b0);
                incr__1[0] = (((((((((bool08_ || bool23_) || bool42_) || boolb1_) || boolc1_) || bool24_) || bool33_) || bool62_) || bool72_) ? 1'd1 : 1'b0);
                incr__1[1] = (((((((((bool15_ || bool52_) || boold1_) || boolf0_) || bool24_) || bool33_) || bool62_) || bool72_) || bool82_) ? 1'd1 : 1'b0);
                incr__1[2] = (bool82_ ? 1'd1 : 1'b0);
                setIncr_d_ = (incr__1 + 4'd9);
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecACLen:  begin
        if (statecase == statecase_1)  begin
          begin
            switcher_ = 2'd3;
            tmp0_ = (1'd1 << (ssss_ - 1'd1));
            tmp1_ = parsToken_d;
            tmp2_ = parsToken_d;
            tmp4_ = (1'd1 << ssss_);
            tmp3_ = (tmp4_ - 1'd1);
            decToken_d_ = ((parsToken_d >= tmp0_) ? {1'sb0,tmp1_} : ({1'sb0,tmp2_} - tmp3_));
            setIncr_d_ = ssss_;
            flag_DecACLen_0_ = (coeffCnt_ == 6'd63);
            if (flag_DecACLen_0_)  begin
              begin
                macInd_ = (macInd_ + 1'd1);
                getReq_d_ = 4'd9;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                coeffCnt_ = (coeffCnt_ + 1'd1);
                getReq_d_ = 4'd8;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecDC:  begin
        if (statecase == statecase_1)  begin
          begin
            val__2 = parsToken_d;
            switcher_ = 1'b0;
            flag_DecDC_0_ = (macInd_ == 13'd4096);
            if (flag_DecDC_0_)  begin
              begin
                did_goto_ = 1;
              end
            end
            if (!did_goto_) bterm00_ = (val__2[8:7] == 1'b0);
            if (!did_goto_) bterm01_ = (val__2[8:6] == 2'd2);
            if (!did_goto_) bterm02_ = (val__2[8:6] == 2'd3);
            if (!did_goto_) bterm03_ = (val__2[8:6] == 3'd4);
            if (!did_goto_) bterm04_ = (val__2[8:6] == 3'd5);
            if (!did_goto_) bterm05_ = (val__2[8:6] == 3'd6);
            if (!did_goto_) bterm06_ = (val__2[8:5] == 4'd14);
            if (!did_goto_) bterm07_ = (val__2[8:4] == 5'd30);
            if (!did_goto_) bterm08_ = (val__2[3] == 1'b0);
            if (!did_goto_) bterm09_ = (val__2[3:2] == 2'd2);
            if (!did_goto_) bterm10_ = (val__2[3:1] == 3'd6);
            if (!did_goto_) bterm11_ = (val__2[3:0] == 4'd14);
            if (!did_goto_) bo4_ = (val__2[8:4] == 5'd31);
            flag_DecDC_1_ = bterm00_;
            if (flag_DecDC_1_)  begin
              begin
                if (!did_goto_) dcLen_ = 1'b0;
                if (!did_goto_) decToken_d_ = lastDC_;
                if (!did_goto_) coeffCnt_ = 1'd1;
                if (!did_goto_) setIncr_d_ = 2'd2;
                if (!did_goto_) getReq_d_ = 4'd8;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                if (!did_goto_) dcLentmp_ = 1'b0;
                if (!did_goto_) dcLentmp_[0] = (((((bterm01_ || bterm03_) || bterm05_) || bterm07_) || (bo4_ && (bterm09_ || bterm11_))) ? 1'd1 : 1'b0);
                if (!did_goto_) dcLentmp_[1] = (((((bterm02_ || bterm03_) || bterm06_) || bterm07_) || (bo4_ && (bterm10_ || bterm11_))) ? 1'd1 : 1'b0);
                if (!did_goto_) dcLentmp_[2] = ((((bterm04_ || bterm05_) || bterm06_) || bterm07_) ? 1'd1 : 1'b0);
                if (!did_goto_) dcLentmp_[3] = (bo4_ ? 1'd1 : 1'b0);
                if (!did_goto_) dcLen_ = dcLentmp_;
                flag_DecDC_2_ = (dcLentmp_ < 3'd6);
                if (flag_DecDC_2_)  begin
                  begin
                    if (!did_goto_) setIncr_d_ = 2'd3;
                  end
                end
                else  begin
                  begin
                    if (!did_goto_) setIncr_d_ = (dcLentmp_ - 2'd2);
                  end
                end
                if (!did_goto_) getReq_d_ = dcLen_;
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_DecDCLen:  begin
        if (statecase == statecase_1)  begin
          begin
            switcher_ = 1'd1;
            tmp5_ = (1'd1 << (dcLen_ - 1'd1));
            flag_DecDCLen_0_ = (parsToken_d >= tmp5_);
            if (flag_DecDCLen_0_)  begin
              begin
                tmp6_ = parsToken_d;
                currToken_ = {1'sb0,tmp6_};
              end
            end
            else  begin
              begin
                tmp7_ = parsToken_d;
                tmp9_ = (1'd1 << dcLen_);
                tmp8_ = (tmp9_ - 1'd1);
                currToken_ = ({1'sb0,tmp7_} - tmp8_);
              end
            end
            decToken_d_ = (currToken_ + lastDC_);
            lastDC_ = (currToken_ + lastDC_);
            setIncr_d_ = dcLen_;
            coeffCnt_ = 1'd1;
            getReq_d_ = 4'd8;
            did_goto_ = 1;
          end
        end
      end
      state_DecRS:  begin
        if (statecase == statecase_1)  begin
          begin
            flag_DecRS_0_ = ((rs_ == 1'b0) || (rs_ == 8'd240));
            if (flag_DecRS_0_)  begin
              begin
                run_ = ((rs_ == 1'b0) ? (7'd64 - coeffCnt_) : 5'd16);
                nolevel_ = 1'b1;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                nolevel_ = 1'b0;
                run_ = rs_[7:4];
                ssss_ = rs_[3:0];
                flag_DecRS_1_ = (run_ == 1'b0);
                if (flag_DecRS_1_)  begin
                  begin
                    getReq_d_ = ssss_;
                    did_goto_ = 1;
                  end
                end
                else  begin
                  begin
                    did_goto_ = 1;
                  end
                end
              end
            end
          end
        end
      end
      state_DecRS_:  begin
        if (statecase == statecase_1)  begin
          begin
            nolevel_ = 1'b0;
            run_ = ftabval_d[7:4];
            ssss_ = ftabval_d[3:0];
            flag_DecRS__0_ = (run_ == 1'b0);
            if (flag_DecRS__0_)  begin
              begin
                getReq_d_ = ssss_;
                did_goto_ = 1;
              end
            end
            else  begin
              begin
                did_goto_ = 1;
              end
            end
          end
        end
      end
      state_spitZero:  begin
        if (statecase == statecase_1)  begin
          begin
            flag_spitZero_0_ = (run_ == 1'b0);
            if (flag_spitZero_0_)  begin
              begin
                flag_spitZero_1_ = nolevel_;
                if (flag_spitZero_1_)  begin
                  begin
                    flag_spitZero_2_ = (coeffCnt_ == 7'd64);
                    if (flag_spitZero_2_)  begin
                      begin
                        macInd_ = (macInd_ + 1'd1);
                        flag_spitZero_3_ = (macInd_ == 13'd4096);
                        if (flag_spitZero_3_)  begin
                          begin
                            did_goto_ = 1;
                          end
                        end
                        if (!did_goto_) getReq_d_ = 4'd9;
                        did_goto_ = 1;
                      end
                    end
                    else  begin
                      begin
                        getReq_d_ = 4'd8;
                        did_goto_ = 1;
                      end
                    end
                  end
                end
                else  begin
                  begin
                    getReq_d_ = ssss_;
                    did_goto_ = 1;
                  end
                end
              end
            end
            else  begin
              begin
                decToken_d_ = 1'b0;
                coeffCnt_ = (coeffCnt_ + 1'd1);
                run_ = (run_ - 1'd1);
              end
            end
          end
        end
      end
      state_done:  begin
      end
    endcase  // case (state_reg)
  end  // always @*

endmodule  // DecSym_d1_ScOrEtMp53_dp
