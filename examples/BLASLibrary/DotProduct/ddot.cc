// cctdfc autocompiled header file
// tdfc version 1.160
// Mon Mar  7 19:07:32 2011

#include "Score.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "ddot.h"
using namespace std;
#include "FP_Add.h"
#include "FP_Mult.h"
char * ddot_name="ddot";
void * ddot_proc_run(void *obj) {
  return(((ddot *)obj)->proc_run()); }
ScoreOperatorElement *ddotinit_instances() {
  return(ScoreOperator::addOperator(ddot_name,0,5,0));  }
ScoreOperatorElement *ddot::instances=ddotinit_instances();

ddot::ddot(DOUBLE_SCORE_STREAM n_cc_x1,DOUBLE_SCORE_STREAM n_cc_y1,DOUBLE_SCORE_STREAM n_cc_x2,DOUBLE_SCORE_STREAM n_cc_y2,DOUBLE_SCORE_STREAM n_cc_z)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(ddot_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    ddot_arg *data;
    struct msgbuf *msgp;
    data=(ddot_arg *)malloc(sizeof(ddot_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_x1);
    data->i1=STREAM_OBJ_TO_ID(n_cc_y1);
    data->i2=STREAM_OBJ_TO_ID(n_cc_x2);
    data->i3=STREAM_OBJ_TO_ID(n_cc_y2);
    data->i4=STREAM_OBJ_TO_ID(n_cc_z);
    alen=sizeof(ddot_arg);
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
       cerr <<"ddot msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    DOUBLE_SCORE_STREAM cc_x1=n_cc_x1;
    DOUBLE_SCORE_STREAM cc_y1=n_cc_y1;
    DOUBLE_SCORE_STREAM cc_x2=n_cc_x2;
    DOUBLE_SCORE_STREAM cc_y2=n_cc_y2;
    DOUBLE_SCORE_STREAM cc_z=n_cc_z;
    DOUBLE_SCORE_STREAM cc_mult_result1;
    DOUBLE_SCORE_STREAM cc_mult_result2;
    cc_mult_result1=NEW_DOUBLE_SCORE_STREAM();
    cc_mult_result2=NEW_DOUBLE_SCORE_STREAM();
    new FP_Mult(cc_x1,cc_y1,cc_mult_result1);
    new FP_Mult(cc_x2,cc_y2,cc_mult_result2);
    new FP_Add(cc_mult_result1,cc_mult_result2,cc_z);
  }
}

void *ddot::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  cerr << "proc_run should never be called for a compose operator!" << endl;
  return((void*)NULL); }

void ddot::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	funlockfile(stdout);
}

#undef NEW_ddot
extern "C" void *NEW_ddot(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4) {
  return((void *) (new ddot( i0, i1, i2, i3, i4)));
}
