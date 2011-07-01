# tdfc-gappa backend autocompiled body file
# tdfc version 1.160
# Fri Jun 17 20:49:32 2011


@fx = fixed<-32,ne>;

betaR_m = float<ieee_64,ne>(betaR);
betaF_m = float<ieee_64,ne>(betaF);
Is_m = float<ieee_64,ne>(Is);
Vt_m = float<ieee_64,ne>(Vt);
Vbe_m = float<ieee_64,ne>(Vbe);
Vbc_m = float<ieee_64,ne>(Vbc);
 Ic_m = ( ((Is_m*((exp((Vbe_m/Vt_m)))-(exp((Vbc_m/Vt_m)))))-((Is_m/betaR_m)*((exp((Vbc_m/Vt_m)))-1))));
 Ib_m = ( (((Is_m/betaF_m)*((exp((Vbe_m/Vt_m)))-1))+((Is_m/betaR_m)*((exp((Vbc_m/Vt_m)))-1))));
 Ie_m = ( ((Is_m*((exp((Vbe_m/Vt_m)))-(exp((Vbc_m/Vt_m)))))+((Is_m/betaF_m)*((exp((Vbe_m/Vt_m)))-1))));
  
betaR_fl = float<ieee_32,ne>(betaR_m);
betaF_fl = float<ieee_32,ne>(betaF_m);
Is_fl = float<ieee_32,ne>(Is_m);
Vt_fl = float<ieee_32,ne>(Vt_m);
Vbe_fl = float<ieee_32,ne>(Vbe_m);
Vbc_fl = float<ieee_32,ne>(Vbc_m);
 Ic_fl float<ieee_32,ne> = ( ((Is_fl*((exp((Vbe_fl/Vt_fl)))-(exp((Vbc_fl/Vt_fl)))))-((Is_fl/betaR_fl)*((exp((Vbc_fl/Vt_fl)))-1))));
 Ib_fl float<ieee_32,ne> = ( (((Is_fl/betaF_fl)*((exp((Vbe_fl/Vt_fl)))-1))+((Is_fl/betaR_fl)*((exp((Vbc_fl/Vt_fl)))-1))));
 Ie_fl float<ieee_32,ne> = ( ((Is_fl*((exp((Vbe_fl/Vt_fl)))-(exp((Vbc_fl/Vt_fl)))))+((Is_fl/betaF_fl)*((exp((Vbe_fl/Vt_fl)))-1))));
  
betaR_dbl = float<ieee_64,ne>(betaR_m);
betaF_dbl = float<ieee_64,ne>(betaF_m);
Is_dbl = float<ieee_64,ne>(Is_m);
Vt_dbl = float<ieee_64,ne>(Vt_m);
Vbe_dbl = float<ieee_64,ne>(Vbe_m);
Vbc_dbl = float<ieee_64,ne>(Vbc_m);
 Ic_dbl float<ieee_64,ne> = ( ((Is_dbl*((exp((Vbe_dbl/Vt_dbl)))-(exp((Vbc_dbl/Vt_dbl)))))-((Is_dbl/betaR_dbl)*((exp((Vbc_dbl/Vt_dbl)))-1))));
 Ib_dbl float<ieee_64,ne> = ( (((Is_dbl/betaF_dbl)*((exp((Vbe_dbl/Vt_dbl)))-1))+((Is_dbl/betaR_dbl)*((exp((Vbc_dbl/Vt_dbl)))-1))));
 Ie_dbl float<ieee_64,ne> = ( ((Is_dbl*((exp((Vbe_dbl/Vt_dbl)))-(exp((Vbc_dbl/Vt_dbl)))))+((Is_dbl/betaF_dbl)*((exp((Vbe_dbl/Vt_dbl)))-1))));
  
betaR_fx = fx(betaR_m);
betaF_fx = fx(betaF_m);
Is_fx = fx(Is_m);
Vt_fx = fx(Vt_m);
Vbe_fx = fx(Vbe_m);
Vbc_fx = fx(Vbc_m);
 Ic_fx fx = ( ((Is_fx*((exp((Vbe_fx/Vt_fx)))-(exp((Vbc_fx/Vt_fx)))))-((Is_fx/betaR_fx)*((exp((Vbc_fx/Vt_fx)))-1))));
 Ib_fx fx = ( (((Is_fx/betaF_fx)*((exp((Vbe_fx/Vt_fx)))-1))+((Is_fx/betaR_fx)*((exp((Vbc_fx/Vt_fx)))-1))));
 Ie_fx fx = ( ((Is_fx*((exp((Vbe_fx/Vt_fx)))-(exp((Vbc_fx/Vt_fx)))))+((Is_fx/betaF_fx)*((exp((Vbe_fx/Vt_fx)))-1))));
  

{

		Vbe in [0,1] /\ 
		Vbc in [0,1] /\ 
		(Ie_m >= 0x1p-53 \/ Ie_m <= - 0x1p-53) /\ 
		(Ib_m >= 0x1p-53 \/ Ib_m <= - 0x1p-53) /\ 
		(Ic_m >= 0x1p-53 \/ Ic_m <= - 0x1p-53)

		->
		(Ie_m-Ie_fl)/Ie_m in ? /\ 
		(Ie_m-Ie_dbl)/Ie_m in ? /\ 
		(Ie_m-Ie_fx)/Ie_m in ? /\ 
		(Ib_m-Ib_fl)/Ib_m in ? /\ 
		(Ib_m-Ib_dbl)/Ib_m in ? /\ 
		(Ib_m-Ib_fx)/Ib_m in ? /\ 
		(Ic_m-Ic_fl)/Ic_m in ? /\ 
		(Ic_m-Ic_dbl)/Ic_m in ? /\ 
		(Ic_m-Ic_fx)/Ic_m in ?
}

 #BJT
