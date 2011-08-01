// cctdfc autocompiled header file
// tdfc version 1.160
// Thu Apr  7 18:06:00 2011

#include "Score.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "tdfc_dgemv.h"
using namespace std;
#include "tdfc_ddot.h"
#include "_copy_x4.h"
char * tdfc_dgemv_name="tdfc_dgemv";
void * tdfc_dgemv_proc_run(void *obj) {
  return(((tdfc_dgemv *)obj)->proc_run()); }
ScoreOperatorElement *tdfc_dgemvinit_instances() {
  return(ScoreOperator::addOperator(tdfc_dgemv_name,2,11,3));  }
ScoreOperatorElement *tdfc_dgemv::instances=tdfc_dgemvinit_instances();

tdfc_dgemv::tdfc_dgemv(unsigned long n_cc_M,unsigned long n_cc_N,DOUBLE_SCORE_STREAM n_cc_A1,DOUBLE_SCORE_STREAM n_cc_A2,DOUBLE_SCORE_STREAM n_cc_A3,DOUBLE_SCORE_STREAM n_cc_A4,DOUBLE_SCORE_STREAM n_cc_x,DOUBLE_SCORE_STREAM n_cc_b1,DOUBLE_SCORE_STREAM n_cc_b2,DOUBLE_SCORE_STREAM n_cc_b3,DOUBLE_SCORE_STREAM n_cc_b4)
{
  int *params=(int *)malloc(2*sizeof(int));
  cc_M=n_cc_M;
  params[0]=n_cc_M;
  cc_N=n_cc_N;
  params[1]=n_cc_N;
  char * name=mangle(tdfc_dgemv_name,2,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    tdfc_dgemv_arg *data;
    struct msgbuf *msgp;
    data=(tdfc_dgemv_arg *)malloc(sizeof(tdfc_dgemv_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_A1);
    data->i1=STREAM_OBJ_TO_ID(n_cc_A2);
    data->i2=STREAM_OBJ_TO_ID(n_cc_A3);
    data->i3=STREAM_OBJ_TO_ID(n_cc_A4);
    data->i4=STREAM_OBJ_TO_ID(n_cc_x);
    data->i5=STREAM_OBJ_TO_ID(n_cc_b1);
    data->i6=STREAM_OBJ_TO_ID(n_cc_b2);
    data->i7=STREAM_OBJ_TO_ID(n_cc_b3);
    data->i8=STREAM_OBJ_TO_ID(n_cc_b4);
    alen=sizeof(tdfc_dgemv_arg);
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
       cerr <<"tdfc_dgemv msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    DOUBLE_SCORE_STREAM cc_A1=n_cc_A1;
    DOUBLE_SCORE_STREAM cc_A2=n_cc_A2;
    DOUBLE_SCORE_STREAM cc_A3=n_cc_A3;
    DOUBLE_SCORE_STREAM cc_A4=n_cc_A4;
    DOUBLE_SCORE_STREAM cc_x=n_cc_x;
    DOUBLE_SCORE_STREAM cc_b1=n_cc_b1;
    DOUBLE_SCORE_STREAM cc_b2=n_cc_b2;
    DOUBLE_SCORE_STREAM cc_b3=n_cc_b3;
    DOUBLE_SCORE_STREAM cc_b4=n_cc_b4;
    DOUBLE_SCORE_STREAM cc_x_1;
    DOUBLE_SCORE_STREAM cc_x_2;
    DOUBLE_SCORE_STREAM cc_x_3;
    DOUBLE_SCORE_STREAM cc_x_4;
    cc_x_1=NEW_DOUBLE_SCORE_STREAM();
    cc_x_2=NEW_DOUBLE_SCORE_STREAM();
    cc_x_3=NEW_DOUBLE_SCORE_STREAM();
    cc_x_4=NEW_DOUBLE_SCORE_STREAM();
    new tdfc_ddot(cc_A1,cc_x_1,cc_b1);
    new tdfc_ddot(cc_A2,cc_x_2,cc_b2);
    new tdfc_ddot(cc_A3,cc_x_3,cc_b3);
    new tdfc_ddot(cc_A4,cc_x_4,cc_b4);
    new _copy_x4(cc_x,cc_x_1,cc_x_2,cc_x_3,cc_x_4);
  }
}

void *tdfc_dgemv::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  cerr << "proc_run should never be called for a compose operator!" << endl;
  return((void*)NULL); }

void tdfc_dgemv::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	funlockfile(stdout);
}

#undef NEW_tdfc_dgemv
extern "C" void *NEW_tdfc_dgemv(unsigned long i0,unsigned long i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4,DOUBLE_SCORE_STREAM i5,DOUBLE_SCORE_STREAM i6,DOUBLE_SCORE_STREAM i7,DOUBLE_SCORE_STREAM i8,DOUBLE_SCORE_STREAM i9,DOUBLE_SCORE_STREAM i10) {
  return((void *) (new tdfc_dgemv( i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10)));
}
