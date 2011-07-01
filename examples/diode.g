# tdfc-gappa backend autocompiled body file
# tdfc version 1.160
# Wed Jun 29 14:57:55 2011


@fx = fixed<-32,ne>;

vj_m = float<ieee_64,ne>(0.0258642);
isat_m = float<ieee_64,ne>(0.001);
v_m = float<ieee_64,ne>(v);
gm_m = ((isat_m*(exp((v_m/vj_m))))/vj_m);
i_m = (isat_m*((exp((v_m/vj_m)))-1));
  
vj_fl = float<ieee_32,ne>(vj_m);
isat_fl = float<ieee_32,ne>(isat_m);
v_fl = float<ieee_32,ne>(v_m);
gm_fl float<ieee_32,ne> = ((isat_fl*(exp((v_fl/vj_fl))))/vj_fl);
i_fl float<ieee_32,ne> = (isat_fl*((exp((v_fl/vj_fl)))-1));
  
vj_dbl = float<ieee_64,ne>(vj_m);
isat_dbl = float<ieee_64,ne>(isat_m);
v_dbl = float<ieee_64,ne>(v_m);
gm_dbl float<ieee_64,ne> = ((isat_dbl*(exp((v_dbl/vj_dbl))))/vj_dbl);
i_dbl float<ieee_64,ne> = (isat_dbl*((exp((v_dbl/vj_dbl)))-1));
  
vj_fx = fx(vj_m);
isat_fx = fx(isat_m);
v_fx = fx(v_m);
gm_fx fx = ((isat_fx*(exp((v_fx/vj_fx))))/vj_fx);
i_fx fx = (isat_fx*((exp((v_fx/vj_fx)))-1));
  

{

		v in [0,0.1] /\ 
		(i_m >= 0x1p-53 \/ i_m <= - 0x1p-53) /\ 
		(gm_m >= 0x1p-53 \/ gm_m <= - 0x1p-53)

		->
		(i_fl-i_m)/i_m in ? /\ 
		(i_dbl-i_m)/i_m in ? /\ 
		(i_fx-i_m)/i_m in ? /\ 
		(gm_fl-gm_m)/gm_m in ? /\ 
		(gm_dbl-gm_m)/gm_m in ? /\ 
		(gm_fx-gm_m)/gm_m in ?
}

#diode
