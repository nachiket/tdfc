// cctdfc autocompiled header file
// tdfc version 1.160
// Thu Aug 27 19:09:06 2009

#include "Score.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "read_seg.h"
char * read_seg_name="read_seg";
void * read_seg_proc_run(void *obj) {
  return(((read_seg *)obj)->proc_run()); }
ScoreOperatorElement *read_seginit_instances() {
  return(ScoreOperator::addOperator(read_seg_name,0,5,0));  }
ScoreOperatorElement *read_seg::instances=read_seginit_instances();

read_seg::read_seg(UNSIGNED_SCORE_STREAM n_cc_addr_a,UNSIGNED_SCORE_STREAM n_cc_addr_b,BOOLEAN_SCORE_STREAM n_cc_write_a,BOOLEAN_SCORE_STREAM n_cc_write_b,UNSIGNED_SCORE_STREAM n_cc_zindex)
{
  int *params=(int *)malloc(0*sizeof(int));
  addInstance(instances,params);
  char * name=mangle(read_seg_name,0,params);
  char * instance_fn=resolve(name);
  if (instance_fn!=(char *)NULL) {
    long slen;
    long alen;
    long blen;
    read_seg_arg *data;
    struct msgbuf *msgp;
    data=(read_seg_arg *)malloc(sizeof(read_seg_arg));
    data->i0=STREAM_OBJ_TO_ID(n_cc_addr_a);
    data->i1=STREAM_OBJ_TO_ID(n_cc_addr_b);
    data->i2=STREAM_OBJ_TO_ID(n_cc_write_a);
    data->i3=STREAM_OBJ_TO_ID(n_cc_write_b);
    data->i4=STREAM_OBJ_TO_ID(n_cc_zindex);
    alen=sizeof(read_seg_arg);
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
       cerr <<"read_seg msgsnd call failed with errno=" << errno << endl;
       exit(2);    }
  }
  else {
    declareIO(1,4);
    bindOutput(0,n_cc_addr_a,new ScoreStreamType(0,6));
    SCORE_MARKWRITESTREAM(n_cc_addr_a,globalCounter->threadCounter);
    bindOutput(1,n_cc_addr_b,new ScoreStreamType(0,6));
    SCORE_MARKWRITESTREAM(n_cc_addr_b,globalCounter->threadCounter);
    bindOutput(2,n_cc_write_a,new ScoreStreamType(0,1));
    SCORE_MARKWRITESTREAM(n_cc_write_a,globalCounter->threadCounter);
    bindOutput(3,n_cc_write_b,new ScoreStreamType(0,1));
    SCORE_MARKWRITESTREAM(n_cc_write_b,globalCounter->threadCounter);
    bindInput(0,n_cc_zindex,new ScoreStreamType(0,6));
    SCORE_MARKREADSTREAM(n_cc_zindex,globalCounter->threadCounter);
    pthread_attr_t *a_thread_attribute=(pthread_attr_t *)malloc(sizeof(pthread_attr_t));
    pthread_attr_init(a_thread_attribute);
    pthread_attr_setdetachstate(a_thread_attribute,PTHREAD_CREATE_DETACHED);
    pthread_create(&rpt,a_thread_attribute,&read_seg_proc_run, this);
  }
}

void *read_seg::proc_run() {
  enum state_syms {STATE_cinco,STATE_cuatro,STATE_dos,STATE_ocho,STATE_seis,STATE_siete,STATE_tres,STATE_uno};
  state_syms state=STATE_uno;
  unsigned long cc_index=0;
  unsigned long cc_bigIndex=0;
  int cc_phase=true;
  int cc_init=true;
  int cc_final=false;
  unsigned long cc_zindex;
  int retime_length_0=0;
  unsigned long *cc_zindex_retime=new unsigned long [retime_length_0+1];
  for (int j=retime_length_0;j>=0;j--)
    cc_zindex_retime[j]=0;
  int *input_free=new int[1];
  for (int i=0;i<1;i++)
    input_free[i]=0;
  int *output_close=new int[4];
  for (int i=0;i<4;i++)
    output_close[i]=0;
  int done=0;
  while (!done) {
    switch(state) {
      case STATE_cinco: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            cc_index=(cc_index+1);
            state=STATE_seis;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_cinco
      case STATE_cuatro: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            cc_index=(cc_index+1);
            state=STATE_cinco;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_cuatro
      case STATE_dos: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            cc_index=(cc_index+1);
            state=STATE_tres;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_dos
      case STATE_ocho: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            if ((cc_index==63)) {
              {
                cc_index=0;
                if (cc_phase) {
                  {
                    cc_phase=false;
                  }
                }
                else {
                  {
                    cc_phase=true;
                  }
                }
                cc_init=false;
                if ((cc_bigIndex==4095)) {
                  {
                    cc_final=true;
                  }
                }
                cc_bigIndex=(cc_bigIndex+1);
              }
            }
            else {
              {
                cc_index=(cc_index+1);
              }
            }
            state=STATE_uno;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_ocho
      case STATE_seis: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            cc_index=(cc_index+1);
            state=STATE_siete;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_seis
      case STATE_siete: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            cc_index=(cc_index+1);
            state=STATE_ocho;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_siete
      case STATE_tres: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            cc_index=(cc_index+1);
            state=STATE_cuatro;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_tres
      case STATE_uno: { 
        {
        int eos_0=STREAM_EOS(in[0]);
        if (1&&!eos_0) {
          cc_zindex=STREAM_READ_NOACC(in[0]);
          for (int j=retime_length_0;j>0;j--)
            cc_zindex_retime[j]=cc_zindex_retime[j-1];
          cc_zindex_retime[0]=cc_zindex;
          {
            if (cc_phase) {
              {
                if ((cc_final==false)) {
                  {
                    STREAM_WRITE_NOACC(out[0],cc_zindex_retime[0]);
                    STREAM_WRITE_NOACC(out[2],true);
                  }
                }
                if ((cc_init==false)) {
                  {
                    STREAM_WRITE_NOACC(out[1],cc_index);
                    STREAM_WRITE_NOACC(out[3],false);
                  }
                }
              }
            }
            else {
              {
                STREAM_WRITE_NOACC(out[1],cc_zindex_retime[0]);
                STREAM_WRITE_NOACC(out[3],true);
                STREAM_WRITE_NOACC(out[0],cc_index);
                STREAM_WRITE_NOACC(out[2],false);
              }
            }
            cc_index=(cc_index+1);
            state=STATE_dos;
          }
        }
        else
         done=1;
        }        break; 
      } // end case STATE_uno
      default: cerr << "ERROR unknown state [" << (int)state << "] encountered in read_seg::proc_run" << endl;
        abort();
    }
  }
  STREAM_CLOSE(out[0]);
  STREAM_CLOSE(out[1]);
  STREAM_CLOSE(out[2]);
  STREAM_CLOSE(out[3]);
  STREAM_FREE(in[0]);
  return((void*)NULL); }

#undef NEW_read_seg
extern "C" void *NEW_read_seg(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1,BOOLEAN_SCORE_STREAM i2,BOOLEAN_SCORE_STREAM i3,UNSIGNED_SCORE_STREAM i4) {
  return((void *) (new read_seg( i0, i1, i2, i3, i4)));
}
