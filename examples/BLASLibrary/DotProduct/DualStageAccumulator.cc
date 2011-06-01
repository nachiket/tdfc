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
#include "DualStageAccumulator.h"
using namespace std;
#include "FP_Add.h"
#include "AccumCtrl.h"
#include "AssignOutput.h"
#include "EnableCoalesceStageData.h"
#include "_copy_x3.h"
char * DualStageAccumulator_name="DualStageAccumulator";
void * DualStageAccumulator_proc_run(void *obj) {
  return(((DualStageAccumulator *)obj)->proc_run()); }
ScoreOperatorElement *DualStageAccumulatorinit_instances() {
  return(ScoreOperator::addOperator(DualStageAccumulator_name,1,3,1));  }
ScoreOperatorElement *DualStageAccumulator::instances=DualStageAccumulatorinit_instances();

DualStageAccumulator::DualStageAccumulator(unsigned long n_cc_LATENCY,DOUBLE_SCORE_STREAM n_cc_dataIn,DOUBLE_SCORE_STREAM n_cc_dataOut)
{
  int *params=(int *)malloc(1*sizeof(int));
  cc_LATENCY=n_cc_LATENCY;
  params[0]=n_cc_LATENCY;
  char * name=mangle(DualStageAccumulator_name,1,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    DualStageAccumulator_arg *data;
    struct msgbuf *msgp;
    data=(DualStageAccumulator_arg *)malloc(sizeof(DualStageAccumulator_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_dataIn);
    data->i1=STREAM_OBJ_TO_ID(n_cc_dataOut);
    alen=sizeof(DualStageAccumulator_arg);
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
       cerr <<"DualStageAccumulator msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    DOUBLE_SCORE_STREAM cc_dataIn=n_cc_dataIn;
    DOUBLE_SCORE_STREAM cc_dataOut=n_cc_dataOut;
    DOUBLE_SCORE_STREAM cc_A;
    DOUBLE_SCORE_STREAM cc_B;
    DOUBLE_SCORE_STREAM cc_C;
    DOUBLE_SCORE_STREAM cc_CoalesceStageDataIn;
    BOOLEAN_SCORE_STREAM cc_CoalesceEnable;
    DOUBLE_SCORE_STREAM cc_C_1;
    DOUBLE_SCORE_STREAM cc_C_2;
    DOUBLE_SCORE_STREAM cc_C_3;
    cc_A=NEW_DOUBLE_SCORE_STREAM();
    cc_B=NEW_DOUBLE_SCORE_STREAM();
    cc_C=NEW_DOUBLE_SCORE_STREAM();
    cc_CoalesceStageDataIn=NEW_DOUBLE_SCORE_STREAM();
    cc_CoalesceEnable=NEW_BOOLEAN_SCORE_STREAM();
    cc_C_1=NEW_DOUBLE_SCORE_STREAM();
    cc_C_2=NEW_DOUBLE_SCORE_STREAM();
    cc_C_3=NEW_DOUBLE_SCORE_STREAM();
    new AccumCtrl(cc_LATENCY,cc_dataIn,cc_C_1,cc_A,cc_B,cc_CoalesceEnable);
    new FP_Add(cc_A,cc_B,cc_C);
    new EnableCoalesceStageData(cc_LATENCY,cc_CoalesceEnable,cc_C_2,cc_CoalesceStageDataIn);
    new AssignOutput(cc_C_3,cc_dataOut);
    new _copy_x3(cc_C,cc_C_1,cc_C_2,cc_C_3);
  }
}

void *DualStageAccumulator::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  cerr << "proc_run should never be called for a compose operator!" << endl;
  return((void*)NULL); }

void DualStageAccumulator::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	funlockfile(stdout);
}

#undef NEW_DualStageAccumulator
extern "C" void *NEW_DualStageAccumulator(unsigned long i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2) {
  return((void *) (new DualStageAccumulator( i0, i1, i2)));
}
