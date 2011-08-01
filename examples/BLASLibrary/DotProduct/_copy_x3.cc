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
#include "_copy_x3.h"
using namespace std;
char * _copy_x3_name="_copy_x3";
void * _copy_x3_proc_run(void *obj) {
  return(((_copy_x3 *)obj)->proc_run()); }
ScoreOperatorElement *_copy_x3init_instances() {
  return(ScoreOperator::addOperator(_copy_x3_name,0,4,0));  }
ScoreOperatorElement *_copy_x3::instances=_copy_x3init_instances();

_copy_x3::_copy_x3(DOUBLE_SCORE_STREAM n_cc_i,DOUBLE_SCORE_STREAM n_cc_o1,DOUBLE_SCORE_STREAM n_cc_o2,DOUBLE_SCORE_STREAM n_cc_o3)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(_copy_x3_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    _copy_x3_arg *data;
    struct msgbuf *msgp;
    data=(_copy_x3_arg *)malloc(sizeof(_copy_x3_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_i);
    data->i1=STREAM_OBJ_TO_ID(n_cc_o1);
    data->i2=STREAM_OBJ_TO_ID(n_cc_o2);
    data->i3=STREAM_OBJ_TO_ID(n_cc_o3);
    alen=sizeof(_copy_x3_arg);
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
       cerr <<"_copy_x3 msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(1,3, _copy_x3_name);
  addInstance(instances,this,params);
    bindInput(0,n_cc_i,new ScoreStreamType(0,0,1,64));
    in[0]->setName("cc_i");
    SCORE_MARKREADSTREAM(n_cc_i,globalCounter->threadCounter);
    bindOutput(0,n_cc_o1,new ScoreStreamType(0,0,1,64));
    out[0]->setName("cc_o1");
    SCORE_MARKWRITESTREAM(n_cc_o1,globalCounter->threadCounter);
    bindOutput(1,n_cc_o2,new ScoreStreamType(0,0,1,64));
    out[1]->setName("cc_o2");
    SCORE_MARKWRITESTREAM(n_cc_o2,globalCounter->threadCounter);
    bindOutput(2,n_cc_o3,new ScoreStreamType(0,0,1,64));
    out[2]->setName("cc_o3");
    SCORE_MARKWRITESTREAM(n_cc_o3,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&_copy_x3_proc_run, this);
  }
}

void *_copy_x3::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  enum state_syms {STATE_only};
  state_syms state=STATE_only;
  double cc_i;
  int retime_length_0=0;
  double *cc_i_retime=new double [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_i_retime[j]=0;
  int *input_free=new int[1];
  for (int i=0;i<1;i++)
    input_free[i]=0;
  int *output_close=new int[3];
  for (int i=0;i<3;i++)
    output_close[i]=0;
  int done=0;
  while (!done) {
        {
        int eos_0=STREAM_EOS(in[0]);
        int eofr_0=STREAM_EOFR(in[0]);
        if (1&&!eos_0&&!eofr_0) {
          cc_i=STREAM_READ_DOUBLE(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_i_retime[j]=cc_i_retime[j-1];
          cc_i_retime[0]=cc_i;
          STREAM_WRITE_DOUBLE(out[0],cc_i_retime[0]);
          STREAM_WRITE_DOUBLE(out[1],cc_i_retime[0]);
          STREAM_WRITE_DOUBLE(out[2],cc_i_retime[0]);
          state=STATE_only;
        }
        else
        {
        if (1&&eofr_0) {
          STREAM_READ_DOUBLE(in[0]);
          FRAME_CLOSE(out[0]);
          FRAME_CLOSE(out[1]);
          FRAME_CLOSE(out[2]);
          state=STATE_only;
        }
        else
        {
        if (0|| eofr_0) {}
        else
         done=1;
        }
        }}  }
  STREAM_FREE(in[0]);
  STREAM_CLOSE(out[0]);
  STREAM_CLOSE(out[1]);
  STREAM_CLOSE(out[2]);
  return((void*)NULL); }

void _copy_x3::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	if(out[0]->src!=NULL && out[0]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_o1\" ]" << endl;
		cout << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_o1\" ]" << endl;
	} else {cout<<"cannot connect cc_o1 "<<endl;}
	if(out[1]->src!=NULL && out[1]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[1]->src->getName() << "->" << out[1]->sink->getName() << "[ label= \" cc_o2\" ]" << endl;
		cout << "\t" << out[1]->src->getName() << "->" << out[1]->sink->getName() << "[ label= \" cc_o2\" ]" << endl;
	} else {cout<<"cannot connect cc_o2 "<<endl;}
	if(out[2]->src!=NULL && out[2]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[2]->src->getName() << "->" << out[2]->sink->getName() << "[ label= \" cc_o3\" ]" << endl;
		cout << "\t" << out[2]->src->getName() << "->" << out[2]->sink->getName() << "[ label= \" cc_o3\" ]" << endl;
	} else {cout<<"cannot connect cc_o3 "<<endl;}
	funlockfile(stdout);
}

#undef NEW__copy_x3
extern "C" void *NEW__copy_x3(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3) {
  return((void *) (new _copy_x3( i0, i1, i2, i3)));
}
