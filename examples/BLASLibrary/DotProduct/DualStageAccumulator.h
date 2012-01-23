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
} DualStageAccumulator_arg;

class DualStageAccumulator: public ScoreOperator {
public: 
  DualStageAccumulator(unsigned long i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
  void *proc_run();
  void get_graphviz_strings();
private: 
  unsigned long cc_LATENCY;
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef DualStageAccumulator* OPERATOR_DualStageAccumulator;
#define NEW_DualStageAccumulator new DualStageAccumulator
#else
typedef void* OPERATOR_DualStageAccumulator;
void *NEW_DualStageAccumulator(unsigned long i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
#endif
