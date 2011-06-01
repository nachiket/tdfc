// cctdfc autocompiled header file
// tdfc version 1.160
// Tue Mar  8 12:54:38 2011

#include "Score.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "FP_Mult.h"
using namespace std;
char * FP_Mult_name="FP_Mult";
void * FP_Mult_proc_run(void *obj) {
  return(((FP_Mult *)obj)->proc_run()); }
ScoreOperatorElement *FP_Multinit_instances() {
  return(ScoreOperator::addOperator(FP_Mult_name,0,3,0));  }
ScoreOperatorElement *FP_Mult::instances=FP_Multinit_instances();

FP_Mult::FP_Mult(DOUBLE_SCORE_STREAM n_cc_x,DOUBLE_SCORE_STREAM n_cc_y,DOUBLE_SCORE_STREAM n_cc_z)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(FP_Mult_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    FP_Mult_arg *data;
    struct msgbuf *msgp;
    data=(FP_Mult_arg *)malloc(sizeof(FP_Mult_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_x);
    data->i1=STREAM_OBJ_TO_ID(n_cc_y);
    data->i2=STREAM_OBJ_TO_ID(n_cc_z);
    alen=sizeof(FP_Mult_arg);
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
       cerr <<"FP_Mult msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(2,1, FP_Mult_name);
  addInstance(instances,this,params);
    bindInput(0,n_cc_x,new ScoreStreamType(0,0,1,64));
    in[0]->setName("cc_x");
    SCORE_MARKREADSTREAM(n_cc_x,globalCounter->threadCounter);
    bindInput(1,n_cc_y,new ScoreStreamType(0,0,1,64));
    in[1]->setName("cc_y");
    SCORE_MARKREADSTREAM(n_cc_y,globalCounter->threadCounter);
    bindOutput(0,n_cc_z,new ScoreStreamType(0,0,1,64));
    out[0]->setName("cc_z");
    SCORE_MARKWRITESTREAM(n_cc_z,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&FP_Mult_proc_run, this);
  }
}

void *FP_Mult::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  enum state_syms {STATE_Multiply};
  state_syms state=STATE_Multiply;
  double cc_x;
  int retime_length_0=0;
  double *cc_x_retime=new double [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_x_retime[j]=0;
  double cc_y;
  int retime_length_1=0;
  double *cc_y_retime=new double [retime_length_1+1];
  for (int j=retime_length_1;j>=0;j--)
    cc_y_retime[j]=0;
  int *input_free=new int[2];
  for (int i=0;i<2;i++)
    input_free[i]=0;
  int *output_close=new int[1];
  for (int i=0;i<1;i++)
    output_close[i]=0;
  int done=0;
  while (!done) {
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        int eos_1=STREAM_EOS(in[1]);
        int eofr_1=STREAM_EOFR(in[1]);
        if (1&&!eos_0&&!eofr_0&&!eos_1&&!eofr_1) {
          cc_x=STREAM_READ_DOUBLE(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_x_retime[j]=cc_x_retime[j-1];
          cc_x_retime[0]=cc_x;
          cc_y=STREAM_READ_DOUBLE(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_y_retime[j]=cc_y_retime[j-1];
          cc_y_retime[0]=cc_y;
          {
            STREAM_WRITE_DOUBLE(out[0],(cc_x_retime[0]*cc_y_retime[0]));
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }  }
  STREAM_FREE(in[0]);
  STREAM_FREE(in[1]);
  STREAM_CLOSE(out[0]);
  return((void*)NULL); }

void FP_Mult::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	if(out[0]->src!=NULL && out[0]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_z\" ]" << endl;
		cout << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_z\" ]" << endl;
	} else {cout<<"cannot connect cc_z "<<endl;}
	funlockfile(stdout);
}

#undef NEW_FP_Mult
extern "C" void *NEW_FP_Mult(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2) {
  return((void *) (new FP_Mult( i0, i1, i2)));
}
