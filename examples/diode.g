# tdfc-gappa backend autocompiled body file
# tdfc version 1.160
# Sun Aug  7 20:51:38 2011


@fx = fixed<-32,ne>;

vj_m = float<ieee_64,ne>(0.0258642);
isat_m = float<ieee_64,ne>(0.001);
v_m = float<ieee_64,ne>(v);
divide_8_m float<ieee_64,ne> = ( v_m / vj_m );
divide_9_m float<ieee_64,ne> = ( v_m / vj_m );
exp_12_m float<ieee_64,ne> = exp ( divide_8_m );
exp_13_m float<ieee_64,ne> = exp ( divide_9_m );
multiply_14_m float<ieee_64,ne> = ( isat_m * exp_12_m );
minus_15_m float<ieee_64,ne> = ( exp_13_m - 1_4_m );
divide_16_m float<ieee_64,ne> = ( multiply_14_m / vj_m );
multiply_17_m float<ieee_64,ne> = ( isat_m * minus_15_m );
gm_18_m float<ieee_64,ne> =  ( divide_16_m );
i_19_m float<ieee_64,ne> =  ( multiply_17_m );
        
vj_fl = float<ieee_32,ne>(vj_m);
isat_fl = float<ieee_32,ne>(isat_m);
v_fl = float<ieee_32,ne>(v_m);
divide_8_fl float<ieee_32,ne> = ( v_fl / vj_fl );
divide_9_fl float<ieee_32,ne> = ( v_fl / vj_fl );
exp_12_fl float<ieee_32,ne> = exp ( divide_8_fl );
exp_13_fl float<ieee_32,ne> = exp ( divide_9_fl );
multiply_14_fl float<ieee_32,ne> = ( isat_fl * exp_12_fl );
minus_15_fl float<ieee_32,ne> = ( exp_13_fl - 1_4_fl );
divide_16_fl float<ieee_32,ne> = ( multiply_14_fl / vj_fl );
multiply_17_fl float<ieee_32,ne> = ( isat_fl * minus_15_fl );
gm_18_fl float<ieee_32,ne> =  ( divide_16_fl );
i_19_fl float<ieee_32,ne> =  ( multiply_17_fl );
        
vj_dbl = float<ieee_64,ne>(vj_m);
isat_dbl = float<ieee_64,ne>(isat_m);
v_dbl = float<ieee_64,ne>(v_m);
divide_8_dbl float<ieee_64,ne> = ( v_dbl / vj_dbl );
divide_9_dbl float<ieee_64,ne> = ( v_dbl / vj_dbl );
exp_12_dbl float<ieee_64,ne> = exp ( divide_8_dbl );
exp_13_dbl float<ieee_64,ne> = exp ( divide_9_dbl );
multiply_14_dbl float<ieee_64,ne> = ( isat_dbl * exp_12_dbl );
minus_15_dbl float<ieee_64,ne> = ( exp_13_dbl - 1_4_dbl );
divide_16_dbl float<ieee_64,ne> = ( multiply_14_dbl / vj_dbl );
multiply_17_dbl float<ieee_64,ne> = ( isat_dbl * minus_15_dbl );
gm_18_dbl float<ieee_64,ne> =  ( divide_16_dbl );
i_19_dbl float<ieee_64,ne> =  ( multiply_17_dbl );
        
vj_fx = fx(vj_m);
isat_fx = fx(isat_m);
v_fx = fx(v_m);
divide_8_fx fx = ( v_fx / vj_fx );
divide_9_fx fx = ( v_fx / vj_fx );
exp_12_fx fx = exp ( divide_8_fx );
exp_13_fx fx = exp ( divide_9_fx );
multiply_14_fx fx = ( isat_fx * exp_12_fx );
minus_15_fx fx = ( exp_13_fx - 1_4_fx );
divide_16_fx fx = ( multiply_14_fx / vj_fx );
multiply_17_fx fx = ( isat_fx * minus_15_fx );
gm_18_fx fx =  ( divide_16_fx );
i_19_fx fx =  ( multiply_17_fx );
        

{

		v in [1e-05,1.5] /\ 
		(i_m >= 0x1p-53 \/ i_m <= - 0x1p-53) /\ 
		(gm_m >= 0x1p-53 \/ gm_m <= - 0x1p-53)

		->
		(i_fl-i_m)/i_m in ? /\ 
		(i_dbl-i_m)/i_m in ? /\ 
		(i_fx-i_m)/i_m in ? /\ 
		(i_m-i_fx) in ? /\ 
		(i_m-i_dbl) in ? /\ 
		(i_m-i_fl) in ? /\ 
		(gm_fl-gm_m)/gm_m in ? /\ 
		(gm_dbl-gm_m)/gm_m in ? /\ 
		(gm_fx-gm_m)/gm_m in ? /\ 
		(gm_m-gm_fx) in ? /\ 
		(gm_m-gm_dbl) in ? /\ 
		(gm_m-gm_fl) in ?
}

#diode
