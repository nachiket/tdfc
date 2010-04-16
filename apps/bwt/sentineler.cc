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
#include "sentineler.h"
using namespace std;
char * sentineler_name="sentineler";
void * sentineler_proc_run(void *obj) {
  return(((sentineler *)obj)->proc_run()); }
ScoreOperatorElement *sentinelerinit_instances() {
  return(ScoreOperator::addOperator(sentineler_name,0,2,0));  }
ScoreOperatorElement *sentineler::instances=sentinelerinit_instances();

sentineler::sentineler(UNSIGNED_SCORE_STREAM n_cc_in,UNSIGNED_SCORE_STREAM n_cc_out)
{
  int *params=(int *)malloc(0*sizeof(int));
  char * name=mangle(sentineler_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    sentineler_arg *data;
    struct msgbuf *msgp;
    data=(sentineler_arg *)malloc(sizeof(sentineler_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_in);
    data->i1=STREAM_OBJ_TO_ID(n_cc_out);
    alen=sizeof(sentineler_arg);
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
       cerr <<"sentineler msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(1,1, sentineler_name);
  addInstance(instances,this,params);
    bindInput(0,n_cc_in,new ScoreStreamType(0,8));
    in[0]->setName("cc_in");
    SCORE_MARKREADSTREAM(n_cc_in,globalCounter->threadCounter);
    bindOutput(0,n_cc_out,new ScoreStreamType(0,9));
    out[0]->setName("cc_out");
    SCORE_MARKWRITESTREAM(n_cc_out,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&sentineler_proc_run, this);
  }
}

void *sentineler::proc_run() {
	sleep(1);
	if(ScoreOperator::fout!=NULL) {
		get_graphviz_strings(); return NULL;
	}
  enum state_syms {STATE_only};
  state_syms state=STATE_only;
  unsigned long cc_in;
  int retime_length_0=0;
  unsigned long *cc_in_retime=new unsigned long [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_in_retime[j]=0;
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
        if (1&&eos_0) {
          if (!input_free[0])
          STREAM_FREE(in[0]);
          input_free[0]=1;
          {
            STREAM_WRITE_NOACC(out[0],256);
            done=1;
          }
        }
        else
        {
        if (1&&!eos_0&&!eofr_0) {
          cc_in=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_in_retime[j]=cc_in_retime[j-1];
          cc_in_retime[0]=cc_in;
          {
            STREAM_WRITE_NOACC(out[0],cc_in_retime[0]);
          }
        }
        else
        {
        if (0) {}
        else
         done=1;
        }
        }}  }
  if (!input_free[0])
    STREAM_FREE(in[0]);
  STREAM_CLOSE(out[0]);
  return((void*)NULL); }

void sentineler::get_graphviz_strings() {
	const char* stupid="null";
	flockfile(stdout);
	if(out[0]->src!=NULL && out[0]->sink!=NULL) {
		*(ScoreOperator::fout) << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_out\" ]" << endl;
		cout << "\t" << out[0]->src->getName() << "->" << out[0]->sink->getName() << "[ label= \" cc_out\" ]" << endl;
	} else {cout<<"cannot connect cc_out "<<endl;}
	funlockfile(stdout);
}

#undef NEW_sentineler
extern "C" void *NEW_sentineler(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1) {
  return((void *) (new sentineler( i0, i1)));
}
