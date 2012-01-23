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
} CoalesceStage_arg;

class CoalesceStage: public ScoreOperator {
public: 
  CoalesceStage(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef CoalesceStage* OPERATOR_CoalesceStage;
#define NEW_CoalesceStage new CoalesceStage
#else
typedef void* OPERATOR_CoalesceStage;
void *NEW_CoalesceStage(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1);
#endif
