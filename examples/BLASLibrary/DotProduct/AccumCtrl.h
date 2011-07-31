// cctdfc autocompiled header file
// tdfc version 1.160
// Tue Mar  8 20:27:00 2011

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
} AccumCtrl_arg;

class AccumCtrl: public ScoreOperator {
public: 
  AccumCtrl(unsigned long i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4,BOOLEAN_SCORE_STREAM i5);
  void *proc_run();
  void get_graphviz_strings();
private: 
  unsigned long cc_latency;
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef AccumCtrl* OPERATOR_AccumCtrl;
#define NEW_AccumCtrl new AccumCtrl
#else
typedef void* OPERATOR_AccumCtrl;
void *NEW_AccumCtrl(unsigned long i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4,BOOLEAN_SCORE_STREAM i5);
#endif
