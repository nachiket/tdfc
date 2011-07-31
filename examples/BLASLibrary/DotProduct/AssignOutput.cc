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
#include "AssignOutput.h"
using namespace std;
char * AssignOutput_name="AssignOutput";
void * AssignOutput_proc_run(void *obj) {
  return(((AssignOutput *)obj)->proc_run()); }
ScoreOperatorElement *AssignOutputinit_instances() {
  return(ScoreOperator::addOperator(AssignOutput_name,0,2,0));  }
ScoreOperatorElement *AssignOutput::instances=AssignOutputinit_instances();

AssignOutput::AssignOutput(DOUBLE_SCORE_STREAM n_cc_dataIn,DOUBLE_SCORE_STREAM n_cc_dataOut)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(AssignOutput_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    AssignOutput_arg *data;
    struct msgbuf *msgp;
    data=(AssignOutput_arg *)malloc(sizeof(AssignOutput_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_dataIn);
    data->i1=STREAM_OBJ_TO_ID(n_cc_dataOut);
    alen=sizeof(AssignOutput_arg);
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
       cerr <<"AssignOutput msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(1,1, AssignOutput_name);
  addInstance(instances,this,params);
    bindInput(0,n_cc_dataIn,new ScoreStreamType(0,0,1,64));
    in[0]->setName("cc_dataIn");
    SCORE_MARKREADSTREAM(n_cc_dataIn,globalCounter->threadCounter);
    bindOutput(0,n_cc_dataOut,new ScoreStreamType(0,0,1,64));
    out[0]->setName("cc_dataOut");
    SCORE_MARKWRITESTREAM(n_cc_dataOut,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&AssignOutput_proc_run, this);
  }
}

void *AssignOutput::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  enum state_syms {STATE_only};
  state_syms state=STATE_only;
  double cc_dataIn;
  int retime_length_0=0;
  double *cc_dataIn_retime=new double [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_dataIn_retime[j]=0;
  int *input_free=new int[1];
  for (int i=0;i<1;i++)
    input_free[i]=0;
  int *output_close=new int[1];
  for (int i=0;i<1;i++)
    output_close[i]=0;
  int done=0;
  while (!done) {
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        if (1&&!eos_0&&!eofr_0) {
          cc_dataIn=STREAM_READ_DOUBLE(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_dataIn_retime[j]=cc_dataIn_retime[j-1];
          cc_dataIn_retime[0]=cc_dataIn;
          {
            STREAM_WRITE_DOUBLE(out[0],cc_dataIn_retime[0]);
            state=STATE_only;
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
  STREAM_CLOSE(out[0]);
  return((void*)NULL); }

void AssignOutput::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	if(out[0]->src!=NULL && out[0]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_dataOut\" ]" << endl;
		cout << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_dataOut\" ]" << endl;
	} else {cout<<"cannot connect cc_dataOut "<<endl;}
	funlockfile(stdout);
}

#undef NEW_AssignOutput
extern "C" void *NEW_AssignOutput(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1) {
  return((void *) (new AssignOutput( i0, i1)));
}
