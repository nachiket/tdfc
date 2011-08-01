// cctdfc autocompiled header file
// tdfc version 1.160
// Mon Mar  7 19:07:32 2011

#include "Score.h"
#include <iostream>
#include <fstream>
#include <pthread.h>
using namespace std;
#ifdef __cplusplus
typedef struct {
   int i0;
   int i1;
   int i2;
   int i3;
   int i4;
} ddot_arg;

class ddot: public ScoreOperator {
public: 
  ddot(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef ddot* OPERATOR_ddot;
#define NEW_ddot new ddot
#else
typedef void* OPERATOR_ddot;
void *NEW_ddot(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4);
#endif
