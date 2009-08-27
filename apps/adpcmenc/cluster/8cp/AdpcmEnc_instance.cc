// cctdfc autocompiled instance file
// tdfc version 1.113
// Tue Apr 11 19:06:21 2000

#include "Score.h"
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "AdpcmEnc.h"
enum copy_unsigned_3_d1_ScOrEtMp32_state_syms {copy_unsigned_3_d1_ScOrEtMp32_state_only,copy_unsigned_3_d1_ScOrEtMp32_state_this_page_is_done};
extern int copy_unsigned_3_d1_ScOrEtMp32_consumes[];
extern int copy_unsigned_3_d1_ScOrEtMp32_produces[];
class Page_copy_unsigned_3_d1_ScOrEtMp32: public ScorePage {
public:
  Page_copy_unsigned_3_d1_ScOrEtMp32(UNSIGNED_SCORE_STREAM n_cc_in,UNSIGNED_SCORE_STREAM n_cc_out1,UNSIGNED_SCORE_STREAM n_cc_out2,UNSIGNED_SCORE_STREAM n_cc_out3) {
    retime_length_0=0;
    cc_in_retime=new unsigned long long [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_in_retime[j]=0;
    declareIO(1,3);
    bindInput(0,n_cc_in,new ScoreStreamType(0,16));
    bindOutput(0,n_cc_out1,new ScoreStreamType(0,16));
    bindOutput(1,n_cc_out2,new ScoreStreamType(0,16));
    bindOutput(2,n_cc_out3,new ScoreStreamType(0,16));
    state=copy_unsigned_3_d1_ScOrEtMp32_state_only;
    states=2;
    produces=copy_unsigned_3_d1_ScOrEtMp32_produces;
    consumes=copy_unsigned_3_d1_ScOrEtMp32_consumes;
    source="copy_unsigned_3 in AdpcmEnc.tdf";
    // cc_w = 16
    loc=NO_LOCATION;
    input_rates=new int[1];
    output_rates=new int[3];
    input_rates[0]=-1;
    output_rates[0]=-1;
    output_rates[1]=-1;
    output_rates[2]=-1;
    input_free=new int[1];
    for (int i=0;i<1;i++)
      input_free[i]=0;
    output_close=new int[3];
    for (int i=0;i<3;i++)
      output_close[i]=0;
  } // constructor 
  int pagestep() { 
    unsigned long long cc_in;
    int done=0;
    int canfire=1;
    switch(state) {
      case copy_unsigned_3_d1_ScOrEtMp32_state_only: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        if (1&& data_0&&!eos_0&&!STREAM_FULL(out[0])&&!STREAM_FULL(out[1])&&!STREAM_FULL(out[2])) {
          cc_in=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_in_retime[j]=cc_in_retime[j-1];
          cc_in_retime[0]=cc_in;
          {
            STREAM_WRITE_NOACC(out[0],cc_in_retime[0]);
            STREAM_WRITE_NOACC(out[1],cc_in_retime[0]);
            STREAM_WRITE_NOACC(out[2],cc_in_retime[0]);
          }
        }
        else
         if (0||(data_0&&eos_0)) done=1; else canfire=0;
        }
        break; 
      } // end case copy_unsigned_3_d1_ScOrEtMp32_state_only
      case copy_unsigned_3_d1_ScOrEtMp32_state_this_page_is_done: {
        return(0);
      } // end case copy_unsigned_3_d1_ScOrEtMp32_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in copy_unsigned_3_d1_ScOrEtMp32::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      STREAM_FREE(in[0]);
      STREAM_CLOSE(out[0]);
      STREAM_CLOSE(out[1]);
      STREAM_CLOSE(out[2]);
      state=copy_unsigned_3_d1_ScOrEtMp32_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  unsigned long long *cc_in_retime;
  int *input_free;
  int *output_close;
};
int copy_unsigned_3_d1_ScOrEtMp32_consumes[]={1,0};
int copy_unsigned_3_d1_ScOrEtMp32_produces[]={7,0};
enum DiffSign_d1_ScOrEtMp33_state_syms {DiffSign_d1_ScOrEtMp33_state_delay1,DiffSign_d1_ScOrEtMp33_state_delay2,DiffSign_d1_ScOrEtMp33_state_delay3,DiffSign_d1_ScOrEtMp33_state_delay4,DiffSign_d1_ScOrEtMp33_state_delay5,DiffSign_d1_ScOrEtMp33_state_fire,DiffSign_d1_ScOrEtMp33_state_this_page_is_done};
extern int DiffSign_d1_ScOrEtMp33_consumes[];
extern int DiffSign_d1_ScOrEtMp33_produces[];
class Page_DiffSign_d1_ScOrEtMp33: public ScorePage {
public:
  Page_DiffSign_d1_ScOrEtMp33(SIGNED_SCORE_STREAM n_cc_inVal,SIGNED_SCORE_STREAM n_cc_inValPred,UNSIGNED_SCORE_STREAM n_cc_outDiff,BOOLEAN_SCORE_STREAM n_cc_outSign_ValPred,BOOLEAN_SCORE_STREAM n_cc_outSign_MuxNibbles) {
    retime_length_0=0;
    cc_inVal_retime=new long [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_inVal_retime[j]=0;
    retime_length_1=0;
    cc_inValPred_retime=new long [retime_length_1+1];
    for (int j=retime_length_1;j>=0;j--)
      cc_inValPred_retime[j]=0;
    declareIO(2,3);
    bindInput(0,n_cc_inVal,new ScoreStreamType(1,16));
    bindInput(1,n_cc_inValPred,new ScoreStreamType(1,16));
    bindOutput(0,n_cc_outDiff,new ScoreStreamType(0,16));
    bindOutput(1,n_cc_outSign_ValPred,new ScoreStreamType(0,1));
    bindOutput(2,n_cc_outSign_MuxNibbles,new ScoreStreamType(0,1));
    state=DiffSign_d1_ScOrEtMp33_state_fire;
    states=7;
    produces=DiffSign_d1_ScOrEtMp33_produces;
    consumes=DiffSign_d1_ScOrEtMp33_consumes;
    source="DiffSign in AdpcmEnc.tdf";
    loc=NO_LOCATION;
    input_rates=new int[2];
    output_rates=new int[3];
    input_rates[0]=-1;
    input_rates[1]=-1;
    output_rates[0]=-1;
    output_rates[1]=-1;
    output_rates[2]=-1;
    input_free=new int[2];
    for (int i=0;i<2;i++)
      input_free[i]=0;
    output_close=new int[3];
    for (int i=0;i<3;i++)
      output_close[i]=0;
  } // constructor 
  int pagestep() { 
    long cc_inVal;
    long cc_inValPred;
    int done=0;
    int canfire=1;
    switch(state) {
      case DiffSign_d1_ScOrEtMp33_state_delay1: { 
        {
        if (1) {
          state=DiffSign_d1_ScOrEtMp33_state_delay2;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DiffSign_d1_ScOrEtMp33_state_delay1
      case DiffSign_d1_ScOrEtMp33_state_delay2: { 
        {
        if (1) {
          state=DiffSign_d1_ScOrEtMp33_state_delay3;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DiffSign_d1_ScOrEtMp33_state_delay2
      case DiffSign_d1_ScOrEtMp33_state_delay3: { 
        {
        if (1) {
          state=DiffSign_d1_ScOrEtMp33_state_delay4;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DiffSign_d1_ScOrEtMp33_state_delay3
      case DiffSign_d1_ScOrEtMp33_state_delay4: { 
        {
        if (1) {
          state=DiffSign_d1_ScOrEtMp33_state_delay5;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DiffSign_d1_ScOrEtMp33_state_delay4
      case DiffSign_d1_ScOrEtMp33_state_delay5: { 
        {
        if (1&&!STREAM_FULL(out[0])&&!STREAM_FULL(out[1])&&!STREAM_FULL(out[2])) {
          {
            long cc_diff=(cc_inVal_retime[0]-cc_inValPred_retime[0]);
            if ((cc_diff>0)) {
              {
                {
                  STREAM_WRITE_NOACC(out[0],cc_diff);
                }
                STREAM_WRITE_NOACC(out[1],false);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            else {
              {
                {
                  STREAM_WRITE_NOACC(out[0],(-(cc_diff)));
                }
                STREAM_WRITE_NOACC(out[1],true);
                STREAM_WRITE_NOACC(out[2],true);
              }
            }
            state=DiffSign_d1_ScOrEtMp33_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DiffSign_d1_ScOrEtMp33_state_delay5
      case DiffSign_d1_ScOrEtMp33_state_fire: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        if (1&& data_0&&!eos_0&& data_1&&!eos_1) {
          cc_inVal=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inVal_retime[j]=cc_inVal_retime[j-1];
          cc_inVal_retime[0]=cc_inVal;
          cc_inValPred=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inValPred_retime[j]=cc_inValPred_retime[j-1];
          cc_inValPred_retime[0]=cc_inValPred;
          state=DiffSign_d1_ScOrEtMp33_state_delay1;
        }
        else
         if (0||(data_0&&eos_0)||(data_1&&eos_1)) done=1; else canfire=0;
        }
        break; 
      } // end case DiffSign_d1_ScOrEtMp33_state_fire
      case DiffSign_d1_ScOrEtMp33_state_this_page_is_done: {
        return(0);
      } // end case DiffSign_d1_ScOrEtMp33_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in DiffSign_d1_ScOrEtMp33::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      STREAM_FREE(in[0]);
      STREAM_FREE(in[1]);
      STREAM_CLOSE(out[0]);
      STREAM_CLOSE(out[1]);
      STREAM_CLOSE(out[2]);
      state=DiffSign_d1_ScOrEtMp33_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  long *cc_inVal_retime;
  int retime_length_1;
  long *cc_inValPred_retime;
  int *input_free;
  int *output_close;
};
int DiffSign_d1_ScOrEtMp33_consumes[]={0,0,0,0,0,3,0};
int DiffSign_d1_ScOrEtMp33_produces[]={0,0,0,0,7,0,0};
enum DivMul2_d1_ScOrEtMp34_state_syms {DivMul2_d1_ScOrEtMp34_state_delay1,DivMul2_d1_ScOrEtMp34_state_delay2,DivMul2_d1_ScOrEtMp34_state_delay3,DivMul2_d1_ScOrEtMp34_state_delay4,DivMul2_d1_ScOrEtMp34_state_delay5,DivMul2_d1_ScOrEtMp34_state_fire,DivMul2_d1_ScOrEtMp34_state_this_page_is_done};
extern int DivMul2_d1_ScOrEtMp34_consumes[];
extern int DivMul2_d1_ScOrEtMp34_produces[];
class Page_DivMul2_d1_ScOrEtMp34: public ScorePage {
public:
  Page_DivMul2_d1_ScOrEtMp34(UNSIGNED_SCORE_STREAM n_cc_inDiff,UNSIGNED_SCORE_STREAM n_cc_inStep,BOOLEAN_SCORE_STREAM n_cc_outDelta2_MuxNibbles,BOOLEAN_SCORE_STREAM n_cc_outDelta2_Index,UNSIGNED_SCORE_STREAM n_cc_outDiffReduced2,UNSIGNED_SCORE_STREAM n_cc_outStepZeroed2) {
    retime_length_0=0;
    cc_inDiff_retime=new unsigned long [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_inDiff_retime[j]=0;
    retime_length_1=0;
    cc_inStep_retime=new unsigned long [retime_length_1+1];
    for (int j=retime_length_1;j>=0;j--)
      cc_inStep_retime[j]=0;
    declareIO(2,4);
    bindInput(0,n_cc_inDiff,new ScoreStreamType(0,16));
    bindInput(1,n_cc_inStep,new ScoreStreamType(0,8));
    bindOutput(0,n_cc_outDelta2_MuxNibbles,new ScoreStreamType(0,1));
    bindOutput(1,n_cc_outDelta2_Index,new ScoreStreamType(0,1));
    bindOutput(2,n_cc_outDiffReduced2,new ScoreStreamType(0,16));
    bindOutput(3,n_cc_outStepZeroed2,new ScoreStreamType(0,16));
    state=DivMul2_d1_ScOrEtMp34_state_fire;
    states=7;
    produces=DivMul2_d1_ScOrEtMp34_produces;
    consumes=DivMul2_d1_ScOrEtMp34_consumes;
    source="DivMul2 in AdpcmEnc.tdf";
    loc=NO_LOCATION;
    input_rates=new int[2];
    output_rates=new int[4];
    input_rates[0]=-1;
    input_rates[1]=-1;
    output_rates[0]=-1;
    output_rates[1]=-1;
    output_rates[2]=-1;
    output_rates[3]=-1;
    input_free=new int[2];
    for (int i=0;i<2;i++)
      input_free[i]=0;
    output_close=new int[4];
    for (int i=0;i<4;i++)
      output_close[i]=0;
  } // constructor 
  int pagestep() { 
    unsigned long cc_inDiff;
    unsigned long cc_inStep;
    int done=0;
    int canfire=1;
    switch(state) {
      case DivMul2_d1_ScOrEtMp34_state_delay1: { 
        {
        if (1) {
          state=DivMul2_d1_ScOrEtMp34_state_delay2;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul2_d1_ScOrEtMp34_state_delay1
      case DivMul2_d1_ScOrEtMp34_state_delay2: { 
        {
        if (1) {
          state=DivMul2_d1_ScOrEtMp34_state_delay3;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul2_d1_ScOrEtMp34_state_delay2
      case DivMul2_d1_ScOrEtMp34_state_delay3: { 
        {
        if (1) {
          state=DivMul2_d1_ScOrEtMp34_state_delay4;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul2_d1_ScOrEtMp34_state_delay3
      case DivMul2_d1_ScOrEtMp34_state_delay4: { 
        {
        if (1) {
          state=DivMul2_d1_ScOrEtMp34_state_delay5;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul2_d1_ScOrEtMp34_state_delay4
      case DivMul2_d1_ScOrEtMp34_state_delay5: { 
        {
        if (1&&!STREAM_FULL(out[0])&&!STREAM_FULL(out[1])&&!STREAM_FULL(out[2])&&!STREAM_FULL(out[3])) {
          {
            unsigned long cc_diffReduced2=(cc_inDiff_retime[0]-cc_inStep_retime[0]);
            if ((cc_diffReduced2>=0)) {
              {
                STREAM_WRITE_NOACC(out[0],true);
                STREAM_WRITE_NOACC(out[1],true);
                {
                  STREAM_WRITE_NOACC(out[2],cc_diffReduced2);
                }
                STREAM_WRITE_NOACC(out[3],cc_inStep_retime[0]);
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[0],false);
                STREAM_WRITE_NOACC(out[1],false);
                STREAM_WRITE_NOACC(out[2],cc_inDiff_retime[0]);
                STREAM_WRITE_NOACC(out[3],0);
              }
            }
            state=DivMul2_d1_ScOrEtMp34_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul2_d1_ScOrEtMp34_state_delay5
      case DivMul2_d1_ScOrEtMp34_state_fire: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        if (1&& data_0&&!eos_0&& data_1&&!eos_1) {
          cc_inDiff=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inDiff_retime[j]=cc_inDiff_retime[j-1];
          cc_inDiff_retime[0]=cc_inDiff;
          cc_inStep=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inStep_retime[j]=cc_inStep_retime[j-1];
          cc_inStep_retime[0]=cc_inStep;
          state=DivMul2_d1_ScOrEtMp34_state_delay1;
        }
        else
         if (0||(data_0&&eos_0)||(data_1&&eos_1)) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul2_d1_ScOrEtMp34_state_fire
      case DivMul2_d1_ScOrEtMp34_state_this_page_is_done: {
        return(0);
      } // end case DivMul2_d1_ScOrEtMp34_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in DivMul2_d1_ScOrEtMp34::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      STREAM_FREE(in[0]);
      STREAM_FREE(in[1]);
      STREAM_CLOSE(out[0]);
      STREAM_CLOSE(out[1]);
      STREAM_CLOSE(out[2]);
      STREAM_CLOSE(out[3]);
      state=DivMul2_d1_ScOrEtMp34_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  unsigned long *cc_inDiff_retime;
  int retime_length_1;
  unsigned long *cc_inStep_retime;
  int *input_free;
  int *output_close;
};
int DivMul2_d1_ScOrEtMp34_consumes[]={0,0,0,0,0,3,0};
int DivMul2_d1_ScOrEtMp34_produces[]={0,0,0,0,15,0,0};
enum DivMul1_d1_ScOrEtMp35_state_syms {DivMul1_d1_ScOrEtMp35_state_delay1,DivMul1_d1_ScOrEtMp35_state_delay2,DivMul1_d1_ScOrEtMp35_state_delay3,DivMul1_d1_ScOrEtMp35_state_delay4,DivMul1_d1_ScOrEtMp35_state_delay5,DivMul1_d1_ScOrEtMp35_state_fire,DivMul1_d1_ScOrEtMp35_state_this_page_is_done};
extern int DivMul1_d1_ScOrEtMp35_consumes[];
extern int DivMul1_d1_ScOrEtMp35_produces[];
class Page_DivMul1_d1_ScOrEtMp35: public ScorePage {
public:
  Page_DivMul1_d1_ScOrEtMp35(UNSIGNED_SCORE_STREAM n_cc_inDiffReduced2,UNSIGNED_SCORE_STREAM n_cc_inStepZeroed2,UNSIGNED_SCORE_STREAM n_cc_inStep,BOOLEAN_SCORE_STREAM n_cc_outDelta1_MuxNibbles,BOOLEAN_SCORE_STREAM n_cc_outDelta1_Index,UNSIGNED_SCORE_STREAM n_cc_outDiffReduced1,UNSIGNED_SCORE_STREAM n_cc_outStepZeroed1,UNSIGNED_SCORE_STREAM n_cc_outVPDiff1) {
    retime_length_0=0;
    cc_inDiffReduced2_retime=new unsigned long [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_inDiffReduced2_retime[j]=0;
    retime_length_1=0;
    cc_inStepZeroed2_retime=new unsigned long [retime_length_1+1];
    for (int j=retime_length_1;j>=0;j--)
      cc_inStepZeroed2_retime[j]=0;
    retime_length_2=0;
    cc_inStep_retime=new unsigned long [retime_length_2+1];
    for (int j=retime_length_2;j>=0;j--)
      cc_inStep_retime[j]=0;
    declareIO(3,5);
    bindInput(0,n_cc_inDiffReduced2,new ScoreStreamType(0,16));
    bindInput(1,n_cc_inStepZeroed2,new ScoreStreamType(0,16));
    bindInput(2,n_cc_inStep,new ScoreStreamType(0,8));
    bindOutput(0,n_cc_outDelta1_MuxNibbles,new ScoreStreamType(0,1));
    bindOutput(1,n_cc_outDelta1_Index,new ScoreStreamType(0,1));
    bindOutput(2,n_cc_outDiffReduced1,new ScoreStreamType(0,16));
    bindOutput(3,n_cc_outStepZeroed1,new ScoreStreamType(0,16));
    bindOutput(4,n_cc_outVPDiff1,new ScoreStreamType(0,16));
    state=DivMul1_d1_ScOrEtMp35_state_fire;
    states=7;
    produces=DivMul1_d1_ScOrEtMp35_produces;
    consumes=DivMul1_d1_ScOrEtMp35_consumes;
    source="DivMul1 in AdpcmEnc.tdf";
    loc=NO_LOCATION;
    input_rates=new int[3];
    output_rates=new int[5];
    input_rates[0]=-1;
    input_rates[1]=-1;
    input_rates[2]=-1;
    output_rates[0]=-1;
    output_rates[1]=-1;
    output_rates[2]=-1;
    output_rates[3]=-1;
    output_rates[4]=-1;
    input_free=new int[3];
    for (int i=0;i<3;i++)
      input_free[i]=0;
    output_close=new int[5];
    for (int i=0;i<5;i++)
      output_close[i]=0;
  } // constructor 
  int pagestep() { 
    unsigned long cc_inDiffReduced2;
    unsigned long cc_inStepZeroed2;
    unsigned long cc_inStep;
    int done=0;
    int canfire=1;
    switch(state) {
      case DivMul1_d1_ScOrEtMp35_state_delay1: { 
        {
        if (1) {
          state=DivMul1_d1_ScOrEtMp35_state_delay2;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul1_d1_ScOrEtMp35_state_delay1
      case DivMul1_d1_ScOrEtMp35_state_delay2: { 
        {
        if (1) {
          state=DivMul1_d1_ScOrEtMp35_state_delay3;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul1_d1_ScOrEtMp35_state_delay2
      case DivMul1_d1_ScOrEtMp35_state_delay3: { 
        {
        if (1) {
          state=DivMul1_d1_ScOrEtMp35_state_delay4;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul1_d1_ScOrEtMp35_state_delay3
      case DivMul1_d1_ScOrEtMp35_state_delay4: { 
        {
        if (1) {
          state=DivMul1_d1_ScOrEtMp35_state_delay5;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul1_d1_ScOrEtMp35_state_delay4
      case DivMul1_d1_ScOrEtMp35_state_delay5: { 
        {
        if (1&&!STREAM_FULL(out[0])&&!STREAM_FULL(out[1])&&!STREAM_FULL(out[2])&&!STREAM_FULL(out[3])&&!STREAM_FULL(out[4])) {
          {
            long cc_diffReduced1=(cc_inDiffReduced2_retime[0]-(cc_inStep_retime[0]>>1));
            if ((cc_diffReduced1>=0)) {
              {
                STREAM_WRITE_NOACC(out[0],true);
                STREAM_WRITE_NOACC(out[1],true);
                {
                  STREAM_WRITE_NOACC(out[2],cc_diffReduced1);
                }
                STREAM_WRITE_NOACC(out[3],(cc_inStep_retime[0]>>1));
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[0],false);
                STREAM_WRITE_NOACC(out[1],false);
                {
                  STREAM_WRITE_NOACC(out[2],cc_diffReduced1);
                }
                STREAM_WRITE_NOACC(out[3],0);
              }
            }
            STREAM_WRITE_NOACC(out[4],((cc_inStep_retime[0]>>3)+cc_inStepZeroed2_retime[0]));
            state=DivMul1_d1_ScOrEtMp35_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul1_d1_ScOrEtMp35_state_delay5
      case DivMul1_d1_ScOrEtMp35_state_fire: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        int data_2=STREAM_DATA(in[2]);
        int eos_2=0;
        if (data_2) eos_2=STREAM_EOS(in[2]);
        if (1&& data_0&&!eos_0&& data_1&&!eos_1&& data_2&&!eos_2) {
          cc_inDiffReduced2=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inDiffReduced2_retime[j]=cc_inDiffReduced2_retime[j-1];
          cc_inDiffReduced2_retime[0]=cc_inDiffReduced2;
          cc_inStepZeroed2=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inStepZeroed2_retime[j]=cc_inStepZeroed2_retime[j-1];
          cc_inStepZeroed2_retime[0]=cc_inStepZeroed2;
          cc_inStep=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_inStep_retime[j]=cc_inStep_retime[j-1];
          cc_inStep_retime[0]=cc_inStep;
          state=DivMul1_d1_ScOrEtMp35_state_delay1;
        }
        else
         if (0||(data_0&&eos_0)||(data_2&&eos_2)||(data_1&&eos_1)) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul1_d1_ScOrEtMp35_state_fire
      case DivMul1_d1_ScOrEtMp35_state_this_page_is_done: {
        return(0);
      } // end case DivMul1_d1_ScOrEtMp35_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in DivMul1_d1_ScOrEtMp35::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      STREAM_FREE(in[0]);
      STREAM_FREE(in[1]);
      STREAM_FREE(in[2]);
      STREAM_CLOSE(out[0]);
      STREAM_CLOSE(out[1]);
      STREAM_CLOSE(out[2]);
      STREAM_CLOSE(out[3]);
      STREAM_CLOSE(out[4]);
      state=DivMul1_d1_ScOrEtMp35_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  unsigned long *cc_inDiffReduced2_retime;
  int retime_length_1;
  unsigned long *cc_inStepZeroed2_retime;
  int retime_length_2;
  unsigned long *cc_inStep_retime;
  int *input_free;
  int *output_close;
};
int DivMul1_d1_ScOrEtMp35_consumes[]={0,0,0,0,0,7,0};
int DivMul1_d1_ScOrEtMp35_produces[]={0,0,0,0,31,0,0};
enum DivMul0_d1_ScOrEtMp36_state_syms {DivMul0_d1_ScOrEtMp36_state_delay1,DivMul0_d1_ScOrEtMp36_state_delay2,DivMul0_d1_ScOrEtMp36_state_delay3,DivMul0_d1_ScOrEtMp36_state_delay4,DivMul0_d1_ScOrEtMp36_state_delay5,DivMul0_d1_ScOrEtMp36_state_delay6,DivMul0_d1_ScOrEtMp36_state_delay7,DivMul0_d1_ScOrEtMp36_state_delay8,DivMul0_d1_ScOrEtMp36_state_fire,DivMul0_d1_ScOrEtMp36_state_this_page_is_done};
extern int DivMul0_d1_ScOrEtMp36_consumes[];
extern int DivMul0_d1_ScOrEtMp36_produces[];
class Page_DivMul0_d1_ScOrEtMp36: public ScorePage {
public:
  Page_DivMul0_d1_ScOrEtMp36(UNSIGNED_SCORE_STREAM n_cc_inDiffReduced1,UNSIGNED_SCORE_STREAM n_cc_inStepZeroed1,UNSIGNED_SCORE_STREAM n_cc_inStep,UNSIGNED_SCORE_STREAM n_cc_inVPDiff1,BOOLEAN_SCORE_STREAM n_cc_outDelta0_MuxNibbles,BOOLEAN_SCORE_STREAM n_cc_outDelta0_Index,UNSIGNED_SCORE_STREAM n_cc_outVPDiffAbs) {
    retime_length_0=0;
    cc_inDiffReduced1_retime=new unsigned long [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_inDiffReduced1_retime[j]=0;
    retime_length_1=0;
    cc_inStepZeroed1_retime=new unsigned long [retime_length_1+1];
    for (int j=retime_length_1;j>=0;j--)
      cc_inStepZeroed1_retime[j]=0;
    retime_length_2=0;
    cc_inStep_retime=new unsigned long [retime_length_2+1];
    for (int j=retime_length_2;j>=0;j--)
      cc_inStep_retime[j]=0;
    retime_length_3=0;
    cc_inVPDiff1_retime=new unsigned long [retime_length_3+1];
    for (int j=retime_length_3;j>=0;j--)
      cc_inVPDiff1_retime[j]=0;
    declareIO(4,3);
    bindInput(0,n_cc_inDiffReduced1,new ScoreStreamType(0,16));
    bindInput(1,n_cc_inStepZeroed1,new ScoreStreamType(0,16));
    bindInput(2,n_cc_inStep,new ScoreStreamType(0,8));
    bindInput(3,n_cc_inVPDiff1,new ScoreStreamType(0,16));
    bindOutput(0,n_cc_outDelta0_MuxNibbles,new ScoreStreamType(0,1));
    bindOutput(1,n_cc_outDelta0_Index,new ScoreStreamType(0,1));
    bindOutput(2,n_cc_outVPDiffAbs,new ScoreStreamType(0,16));
    state=DivMul0_d1_ScOrEtMp36_state_fire;
    states=10;
    produces=DivMul0_d1_ScOrEtMp36_produces;
    consumes=DivMul0_d1_ScOrEtMp36_consumes;
    source="DivMul0 in AdpcmEnc.tdf";
    loc=NO_LOCATION;
    input_rates=new int[4];
    output_rates=new int[3];
    input_rates[0]=-1;
    input_rates[1]=-1;
    input_rates[2]=-1;
    input_rates[3]=-1;
    output_rates[0]=-1;
    output_rates[1]=-1;
    output_rates[2]=-1;
    input_free=new int[4];
    for (int i=0;i<4;i++)
      input_free[i]=0;
    output_close=new int[3];
    for (int i=0;i<3;i++)
      output_close[i]=0;
  } // constructor 
  int pagestep() { 
    unsigned long cc_inDiffReduced1;
    unsigned long cc_inStepZeroed1;
    unsigned long cc_inStep;
    unsigned long cc_inVPDiff1;
    int done=0;
    int canfire=1;
    switch(state) {
      case DivMul0_d1_ScOrEtMp36_state_delay1: { 
        {
        if (1) {
          state=DivMul0_d1_ScOrEtMp36_state_delay2;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay1
      case DivMul0_d1_ScOrEtMp36_state_delay2: { 
        {
        if (1) {
          state=DivMul0_d1_ScOrEtMp36_state_delay3;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay2
      case DivMul0_d1_ScOrEtMp36_state_delay3: { 
        {
        if (1) {
          state=DivMul0_d1_ScOrEtMp36_state_delay4;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay3
      case DivMul0_d1_ScOrEtMp36_state_delay4: { 
        {
        if (1) {
          state=DivMul0_d1_ScOrEtMp36_state_delay5;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay4
      case DivMul0_d1_ScOrEtMp36_state_delay5: { 
        {
        if (1) {
          state=DivMul0_d1_ScOrEtMp36_state_delay6;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay5
      case DivMul0_d1_ScOrEtMp36_state_delay6: { 
        {
        if (1) {
          state=DivMul0_d1_ScOrEtMp36_state_delay7;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay6
      case DivMul0_d1_ScOrEtMp36_state_delay7: { 
        {
        if (1) {
          state=DivMul0_d1_ScOrEtMp36_state_delay8;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay7
      case DivMul0_d1_ScOrEtMp36_state_delay8: { 
        {
        if (1&&!STREAM_FULL(out[0])&&!STREAM_FULL(out[1])&&!STREAM_FULL(out[2])) {
          {
            unsigned long cc_VPDiff0=(cc_inVPDiff1_retime[0]+cc_inStepZeroed1_retime[0]);
            long cc_diffReduced0=(cc_inDiffReduced1_retime[0]-(cc_inStep_retime[0]>>2));
            unsigned long cc_stepZeroed0;
            if ((cc_diffReduced0>=0)) {
              {
                STREAM_WRITE_NOACC(out[0],true);
                STREAM_WRITE_NOACC(out[1],true);
                cc_stepZeroed0=(cc_inStep_retime[0]>>2);
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[0],false);
                STREAM_WRITE_NOACC(out[1],false);
                cc_stepZeroed0=0;
              }
            }
            {
              STREAM_WRITE_NOACC(out[2],(cc_VPDiff0+cc_stepZeroed0));
            }
            state=DivMul0_d1_ScOrEtMp36_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_delay8
      case DivMul0_d1_ScOrEtMp36_state_fire: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        int data_2=STREAM_DATA(in[2]);
        int eos_2=0;
        if (data_2) eos_2=STREAM_EOS(in[2]);
        int data_3=STREAM_DATA(in[3]);
        int eos_3=0;
        if (data_3) eos_3=STREAM_EOS(in[3]);
        if (1&& data_0&&!eos_0&& data_1&&!eos_1&& data_2&&!eos_2&& data_3&&!eos_3) {
          cc_inDiffReduced1=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inDiffReduced1_retime[j]=cc_inDiffReduced1_retime[j-1];
          cc_inDiffReduced1_retime[0]=cc_inDiffReduced1;
          cc_inStepZeroed1=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inStepZeroed1_retime[j]=cc_inStepZeroed1_retime[j-1];
          cc_inStepZeroed1_retime[0]=cc_inStepZeroed1;
          cc_inStep=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_inStep_retime[j]=cc_inStep_retime[j-1];
          cc_inStep_retime[0]=cc_inStep;
          cc_inVPDiff1=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_inVPDiff1_retime[j]=cc_inVPDiff1_retime[j-1];
          cc_inVPDiff1_retime[0]=cc_inVPDiff1;
          state=DivMul0_d1_ScOrEtMp36_state_delay1;
        }
        else
         if (0||(data_0&&eos_0)||(data_2&&eos_2)||(data_1&&eos_1)||(data_3&&eos_3)) done=1; else canfire=0;
        }
        break; 
      } // end case DivMul0_d1_ScOrEtMp36_state_fire
      case DivMul0_d1_ScOrEtMp36_state_this_page_is_done: {
        return(0);
      } // end case DivMul0_d1_ScOrEtMp36_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in DivMul0_d1_ScOrEtMp36::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      STREAM_FREE(in[0]);
      STREAM_FREE(in[1]);
      STREAM_FREE(in[2]);
      STREAM_FREE(in[3]);
      STREAM_CLOSE(out[0]);
      STREAM_CLOSE(out[1]);
      STREAM_CLOSE(out[2]);
      state=DivMul0_d1_ScOrEtMp36_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  unsigned long *cc_inDiffReduced1_retime;
  int retime_length_1;
  unsigned long *cc_inStepZeroed1_retime;
  int retime_length_2;
  unsigned long *cc_inStep_retime;
  int retime_length_3;
  unsigned long *cc_inVPDiff1_retime;
  int *input_free;
  int *output_close;
};
int DivMul0_d1_ScOrEtMp36_consumes[]={0,0,0,0,0,0,0,0,15,0};
int DivMul0_d1_ScOrEtMp36_produces[]={0,0,0,0,0,0,0,7,0,0};
enum ValPredUpdate_d1_ScOrEtMp37_state_syms {ValPredUpdate_d1_ScOrEtMp37_state_delay1,ValPredUpdate_d1_ScOrEtMp37_state_delay2,ValPredUpdate_d1_ScOrEtMp37_state_delay3,ValPredUpdate_d1_ScOrEtMp37_state_delay4,ValPredUpdate_d1_ScOrEtMp37_state_delay5,ValPredUpdate_d1_ScOrEtMp37_state_delay6,ValPredUpdate_d1_ScOrEtMp37_state_delay7,ValPredUpdate_d1_ScOrEtMp37_state_fire,ValPredUpdate_d1_ScOrEtMp37_state_init,ValPredUpdate_d1_ScOrEtMp37_state_this_page_is_done};
extern int ValPredUpdate_d1_ScOrEtMp37_consumes[];
extern int ValPredUpdate_d1_ScOrEtMp37_produces[];
class Page_ValPredUpdate_d1_ScOrEtMp37: public ScorePage {
public:
  Page_ValPredUpdate_d1_ScOrEtMp37(UNSIGNED_SCORE_STREAM n_cc_inVPDiffAbs,BOOLEAN_SCORE_STREAM n_cc_inSign,SIGNED_SCORE_STREAM n_cc_outValPred) {
    retime_length_0=0;
    cc_inVPDiffAbs_retime=new unsigned long [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_inVPDiffAbs_retime[j]=0;
    retime_length_1=0;
    cc_inSign_retime=new int [retime_length_1+1];
    for (int j=retime_length_1;j>=0;j--)
      cc_inSign_retime[j]=0;
    declareIO(2,1);
    bindInput(0,n_cc_inVPDiffAbs,new ScoreStreamType(0,16));
    bindInput(1,n_cc_inSign,new ScoreStreamType(0,1));
    bindOutput(0,n_cc_outValPred,new ScoreStreamType(1,16));
    state=ValPredUpdate_d1_ScOrEtMp37_state_init;
    states=10;
    produces=ValPredUpdate_d1_ScOrEtMp37_produces;
    consumes=ValPredUpdate_d1_ScOrEtMp37_consumes;
    source="ValPredUpdate in AdpcmEnc.tdf";
    loc=NO_LOCATION;
    input_rates=new int[2];
    output_rates=new int[1];
    input_rates[0]=-1;
    input_rates[1]=-1;
    output_rates[0]=-1;
    input_free=new int[2];
    for (int i=0;i<2;i++)
      input_free[i]=0;
    output_close=new int[1];
    for (int i=0;i<1;i++)
      output_close[i]=0;
    cc_valPred=0;
  } // constructor 
  int pagestep() { 
    unsigned long cc_inVPDiffAbs;
    int cc_inSign;
    int done=0;
    int canfire=1;
    switch(state) {
      case ValPredUpdate_d1_ScOrEtMp37_state_delay1: { 
        {
        if (1) {
          state=ValPredUpdate_d1_ScOrEtMp37_state_delay2;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_delay1
      case ValPredUpdate_d1_ScOrEtMp37_state_delay2: { 
        {
        if (1) {
          state=ValPredUpdate_d1_ScOrEtMp37_state_delay3;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_delay2
      case ValPredUpdate_d1_ScOrEtMp37_state_delay3: { 
        {
        if (1) {
          state=ValPredUpdate_d1_ScOrEtMp37_state_delay4;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_delay3
      case ValPredUpdate_d1_ScOrEtMp37_state_delay4: { 
        {
        if (1) {
          state=ValPredUpdate_d1_ScOrEtMp37_state_delay5;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_delay4
      case ValPredUpdate_d1_ScOrEtMp37_state_delay5: { 
        {
        if (1) {
          state=ValPredUpdate_d1_ScOrEtMp37_state_delay6;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_delay5
      case ValPredUpdate_d1_ScOrEtMp37_state_delay6: { 
        {
        if (1) {
          state=ValPredUpdate_d1_ScOrEtMp37_state_delay7;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_delay6
      case ValPredUpdate_d1_ScOrEtMp37_state_delay7: { 
        {
        if (1&&!STREAM_FULL(out[0])) {
          {
            long cc_VPDiff=((cc_inSign_retime[0])?((-(cc_inVPDiffAbs_retime[0]))):(cc_inVPDiffAbs_retime[0]));
            long cc_valPredTmp=((cc_valPred+cc_VPDiff)+((cc_inSign_retime[0])?(1):(0)));
            if ((cc_valPredTmp<-32768)) {
              cc_valPred=-32768;
            }
            else {
              if ((cc_valPredTmp>32767)) {
                cc_valPred=32767;
              }
              else {
                cc_valPred=cc_valPredTmp;
              }
            }
            STREAM_WRITE_NOACC(out[0],cc_valPred);
            state=ValPredUpdate_d1_ScOrEtMp37_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_delay7
      case ValPredUpdate_d1_ScOrEtMp37_state_fire: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        if (1&& data_0&&!eos_0&& data_1&&!eos_1) {
          cc_inVPDiffAbs=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inVPDiffAbs_retime[j]=cc_inVPDiffAbs_retime[j-1];
          cc_inVPDiffAbs_retime[0]=cc_inVPDiffAbs;
          cc_inSign=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inSign_retime[j]=cc_inSign_retime[j-1];
          cc_inSign_retime[0]=cc_inSign;
          state=ValPredUpdate_d1_ScOrEtMp37_state_delay1;
        }
        else
         if (0||(data_1&&eos_1)||(data_0&&eos_0)) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_fire
      case ValPredUpdate_d1_ScOrEtMp37_state_init: { 
        {
        if (1&&!STREAM_FULL(out[0])) {
          {
            STREAM_WRITE_NOACC(out[0],0);
            state=ValPredUpdate_d1_ScOrEtMp37_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_init
      case ValPredUpdate_d1_ScOrEtMp37_state_this_page_is_done: {
        return(0);
      } // end case ValPredUpdate_d1_ScOrEtMp37_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in ValPredUpdate_d1_ScOrEtMp37::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      STREAM_FREE(in[0]);
      STREAM_FREE(in[1]);
      STREAM_CLOSE(out[0]);
      state=ValPredUpdate_d1_ScOrEtMp37_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  unsigned long *cc_inVPDiffAbs_retime;
  int retime_length_1;
  int *cc_inSign_retime;
  long cc_valPred;
  int *input_free;
  int *output_close;
};
int ValPredUpdate_d1_ScOrEtMp37_consumes[]={0,0,0,0,0,0,0,3,0,0};
int ValPredUpdate_d1_ScOrEtMp37_produces[]={0,0,0,0,0,0,1,0,1,0};
enum IndexOffsetTable_d1_ScOrEtMp38_state_syms {IndexOffsetTable_d1_ScOrEtMp38_state_delay1,IndexOffsetTable_d1_ScOrEtMp38_state_delay2,IndexOffsetTable_d1_ScOrEtMp38_state_delay3,IndexOffsetTable_d1_ScOrEtMp38_state_delay4,IndexOffsetTable_d1_ScOrEtMp38_state_delay5,IndexOffsetTable_d1_ScOrEtMp38_state_delay6,IndexOffsetTable_d1_ScOrEtMp38_state_fire,IndexOffsetTable_d1_ScOrEtMp38_state_init,IndexOffsetTable_d1_ScOrEtMp38_state_this_page_is_done};
extern int IndexOffsetTable_d1_ScOrEtMp38_consumes[];
extern int IndexOffsetTable_d1_ScOrEtMp38_produces[];
class Page_IndexOffsetTable_d1_ScOrEtMp38: public ScorePage {
public:
  Page_IndexOffsetTable_d1_ScOrEtMp38(UNSIGNED_SCORE_STREAM n_cc_inIndex,BOOLEAN_SCORE_STREAM n_cc_inDelta2,BOOLEAN_SCORE_STREAM n_cc_inDelta1,BOOLEAN_SCORE_STREAM n_cc_inDelta0,UNSIGNED_SCORE_STREAM n_cc_outIndex_Index,UNSIGNED_SCORE_STREAM n_cc_outIndex_StepTable) {
    retime_length_0=0;
    cc_inIndex_retime=new unsigned long [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_inIndex_retime[j]=0;
    retime_length_1=0;
    cc_inDelta2_retime=new int [retime_length_1+1];
    for (int j=retime_length_1;j>=0;j--)
      cc_inDelta2_retime[j]=0;
    retime_length_2=0;
    cc_inDelta1_retime=new int [retime_length_2+1];
    for (int j=retime_length_2;j>=0;j--)
      cc_inDelta1_retime[j]=0;
    retime_length_3=0;
    cc_inDelta0_retime=new int [retime_length_3+1];
    for (int j=retime_length_3;j>=0;j--)
      cc_inDelta0_retime[j]=0;
    declareIO(4,2);
    bindInput(0,n_cc_inIndex,new ScoreStreamType(0,7));
    bindInput(1,n_cc_inDelta2,new ScoreStreamType(0,1));
    bindInput(2,n_cc_inDelta1,new ScoreStreamType(0,1));
    bindInput(3,n_cc_inDelta0,new ScoreStreamType(0,1));
    bindOutput(0,n_cc_outIndex_Index,new ScoreStreamType(0,7));
    bindOutput(1,n_cc_outIndex_StepTable,new ScoreStreamType(0,7));
    state=IndexOffsetTable_d1_ScOrEtMp38_state_init;
    states=9;
    produces=IndexOffsetTable_d1_ScOrEtMp38_produces;
    consumes=IndexOffsetTable_d1_ScOrEtMp38_consumes;
    source="IndexOffsetTable in AdpcmEnc.tdf";
    loc=NO_LOCATION;
    input_rates=new int[4];
    output_rates=new int[2];
    input_rates[0]=-1;
    input_rates[1]=-1;
    input_rates[2]=-1;
    input_rates[3]=-1;
    output_rates[0]=-1;
    output_rates[1]=-1;
    input_free=new int[4];
    for (int i=0;i<4;i++)
      input_free[i]=0;
    output_close=new int[2];
    for (int i=0;i<2;i++)
      output_close[i]=0;
  } // constructor 
  int pagestep() { 
    unsigned long cc_inIndex;
    int cc_inDelta2;
    int cc_inDelta1;
    int cc_inDelta0;
    int done=0;
    int canfire=1;
    switch(state) {
      case IndexOffsetTable_d1_ScOrEtMp38_state_delay1: { 
        {
        if (1) {
          state=IndexOffsetTable_d1_ScOrEtMp38_state_delay2;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_delay1
      case IndexOffsetTable_d1_ScOrEtMp38_state_delay2: { 
        {
        if (1) {
          state=IndexOffsetTable_d1_ScOrEtMp38_state_delay3;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_delay2
      case IndexOffsetTable_d1_ScOrEtMp38_state_delay3: { 
        {
        if (1) {
          state=IndexOffsetTable_d1_ScOrEtMp38_state_delay4;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_delay3
      case IndexOffsetTable_d1_ScOrEtMp38_state_delay4: { 
        {
        if (1) {
          state=IndexOffsetTable_d1_ScOrEtMp38_state_delay5;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_delay4
      case IndexOffsetTable_d1_ScOrEtMp38_state_delay5: { 
        {
        if (1) {
          state=IndexOffsetTable_d1_ScOrEtMp38_state_delay6;
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_delay5
      case IndexOffsetTable_d1_ScOrEtMp38_state_delay6: { 
        {
        if (1&&!STREAM_FULL(out[0])&&!STREAM_FULL(out[1])) {
          {
            long cc_indexOffset=((cc_inDelta2_retime[0])?(((cc_inDelta1_retime[0])?(((cc_inDelta0_retime[0])?(8):(6))):(((cc_inDelta0_retime[0])?(4):(2))))):(-1));
            long cc_newIndex=(cc_inIndex_retime[0]+cc_indexOffset);
            long cc_newIndexM88=(cc_newIndex-88);
            unsigned long cc_outIndex=(((cc_inIndex_retime[0]<0))?(0):((((cc_newIndexM88<88))?(88):(cc_newIndex))));
            STREAM_WRITE_NOACC(out[0],cc_outIndex);
            STREAM_WRITE_NOACC(out[1],cc_outIndex);
            state=IndexOffsetTable_d1_ScOrEtMp38_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_delay6
      case IndexOffsetTable_d1_ScOrEtMp38_state_fire: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        int data_2=STREAM_DATA(in[2]);
        int eos_2=0;
        if (data_2) eos_2=STREAM_EOS(in[2]);
        int data_3=STREAM_DATA(in[3]);
        int eos_3=0;
        if (data_3) eos_3=STREAM_EOS(in[3]);
        if (1&& data_0&&!eos_0&& data_1&&!eos_1&& data_2&&!eos_2&& data_3&&!eos_3) {
          cc_inIndex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inIndex_retime[j]=cc_inIndex_retime[j-1];
          cc_inIndex_retime[0]=cc_inIndex;
          cc_inDelta2=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inDelta2_retime[j]=cc_inDelta2_retime[j-1];
          cc_inDelta2_retime[0]=cc_inDelta2;
          cc_inDelta1=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_inDelta1_retime[j]=cc_inDelta1_retime[j-1];
          cc_inDelta1_retime[0]=cc_inDelta1;
          cc_inDelta0=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_inDelta0_retime[j]=cc_inDelta0_retime[j-1];
          cc_inDelta0_retime[0]=cc_inDelta0;
          state=IndexOffsetTable_d1_ScOrEtMp38_state_delay1;
        }
        else
         if (0||(data_3&&eos_3)||(data_2&&eos_2)||(data_1&&eos_1)||(data_0&&eos_0)) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_fire
      case IndexOffsetTable_d1_ScOrEtMp38_state_init: { 
        {
        if (1&&!STREAM_FULL(out[0])&&!STREAM_FULL(out[1])) {
          {
            STREAM_WRITE_NOACC(out[0],0);
            STREAM_WRITE_NOACC(out[1],0);
            state=IndexOffsetTable_d1_ScOrEtMp38_state_fire;
          }
        }
        else
         if (0) done=1; else canfire=0;
        }
        break; 
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_init
      case IndexOffsetTable_d1_ScOrEtMp38_state_this_page_is_done: {
        return(0);
      } // end case IndexOffsetTable_d1_ScOrEtMp38_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in IndexOffsetTable_d1_ScOrEtMp38::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      STREAM_FREE(in[0]);
      STREAM_FREE(in[1]);
      STREAM_FREE(in[2]);
      STREAM_FREE(in[3]);
      STREAM_CLOSE(out[0]);
      STREAM_CLOSE(out[1]);
      state=IndexOffsetTable_d1_ScOrEtMp38_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  unsigned long *cc_inIndex_retime;
  int retime_length_1;
  int *cc_inDelta2_retime;
  int retime_length_2;
  int *cc_inDelta1_retime;
  int retime_length_3;
  int *cc_inDelta0_retime;
  int *input_free;
  int *output_close;
};
int IndexOffsetTable_d1_ScOrEtMp38_consumes[]={0,0,0,0,0,0,15,0,0};
int IndexOffsetTable_d1_ScOrEtMp38_produces[]={0,0,0,0,0,3,0,3,0};
enum MuxNibbles_d1_ScOrEtMp39_state_syms {MuxNibbles_d1_ScOrEtMp39_state_fire_nibble1,MuxNibbles_d1_ScOrEtMp39_state_fire_nibble2,MuxNibbles_d1_ScOrEtMp39_state_this_page_is_done};
extern int MuxNibbles_d1_ScOrEtMp39_consumes[];
extern int MuxNibbles_d1_ScOrEtMp39_produces[];
class Page_MuxNibbles_d1_ScOrEtMp39: public ScorePage {
public:
  Page_MuxNibbles_d1_ScOrEtMp39(BOOLEAN_SCORE_STREAM n_cc_inSign,BOOLEAN_SCORE_STREAM n_cc_inDelta2,BOOLEAN_SCORE_STREAM n_cc_inDelta1,BOOLEAN_SCORE_STREAM n_cc_inDelta0,UNSIGNED_SCORE_STREAM n_cc_outAdpcmEnc) {
    retime_length_0=0;
    cc_inSign_retime=new int [retime_length_0+1];
    for (int j=retime_length_0;j>=0;j--)
      cc_inSign_retime[j]=0;
    retime_length_1=0;
    cc_inDelta2_retime=new int [retime_length_1+1];
    for (int j=retime_length_1;j>=0;j--)
      cc_inDelta2_retime[j]=0;
    retime_length_2=0;
    cc_inDelta1_retime=new int [retime_length_2+1];
    for (int j=retime_length_2;j>=0;j--)
      cc_inDelta1_retime[j]=0;
    retime_length_3=0;
    cc_inDelta0_retime=new int [retime_length_3+1];
    for (int j=retime_length_3;j>=0;j--)
      cc_inDelta0_retime[j]=0;
    declareIO(4,1);
    bindInput(0,n_cc_inSign,new ScoreStreamType(0,1));
    bindInput(1,n_cc_inDelta2,new ScoreStreamType(0,1));
    bindInput(2,n_cc_inDelta1,new ScoreStreamType(0,1));
    bindInput(3,n_cc_inDelta0,new ScoreStreamType(0,1));
    bindOutput(0,n_cc_outAdpcmEnc,new ScoreStreamType(0,8));
    state=MuxNibbles_d1_ScOrEtMp39_state_fire_nibble1;
    states=3;
    produces=MuxNibbles_d1_ScOrEtMp39_produces;
    consumes=MuxNibbles_d1_ScOrEtMp39_consumes;
    source="MuxNibbles in AdpcmEnc.tdf";
    loc=NO_LOCATION;
    input_rates=new int[4];
    output_rates=new int[1];
    input_rates[0]=-1;
    input_rates[1]=-1;
    input_rates[2]=-1;
    input_rates[3]=-1;
    output_rates[0]=-1;
    input_free=new int[4];
    for (int i=0;i<4;i++)
      input_free[i]=0;
    output_close=new int[1];
    for (int i=0;i<1;i++)
      output_close[i]=0;
  } // constructor 
  int pagestep() { 
    int cc_inSign;
    int cc_inDelta2;
    int cc_inDelta1;
    int cc_inDelta0;
    int done=0;
    int canfire=1;
    switch(state) {
      case MuxNibbles_d1_ScOrEtMp39_state_fire_nibble1: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        int data_2=STREAM_DATA(in[2]);
        int eos_2=0;
        if (data_2) eos_2=STREAM_EOS(in[2]);
        int data_3=STREAM_DATA(in[3]);
        int eos_3=0;
        if (data_3) eos_3=STREAM_EOS(in[3]);
        if (1&& data_0&&!eos_0&& data_1&&!eos_1&& data_2&&!eos_2&& data_3&&!eos_3) {
          cc_inSign=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inSign_retime[j]=cc_inSign_retime[j-1];
          cc_inSign_retime[0]=cc_inSign;
          cc_inDelta2=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inDelta2_retime[j]=cc_inDelta2_retime[j-1];
          cc_inDelta2_retime[0]=cc_inDelta2;
          cc_inDelta1=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_inDelta1_retime[j]=cc_inDelta1_retime[j-1];
          cc_inDelta1_retime[0]=cc_inDelta1;
          cc_inDelta0=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_inDelta0_retime[j]=cc_inDelta0_retime[j-1];
          cc_inDelta0_retime[0]=cc_inDelta0;
          {
            cc_nibble=(((((((((cc_inSign_retime[0])?(1):(0)))<<(1))|((cc_inDelta2_retime[0])?(1):(0)))<<(1))|((cc_inDelta1_retime[0])?(1):(0)))<<(1))|(((cc_inDelta0_retime[0])?(1):(0))));
            state=MuxNibbles_d1_ScOrEtMp39_state_fire_nibble2;
          }
        }
        else
         if (0||(data_3&&eos_3)||(data_2&&eos_2)||(data_1&&eos_1)||(data_0&&eos_0)) done=1; else canfire=0;
        }
        break; 
      } // end case MuxNibbles_d1_ScOrEtMp39_state_fire_nibble1
      case MuxNibbles_d1_ScOrEtMp39_state_fire_nibble2: { 
        {
        int data_0=STREAM_DATA(in[0]);
        int eos_0=0;
        if (data_0) eos_0=STREAM_EOS(in[0]);
        int data_1=STREAM_DATA(in[1]);
        int eos_1=0;
        if (data_1) eos_1=STREAM_EOS(in[1]);
        int data_2=STREAM_DATA(in[2]);
        int eos_2=0;
        if (data_2) eos_2=STREAM_EOS(in[2]);
        int data_3=STREAM_DATA(in[3]);
        int eos_3=0;
        if (data_3) eos_3=STREAM_EOS(in[3]);
        if (1&& data_0&&eos_0&& data_1&&eos_1&& data_2&&eos_2&& data_3&&eos_3&&!STREAM_FULL(out[0])) {
          if (!input_free[0])
          STREAM_FREE(in[0]);
          input_free[0]=1;
          if (!input_free[1])
          STREAM_FREE(in[1]);
          input_free[1]=1;
          if (!input_free[2])
          STREAM_FREE(in[2]);
          input_free[2]=1;
          if (!input_free[3])
          STREAM_FREE(in[3]);
          input_free[3]=1;
          {
            STREAM_WRITE_NOACC(out[0],(((cc_nibble)<<(4))|(0)));
            STREAM_CLOSE(out[0]);
            output_close[0]=1;
            done=1;
          }
        }
        else
        {
        if (1&& data_0&&!eos_0&& data_1&&!eos_1&& data_2&&!eos_2&& data_3&&!eos_3&&!STREAM_FULL(out[0])) {
          cc_inSign=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_inSign_retime[j]=cc_inSign_retime[j-1];
          cc_inSign_retime[0]=cc_inSign;
          cc_inDelta2=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_inDelta2_retime[j]=cc_inDelta2_retime[j-1];
          cc_inDelta2_retime[0]=cc_inDelta2;
          cc_inDelta1=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_inDelta1_retime[j]=cc_inDelta1_retime[j-1];
          cc_inDelta1_retime[0]=cc_inDelta1;
          cc_inDelta0=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_inDelta0_retime[j]=cc_inDelta0_retime[j-1];
          cc_inDelta0_retime[0]=cc_inDelta0;
          {
            STREAM_WRITE_NOACC(out[0],(((((((((cc_nibble)<<(1))|((cc_inSign_retime[0])?(1):(0)))<<(1))|((cc_inDelta2_retime[0])?(1):(0)))<<(1))|((cc_inDelta1_retime[0])?(1):(0)))<<(1))|(((cc_inDelta0_retime[0])?(1):(0)))));
            state=MuxNibbles_d1_ScOrEtMp39_state_fire_nibble1;
          }
        }
        else
         if (0||(data_3&&eos_3)||(data_2&&eos_2)||(data_1&&eos_1)||(data_0&&eos_0)) done=1; else canfire=0;
        }}
        break; 
      } // end case MuxNibbles_d1_ScOrEtMp39_state_fire_nibble2
      case MuxNibbles_d1_ScOrEtMp39_state_this_page_is_done: {
        return(0);
      } // end case MuxNibbles_d1_ScOrEtMp39_state_this_page_is_done
      default: cerr << "ERROR unknown state [" << state << "] encountered in MuxNibbles_d1_ScOrEtMp39::pagestep" << endl;
        abort();
    }
    if (canfire) fire++; else stall++;
    if (done) {
      if (!input_free[0])
        STREAM_FREE(in[0]);
      if (!input_free[1])
        STREAM_FREE(in[1]);
      if (!input_free[2])
        STREAM_FREE(in[2]);
      if (!input_free[3])
        STREAM_FREE(in[3]);
      if (!output_close[0])
        STREAM_CLOSE(out[0]);
      state=MuxNibbles_d1_ScOrEtMp39_state_this_page_is_done;
      return(0);
    }
    else return(1);
  } // pagestep
private:
  int retime_length_0;
  int *cc_inSign_retime;
  int retime_length_1;
  int *cc_inDelta2_retime;
  int retime_length_2;
  int *cc_inDelta1_retime;
  int retime_length_3;
  int *cc_inDelta0_retime;
  unsigned long cc_nibble;
  int *input_free;
  int *output_close;
};
int MuxNibbles_d1_ScOrEtMp39_consumes[]={15,15,0};
int MuxNibbles_d1_ScOrEtMp39_produces[]={0,1,0};
class AdpcmEnc_instance: public ScoreOperatorInstance {
public:
  AdpcmEnc_instance(SIGNED_SCORE_STREAM cc_inAdpcmEnc,UNSIGNED_SCORE_STREAM cc_outAdpcmEnc) {
    pages=8;
    segments=1;
    page=new (ScorePage *)[8];
    segment=new (ScoreSegment *)[1];
    page_group=new int[8];
    segment_group=new int[1];
    // local declarations 
    BOOLEAN_SCORE_STREAM ScOrEtMp10; // outDelta0_MuxNibbles
    BOOLEAN_SCORE_STREAM ScOrEtMp11; // outDelta1_Index
    BOOLEAN_SCORE_STREAM ScOrEtMp12; // outDelta1_MuxNibbles
    BOOLEAN_SCORE_STREAM ScOrEtMp13; // outDelta2_Index
    BOOLEAN_SCORE_STREAM ScOrEtMp14; // outDelta2_MuxNibbles
    UNSIGNED_SCORE_STREAM ScOrEtMp15; // outDiff
    UNSIGNED_SCORE_STREAM ScOrEtMp16; // outDiffReduced1
    UNSIGNED_SCORE_STREAM ScOrEtMp17; // outDiffReduced2
    UNSIGNED_SCORE_STREAM ScOrEtMp18; // outIndex_Index
    UNSIGNED_SCORE_STREAM ScOrEtMp19; // outIndex_StepTable
    BOOLEAN_SCORE_STREAM ScOrEtMp20; // outSign_MuxNibbles
    BOOLEAN_SCORE_STREAM ScOrEtMp21; // outSign_ValPred
    UNSIGNED_SCORE_STREAM ScOrEtMp22; // outStep
    UNSIGNED_SCORE_STREAM ScOrEtMp23; // outStepZeroed1
    UNSIGNED_SCORE_STREAM ScOrEtMp24; // outStepZeroed2
    UNSIGNED_SCORE_STREAM ScOrEtMp25; // outStep_DivMul0
    UNSIGNED_SCORE_STREAM ScOrEtMp26; // outStep_DivMul1
    UNSIGNED_SCORE_STREAM ScOrEtMp27; // outStep_DivMul2
    UNSIGNED_SCORE_STREAM ScOrEtMp28; // outVPDiff1
    UNSIGNED_SCORE_STREAM ScOrEtMp29; // outVPDiffAbs
    SIGNED_SCORE_STREAM ScOrEtMp30; // outValPred
    UNSIGNED_SCORE_SEGMENT ScOrEtMp31; // stepTable
    BOOLEAN_SCORE_STREAM ScOrEtMp9; // outDelta0_Index
    // create local streams and segments 
    ScOrEtMp10=NEW_BOOLEAN_SCORE_STREAM();
    ScOrEtMp11=NEW_BOOLEAN_SCORE_STREAM();
    ScOrEtMp12=NEW_BOOLEAN_SCORE_STREAM();
    ScOrEtMp13=NEW_BOOLEAN_SCORE_STREAM();
    ScOrEtMp14=NEW_BOOLEAN_SCORE_STREAM();
    ScOrEtMp15=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp16=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp17=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp18=NEW_UNSIGNED_SCORE_STREAM(7);
    ScOrEtMp19=NEW_UNSIGNED_SCORE_STREAM(7);
    ScOrEtMp20=NEW_BOOLEAN_SCORE_STREAM();
    ScOrEtMp21=NEW_BOOLEAN_SCORE_STREAM();
    ScOrEtMp22=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp23=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp24=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp25=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp26=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp27=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp28=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp29=NEW_UNSIGNED_SCORE_STREAM(16);
    ScOrEtMp30=NEW_SIGNED_SCORE_STREAM(16);
    ScOrEtMp31=NEW_UNSIGNED_SCORE_SEGMENT(89,16);
    ((unsigned long *)ScOrEtMp31->data())[0]=7;
    ((unsigned long *)ScOrEtMp31->data())[1]=8;
    ((unsigned long *)ScOrEtMp31->data())[2]=9;
    ((unsigned long *)ScOrEtMp31->data())[3]=10;
    ((unsigned long *)ScOrEtMp31->data())[4]=11;
    ((unsigned long *)ScOrEtMp31->data())[5]=12;
    ((unsigned long *)ScOrEtMp31->data())[6]=13;
    ((unsigned long *)ScOrEtMp31->data())[7]=14;
    ((unsigned long *)ScOrEtMp31->data())[8]=16;
    ((unsigned long *)ScOrEtMp31->data())[9]=17;
    ((unsigned long *)ScOrEtMp31->data())[10]=19;
    ((unsigned long *)ScOrEtMp31->data())[11]=21;
    ((unsigned long *)ScOrEtMp31->data())[12]=23;
    ((unsigned long *)ScOrEtMp31->data())[13]=25;
    ((unsigned long *)ScOrEtMp31->data())[14]=28;
    ((unsigned long *)ScOrEtMp31->data())[15]=31;
    ((unsigned long *)ScOrEtMp31->data())[16]=34;
    ((unsigned long *)ScOrEtMp31->data())[17]=37;
    ((unsigned long *)ScOrEtMp31->data())[18]=41;
    ((unsigned long *)ScOrEtMp31->data())[19]=45;
    ((unsigned long *)ScOrEtMp31->data())[20]=50;
    ((unsigned long *)ScOrEtMp31->data())[21]=55;
    ((unsigned long *)ScOrEtMp31->data())[22]=60;
    ((unsigned long *)ScOrEtMp31->data())[23]=66;
    ((unsigned long *)ScOrEtMp31->data())[24]=73;
    ((unsigned long *)ScOrEtMp31->data())[25]=80;
    ((unsigned long *)ScOrEtMp31->data())[26]=88;
    ((unsigned long *)ScOrEtMp31->data())[27]=97;
    ((unsigned long *)ScOrEtMp31->data())[28]=107;
    ((unsigned long *)ScOrEtMp31->data())[29]=118;
    ((unsigned long *)ScOrEtMp31->data())[30]=130;
    ((unsigned long *)ScOrEtMp31->data())[31]=143;
    ((unsigned long *)ScOrEtMp31->data())[32]=157;
    ((unsigned long *)ScOrEtMp31->data())[33]=173;
    ((unsigned long *)ScOrEtMp31->data())[34]=190;
    ((unsigned long *)ScOrEtMp31->data())[35]=209;
    ((unsigned long *)ScOrEtMp31->data())[36]=230;
    ((unsigned long *)ScOrEtMp31->data())[37]=253;
    ((unsigned long *)ScOrEtMp31->data())[38]=279;
    ((unsigned long *)ScOrEtMp31->data())[39]=307;
    ((unsigned long *)ScOrEtMp31->data())[40]=337;
    ((unsigned long *)ScOrEtMp31->data())[41]=371;
    ((unsigned long *)ScOrEtMp31->data())[42]=408;
    ((unsigned long *)ScOrEtMp31->data())[43]=449;
    ((unsigned long *)ScOrEtMp31->data())[44]=494;
    ((unsigned long *)ScOrEtMp31->data())[45]=544;
    ((unsigned long *)ScOrEtMp31->data())[46]=598;
    ((unsigned long *)ScOrEtMp31->data())[47]=658;
    ((unsigned long *)ScOrEtMp31->data())[48]=724;
    ((unsigned long *)ScOrEtMp31->data())[49]=796;
    ((unsigned long *)ScOrEtMp31->data())[50]=876;
    ((unsigned long *)ScOrEtMp31->data())[51]=963;
    ((unsigned long *)ScOrEtMp31->data())[52]=1060;
    ((unsigned long *)ScOrEtMp31->data())[53]=1166;
    ((unsigned long *)ScOrEtMp31->data())[54]=1282;
    ((unsigned long *)ScOrEtMp31->data())[55]=1411;
    ((unsigned long *)ScOrEtMp31->data())[56]=1552;
    ((unsigned long *)ScOrEtMp31->data())[57]=1707;
    ((unsigned long *)ScOrEtMp31->data())[58]=1878;
    ((unsigned long *)ScOrEtMp31->data())[59]=2066;
    ((unsigned long *)ScOrEtMp31->data())[60]=2272;
    ((unsigned long *)ScOrEtMp31->data())[61]=2499;
    ((unsigned long *)ScOrEtMp31->data())[62]=2749;
    ((unsigned long *)ScOrEtMp31->data())[63]=3024;
    ((unsigned long *)ScOrEtMp31->data())[64]=3327;
    ((unsigned long *)ScOrEtMp31->data())[65]=3660;
    ((unsigned long *)ScOrEtMp31->data())[66]=4026;
    ((unsigned long *)ScOrEtMp31->data())[67]=4428;
    ((unsigned long *)ScOrEtMp31->data())[68]=4871;
    ((unsigned long *)ScOrEtMp31->data())[69]=5358;
    ((unsigned long *)ScOrEtMp31->data())[70]=5894;
    ((unsigned long *)ScOrEtMp31->data())[71]=6484;
    ((unsigned long *)ScOrEtMp31->data())[72]=7132;
    ((unsigned long *)ScOrEtMp31->data())[73]=7845;
    ((unsigned long *)ScOrEtMp31->data())[74]=8630;
    ((unsigned long *)ScOrEtMp31->data())[75]=9493;
    ((unsigned long *)ScOrEtMp31->data())[76]=10442;
    ((unsigned long *)ScOrEtMp31->data())[77]=11487;
    ((unsigned long *)ScOrEtMp31->data())[78]=12635;
    ((unsigned long *)ScOrEtMp31->data())[79]=13899;
    ((unsigned long *)ScOrEtMp31->data())[80]=15289;
    ((unsigned long *)ScOrEtMp31->data())[81]=16818;
    ((unsigned long *)ScOrEtMp31->data())[82]=18500;
    ((unsigned long *)ScOrEtMp31->data())[83]=20350;
    ((unsigned long *)ScOrEtMp31->data())[84]=22385;
    ((unsigned long *)ScOrEtMp31->data())[85]=24623;
    ((unsigned long *)ScOrEtMp31->data())[86]=27086;
    ((unsigned long *)ScOrEtMp31->data())[87]=29794;
    ((unsigned long *)ScOrEtMp31->data())[88]=32767;
    ScOrEtMp9=NEW_BOOLEAN_SCORE_STREAM();
    // compose body statements 
    ScoreSegment * segment_r_0_inst=new ScoreSegmentReadOnly(16,7,89,ScOrEtMp31,ScOrEtMp19,ScOrEtMp22); /* segment_r */
    segment[0]=segment_r_0_inst;
    segment_group[0]=NO_GROUP;
    ScorePage * copy_unsigned_3_d1_ScOrEtMp32_0_inst=new Page_copy_unsigned_3_d1_ScOrEtMp32(ScOrEtMp22,ScOrEtMp25,ScOrEtMp26,ScOrEtMp27); /* copy_unsigned_3 */
    page[0]=copy_unsigned_3_d1_ScOrEtMp32_0_inst;
    page_group[0]=0;
    ScorePage * DiffSign_d1_ScOrEtMp33_1_inst=new Page_DiffSign_d1_ScOrEtMp33(cc_inAdpcmEnc,ScOrEtMp30,ScOrEtMp15,ScOrEtMp21,ScOrEtMp20); /* DiffSign */
    page[1]=DiffSign_d1_ScOrEtMp33_1_inst;
    page_group[1]=0;
    ScorePage * DivMul2_d1_ScOrEtMp34_2_inst=new Page_DivMul2_d1_ScOrEtMp34(ScOrEtMp15,ScOrEtMp27,ScOrEtMp14,ScOrEtMp13,ScOrEtMp17,ScOrEtMp24); /* DivMul2 */
    page[2]=DivMul2_d1_ScOrEtMp34_2_inst;
    page_group[2]=0;
    ScorePage * DivMul1_d1_ScOrEtMp35_3_inst=new Page_DivMul1_d1_ScOrEtMp35(ScOrEtMp17,ScOrEtMp24,ScOrEtMp26,ScOrEtMp12,ScOrEtMp11,ScOrEtMp16,ScOrEtMp23,ScOrEtMp28); /* DivMul1 */
    page[3]=DivMul1_d1_ScOrEtMp35_3_inst;
    page_group[3]=0;
    ScorePage * DivMul0_d1_ScOrEtMp36_4_inst=new Page_DivMul0_d1_ScOrEtMp36(ScOrEtMp16,ScOrEtMp23,ScOrEtMp25,ScOrEtMp28,ScOrEtMp10,ScOrEtMp9,ScOrEtMp29); /* DivMul0 */
    page[4]=DivMul0_d1_ScOrEtMp36_4_inst;
    page_group[4]=0;
    ScorePage * ValPredUpdate_d1_ScOrEtMp37_5_inst=new Page_ValPredUpdate_d1_ScOrEtMp37(ScOrEtMp29,ScOrEtMp21,ScOrEtMp30); /* ValPredUpdate */
    page[5]=ValPredUpdate_d1_ScOrEtMp37_5_inst;
    page_group[5]=0;
    ScorePage * IndexOffsetTable_d1_ScOrEtMp38_6_inst=new Page_IndexOffsetTable_d1_ScOrEtMp38(ScOrEtMp18,ScOrEtMp13,ScOrEtMp11,ScOrEtMp9,ScOrEtMp18,ScOrEtMp19); /* IndexOffsetTable */
    page[6]=IndexOffsetTable_d1_ScOrEtMp38_6_inst;
    page_group[6]=0;
    ScorePage * MuxNibbles_d1_ScOrEtMp39_7_inst=new Page_MuxNibbles_d1_ScOrEtMp39(ScOrEtMp20,ScOrEtMp14,ScOrEtMp12,ScOrEtMp10,cc_outAdpcmEnc); /* MuxNibbles */
    page[7]=MuxNibbles_d1_ScOrEtMp39_7_inst;
    page_group[7]=0;
    // end body statements 
  }
}; // AdpcmEnc_instance
extern "C" ScoreOperatorInstance *construct(char *argbuf) {
  AdpcmEnc_arg *data;
  data=(AdpcmEnc_arg *)malloc(sizeof(AdpcmEnc_arg));
  memcpy(data,argbuf,sizeof(AdpcmEnc_arg));
  return(new AdpcmEnc_instance(((SIGNED_SCORE_STREAM)STREAM_ID_TO_OBJ(data->i0)),((UNSIGNED_SCORE_STREAM)STREAM_ID_TO_OBJ(data->i1))));
}
