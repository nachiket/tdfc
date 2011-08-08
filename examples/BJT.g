# tdfc-gappa backend autocompiled body file
# tdfc version 1.160
# Thu Aug  4 12:33:44 2011


@fx = fixed<-32,ne>;

betaR_m = float<ieee_64,ne>(betaR);
betaF_m = float<ieee_64,ne>(betaF);
Is_m = float<ieee_64,ne>(Is);
Vt_m = float<ieee_64,ne>(Vt);
n_m = float<ieee_64,ne>(n);
Vb_m = float<ieee_64,ne>(Vb);
Vc_m = float<ieee_64,ne>(Vc);
Ve_m = float<ieee_64,ne>(Ve);
Vbe_m = float<ieee_64,ne>((Vb_m-Ve_m))
Vbc_m = float<ieee_64,ne>((Vb_m-Vc_m))
Ic_m = ((Is_m*((exp((Vbe_m/(n_m*Vt_m))))-(exp((Vbc_m/(n_m*Vt_m))))))-((Is_m/betaR_m)*((exp((Vbc_m/(n_m*Vt_m))))-1)))
Ib_m = (((Is_m/betaF_m)*((exp((Vbe_m/(n_m*Vt_m))))-1))+((Is_m/betaR_m)*((exp((Vbc_m/(n_m*Vt_m))))-1)))
Ie_m = ((Is_m*((exp((Vbe_m/(n_m*Vt_m))))-(exp((Vbc_m/(n_m*Vt_m))))))+((Is_m/betaF_m)*((exp((Vbe_m/(n_m*Vt_m))))-1)))
  
betaR_fl = float<ieee_32,ne>(betaR_m);
betaF_fl = float<ieee_32,ne>(betaF_m);
Is_fl = float<ieee_32,ne>(Is_m);
Vt_fl = float<ieee_32,ne>(Vt_m);
n_fl = float<ieee_32,ne>(n_m);
Vb_fl = float<ieee_32,ne>(Vb_m);
Vc_fl = float<ieee_32,ne>(Vc_m);
Ve_fl = float<ieee_32,ne>(Ve_m);
Vbe_fl = float<ieee_32,ne>((Vb_fl-Ve_fl))
Vbc_fl = float<ieee_32,ne>((Vb_fl-Vc_fl))
Ic_fl float<ieee_32,ne>=((Is_fl*((exp((Vbe_fl/(n_fl*Vt_fl))))-(exp((Vbc_fl/(n_fl*Vt_fl))))))-((Is_fl/betaR_fl)*((exp((Vbc_fl/(n_fl*Vt_fl))))-1)))
Ib_fl float<ieee_32,ne>=(((Is_fl/betaF_fl)*((exp((Vbe_fl/(n_fl*Vt_fl))))-1))+((Is_fl/betaR_fl)*((exp((Vbc_fl/(n_fl*Vt_fl))))-1)))
Ie_fl float<ieee_32,ne>=((Is_fl*((exp((Vbe_fl/(n_fl*Vt_fl))))-(exp((Vbc_fl/(n_fl*Vt_fl))))))+((Is_fl/betaF_fl)*((exp((Vbe_fl/(n_fl*Vt_fl))))-1)))
  
betaR_dbl = float<ieee_64,ne>(betaR_m);
betaF_dbl = float<ieee_64,ne>(betaF_m);
Is_dbl = float<ieee_64,ne>(Is_m);
Vt_dbl = float<ieee_64,ne>(Vt_m);
n_dbl = float<ieee_64,ne>(n_m);
Vb_dbl = float<ieee_64,ne>(Vb_m);
Vc_dbl = float<ieee_64,ne>(Vc_m);
Ve_dbl = float<ieee_64,ne>(Ve_m);
Vbe_dbl = float<ieee_64,ne>((Vb_dbl-Ve_dbl))
Vbc_dbl = float<ieee_64,ne>((Vb_dbl-Vc_dbl))
Ic_dbl float<ieee_64,ne>=((Is_dbl*((exp((Vbe_dbl/(n_dbl*Vt_dbl))))-(exp((Vbc_dbl/(n_dbl*Vt_dbl))))))-((Is_dbl/betaR_dbl)*((exp((Vbc_dbl/(n_dbl*Vt_dbl))))-1)))
Ib_dbl float<ieee_64,ne>=(((Is_dbl/betaF_dbl)*((exp((Vbe_dbl/(n_dbl*Vt_dbl))))-1))+((Is_dbl/betaR_dbl)*((exp((Vbc_dbl/(n_dbl*Vt_dbl))))-1)))
Ie_dbl float<ieee_64,ne>=((Is_dbl*((exp((Vbe_dbl/(n_dbl*Vt_dbl))))-(exp((Vbc_dbl/(n_dbl*Vt_dbl))))))+((Is_dbl/betaF_dbl)*((exp((Vbe_dbl/(n_dbl*Vt_dbl))))-1)))
  
betaR_fx = fx(betaR_m);
betaF_fx = fx(betaF_m);
Is_fx = fx(Is_m);
Vt_fx = fx(Vt_m);
n_fx = fx(n_m);
Vb_fx = fx(Vb_m);
Vc_fx = fx(Vc_m);
Ve_fx = fx(Ve_m);
Vbe_fx = fx((Vb_fx-Ve_fx))
Vbc_fx = fx((Vb_fx-Vc_fx))
Ic_fx fx=((Is_fx*((exp((Vbe_fx/(n_fx*Vt_fx))))-(exp((Vbc_fx/(n_fx*Vt_fx))))))-((Is_fx/betaR_fx)*((exp((Vbc_fx/(n_fx*Vt_fx))))-1)))
Ib_fx fx=(((Is_fx/betaF_fx)*((exp((Vbe_fx/(n_fx*Vt_fx))))-1))+((Is_fx/betaR_fx)*((exp((Vbc_fx/(n_fx*Vt_fx))))-1)))
Ie_fx fx=((Is_fx*((exp((Vbe_fx/(n_fx*Vt_fx))))-(exp((Vbc_fx/(n_fx*Vt_fx))))))+((Is_fx/betaF_fx)*((exp((Vbe_fx/(n_fx*Vt_fx))))-1)))
  

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
		(Ib_fl-Ib_m)/Ib_m in ? /\ 
		(Ib_dbl-Ib_m)/Ib_m in ? /\ 
		(Ib_fx-Ib_m)/Ib_m in ? /\ 
		(Ic_fl-Ic_m)/Ic_m in ? /\ 
		(Ic_dbl-Ic_m)/Ic_m in ? /\ 
		(Ic_fx-Ic_m)/Ic_m in ?
}

#BJT
