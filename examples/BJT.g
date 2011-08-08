# tdfc-gappa backend autocompiled body file
# tdfc version 1.160
# Mon Aug  8 13:33:47 2011


@fx = fixed<-32,ne>;

betaR_m = float<ieee_64,ne>(betaR);
betaF_m = float<ieee_64,ne>(betaF);
Is_m = float<ieee_64,ne>(Is);
Vt_m = float<ieee_64,ne>(Vt);
n_m = float<ieee_64,ne>(n);
Vb_m = float<ieee_64,ne>(Vb);
Vc_m = float<ieee_64,ne>(Vc);
Ve_m = float<ieee_64,ne>(Ve);
multiply_16_m float<ieee_64,ne> = ( n * Vt );
multiply_17_m float<ieee_64,ne> = ( n * Vt );
divide_18_m float<ieee_64,ne> = ( Is / betaR );
multiply_19_m float<ieee_64,ne> = ( n * Vt );
divide_20_m float<ieee_64,ne> = ( Is / betaF );
multiply_21_m float<ieee_64,ne> = ( n * Vt );
divide_22_m float<ieee_64,ne> = ( Is / betaR );
multiply_23_m float<ieee_64,ne> = ( n * Vt );
multiply_24_m float<ieee_64,ne> = ( n * Vt );
multiply_25_m float<ieee_64,ne> = ( n * Vt );
divide_26_m float<ieee_64,ne> = ( Is / betaF );
multiply_27_m float<ieee_64,ne> = ( n * Vt );
divide_31_m float<ieee_64,ne> = ( Vbe_2 / multiply_16 );
divide_32_m float<ieee_64,ne> = ( Vbc_5 / multiply_17 );
divide_33_m float<ieee_64,ne> = ( Vbc_5 / multiply_19 );
divide_34_m float<ieee_64,ne> = ( Vbe_2 / multiply_21 );
divide_35_m float<ieee_64,ne> = ( Vbc_5 / multiply_23 );
divide_36_m float<ieee_64,ne> = ( Vbe_2 / multiply_24 );
divide_37_m float<ieee_64,ne> = ( Vbc_5 / multiply_25 );
divide_38_m float<ieee_64,ne> = ( Vbe_2 / multiply_27 );
exp_39_m float<ieee_64,ne> = exp ( divide_31_m );
exp_40_m float<ieee_64,ne> = exp ( divide_32_m );
exp_41_m float<ieee_64,ne> = exp ( divide_33_m );
exp_42_m float<ieee_64,ne> = exp ( divide_34_m );
exp_43_m float<ieee_64,ne> = exp ( divide_35_m );
exp_44_m float<ieee_64,ne> = exp ( divide_36_m );
exp_45_m float<ieee_64,ne> = exp ( divide_37_m );
exp_46_m float<ieee_64,ne> = exp ( divide_38_m );
minus_47_m float<ieee_64,ne> = ( exp_39 - exp_40 );
minus_48_m float<ieee_64,ne> = ( exp_41 - 1_7_m );
minus_49_m float<ieee_64,ne> = ( exp_42 - 1_9_m );
minus_50_m float<ieee_64,ne> = ( exp_43 - 1_10_m );
minus_51_m float<ieee_64,ne> = ( exp_44 - exp_45 );
minus_52_m float<ieee_64,ne> = ( exp_46 - 1_11_m );
multiply_53_m float<ieee_64,ne> = ( Is * minus_47 );
multiply_54_m float<ieee_64,ne> = ( divide_18 * minus_48 );
multiply_55_m float<ieee_64,ne> = ( divide_20 * minus_49 );
multiply_56_m float<ieee_64,ne> = ( divide_22 * minus_50 );
multiply_57_m float<ieee_64,ne> = ( Is * minus_51 );
multiply_58_m float<ieee_64,ne> = ( divide_26 * minus_52 );
minus_59_m float<ieee_64,ne> = ( multiply_53 - multiply_54 );
plus_60_m float<ieee_64,ne> = ( multiply_55 + multiply_56 );
plus_61_m float<ieee_64,ne> = ( multiply_57 + multiply_58 );
Ic_62_m float<ieee_64,ne> =  ( minus_59_m );
Ib_63_m float<ieee_64,ne> =  ( plus_60_m );
Ie_64_m float<ieee_64,ne> =  ( plus_61_m );
        
betaR_fl = float<ieee_32,ne>(betaR_m);
betaF_fl = float<ieee_32,ne>(betaF_m);
Is_fl = float<ieee_32,ne>(Is_m);
Vt_fl = float<ieee_32,ne>(Vt_m);
n_fl = float<ieee_32,ne>(n_m);
Vb_fl = float<ieee_32,ne>(Vb_m);
Vc_fl = float<ieee_32,ne>(Vc_m);
Ve_fl = float<ieee_32,ne>(Ve_m);
multiply_16_fl float<ieee_32,ne> = ( n * Vt );
multiply_17_fl float<ieee_32,ne> = ( n * Vt );
divide_18_fl float<ieee_32,ne> = ( Is / betaR );
multiply_19_fl float<ieee_32,ne> = ( n * Vt );
divide_20_fl float<ieee_32,ne> = ( Is / betaF );
multiply_21_fl float<ieee_32,ne> = ( n * Vt );
divide_22_fl float<ieee_32,ne> = ( Is / betaR );
multiply_23_fl float<ieee_32,ne> = ( n * Vt );
multiply_24_fl float<ieee_32,ne> = ( n * Vt );
multiply_25_fl float<ieee_32,ne> = ( n * Vt );
divide_26_fl float<ieee_32,ne> = ( Is / betaF );
multiply_27_fl float<ieee_32,ne> = ( n * Vt );
divide_31_fl float<ieee_32,ne> = ( Vbe_2 / multiply_16 );
divide_32_fl float<ieee_32,ne> = ( Vbc_5 / multiply_17 );
divide_33_fl float<ieee_32,ne> = ( Vbc_5 / multiply_19 );
divide_34_fl float<ieee_32,ne> = ( Vbe_2 / multiply_21 );
divide_35_fl float<ieee_32,ne> = ( Vbc_5 / multiply_23 );
divide_36_fl float<ieee_32,ne> = ( Vbe_2 / multiply_24 );
divide_37_fl float<ieee_32,ne> = ( Vbc_5 / multiply_25 );
divide_38_fl float<ieee_32,ne> = ( Vbe_2 / multiply_27 );
exp_39_fl float<ieee_32,ne> = exp ( divide_31_fl );
exp_40_fl float<ieee_32,ne> = exp ( divide_32_fl );
exp_41_fl float<ieee_32,ne> = exp ( divide_33_fl );
exp_42_fl float<ieee_32,ne> = exp ( divide_34_fl );
exp_43_fl float<ieee_32,ne> = exp ( divide_35_fl );
exp_44_fl float<ieee_32,ne> = exp ( divide_36_fl );
exp_45_fl float<ieee_32,ne> = exp ( divide_37_fl );
exp_46_fl float<ieee_32,ne> = exp ( divide_38_fl );
minus_47_fl float<ieee_32,ne> = ( exp_39 - exp_40 );
minus_48_fl float<ieee_32,ne> = ( exp_41 - 1_7_fl );
minus_49_fl float<ieee_32,ne> = ( exp_42 - 1_9_fl );
minus_50_fl float<ieee_32,ne> = ( exp_43 - 1_10_fl );
minus_51_fl float<ieee_32,ne> = ( exp_44 - exp_45 );
minus_52_fl float<ieee_32,ne> = ( exp_46 - 1_11_fl );
multiply_53_fl float<ieee_32,ne> = ( Is * minus_47 );
multiply_54_fl float<ieee_32,ne> = ( divide_18 * minus_48 );
multiply_55_fl float<ieee_32,ne> = ( divide_20 * minus_49 );
multiply_56_fl float<ieee_32,ne> = ( divide_22 * minus_50 );
multiply_57_fl float<ieee_32,ne> = ( Is * minus_51 );
multiply_58_fl float<ieee_32,ne> = ( divide_26 * minus_52 );
minus_59_fl float<ieee_32,ne> = ( multiply_53 - multiply_54 );
plus_60_fl float<ieee_32,ne> = ( multiply_55 + multiply_56 );
plus_61_fl float<ieee_32,ne> = ( multiply_57 + multiply_58 );
Ic_62_fl float<ieee_32,ne> =  ( minus_59_fl );
Ib_63_fl float<ieee_32,ne> =  ( plus_60_fl );
Ie_64_fl float<ieee_32,ne> =  ( plus_61_fl );
        
betaR_dbl = float<ieee_64,ne>(betaR_m);
betaF_dbl = float<ieee_64,ne>(betaF_m);
Is_dbl = float<ieee_64,ne>(Is_m);
Vt_dbl = float<ieee_64,ne>(Vt_m);
n_dbl = float<ieee_64,ne>(n_m);
Vb_dbl = float<ieee_64,ne>(Vb_m);
Vc_dbl = float<ieee_64,ne>(Vc_m);
Ve_dbl = float<ieee_64,ne>(Ve_m);
multiply_16_dbl float<ieee_64,ne> = ( n * Vt );
multiply_17_dbl float<ieee_64,ne> = ( n * Vt );
divide_18_dbl float<ieee_64,ne> = ( Is / betaR );
multiply_19_dbl float<ieee_64,ne> = ( n * Vt );
divide_20_dbl float<ieee_64,ne> = ( Is / betaF );
multiply_21_dbl float<ieee_64,ne> = ( n * Vt );
divide_22_dbl float<ieee_64,ne> = ( Is / betaR );
multiply_23_dbl float<ieee_64,ne> = ( n * Vt );
multiply_24_dbl float<ieee_64,ne> = ( n * Vt );
multiply_25_dbl float<ieee_64,ne> = ( n * Vt );
divide_26_dbl float<ieee_64,ne> = ( Is / betaF );
multiply_27_dbl float<ieee_64,ne> = ( n * Vt );
divide_31_dbl float<ieee_64,ne> = ( Vbe_2 / multiply_16 );
divide_32_dbl float<ieee_64,ne> = ( Vbc_5 / multiply_17 );
divide_33_dbl float<ieee_64,ne> = ( Vbc_5 / multiply_19 );
divide_34_dbl float<ieee_64,ne> = ( Vbe_2 / multiply_21 );
divide_35_dbl float<ieee_64,ne> = ( Vbc_5 / multiply_23 );
divide_36_dbl float<ieee_64,ne> = ( Vbe_2 / multiply_24 );
divide_37_dbl float<ieee_64,ne> = ( Vbc_5 / multiply_25 );
divide_38_dbl float<ieee_64,ne> = ( Vbe_2 / multiply_27 );
exp_39_dbl float<ieee_64,ne> = exp ( divide_31_dbl );
exp_40_dbl float<ieee_64,ne> = exp ( divide_32_dbl );
exp_41_dbl float<ieee_64,ne> = exp ( divide_33_dbl );
exp_42_dbl float<ieee_64,ne> = exp ( divide_34_dbl );
exp_43_dbl float<ieee_64,ne> = exp ( divide_35_dbl );
exp_44_dbl float<ieee_64,ne> = exp ( divide_36_dbl );
exp_45_dbl float<ieee_64,ne> = exp ( divide_37_dbl );
exp_46_dbl float<ieee_64,ne> = exp ( divide_38_dbl );
minus_47_dbl float<ieee_64,ne> = ( exp_39 - exp_40 );
minus_48_dbl float<ieee_64,ne> = ( exp_41 - 1_7_dbl );
minus_49_dbl float<ieee_64,ne> = ( exp_42 - 1_9_dbl );
minus_50_dbl float<ieee_64,ne> = ( exp_43 - 1_10_dbl );
minus_51_dbl float<ieee_64,ne> = ( exp_44 - exp_45 );
minus_52_dbl float<ieee_64,ne> = ( exp_46 - 1_11_dbl );
multiply_53_dbl float<ieee_64,ne> = ( Is * minus_47 );
multiply_54_dbl float<ieee_64,ne> = ( divide_18 * minus_48 );
multiply_55_dbl float<ieee_64,ne> = ( divide_20 * minus_49 );
multiply_56_dbl float<ieee_64,ne> = ( divide_22 * minus_50 );
multiply_57_dbl float<ieee_64,ne> = ( Is * minus_51 );
multiply_58_dbl float<ieee_64,ne> = ( divide_26 * minus_52 );
minus_59_dbl float<ieee_64,ne> = ( multiply_53 - multiply_54 );
plus_60_dbl float<ieee_64,ne> = ( multiply_55 + multiply_56 );
plus_61_dbl float<ieee_64,ne> = ( multiply_57 + multiply_58 );
Ic_62_dbl float<ieee_64,ne> =  ( minus_59_dbl );
Ib_63_dbl float<ieee_64,ne> =  ( plus_60_dbl );
Ie_64_dbl float<ieee_64,ne> =  ( plus_61_dbl );
        
betaR_fx = fx(betaR_m);
betaF_fx = fx(betaF_m);
Is_fx = fx(Is_m);
Vt_fx = fx(Vt_m);
n_fx = fx(n_m);
Vb_fx = fx(Vb_m);
Vc_fx = fx(Vc_m);
Ve_fx = fx(Ve_m);
multiply_16_fx fx = ( n * Vt );
multiply_17_fx fx = ( n * Vt );
divide_18_fx fx = ( Is / betaR );
multiply_19_fx fx = ( n * Vt );
divide_20_fx fx = ( Is / betaF );
multiply_21_fx fx = ( n * Vt );
divide_22_fx fx = ( Is / betaR );
multiply_23_fx fx = ( n * Vt );
multiply_24_fx fx = ( n * Vt );
multiply_25_fx fx = ( n * Vt );
divide_26_fx fx = ( Is / betaF );
multiply_27_fx fx = ( n * Vt );
divide_31_fx fx = ( Vbe_2 / multiply_16 );
divide_32_fx fx = ( Vbc_5 / multiply_17 );
divide_33_fx fx = ( Vbc_5 / multiply_19 );
divide_34_fx fx = ( Vbe_2 / multiply_21 );
divide_35_fx fx = ( Vbc_5 / multiply_23 );
divide_36_fx fx = ( Vbe_2 / multiply_24 );
divide_37_fx fx = ( Vbc_5 / multiply_25 );
divide_38_fx fx = ( Vbe_2 / multiply_27 );
exp_39_fx fx = exp ( divide_31_fx );
exp_40_fx fx = exp ( divide_32_fx );
exp_41_fx fx = exp ( divide_33_fx );
exp_42_fx fx = exp ( divide_34_fx );
exp_43_fx fx = exp ( divide_35_fx );
exp_44_fx fx = exp ( divide_36_fx );
exp_45_fx fx = exp ( divide_37_fx );
exp_46_fx fx = exp ( divide_38_fx );
minus_47_fx fx = ( exp_39 - exp_40 );
minus_48_fx fx = ( exp_41 - 1_7_fx );
minus_49_fx fx = ( exp_42 - 1_9_fx );
minus_50_fx fx = ( exp_43 - 1_10_fx );
minus_51_fx fx = ( exp_44 - exp_45 );
minus_52_fx fx = ( exp_46 - 1_11_fx );
multiply_53_fx fx = ( Is * minus_47 );
multiply_54_fx fx = ( divide_18 * minus_48 );
multiply_55_fx fx = ( divide_20 * minus_49 );
multiply_56_fx fx = ( divide_22 * minus_50 );
multiply_57_fx fx = ( Is * minus_51 );
multiply_58_fx fx = ( divide_26 * minus_52 );
minus_59_fx fx = ( multiply_53 - multiply_54 );
plus_60_fx fx = ( multiply_55 + multiply_56 );
plus_61_fx fx = ( multiply_57 + multiply_58 );
Ic_62_fx fx =  ( minus_59_fx );
Ib_63_fx fx =  ( plus_60_fx );
Ie_64_fx fx =  ( plus_61_fx );
        

{

		Vb in [0,1] /\ 
		Vc in [0,1] /\ 
		Ve in [0,1] /\ 
		(Ie_m >= 0x1p-53 \/ Ie_m <= - 0x1p-53) /\ 
		(Ib_m >= 0x1p-53 \/ Ib_m <= - 0x1p-53) /\ 
		(Ic_m >= 0x1p-53 \/ Ic_m <= - 0x1p-53)

		->
		(Ie_fl-Ie_m)/Ie_m in ? /\ 
		(Ie_dbl-Ie_m)/Ie_m in ? /\ 
		(Ie_fx-Ie_m)/Ie_m in ? /\ 
		(Ie_m-Ie_fx) in ? /\ 
		(Ie_m-Ie_dbl) in ? /\ 
		(Ie_m-Ie_fl) in ? /\ 
		(Ib_fl-Ib_m)/Ib_m in ? /\ 
		(Ib_dbl-Ib_m)/Ib_m in ? /\ 
		(Ib_fx-Ib_m)/Ib_m in ? /\ 
		(Ib_m-Ib_fx) in ? /\ 
		(Ib_m-Ib_dbl) in ? /\ 
		(Ib_m-Ib_fl) in ? /\ 
		(Ic_fl-Ic_m)/Ic_m in ? /\ 
		(Ic_dbl-Ic_m)/Ic_m in ? /\ 
		(Ic_fx-Ic_m)/Ic_m in ? /\ 
		(Ic_m-Ic_fx) in ? /\ 
		(Ic_m-Ic_dbl) in ? /\ 
		(Ic_m-Ic_fl) in ?
}

#BJT
