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
#include "bwtsingle.h"
using namespace std;
#include "sentineler.h"
#include "bwt.h"
char * bwtsingle_name="bwtsingle";
void * bwtsingle_proc_run(void *obj) {
  return(((bwtsingle *)obj)->proc_run()); }
ScoreOperatorElement *bwtsingleinit_instances() {
  return(ScoreOperator::addOperator(bwtsingle_name,0,8,0));  }
ScoreOperatorElement *bwtsingle::instances=bwtsingleinit_instances();

bwtsingle::bwtsingle(UNSIGNED_SCORE_STREAM n_cc_in,UNSIGNED_SCORE_SEGMENT n_cc_v,SIGNED_SCORE_SEGMENT n_cc_pos,SIGNED_SCORE_SEGMENT n_cc_prm,SIGNED_SCORE_SEGMENT n_cc_count,BOOLEAN_SCORE_SEGMENT n_cc_bh,BOOLEAN_SCORE_SEGMENT n_cc_b2h,UNSIGNED_SCORE_STREAM n_cc_out)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(bwtsingle_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    bwtsingle_arg *data;
    struct msgbuf *msgp;
    data=(bwtsingle_arg *)malloc(sizeof(bwtsingle_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_in);
    data->i1=SEGMENT_OBJ_TO_ID(n_cc_v);
    data->i2=SEGMENT_OBJ_TO_ID(n_cc_pos);
    data->i3=SEGMENT_OBJ_TO_ID(n_cc_prm);
    data->i4=SEGMENT_OBJ_TO_ID(n_cc_count);
    data->i5=SEGMENT_OBJ_TO_ID(n_cc_bh);
    data->i6=SEGMENT_OBJ_TO_ID(n_cc_b2h);
    data->i7=STREAM_OBJ_TO_ID(n_cc_out);
    alen=sizeof(bwtsingle_arg);
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
       cerr <<"bwtsingle msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    UNSIGNED_SCORE_STREAM cc_in=n_cc_in;
    UNSIGNED_SCORE_SEGMENT cc_v=n_cc_v;
    SIGNED_SCORE_SEGMENT cc_pos=n_cc_pos;
    SIGNED_SCORE_SEGMENT cc_prm=n_cc_prm;
    SIGNED_SCORE_SEGMENT cc_count=n_cc_count;
    BOOLEAN_SCORE_SEGMENT cc_bh=n_cc_bh;
    BOOLEAN_SCORE_SEGMENT cc_b2h=n_cc_b2h;
    UNSIGNED_SCORE_STREAM cc_out=n_cc_out;
    UNSIGNED_SCORE_STREAM cc_sentineled;
    UNSIGNED_SCORE_STREAM cc_vaddr;
    UNSIGNED_SCORE_STREAM cc_vdatar;
    UNSIGNED_SCORE_STREAM cc_vdataw;
    BOOLEAN_SCORE_STREAM cc_vwrite;
    UNSIGNED_SCORE_STREAM cc_posaddr;
    SIGNED_SCORE_STREAM cc_posdatar;
    SIGNED_SCORE_STREAM cc_posdataw;
    BOOLEAN_SCORE_STREAM cc_poswrite;
    UNSIGNED_SCORE_STREAM cc_prmaddr;
    SIGNED_SCORE_STREAM cc_prmdatar;
    SIGNED_SCORE_STREAM cc_prmdataw;
    BOOLEAN_SCORE_STREAM cc_prmwrite;
    UNSIGNED_SCORE_STREAM cc_countaddr;
    SIGNED_SCORE_STREAM cc_countdatar;
    SIGNED_SCORE_STREAM cc_countdataw;
    BOOLEAN_SCORE_STREAM cc_countwrite;
    UNSIGNED_SCORE_STREAM cc_bhaddr;
    BOOLEAN_SCORE_STREAM cc_bhdatar;
    BOOLEAN_SCORE_STREAM cc_bhdataw;
    BOOLEAN_SCORE_STREAM cc_bhwrite;
    UNSIGNED_SCORE_STREAM cc_b2haddr;
    BOOLEAN_SCORE_STREAM cc_b2hdatar;
    BOOLEAN_SCORE_STREAM cc_b2hdataw;
    BOOLEAN_SCORE_STREAM cc_b2hwrite;
    cc_sentineled=NEW_UNSIGNED_SCORE_STREAM(9);
    cc_vaddr=NEW_UNSIGNED_SCORE_STREAM(32);
    cc_vdatar=NEW_UNSIGNED_SCORE_STREAM(9);
    cc_vdataw=NEW_UNSIGNED_SCORE_STREAM(9);
    cc_vwrite=NEW_BOOLEAN_SCORE_STREAM();
    cc_posaddr=NEW_UNSIGNED_SCORE_STREAM(32);
    cc_posdatar=NEW_SIGNED_SCORE_STREAM(32);
    cc_posdataw=NEW_SIGNED_SCORE_STREAM(32);
    cc_poswrite=NEW_BOOLEAN_SCORE_STREAM();
    cc_prmaddr=NEW_UNSIGNED_SCORE_STREAM(32);
    cc_prmdatar=NEW_SIGNED_SCORE_STREAM(32);
    cc_prmdataw=NEW_SIGNED_SCORE_STREAM(32);
    cc_prmwrite=NEW_BOOLEAN_SCORE_STREAM();
    cc_countaddr=NEW_UNSIGNED_SCORE_STREAM(32);
    cc_countdatar=NEW_SIGNED_SCORE_STREAM(32);
    cc_countdataw=NEW_SIGNED_SCORE_STREAM(32);
    cc_countwrite=NEW_BOOLEAN_SCORE_STREAM();
    cc_bhaddr=NEW_UNSIGNED_SCORE_STREAM(32);
    cc_bhdatar=NEW_BOOLEAN_SCORE_STREAM();
    cc_bhdataw=NEW_BOOLEAN_SCORE_STREAM();
    cc_bhwrite=NEW_BOOLEAN_SCORE_STREAM();
    cc_b2haddr=NEW_UNSIGNED_SCORE_STREAM(32);
    cc_b2hdatar=NEW_BOOLEAN_SCORE_STREAM();
    cc_b2hdataw=NEW_BOOLEAN_SCORE_STREAM();
    cc_b2hwrite=NEW_BOOLEAN_SCORE_STREAM();
    new sentineler(cc_in,cc_sentineled);
    new bwt(cc_sentineled,cc_vaddr,cc_vdatar,cc_vdataw,cc_vwrite,cc_posaddr,cc_posdatar,cc_posdataw,cc_poswrite,cc_prmaddr,cc_prmdatar,cc_prmdataw,cc_prmwrite,cc_countaddr,cc_countdatar,cc_countdataw,cc_countwrite,cc_bhaddr,cc_bhdatar,cc_bhdataw,cc_bhwrite,cc_b2haddr,cc_b2hdatar,cc_b2hdataw,cc_b2hwrite,cc_out);
    new ScoreSegmentOperatorReadWrite(9,32,5001,cc_v,cc_vaddr,cc_vdatar,cc_vdataw,cc_vwrite);
    new ScoreSegmentOperatorReadWrite(32,32,5001,cc_pos,cc_posaddr,cc_posdatar,cc_posdataw,cc_poswrite);
    new ScoreSegmentOperatorReadWrite(32,32,5001,cc_prm,cc_prmaddr,cc_prmdatar,cc_prmdataw,cc_prmwrite);
    new ScoreSegmentOperatorReadWrite(32,32,5001,cc_count,cc_countaddr,cc_countdatar,cc_countdataw,cc_countwrite);
    new ScoreSegmentOperatorReadWrite(1,32,5002,cc_bh,cc_bhaddr,cc_bhdatar,cc_bhdataw,cc_bhwrite);
    new ScoreSegmentOperatorReadWrite(1,32,5002,cc_b2h,cc_b2haddr,cc_b2hdatar,cc_b2hdataw,cc_b2hwrite);
  }
}

void *bwtsingle::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  cerr << "proc_run should never be called for a compose operator!" << endl;
  return((void*)NULL); }

void bwtsingle::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	funlockfile(stdout);
}

#undef NEW_bwtsingle
extern "C" void *NEW_bwtsingle(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_SEGMENT i1,SIGNED_SCORE_SEGMENT i2,SIGNED_SCORE_SEGMENT i3,SIGNED_SCORE_SEGMENT i4,BOOLEAN_SCORE_SEGMENT i5,BOOLEAN_SCORE_SEGMENT i6,UNSIGNED_SCORE_STREAM i7) {
  return((void *) (new bwtsingle( i0, i1, i2, i3, i4, i5, i6, i7)));
}
