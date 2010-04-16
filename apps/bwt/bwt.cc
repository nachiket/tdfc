// cctdfc autocompiled header file
// tdfc version 1.160
// Fri Apr 16 09:58:58 2010

#include "Score.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "bwt.h"
using namespace std;
char * bwt_name="bwt";
void * bwt_proc_run(void *obj) {
  return(((bwt *)obj)->proc_run()); }
ScoreOperatorElement *bwtinit_instances() {
  return(ScoreOperator::addOperator(bwt_name,0,26,0));  }
ScoreOperatorElement *bwt::instances=bwtinit_instances();

bwt::bwt(UNSIGNED_SCORE_STREAM n_cc_in,UNSIGNED_SCORE_STREAM n_cc_vaddr,UNSIGNED_SCORE_STREAM n_cc_vdatar,UNSIGNED_SCORE_STREAM n_cc_vdataw,BOOLEAN_SCORE_STREAM n_cc_vwrite,UNSIGNED_SCORE_STREAM n_cc_posaddr,SIGNED_SCORE_STREAM n_cc_posdatar,SIGNED_SCORE_STREAM n_cc_posdataw,BOOLEAN_SCORE_STREAM n_cc_poswrite,UNSIGNED_SCORE_STREAM n_cc_prmaddr,SIGNED_SCORE_STREAM n_cc_prmdatar,SIGNED_SCORE_STREAM n_cc_prmdataw,BOOLEAN_SCORE_STREAM n_cc_prmwrite,UNSIGNED_SCORE_STREAM n_cc_countaddr,SIGNED_SCORE_STREAM n_cc_countdatar,SIGNED_SCORE_STREAM n_cc_countdataw,BOOLEAN_SCORE_STREAM n_cc_countwrite,UNSIGNED_SCORE_STREAM n_cc_bhaddr,BOOLEAN_SCORE_STREAM n_cc_bhdatar,BOOLEAN_SCORE_STREAM n_cc_bhdataw,BOOLEAN_SCORE_STREAM n_cc_bhwrite,UNSIGNED_SCORE_STREAM n_cc_b2haddr,BOOLEAN_SCORE_STREAM n_cc_b2hdatar,BOOLEAN_SCORE_STREAM n_cc_b2hdataw,BOOLEAN_SCORE_STREAM n_cc_b2hwrite,UNSIGNED_SCORE_STREAM n_cc_out)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(bwt_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    bwt_arg *data;
    struct msgbuf *msgp;
    data=(bwt_arg *)malloc(sizeof(bwt_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_in);
    data->i1=STREAM_OBJ_TO_ID(n_cc_vaddr);
    data->i2=STREAM_OBJ_TO_ID(n_cc_vdatar);
    data->i3=STREAM_OBJ_TO_ID(n_cc_vdataw);
    data->i4=STREAM_OBJ_TO_ID(n_cc_vwrite);
    data->i5=STREAM_OBJ_TO_ID(n_cc_posaddr);
    data->i6=STREAM_OBJ_TO_ID(n_cc_posdatar);
    data->i7=STREAM_OBJ_TO_ID(n_cc_posdataw);
    data->i8=STREAM_OBJ_TO_ID(n_cc_poswrite);
    data->i9=STREAM_OBJ_TO_ID(n_cc_prmaddr);
    data->i10=STREAM_OBJ_TO_ID(n_cc_prmdatar);
    data->i11=STREAM_OBJ_TO_ID(n_cc_prmdataw);
    data->i12=STREAM_OBJ_TO_ID(n_cc_prmwrite);
    data->i13=STREAM_OBJ_TO_ID(n_cc_countaddr);
    data->i14=STREAM_OBJ_TO_ID(n_cc_countdatar);
    data->i15=STREAM_OBJ_TO_ID(n_cc_countdataw);
    data->i16=STREAM_OBJ_TO_ID(n_cc_countwrite);
    data->i17=STREAM_OBJ_TO_ID(n_cc_bhaddr);
    data->i18=STREAM_OBJ_TO_ID(n_cc_bhdatar);
    data->i19=STREAM_OBJ_TO_ID(n_cc_bhdataw);
    data->i20=STREAM_OBJ_TO_ID(n_cc_bhwrite);
    data->i21=STREAM_OBJ_TO_ID(n_cc_b2haddr);
    data->i22=STREAM_OBJ_TO_ID(n_cc_b2hdatar);
    data->i23=STREAM_OBJ_TO_ID(n_cc_b2hdataw);
    data->i24=STREAM_OBJ_TO_ID(n_cc_b2hwrite);
    data->i25=STREAM_OBJ_TO_ID(n_cc_out);
    alen=sizeof(bwt_arg);
    slen=strlen(instance_fn);
    blen=sizeof(long)+sizeof(long)+slen+alen;
    msgp=(struct msgbuf *)malloc(sizeof(msgbuf)+sizeof(char)*(blen-1));
    int sid=schedulerId();
    memcpy(msgp->mtext,&alen,sizeof(long));
    memcpy(msgp->mtext+sizeof(long),&slen,sizeof(long));
    memcpy(msgp->mtext+sizeof(long)*2,instance_fn,slen);
    memcpy(msgp->mtext+sizeof(long)*2+slen,data,alen);
    msgp->mtype=SCORE_INSTANTIATE_MESSAGE_TYPE;
    int res=msgsnd(sid, msgp, blen, 0) ;  
    if (res==-1) {
       cerr <<"bwt msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(7,19, bwt_name);
  addInstance(instances,this,params);
    bindInput(0,n_cc_in,new ScoreStreamType(0,9));
    in[0]->setName("cc_in");
    SCORE_MARKREADSTREAM(n_cc_in,globalCounter->threadCounter);
    bindOutput(0,n_cc_vaddr,new ScoreStreamType(0,32));
    out[0]->setName("cc_vaddr");
    SCORE_MARKWRITESTREAM(n_cc_vaddr,globalCounter->threadCounter);
    bindInput(1,n_cc_vdatar,new ScoreStreamType(0,9));
    in[1]->setName("cc_vdatar");
    SCORE_MARKREADSTREAM(n_cc_vdatar,globalCounter->threadCounter);
    bindOutput(1,n_cc_vdataw,new ScoreStreamType(0,9));
    out[1]->setName("cc_vdataw");
    SCORE_MARKWRITESTREAM(n_cc_vdataw,globalCounter->threadCounter);
    bindOutput(2,n_cc_vwrite,new ScoreStreamType(0,1));
    out[2]->setName("cc_vwrite");
    SCORE_MARKWRITESTREAM(n_cc_vwrite,globalCounter->threadCounter);
    bindOutput(3,n_cc_posaddr,new ScoreStreamType(0,32));
    out[3]->setName("cc_posaddr");
    SCORE_MARKWRITESTREAM(n_cc_posaddr,globalCounter->threadCounter);
    bindInput(2,n_cc_posdatar,new ScoreStreamType(1,32));
    in[2]->setName("cc_posdatar");
    SCORE_MARKREADSTREAM(n_cc_posdatar,globalCounter->threadCounter);
    bindOutput(4,n_cc_posdataw,new ScoreStreamType(1,32));
    out[4]->setName("cc_posdataw");
    SCORE_MARKWRITESTREAM(n_cc_posdataw,globalCounter->threadCounter);
    bindOutput(5,n_cc_poswrite,new ScoreStreamType(0,1));
    out[5]->setName("cc_poswrite");
    SCORE_MARKWRITESTREAM(n_cc_poswrite,globalCounter->threadCounter);
    bindOutput(6,n_cc_prmaddr,new ScoreStreamType(0,32));
    out[6]->setName("cc_prmaddr");
    SCORE_MARKWRITESTREAM(n_cc_prmaddr,globalCounter->threadCounter);
    bindInput(3,n_cc_prmdatar,new ScoreStreamType(1,32));
    in[3]->setName("cc_prmdatar");
    SCORE_MARKREADSTREAM(n_cc_prmdatar,globalCounter->threadCounter);
    bindOutput(7,n_cc_prmdataw,new ScoreStreamType(1,32));
    out[7]->setName("cc_prmdataw");
    SCORE_MARKWRITESTREAM(n_cc_prmdataw,globalCounter->threadCounter);
    bindOutput(8,n_cc_prmwrite,new ScoreStreamType(0,1));
    out[8]->setName("cc_prmwrite");
    SCORE_MARKWRITESTREAM(n_cc_prmwrite,globalCounter->threadCounter);
    bindOutput(9,n_cc_countaddr,new ScoreStreamType(0,32));
    out[9]->setName("cc_countaddr");
    SCORE_MARKWRITESTREAM(n_cc_countaddr,globalCounter->threadCounter);
    bindInput(4,n_cc_countdatar,new ScoreStreamType(1,32));
    in[4]->setName("cc_countdatar");
    SCORE_MARKREADSTREAM(n_cc_countdatar,globalCounter->threadCounter);
    bindOutput(10,n_cc_countdataw,new ScoreStreamType(1,32));
    out[10]->setName("cc_countdataw");
    SCORE_MARKWRITESTREAM(n_cc_countdataw,globalCounter->threadCounter);
    bindOutput(11,n_cc_countwrite,new ScoreStreamType(0,1));
    out[11]->setName("cc_countwrite");
    SCORE_MARKWRITESTREAM(n_cc_countwrite,globalCounter->threadCounter);
    bindOutput(12,n_cc_bhaddr,new ScoreStreamType(0,32));
    out[12]->setName("cc_bhaddr");
    SCORE_MARKWRITESTREAM(n_cc_bhaddr,globalCounter->threadCounter);
    bindInput(5,n_cc_bhdatar,new ScoreStreamType(0,1));
    in[5]->setName("cc_bhdatar");
    SCORE_MARKREADSTREAM(n_cc_bhdatar,globalCounter->threadCounter);
    bindOutput(13,n_cc_bhdataw,new ScoreStreamType(0,1));
    out[13]->setName("cc_bhdataw");
    SCORE_MARKWRITESTREAM(n_cc_bhdataw,globalCounter->threadCounter);
    bindOutput(14,n_cc_bhwrite,new ScoreStreamType(0,1));
    out[14]->setName("cc_bhwrite");
    SCORE_MARKWRITESTREAM(n_cc_bhwrite,globalCounter->threadCounter);
    bindOutput(15,n_cc_b2haddr,new ScoreStreamType(0,32));
    out[15]->setName("cc_b2haddr");
    SCORE_MARKWRITESTREAM(n_cc_b2haddr,globalCounter->threadCounter);
    bindInput(6,n_cc_b2hdatar,new ScoreStreamType(0,1));
    in[6]->setName("cc_b2hdatar");
    SCORE_MARKREADSTREAM(n_cc_b2hdatar,globalCounter->threadCounter);
    bindOutput(16,n_cc_b2hdataw,new ScoreStreamType(0,1));
    out[16]->setName("cc_b2hdataw");
    SCORE_MARKWRITESTREAM(n_cc_b2hdataw,globalCounter->threadCounter);
    bindOutput(17,n_cc_b2hwrite,new ScoreStreamType(0,1));
    out[17]->setName("cc_b2hwrite");
    SCORE_MARKWRITESTREAM(n_cc_b2hwrite,globalCounter->threadCounter);
    bindOutput(18,n_cc_out,new ScoreStreamType(0,8));
    out[18]->setName("cc_out");
    SCORE_MARKWRITESTREAM(n_cc_out,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&bwt_proc_run, this);
  }
}

void *bwt::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  enum state_syms {STATE_alpha,STATE_b2hcaw,STATE_beta,STATE_bhg,STATE_bhi,STATE_caw,STATE_cloop,STATE_cluckloop,STATE_countbark,STATE_counthiss,STATE_endgloop,STATE_endqloop,STATE_endwoofloop,STATE_floop,STATE_gloop,STATE_hloop,STATE_iloop,STATE_kiloop,STATE_kloop,STATE_mooloop,STATE_mootest,STATE_oinkloop,STATE_posa,STATE_posc,STATE_poscluck,STATE_posk,STATE_pospurr,STATE_posq,STATE_prmki,STATE_prmoink,STATE_prmroar,STATE_prmt,STATE_purrloop,STATE_qloop,STATE_weasel,STATE_weaseltest,STATE_woofloop,STATE_woofy,STATE_yowsa};
  state_syms state=STATE_alpha;
  unsigned long cc_a;
  unsigned long cc_b=0;
  unsigned long cc_c;
  unsigned long cc_f;
  unsigned long cc_g;
  unsigned long cc_h=1;
  unsigned long cc_i=0;
  unsigned long cc_j;
  unsigned long cc_k=0;
  unsigned long cc_n=0;
  unsigned long cc_q;
  unsigned long cc_r;
  unsigned long cc_t;
  long long cc_y=0;
  long long cc_ki;
  long long cc_kj;
  long long cc_posc;
  long long cc_posk;
  long long cc_posq;
  long long cc_foozle;
  long long cc_nyar;
  unsigned long cc_bark;
  unsigned long cc_oink=0;
  unsigned long cc_cluck=0;
  unsigned long cc_purr=0;
  unsigned long cc_moo=0;
  unsigned long cc_meow=0;
  unsigned long cc_woof=0;
  unsigned long cc_roar;
  unsigned long cc_hiss;
  long long cc_boo;
  long long cc_splat;
  unsigned long cc_quack;
  long long cc_caw;
  long long cc_poscluck;
  unsigned long cc_sentinelindex;
  long long cc_pospurr;
  unsigned long cc_in;
  int retime_length_0=0;
  unsigned long *cc_in_retime=new unsigned long [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_in_retime[j]=0;
  unsigned long cc_vdatar;
  int retime_length_1=0;
  unsigned long *cc_vdatar_retime=new unsigned long [retime_length_1+1];
  for (int j=retime_length_1;j>=0;j--)
    cc_vdatar_retime[j]=0;
  long long cc_posdatar;
  int retime_length_2=0;
  long long *cc_posdatar_retime=new long long [retime_length_2+1];
  for (int j=retime_length_2;j>=0;j--)
    cc_posdatar_retime[j]=0;
  long long cc_prmdatar;
  int retime_length_3=0;
  long long *cc_prmdatar_retime=new long long [retime_length_3+1];
  for (int j=retime_length_3;j>=0;j--)
    cc_prmdatar_retime[j]=0;
  long long cc_countdatar;
  int retime_length_4=0;
  long long *cc_countdatar_retime=new long long [retime_length_4+1];
  for (int j=retime_length_4;j>=0;j--)
    cc_countdatar_retime[j]=0;
  int cc_bhdatar;
  int retime_length_5=0;
  int *cc_bhdatar_retime=new int [retime_length_5+1];
  for (int j=retime_length_5;j>=0;j--)
    cc_bhdatar_retime[j]=0;
  int cc_b2hdatar;
  int retime_length_6=0;
  int *cc_b2hdatar_retime=new int [retime_length_6+1];
  for (int j=retime_length_6;j>=0;j--)
    cc_b2hdatar_retime[j]=0;
  int *input_free=new int[7];
  for (int i=0;i<7;i++)
    input_free[i]=0;
  int *output_close=new int[19];
  for (int i=0;i<19;i++)
    output_close[i]=0;
  int done=0;
  while (!done) {
    switch(state) {
      case STATE_alpha: { 
        {
        if (1) {
          {
            if ((cc_b<257)) {
              {
                STREAM_WRITE_NOACC(out[3],cc_b);
                STREAM_WRITE_NOACC(out[4],-1);
                STREAM_WRITE_NOACC(out[5],true);
                cc_b=(cc_b+1);
              }
            }
            else {
              {
                state=STATE_beta;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_alpha
      case STATE_b2hcaw: { 
        {
        int eos_0=STREAM_EOS(in[6]);
        int eofr_0=STREAM_EOFR(in[6]);
        if (1&&!eos_0&&!eofr_0) {
          cc_b2hdatar=STREAM_READ_NOACC(in[6]);
          for (int j=retime_length_6;j>0;j--)
            cc_b2hdatar_retime[j]=cc_b2hdatar_retime[j-1];
          cc_b2hdatar_retime[0]=cc_b2hdatar;
          {
            if (cc_b2hdatar_retime[0]) {
              {
                cc_j=((unsigned long long)((cc_caw+1)));
                state=STATE_weasel;
              }
            }
            else {
              {
                cc_c=(cc_c+1);
                state=STATE_cloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_b2hcaw
      case STATE_beta: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        if (1&&eos_0) {
          if (!input_free[0])
          STREAM_FREE(in[0]);
          input_free[0]=1;
          {
            state=STATE_kloop;
          }
        }
        else
        {
        if (1&&!eos_0&&!eofr_0) {
          cc_in=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_in_retime[j]=cc_in_retime[j-1];
          cc_in_retime[0]=cc_in;
          {
            cc_a=cc_in_retime[0];
            STREAM_WRITE_NOACC(out[0],cc_n);
            STREAM_WRITE_NOACC(out[1],cc_a);
            STREAM_WRITE_NOACC(out[2],true);
            STREAM_WRITE_NOACC(out[3],cc_a);
            STREAM_WRITE_NOACC(out[5],false);
            state=STATE_posa;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }}        break; 
      } // end case STATE_beta
      case STATE_bhg: { 
        {
        int eos_0=STREAM_EOS(in[5]);
        int eofr_0=STREAM_EOFR(in[5]);
        if (1&&!eos_0&&!eofr_0) {
          cc_bhdatar=STREAM_READ_NOACC(in[5]);
          for (int j=retime_length_5;j>0;j--)
            cc_bhdatar_retime[j]=cc_bhdatar_retime[j-1];
          cc_bhdatar_retime[0]=cc_bhdatar;
          {
            if (cc_bhdatar_retime[0]) {
              {
                cc_r=(cc_g-1);
                state=STATE_endgloop;
              }
            }
            else {
              {
                cc_g=(cc_g+1);
                state=STATE_gloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_bhg
      case STATE_bhi: { 
        {
        int eos_0=STREAM_EOS(in[5]);
        int eofr_0=STREAM_EOFR(in[5]);
        if (1&&!eos_0&&!eofr_0) {
          cc_bhdatar=STREAM_READ_NOACC(in[5]);
          for (int j=retime_length_5;j>0;j--)
            cc_bhdatar_retime[j]=cc_bhdatar_retime[j-1];
          cc_bhdatar_retime[0]=cc_bhdatar;
          {
            if (cc_bhdatar_retime[0]) {
              {
                cc_r=cc_i;
                cc_g=(cc_i+1);
                state=STATE_gloop;
              }
            }
            else {
              {
                cc_i=(cc_i+1);
                state=STATE_iloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_bhi
      case STATE_caw: { 
        {
        int eos_0=STREAM_EOS(in[3]);
        int eofr_0=STREAM_EOFR(in[3]);
        if (1&&!eos_0&&!eofr_0) {
          cc_prmdatar=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_prmdatar_retime[j]=cc_prmdatar_retime[j-1];
          cc_prmdatar_retime[0]=cc_prmdatar;
          {
            cc_caw=cc_prmdatar_retime[0];
            STREAM_WRITE_NOACC(out[15],((unsigned long)(cc_caw)));
            STREAM_WRITE_NOACC(out[17],false);
            state=STATE_b2hcaw;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_caw
      case STATE_cloop: { 
        {
        if (1) {
          {
            if ((cc_c<=cc_r)) {
              {
                STREAM_WRITE_NOACC(out[3],cc_c);
                STREAM_WRITE_NOACC(out[5],false);
                state=STATE_posc;
              }
            }
            else {
              {
                cc_i=(cc_i+1);
                state=STATE_iloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_cloop
      case STATE_cluckloop: { 
        {
        if (1) {
          {
            if ((cc_cluck<cc_n)) {
              {
                STREAM_WRITE_NOACC(out[3],cc_cluck);
                STREAM_WRITE_NOACC(out[5],false);
                state=STATE_poscluck;
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[18],(cc_sentinelindex>>24));
                STREAM_WRITE_NOACC(out[18],((cc_sentinelindex>>16)&255));
                STREAM_WRITE_NOACC(out[18],((cc_sentinelindex>>8)&255));
                STREAM_WRITE_NOACC(out[18],(cc_sentinelindex&255));
                state=STATE_purrloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_cluckloop
      case STATE_countbark: { 
        {
        int eos_0=STREAM_EOS(in[4]);
        int eofr_0=STREAM_EOFR(in[4]);
        if (1&&!eos_0&&!eofr_0) {
          cc_countdatar=STREAM_READ_NOACC(in[4]);
          for (int j=retime_length_4;j>0;j--)
            cc_countdatar_retime[j]=cc_countdatar_retime[j-1];
          cc_countdatar_retime[0]=cc_countdatar;
          {
            cc_boo=cc_countdatar_retime[0];
            cc_splat=(cc_bark+cc_boo);
            STREAM_WRITE_NOACC(out[9],cc_bark);
            STREAM_WRITE_NOACC(out[10],(cc_boo+1));
            STREAM_WRITE_NOACC(out[11],true);
            STREAM_WRITE_NOACC(out[6],cc_t);
            STREAM_WRITE_NOACC(out[7],cc_splat);
            STREAM_WRITE_NOACC(out[8],true);
            STREAM_WRITE_NOACC(out[15],((unsigned long)(cc_splat)));
            STREAM_WRITE_NOACC(out[16],true);
            STREAM_WRITE_NOACC(out[17],true);
            cc_q=(cc_q+1);
            state=STATE_qloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_countbark
      case STATE_counthiss: { 
        {
        int eos_0=STREAM_EOS(in[4]);
        int eofr_0=STREAM_EOFR(in[4]);
        if (1&&!eos_0&&!eofr_0) {
          cc_countdatar=STREAM_READ_NOACC(in[4]);
          for (int j=retime_length_4;j>0;j--)
            cc_countdatar_retime[j]=cc_countdatar_retime[j-1];
          cc_countdatar_retime[0]=cc_countdatar;
          {
            cc_nyar=cc_countdatar_retime[0];
            STREAM_WRITE_NOACC(out[9],cc_hiss);
            STREAM_WRITE_NOACC(out[10],(cc_nyar+1));
            STREAM_WRITE_NOACC(out[11],true);
            cc_foozle=(cc_hiss+cc_nyar);
            STREAM_WRITE_NOACC(out[6],cc_roar);
            STREAM_WRITE_NOACC(out[7],cc_foozle);
            STREAM_WRITE_NOACC(out[8],true);
            STREAM_WRITE_NOACC(out[15],((unsigned long)(cc_foozle)));
            STREAM_WRITE_NOACC(out[16],true);
            STREAM_WRITE_NOACC(out[17],true);
            state=STATE_iloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_counthiss
      case STATE_endgloop: { 
        {
        if (1) {
          {
            cc_q=cc_i;
            state=STATE_qloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_endgloop
      case STATE_endqloop: { 
        {
        if (1) {
          {
            cc_c=cc_i;
            state=STATE_cloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_endqloop
      case STATE_endwoofloop: { 
        {
        if (1) {
          {
            cc_roar=(cc_n-cc_h);
            STREAM_WRITE_NOACC(out[6],cc_roar);
            STREAM_WRITE_NOACC(out[8],false);
            state=STATE_prmroar;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_endwoofloop
      case STATE_floop: { 
        {
        if (1) {
          {
            if ((cc_f<cc_quack)) {
              {
                STREAM_WRITE_NOACC(out[15],cc_f);
                STREAM_WRITE_NOACC(out[16],false);
                STREAM_WRITE_NOACC(out[17],true);
                cc_f=(cc_f+1);
              }
            }
            else {
              {
                cc_c=(cc_c+1);
                state=STATE_cloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_floop
      case STATE_gloop: { 
        {
        if (1) {
          {
            if ((cc_g<(cc_n+1))) {
              {
                STREAM_WRITE_NOACC(out[12],cc_g);
                STREAM_WRITE_NOACC(out[14],false);
                state=STATE_bhg;
              }
            }
            else {
              {
                state=STATE_endgloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_gloop
      case STATE_hloop: { 
        {
        if (1) {
          {
            if ((cc_h<cc_n)) {
              {
                state=STATE_woofloop;
              }
            }
            else {
              {
                state=STATE_cluckloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_hloop
      case STATE_iloop: { 
        {
        if (1) {
          {
            if ((cc_i<cc_n)) {
              {
                STREAM_WRITE_NOACC(out[12],cc_i);
                STREAM_WRITE_NOACC(out[14],false);
                state=STATE_bhi;
              }
            }
            else {
              {
                state=STATE_mooloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_iloop
      case STATE_kiloop: { 
        {
        if (1) {
          {
            if ((cc_ki!=(-(1)))) {
              {
                STREAM_WRITE_NOACC(out[6],((unsigned long)(cc_ki)));
                STREAM_WRITE_NOACC(out[8],false);
                state=STATE_prmki;
              }
            }
            else {
              {
                cc_k=(cc_k+1);
                state=STATE_kloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_kiloop
      case STATE_kloop: { 
        {
        if (1) {
          {
            if ((cc_k<257)) {
              {
                STREAM_WRITE_NOACC(out[3],cc_k);
                STREAM_WRITE_NOACC(out[5],false);
                state=STATE_posk;
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[12],cc_n);
                STREAM_WRITE_NOACC(out[13],true);
                STREAM_WRITE_NOACC(out[14],true);
                state=STATE_oinkloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_kloop
      case STATE_mooloop: { 
        {
        if (1) {
          {
            if ((cc_moo<cc_n)) {
              {
                STREAM_WRITE_NOACC(out[6],cc_moo);
                STREAM_WRITE_NOACC(out[8],false);
                STREAM_WRITE_NOACC(out[15],cc_moo);
                STREAM_WRITE_NOACC(out[17],false);
                state=STATE_mootest;
              }
            }
            else {
              {
                cc_h=(cc_h*2);
                state=STATE_hloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_mooloop
      case STATE_mootest: { 
        {
        int eos_0=STREAM_EOS(in[3]);
        int eofr_0=STREAM_EOFR(in[3]);
        int eos_1=STREAM_EOS(in[6]);
        int eofr_1=STREAM_EOFR(in[6]);
        if (1&&!eos_0&&!eofr_0&&!eos_1&&!eofr_1) {
          cc_prmdatar=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_prmdatar_retime[j]=cc_prmdatar_retime[j-1];
          cc_prmdatar_retime[0]=cc_prmdatar;
          cc_b2hdatar=STREAM_READ_NOACC(in[6]);
          for (int j=retime_length_6;j>0;j--)
            cc_b2hdatar_retime[j]=cc_b2hdatar_retime[j-1];
          cc_b2hdatar_retime[0]=cc_b2hdatar;
          {
            STREAM_WRITE_NOACC(out[3],((unsigned long)(cc_prmdatar_retime[0])));
            STREAM_WRITE_NOACC(out[4],((long long)(cc_moo)));
            STREAM_WRITE_NOACC(out[5],true);
            if (cc_b2hdatar_retime[0]) {
              {
                STREAM_WRITE_NOACC(out[12],cc_moo);
                STREAM_WRITE_NOACC(out[13],true);
                STREAM_WRITE_NOACC(out[14],true);
              }
            }
            cc_moo=(cc_moo+1);
            state=STATE_mooloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_mootest
      case STATE_oinkloop: { 
        {
        if (1) {
          {
            if ((cc_oink<cc_n)) {
              {
                STREAM_WRITE_NOACC(out[6],cc_oink);
                STREAM_WRITE_NOACC(out[8],false);
                state=STATE_prmoink;
              }
            }
            else {
              {
                state=STATE_hloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_oinkloop
      case STATE_posa: { 
        {
        int eos_0=STREAM_EOS(in[2]);
        int eofr_0=STREAM_EOFR(in[2]);
        if (1&&!eos_0&&!eofr_0) {
          cc_posdatar=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_posdatar_retime[j]=cc_posdatar_retime[j-1];
          cc_posdatar_retime[0]=cc_posdatar;
          {
            STREAM_WRITE_NOACC(out[6],cc_n);
            STREAM_WRITE_NOACC(out[7],cc_posdatar_retime[0]);
            STREAM_WRITE_NOACC(out[8],true);
            STREAM_WRITE_NOACC(out[3],cc_a);
            STREAM_WRITE_NOACC(out[4],((long long)(cc_n)));
            STREAM_WRITE_NOACC(out[5],true);
            cc_n=(cc_n+1);
            state=STATE_beta;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_posa
      case STATE_posc: { 
        {
        int eos_0=STREAM_EOS(in[2]);
        int eofr_0=STREAM_EOFR(in[2]);
        if (1&&!eos_0&&!eofr_0) {
          cc_posdatar=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_posdatar_retime[j]=cc_posdatar_retime[j-1];
          cc_posdatar_retime[0]=cc_posdatar;
          {
            cc_posc=cc_posdatar_retime[0];
            if (((cc_posc>=cc_h)&&(cc_posc<=((cc_n+cc_h)-1)))) {
              {
                STREAM_WRITE_NOACC(out[6],((unsigned long long)((cc_posc-cc_h))));
                STREAM_WRITE_NOACC(out[8],false);
                state=STATE_caw;
              }
            }
            else {
              {
                cc_c=(cc_c+1);
                state=STATE_cloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_posc
      case STATE_poscluck: { 
        {
        int eos_0=STREAM_EOS(in[2]);
        int eofr_0=STREAM_EOFR(in[2]);
        if (1&&!eos_0&&!eofr_0) {
          cc_posdatar=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_posdatar_retime[j]=cc_posdatar_retime[j-1];
          cc_posdatar_retime[0]=cc_posdatar;
          {
            cc_poscluck=cc_posdatar_retime[0];
            if ((cc_poscluck==0)) {
              {
                cc_sentinelindex=cc_cluck;
              }
            }
            else {
              if ((cc_poscluck==1)) {
                {
                  STREAM_WRITE_NOACC(out[18],(cc_cluck>>24));
                  STREAM_WRITE_NOACC(out[18],((cc_cluck>>16)&255));
                  STREAM_WRITE_NOACC(out[18],((cc_cluck>>8)&255));
                  STREAM_WRITE_NOACC(out[18],(cc_cluck&255));
                }
              }
            }
            cc_cluck=(cc_cluck+1);
            state=STATE_cluckloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_poscluck
      case STATE_posk: { 
        {
        int eos_0=STREAM_EOS(in[2]);
        int eofr_0=STREAM_EOFR(in[2]);
        if (1&&!eos_0&&!eofr_0) {
          cc_posdatar=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_posdatar_retime[j]=cc_posdatar_retime[j-1];
          cc_posdatar_retime[0]=cc_posdatar;
          {
            cc_posk=cc_posdatar_retime[0];
            cc_ki=cc_posk;
            state=STATE_kiloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_posk
      case STATE_pospurr: { 
        {
        int eos_0=STREAM_EOS(in[2]);
        int eofr_0=STREAM_EOFR(in[2]);
        if (1&&!eos_0&&!eofr_0) {
          cc_posdatar=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_posdatar_retime[j]=cc_posdatar_retime[j-1];
          cc_posdatar_retime[0]=cc_posdatar;
          {
            cc_pospurr=cc_posdatar_retime[0];
            if ((cc_pospurr==0)) {
              {
                STREAM_WRITE_NOACC(out[18],0);
                cc_purr=(cc_purr+1);
                state=STATE_purrloop;
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[0],((unsigned long long)((cc_pospurr-1))));
                STREAM_WRITE_NOACC(out[2],false);
                state=STATE_yowsa;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_pospurr
      case STATE_posq: { 
        {
        int eos_0=STREAM_EOS(in[2]);
        int eofr_0=STREAM_EOFR(in[2]);
        if (1&&!eos_0&&!eofr_0) {
          cc_posdatar=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_posdatar_retime[j]=cc_posdatar_retime[j-1];
          cc_posdatar_retime[0]=cc_posdatar;
          {
            cc_posq=cc_posdatar_retime[0];
            if (((cc_posq>=cc_h)&&(cc_posq<=((cc_n+cc_h)-1)))) {
              {
                cc_t=((unsigned long long)((cc_posq-cc_h)));
                STREAM_WRITE_NOACC(out[6],cc_t);
                STREAM_WRITE_NOACC(out[8],false);
                state=STATE_prmt;
              }
            }
            else {
              {
                cc_q=(cc_q+1);
                state=STATE_qloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_posq
      case STATE_prmki: { 
        {
        int eos_0=STREAM_EOS(in[3]);
        int eofr_0=STREAM_EOFR(in[3]);
        if (1&&!eos_0&&!eofr_0) {
          cc_prmdatar=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_prmdatar_retime[j]=cc_prmdatar_retime[j-1];
          cc_prmdatar_retime[0]=cc_prmdatar;
          {
            cc_kj=cc_prmdatar_retime[0];
            STREAM_WRITE_NOACC(out[6],((unsigned long)(cc_ki)));
            STREAM_WRITE_NOACC(out[7],cc_y);
            STREAM_WRITE_NOACC(out[8],true);
            STREAM_WRITE_NOACC(out[12],((unsigned long)(cc_y)));
            STREAM_WRITE_NOACC(out[13],(cc_ki==cc_posk));
            STREAM_WRITE_NOACC(out[14],true);
            cc_y=(cc_y+1);
            cc_ki=cc_kj;
            state=STATE_kiloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_prmki
      case STATE_prmoink: { 
        {
        int eos_0=STREAM_EOS(in[3]);
        int eofr_0=STREAM_EOFR(in[3]);
        if (1&&!eos_0&&!eofr_0) {
          cc_prmdatar=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_prmdatar_retime[j]=cc_prmdatar_retime[j-1];
          cc_prmdatar_retime[0]=cc_prmdatar;
          {
            STREAM_WRITE_NOACC(out[3],((unsigned long)(cc_prmdatar_retime[0])));
            STREAM_WRITE_NOACC(out[4],((long long)(cc_oink)));
            STREAM_WRITE_NOACC(out[5],true);
            cc_oink=(cc_oink+1);
            state=STATE_oinkloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_prmoink
      case STATE_prmroar: { 
        {
        int eos_0=STREAM_EOS(in[3]);
        int eofr_0=STREAM_EOFR(in[3]);
        if (1&&!eos_0&&!eofr_0) {
          cc_prmdatar=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_prmdatar_retime[j]=cc_prmdatar_retime[j-1];
          cc_prmdatar_retime[0]=cc_prmdatar;
          {
            cc_hiss=((unsigned long)(cc_prmdatar_retime[0]));
            STREAM_WRITE_NOACC(out[9],cc_hiss);
            STREAM_WRITE_NOACC(out[11],false);
            state=STATE_counthiss;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_prmroar
      case STATE_prmt: { 
        {
        int eos_0=STREAM_EOS(in[3]);
        int eofr_0=STREAM_EOFR(in[3]);
        if (1&&!eos_0&&!eofr_0) {
          cc_prmdatar=STREAM_READ_NOACC(in[3]);
          for (int j=retime_length_3;j>0;j--)
            cc_prmdatar_retime[j]=cc_prmdatar_retime[j-1];
          cc_prmdatar_retime[0]=cc_prmdatar;
          {
            cc_bark=((unsigned long)(cc_prmdatar_retime[0]));
            STREAM_WRITE_NOACC(out[9],cc_bark);
            STREAM_WRITE_NOACC(out[11],false);
            state=STATE_countbark;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_prmt
      case STATE_purrloop: { 
        {
        if (1) {
          {
            if ((cc_purr<cc_n)) {
              {
                STREAM_WRITE_NOACC(out[3],cc_purr);
                STREAM_WRITE_NOACC(out[5],false);
                state=STATE_pospurr;
              }
            }
            else {
              {
                done=1;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_purrloop
      case STATE_qloop: { 
        {
        if (1) {
          {
            if ((cc_q<=cc_r)) {
              {
                STREAM_WRITE_NOACC(out[3],cc_q);
                STREAM_WRITE_NOACC(out[5],false);
                state=STATE_posq;
              }
            }
            else {
              {
                state=STATE_endqloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_qloop
      case STATE_weasel: { 
        {
        if (1) {
          {
            STREAM_WRITE_NOACC(out[12],cc_j);
            STREAM_WRITE_NOACC(out[14],false);
            STREAM_WRITE_NOACC(out[15],cc_j);
            STREAM_WRITE_NOACC(out[17],false);
            state=STATE_weaseltest;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_weasel
      case STATE_weaseltest: { 
        {
        int eos_0=STREAM_EOS(in[5]);
        int eofr_0=STREAM_EOFR(in[5]);
        int eos_1=STREAM_EOS(in[6]);
        int eofr_1=STREAM_EOFR(in[6]);
        if (1&&!eos_0&&!eofr_0&&!eos_1&&!eofr_1) {
          cc_bhdatar=STREAM_READ_NOACC(in[5]);
          for (int j=retime_length_5;j>0;j--)
            cc_bhdatar_retime[j]=cc_bhdatar_retime[j-1];
          cc_bhdatar_retime[0]=cc_bhdatar;
          cc_b2hdatar=STREAM_READ_NOACC(in[6]);
          for (int j=retime_length_6;j>0;j--)
            cc_b2hdatar_retime[j]=cc_b2hdatar_retime[j-1];
          cc_b2hdatar_retime[0]=cc_b2hdatar;
          {
            if ((cc_bhdatar_retime[0]||(!(cc_b2hdatar_retime[0])))) {
              {
                cc_quack=cc_j;
                cc_f=((unsigned long long)((cc_caw+1)));
                state=STATE_floop;
              }
            }
            else {
              {
                cc_j=(cc_j+1);
                state=STATE_weasel;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_weaseltest
      case STATE_woofloop: { 
        {
        if (1) {
          {
            if ((cc_woof<cc_n)) {
              {
                STREAM_WRITE_NOACC(out[12],cc_woof);
                STREAM_WRITE_NOACC(out[14],false);
                STREAM_WRITE_NOACC(out[3],cc_woof);
                STREAM_WRITE_NOACC(out[5],false);
                state=STATE_woofy;
              }
            }
            else {
              {
                state=STATE_endwoofloop;
              }
            }
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_woofloop
      case STATE_woofy: { 
        {
        int eos_1=STREAM_EOS(in[5]);
        int eofr_1=STREAM_EOFR(in[5]);
        int eos_0=STREAM_EOS(in[2]);
        int eofr_0=STREAM_EOFR(in[2]);
        if (1&&!eos_1&&!eofr_1&&!eos_0&&!eofr_0) {
          cc_bhdatar=STREAM_READ_NOACC(in[5]);
          for (int j=retime_length_5;j>0;j--)
            cc_bhdatar_retime[j]=cc_bhdatar_retime[j-1];
          cc_bhdatar_retime[0]=cc_bhdatar;
          cc_posdatar=STREAM_READ_NOACC(in[2]);
          for (int j=retime_length_2;j>0;j--)
            cc_posdatar_retime[j]=cc_posdatar_retime[j-1];
          cc_posdatar_retime[0]=cc_posdatar;
          {
            if (cc_bhdatar_retime[0]) {
              {
                cc_meow=cc_woof;
                STREAM_WRITE_NOACC(out[9],cc_meow);
                STREAM_WRITE_NOACC(out[10],0);
                STREAM_WRITE_NOACC(out[11],true);
              }
            }
            STREAM_WRITE_NOACC(out[6],((unsigned long)(cc_posdatar_retime[0])));
            STREAM_WRITE_NOACC(out[7],((long long)(cc_meow)));
            STREAM_WRITE_NOACC(out[8],true);
            cc_woof=(cc_woof+1);
            state=STATE_woofloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_woofy
      case STATE_yowsa: { 
        {
        int eos_0=STREAM_EOS(in[1]);
        int eofr_0=STREAM_EOFR(in[1]);
        if (1&&!eos_0&&!eofr_0) {
          cc_vdatar=STREAM_READ_NOACC(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_vdatar_retime[j]=cc_vdatar_retime[j-1];
          cc_vdatar_retime[0]=cc_vdatar;
          {
            STREAM_WRITE_NOACC(out[18],cc_vdatar_retime[0]);
            cc_purr=(cc_purr+1);
            state=STATE_purrloop;
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_yowsa
      default: cerr << "ERROR unknown state [" << (int)state << "] encountered in bwt::proc_run" << endl;
        abort();
    }
  }
  if (!input_free[0])
    STREAM_FREE(in[0]);
  STREAM_CLOSE(out[0]);
  STREAM_FREE(in[1]);
  STREAM_CLOSE(out[1]);
  STREAM_CLOSE(out[2]);
  STREAM_CLOSE(out[3]);
  STREAM_FREE(in[2]);
  STREAM_CLOSE(out[4]);
  STREAM_CLOSE(out[5]);
  STREAM_CLOSE(out[6]);
  STREAM_FREE(in[3]);
  STREAM_CLOSE(out[7]);
  STREAM_CLOSE(out[8]);
  STREAM_CLOSE(out[9]);
  STREAM_FREE(in[4]);
  STREAM_CLOSE(out[10]);
  STREAM_CLOSE(out[11]);
  STREAM_CLOSE(out[12]);
  STREAM_FREE(in[5]);
  STREAM_CLOSE(out[13]);
  STREAM_CLOSE(out[14]);
  STREAM_CLOSE(out[15]);
  STREAM_FREE(in[6]);
  STREAM_CLOSE(out[16]);
  STREAM_CLOSE(out[17]);
  STREAM_CLOSE(out[18]);
  return((void*)NULL); }

void bwt::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	if(out[0]->src!=NULL && out[0]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_vaddr\" ]" << endl;
		cout << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_vaddr\" ]" << endl;
	} else {cout<<"cannot connect cc_vaddr "<<endl;}
	if(out[1]->src!=NULL && out[1]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[1]->src->getName() << "->" << out[1]->sink->getName() << "[ label= \" cc_vdataw\" ]" << endl;
		cout << "\t" << out[1]->src->getName() << "->" << out[1]->sink->getName() << "[ label= \" cc_vdataw\" ]" << endl;
	} else {cout<<"cannot connect cc_vdataw "<<endl;}
	if(out[2]->src!=NULL && out[2]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[2]->src->getName() << "->" << out[2]->sink->getName() << "[ label= \" cc_vwrite\" ]" << endl;
		cout << "\t" << out[2]->src->getName() << "->" << out[2]->sink->getName() << "[ label= \" cc_vwrite\" ]" << endl;
	} else {cout<<"cannot connect cc_vwrite "<<endl;}
	if(out[3]->src!=NULL && out[3]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[3]->src->getName() << "->" << out[3]->sink->getName() << "[ label= \" cc_posaddr\" ]" << endl;
		cout << "\t" << out[3]->src->getName() << "->" << out[3]->sink->getName() << "[ label= \" cc_posaddr\" ]" << endl;
	} else {cout<<"cannot connect cc_posaddr "<<endl;}
	if(out[4]->src!=NULL && out[4]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[4]->src->getName() << "->" << out[4]->sink->getName() << "[ label= \" cc_posdataw\" ]" << endl;
		cout << "\t" << out[4]->src->getName() << "->" << out[4]->sink->getName() << "[ label= \" cc_posdataw\" ]" << endl;
	} else {cout<<"cannot connect cc_posdataw "<<endl;}
	if(out[5]->src!=NULL && out[5]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[5]->src->getName() << "->" << out[5]->sink->getName() << "[ label= \" cc_poswrite\" ]" << endl;
		cout << "\t" << out[5]->src->getName() << "->" << out[5]->sink->getName() << "[ label= \" cc_poswrite\" ]" << endl;
	} else {cout<<"cannot connect cc_poswrite "<<endl;}
	if(out[6]->src!=NULL && out[6]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[6]->src->getName() << "->" << out[6]->sink->getName() << "[ label= \" cc_prmaddr\" ]" << endl;
		cout << "\t" << out[6]->src->getName() << "->" << out[6]->sink->getName() << "[ label= \" cc_prmaddr\" ]" << endl;
	} else {cout<<"cannot connect cc_prmaddr "<<endl;}
	if(out[7]->src!=NULL && out[7]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[7]->src->getName() << "->" << out[7]->sink->getName() << "[ label= \" cc_prmdataw\" ]" << endl;
		cout << "\t" << out[7]->src->getName() << "->" << out[7]->sink->getName() << "[ label= \" cc_prmdataw\" ]" << endl;
	} else {cout<<"cannot connect cc_prmdataw "<<endl;}
	if(out[8]->src!=NULL && out[8]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[8]->src->getName() << "->" << out[8]->sink->getName() << "[ label= \" cc_prmwrite\" ]" << endl;
		cout << "\t" << out[8]->src->getName() << "->" << out[8]->sink->getName() << "[ label= \" cc_prmwrite\" ]" << endl;
	} else {cout<<"cannot connect cc_prmwrite "<<endl;}
	if(out[9]->src!=NULL && out[9]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[9]->src->getName() << "->" << out[9]->sink->getName() << "[ label= \" cc_countaddr\" ]" << endl;
		cout << "\t" << out[9]->src->getName() << "->" << out[9]->sink->getName() << "[ label= \" cc_countaddr\" ]" << endl;
	} else {cout<<"cannot connect cc_countaddr "<<endl;}
	if(out[10]->src!=NULL && out[10]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[10]->src->getName() << "->" << out[10]->sink->getName() << "[ label= \" cc_countdataw\" ]" << endl;
		cout << "\t" << out[10]->src->getName() << "->" << out[10]->sink->getName() << "[ label= \" cc_countdataw\" ]" << endl;
	} else {cout<<"cannot connect cc_countdataw "<<endl;}
	if(out[11]->src!=NULL && out[11]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[11]->src->getName() << "->" << out[11]->sink->getName() << "[ label= \" cc_countwrite\" ]" << endl;
		cout << "\t" << out[11]->src->getName() << "->" << out[11]->sink->getName() << "[ label= \" cc_countwrite\" ]" << endl;
	} else {cout<<"cannot connect cc_countwrite "<<endl;}
	if(out[12]->src!=NULL && out[12]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[12]->src->getName() << "->" << out[12]->sink->getName() << "[ label= \" cc_bhaddr\" ]" << endl;
		cout << "\t" << out[12]->src->getName() << "->" << out[12]->sink->getName() << "[ label= \" cc_bhaddr\" ]" << endl;
	} else {cout<<"cannot connect cc_bhaddr "<<endl;}
	if(out[13]->src!=NULL && out[13]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[13]->src->getName() << "->" << out[13]->sink->getName() << "[ label= \" cc_bhdataw\" ]" << endl;
		cout << "\t" << out[13]->src->getName() << "->" << out[13]->sink->getName() << "[ label= \" cc_bhdataw\" ]" << endl;
	} else {cout<<"cannot connect cc_bhdataw "<<endl;}
	if(out[14]->src!=NULL && out[14]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[14]->src->getName() << "->" << out[14]->sink->getName() << "[ label= \" cc_bhwrite\" ]" << endl;
		cout << "\t" << out[14]->src->getName() << "->" << out[14]->sink->getName() << "[ label= \" cc_bhwrite\" ]" << endl;
	} else {cout<<"cannot connect cc_bhwrite "<<endl;}
	if(out[15]->src!=NULL && out[15]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[15]->src->getName() << "->" << out[15]->sink->getName() << "[ label= \" cc_b2haddr\" ]" << endl;
		cout << "\t" << out[15]->src->getName() << "->" << out[15]->sink->getName() << "[ label= \" cc_b2haddr\" ]" << endl;
	} else {cout<<"cannot connect cc_b2haddr "<<endl;}
	if(out[16]->src!=NULL && out[16]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[16]->src->getName() << "->" << out[16]->sink->getName() << "[ label= \" cc_b2hdataw\" ]" << endl;
		cout << "\t" << out[16]->src->getName() << "->" << out[16]->sink->getName() << "[ label= \" cc_b2hdataw\" ]" << endl;
	} else {cout<<"cannot connect cc_b2hdataw "<<endl;}
	if(out[17]->src!=NULL && out[17]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[17]->src->getName() << "->" << out[17]->sink->getName() << "[ label= \" cc_b2hwrite\" ]" << endl;
		cout << "\t" << out[17]->src->getName() << "->" << out[17]->sink->getName() << "[ label= \" cc_b2hwrite\" ]" << endl;
	} else {cout<<"cannot connect cc_b2hwrite "<<endl;}
	if(out[18]->src!=NULL && out[18]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[18]->src->getName() << "->" << out[18]->sink->getName() << "[ label= \" cc_out\" ]" << endl;
		cout << "\t" << out[18]->src->getName() << "->" << out[18]->sink->getName() << "[ label= \" cc_out\" ]" << endl;
	} else {cout<<"cannot connect cc_out "<<endl;}
	funlockfile(stdout);
}

#undef NEW_bwt
extern "C" void *NEW_bwt(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1,UNSIGNED_SCORE_STREAM i2,UNSIGNED_SCORE_STREAM i3,BOOLEAN_SCORE_STREAM i4,UNSIGNED_SCORE_STREAM i5,SIGNED_SCORE_STREAM i6,SIGNED_SCORE_STREAM i7,BOOLEAN_SCORE_STREAM i8,UNSIGNED_SCORE_STREAM i9,SIGNED_SCORE_STREAM i10,SIGNED_SCORE_STREAM i11,BOOLEAN_SCORE_STREAM i12,UNSIGNED_SCORE_STREAM i13,SIGNED_SCORE_STREAM i14,SIGNED_SCORE_STREAM i15,BOOLEAN_SCORE_STREAM i16,UNSIGNED_SCORE_STREAM i17,BOOLEAN_SCORE_STREAM i18,BOOLEAN_SCORE_STREAM i19,BOOLEAN_SCORE_STREAM i20,UNSIGNED_SCORE_STREAM i21,BOOLEAN_SCORE_STREAM i22,BOOLEAN_SCORE_STREAM i23,BOOLEAN_SCORE_STREAM i24,UNSIGNED_SCORE_STREAM i25) {
  return((void *) (new bwt( i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16, i17, i18, i19, i20, i21, i22, i23, i24, i25)));
}
