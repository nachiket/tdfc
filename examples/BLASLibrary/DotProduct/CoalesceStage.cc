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
#include "CoalesceStage.h"
using namespace std;
#include "FP_Add.h"
#include "AssignOutput.h"
#include "_copy_x2.h"
char * CoalesceStage_name="CoalesceStage";
void * CoalesceStage_proc_run(void *obj) {
  return(((CoalesceStage *)obj)->proc_run()); }
ScoreOperatorElement *CoalesceStageinit_instances() {
  return(ScoreOperator::addOperator(CoalesceStage_name,0,2,0));  }
ScoreOperatorElement *CoalesceStage::instances=CoalesceStageinit_instances();

CoalesceStage::CoalesceStage(DOUBLE_SCORE_STREAM n_cc_dataIn,DOUBLE_SCORE_STREAM n_cc_dataOut)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(CoalesceStage_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    CoalesceStage_arg *data;
    struct msgbuf *msgp;
    data=(CoalesceStage_arg *)malloc(sizeof(CoalesceStage_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_dataIn);
    data->i1=STREAM_OBJ_TO_ID(n_cc_dataOut);
    alen=sizeof(CoalesceStage_arg);
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
       cerr <<"CoalesceStage msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    DOUBLE_SCORE_STREAM cc_dataIn=n_cc_dataIn;
    DOUBLE_SCORE_STREAM cc_dataOut=n_cc_dataOut;
    DOUBLE_SCORE_STREAM cc_Reg_11;
    DOUBLE_SCORE_STREAM cc_AdderOut1;
    DOUBLE_SCORE_STREAM cc_Reg_21;
    DOUBLE_SCORE_STREAM cc_Reg_22;
    DOUBLE_SCORE_STREAM cc_AdderOut2;
    DOUBLE_SCORE_STREAM cc_dataIn_1;
    DOUBLE_SCORE_STREAM cc_dataIn_2;
    DOUBLE_SCORE_STREAM cc_AdderOut1_1;
    DOUBLE_SCORE_STREAM cc_AdderOut1_2;
    cc_Reg_11=NEW_DOUBLE_SCORE_STREAM();
    cc_AdderOut1=NEW_DOUBLE_SCORE_STREAM();
    cc_Reg_21=NEW_DOUBLE_SCORE_STREAM();
    cc_Reg_22=NEW_DOUBLE_SCORE_STREAM();
    cc_AdderOut2=NEW_DOUBLE_SCORE_STREAM();
    cc_dataIn_1=NEW_DOUBLE_SCORE_STREAM();
    cc_dataIn_2=NEW_DOUBLE_SCORE_STREAM();
    cc_AdderOut1_1=NEW_DOUBLE_SCORE_STREAM();
    cc_AdderOut1_2=NEW_DOUBLE_SCORE_STREAM();
    new FP_Add(cc_Reg_11,cc_dataIn_1,cc_AdderOut1);
    new AssignOutput(cc_dataIn_2,cc_Reg_11);
    new FP_Add(cc_Reg_22,cc_AdderOut1_1,cc_AdderOut2);
    new AssignOutput(cc_AdderOut1_2,cc_Reg_21);
    new AssignOutput(cc_Reg_21,cc_Reg_22);
    new AssignOutput(cc_AdderOut2,cc_dataOut);
    new _copy_x2(cc_dataIn,cc_dataIn_1,cc_dataIn_2);
    new _copy_x2(cc_AdderOut1,cc_AdderOut1_1,cc_AdderOut1_2);
  }
}

void *CoalesceStage::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  cerr << "proc_run should never be called for a compose operator!" << endl;
  return((void*)NULL); }

void CoalesceStage::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	funlockfile(stdout);
}

#undef NEW_CoalesceStage
extern "C" void *NEW_CoalesceStage(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1) {
  return((void *) (new CoalesceStage( i0, i1)));
}
