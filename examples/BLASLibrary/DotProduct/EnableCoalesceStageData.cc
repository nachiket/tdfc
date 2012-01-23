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
#include "EnableCoalesceStageData.h"
using namespace std;
char * EnableCoalesceStageData_name="EnableCoalesceStageData";
void * EnableCoalesceStageData_proc_run(void *obj) {
  return(((EnableCoalesceStageData *)obj)->proc_run()); }
ScoreOperatorElement *EnableCoalesceStageDatainit_instances() {
  return(ScoreOperator::addOperator(EnableCoalesceStageData_name,1,4,1));  }
ScoreOperatorElement *EnableCoalesceStageData::instances=EnableCoalesceStageDatainit_instances();

EnableCoalesceStageData::EnableCoalesceStageData(unsigned long n_cc_LATENCY,BOOLEAN_SCORE_STREAM n_cc_AccumIn,DOUBLE_SCORE_STREAM n_cc_dataIn,DOUBLE_SCORE_STREAM n_cc_dataOut)
{
  int *params=(int *)malloc(1*sizeof(int));
  cc_LATENCY=n_cc_LATENCY;
  params[0]=n_cc_LATENCY;
  char * name=mangle(EnableCoalesceStageData_name,1,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    EnableCoalesceStageData_arg *data;
    struct msgbuf *msgp;
    data=(EnableCoalesceStageData_arg *)malloc(sizeof(EnableCoalesceStageData_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_AccumIn);
    data->i1=STREAM_OBJ_TO_ID(n_cc_dataIn);
    data->i2=STREAM_OBJ_TO_ID(n_cc_dataOut);
    alen=sizeof(EnableCoalesceStageData_arg);
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
       cerr <<"EnableCoalesceStageData msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(2,1, EnableCoalesceStageData_name);
  addInstance(instances,this,params);
    bindInput(0,n_cc_AccumIn,new ScoreStreamType(0,1));
    in[0]->setName("cc_AccumIn");
    SCORE_MARKREADSTREAM(n_cc_AccumIn,globalCounter->threadCounter);
    bindInput(1,n_cc_dataIn,new ScoreStreamType(0,0,1,64));
    in[1]->setName("cc_dataIn");
    SCORE_MARKREADSTREAM(n_cc_dataIn,globalCounter->threadCounter);
    bindOutput(0,n_cc_dataOut,new ScoreStreamType(0,0,1,64));
    out[0]->setName("cc_dataOut");
    SCORE_MARKWRITESTREAM(n_cc_dataOut,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&EnableCoalesceStageData_proc_run, this);
  }
}

void *EnableCoalesceStageData::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  enum state_syms {STATE_AssignInput,STATE_Wait};
  state_syms state=STATE_Wait;
  unsigned long cc_counter;
  int cc_AccumIn;
  int retime_length_0=0;
  int *cc_AccumIn_retime=new int [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_AccumIn_retime[j]=0;
  double cc_dataIn;
  int retime_length_1=0;
  double *cc_dataIn_retime=new double [retime_length_1+1];
  for (int j=retime_length_1;j>=0;j--)
    cc_dataIn_retime[j]=0;
  int *input_free=new int[2];
  for (int i=0;i<2;i++)
    input_free[i]=0;
  int *output_close=new int[1];
  for (int i=0;i<1;i++)
    output_close[i]=0;
  int done=0;
  while (!done) {
    switch(state) {
      case STATE_AssignInput: { 
        {
        int eos_0=STREAM_EOS(in[1]);
        int eofr_0=STREAM_EOFR(in[1]);
        if (1&&!eos_0&&!eofr_0) {
          cc_dataIn=STREAM_READ_DOUBLE(in[1]);
          for (int j=retime_length_1;j>0;j--)
            cc_dataIn_retime[j]=cc_dataIn_retime[j-1];
          cc_dataIn_retime[0]=cc_dataIn;
          {
            cc_counter=(cc_counter+1);
            STREAM_WRITE_DOUBLE(out[0],cc_dataIn_retime[0]);
            fprintf(stderr,"dataOut = %f\n", cc_dataIn_retime[0]);
            if ((cc_counter==cc_LATENCY)) {
              {
                state=STATE_Wait;
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
      } // end case STATE_AssignInput
      case STATE_Wait: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        if (1&&!eos_0&&!eofr_0) {
          cc_AccumIn=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_AccumIn_retime[j]=cc_AccumIn_retime[j-1];
          cc_AccumIn_retime[0]=cc_AccumIn;
          {
            if ((cc_AccumIn_retime[0]==false)) {
              {
                STREAM_WRITE_DOUBLE(out[0],0);
              }
            }
            else {
              {
                cc_counter=0;
                fprintf(stderr,"End of Frame detected\n");
                state=STATE_AssignInput;
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
      } // end case STATE_Wait
      default: cerr << "ERROR unknown state [" << (int)state << "] encountered in EnableCoalesceStageData::proc_run" << endl;
        abort();
    }
  }
  STREAM_FREE(in[0]);
  STREAM_FREE(in[1]);
  STREAM_CLOSE(out[0]);
  return((void*)NULL); }

void EnableCoalesceStageData::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	if(out[0]->src!=NULL && out[0]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_dataOut\" ]" << endl;
		cout << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_dataOut\" ]" << endl;
	} else {cout<<"cannot connect cc_dataOut "<<endl;}
	funlockfile(stdout);
}

#undef NEW_EnableCoalesceStageData
extern "C" void *NEW_EnableCoalesceStageData(unsigned long i0,BOOLEAN_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3) {
  return((void *) (new EnableCoalesceStageData( i0, i1, i2, i3)));
}
