// cctdfc autocompiled header file
// tdfc version 1.160
// Thu Aug 27 19:09:06 2009

#include "Score.h"
#include <pthread.h>
#ifdef __cplusplus
typedef struct {
   int i0;
   int i1;
   int i2;
   int i3;
   int i4;
} read_seg_arg;

class read_seg: public ScoreOperator {
public: 
  read_seg(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1,BOOLEAN_SCORE_STREAM i2,BOOLEAN_SCORE_STREAM i3,UNSIGNED_SCORE_STREAM i4);
  void *proc_run();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef read_seg* OPERATOR_read_seg;
#define NEW_read_seg new read_seg
#else
typedef void* OPERATOR_read_seg;
void *NEW_read_seg(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1,BOOLEAN_SCORE_STREAM i2,BOOLEAN_SCORE_STREAM i3,UNSIGNED_SCORE_STREAM i4);
#endif
