// cctdfc autocompiled header file
// tdfc version 1.160
// Tue Mar  8 20:27:00 2011

#include "Score.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "AccumCtrl.h"
using namespace std;
char * AccumCtrl_name="AccumCtrl";
void * AccumCtrl_proc_run(void *obj) {
  return(((AccumCtrl *)obj)->proc_run()); }
ScoreOperatorElement *AccumCtrlinit_instances() {
  return(ScoreOperator::addOperator(AccumCtrl_name,1,6,1));  }
ScoreOperatorElement *AccumCtrl::instances=AccumCtrlinit_instances();

AccumCtrl::AccumCtrl(unsigned long n_cc_latency,DOUBLE_SCORE_STREAM n_cc_AccumIn1,DOUBLE_SCORE_STREAM n_cc_AccumIn2,DOUBLE_SCORE_STREAM n_cc_AccumOut1,DOUBLE_SCORE_STREAM n_cc_AccumOut2,BOOLEAN_SCORE_STREAM n_cc_startCoalesce)
{
  int *params=(int *)malloc(1*sizeof(int));
  cc_latency=n_cc_latency;
  params[0]=n_cc_latency;
  char * name=mangle(AccumCtrl_name,1,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    AccumCtrl_arg *data;
    struct msgbuf *msgp;
    data=(AccumCtrl_arg *)malloc(sizeof(AccumCtrl_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_AccumIn1);
    data->i1=STREAM_OBJ_TO_ID(n_cc_AccumIn2);
    data->i2=STREAM_OBJ_TO_ID(n_cc_AccumOut1);
    data->i3=STREAM_OBJ_TO_ID(n_cc_AccumOut2);
    data->i4=STREAM_OBJ_TO_ID(n_cc_startCoalesce);
    alen=sizeof(AccumCtrl_arg);
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
       cerr <<"AccumCtrl msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(2,3, AccumCtrl_name);
  addInstance(instances,this,params);
    bindInput(0,n_cc_AccumIn1,new ScoreStreamType(0,0,1,64));
    in[0]->setName("cc_AccumIn1");
    SCORE_MARKREADSTREAM(n_cc_AccumIn1,globalCounter->threadCounter);
    bindInput(1,n_cc_AccumIn2,new ScoreStreamType(0,0,1,64));
    in[1]->setName("cc_AccumIn2");
    SCORE_MARKREADSTREAM(n_cc_AccumIn2,globalCounter->threadCounter);
    bindOutput(0,n_cc_AccumOut1,new ScoreStreamType(0,0,1,64));
    out[0]->setName("cc_AccumOut1");
    SCORE_MARKWRITESTREAM(n_cc_AccumOut1,globalCounter->threadCounter);
    bindOutput(1,n_cc_AccumOut2,new ScoreStreamType(0,0,1,64));
    out[1]->setName("cc_AccumOut2");
    SCORE_MARKWRITESTREAM(n_cc_AccumOut2,globalCounter->threadCounter);
    bindOutput(2,n_cc_startCoalesce,new ScoreStreamType(0,1));
    out[2]->setName("cc_startCoalesce");
    SCORE_MARKWRITESTREAM(n_cc_startCoalesce,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&AccumCtrl_proc_run, this);
  }
}

void *AccumCtrl::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  enum state_syms {STATE_Fill,STATE_Steady,STATE_Wait};
  state_syms state=STATE_Wait;
  unsigned long cc_counter=0;
  double cc_AccumIn1;
  int retime_length_0=0;
  double *cc_AccumIn1_retime=new double [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_AccumIn1_retime[j]=0;
  double cc_AccumIn2;
  int retime_length_1=0;
  double *cc_AccumIn2_retime=new double [retime_length_1+1];
  for (int j=retime_length_1;j>=0;j--)
    cc_AccumIn2_retime[j]=0;
  int *input_free=new int[2];
  for (int i=0;i<2;i++)
    input_free[i]=0;
  int *output_close=new int[3];
  for (int i=0;i<3;i++)
    output_close[i]=0;
  int done=0;
  while (!done) {
    switch(state) {
      case STATE_Fill: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        if (1&&!eos_0&&!eofr_0) {
          cc_AccumIn1=STREAM_READ_DOUBLE(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_AccumIn1_retime[j]=cc_AccumIn1_retime[j-1];
          cc_AccumIn1_retime[0]=cc_AccumIn1;
          {
            STREAM_WRITE_NOACC(out[2],false);
            STREAM_WRITE_DOUBLE(out[0],cc_AccumIn1_retime[0]);
            STREAM_WRITE_DOUBLE(out[1],0);
            cc_counter=(cc_counter+1);
            if ((cc_counter==cc_latency)) {
              {
                state=STATE_Steady;
              }
            }
            fprintf(stderr,"Filling ...\n");
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_Fill
      case STATE_Steady: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        int eos_1=STREAM_EOS(in[1]);
        int eofr_1=STREAM_EOFR(in[1]);
        if (1&&!eos_0&&!eofr_0&&!eos_1&&!eofr_1) {
          cc_AccumIn1=STREAM_READ_DOUBLE(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_AccumIn1_retime[j]=cc_AccumIn1_retime[j-1];
          cc_AccumIn1_retime[0]=cc_AccumIn1;
          cc_AccumIn2=STREAM_READ_DOUBLE(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_AccumIn2_retime[j]=cc_AccumIn2_retime[j-1];
          cc_AccumIn2_retime[0]=cc_AccumIn2;
          {
            STREAM_WRITE_NOACC(out[2],false);
            STREAM_WRITE_DOUBLE(out[0],cc_AccumIn1_retime[0]);
            STREAM_WRITE_DOUBLE(out[1],cc_AccumIn2_retime[0]);
            fprintf(stderr,"Steady State ...\n");
          }
        }
        else
        {
        if (1&&eofr_0) {
          STREAM_READ_DOUBLE(in[0]);
          {
            STREAM_WRITE_NOACC(out[2],true);
            fprintf(stderr,"End of Frame detected\n");
            cc_counter=0;
            state=STATE_Wait;
          }
        }
        else
        {
        if (0|| eofr_0) {}
        else
         done=1;
        }
        }}        break; 
      } // end case STATE_Steady
      case STATE_Wait: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        if (1&&!eos_0&&!eofr_0) {
          cc_AccumIn1=STREAM_READ_DOUBLE(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_AccumIn1_retime[j]=cc_AccumIn1_retime[j-1];
          cc_AccumIn1_retime[0]=cc_AccumIn1;
          {
            STREAM_WRITE_NOACC(out[2],false);
            STREAM_WRITE_DOUBLE(out[0],cc_AccumIn1_retime[0]);
            STREAM_WRITE_DOUBLE(out[1],0);
            cc_counter=(cc_counter+1);
            state=STATE_Fill;
            fprintf(stderr,"Waiting ...\n");
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }        break; 
      } // end case STATE_Wait
      default: cerr << "ERROR unknown state [" << (int)state << "] encountered in AccumCtrl::proc_run" << endl;
        abort();
    }
  }
  STREAM_FREE(in[0]);
  STREAM_FREE(in[1]);
  STREAM_CLOSE(out[0]);
  STREAM_CLOSE(out[1]);
  STREAM_CLOSE(out[2]);
  return((void*)NULL); }

void AccumCtrl::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	if(out[0]->src!=NULL && out[0]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_AccumOut1\" ]" << endl;
		cout << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_AccumOut1\" ]" << endl;
	} else {cout<<"cannot connect cc_AccumOut1 "<<endl;}
	if(out[1]->src!=NULL && out[1]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[1]->src->getName() << "->" << out[1]->sink->getName() << "[ label= \" cc_AccumOut2\" ]" << endl;
		cout << "\t" << out[1]->src->getName() << "->" << out[1]->sink->getName() << "[ label= \" cc_AccumOut2\" ]" << endl;
	} else {cout<<"cannot connect cc_AccumOut2 "<<endl;}
	if(out[2]->src!=NULL && out[2]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[2]->src->getName() << "->" << out[2]->sink->getName() << "[ label= \" cc_startCoalesce\" ]" << endl;
		cout << "\t" << out[2]->src->getName() << "->" << out[2]->sink->getName() << "[ label= \" cc_startCoalesce\" ]" << endl;
	} else {cout<<"cannot connect cc_startCoalesce "<<endl;}
	funlockfile(stdout);
}

#undef NEW_AccumCtrl
extern "C" void *NEW_AccumCtrl(unsigned long i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4,BOOLEAN_SCORE_STREAM i5) {
  return((void *) (new AccumCtrl( i0, i1, i2, i3, i4, i5)));
}
