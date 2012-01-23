// cctdfc autocompiled header file
// tdfc version 1.160
// Thu Apr  7 18:06:00 2011

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
} tdfc_ddot_arg;

class tdfc_ddot: public ScoreOperator {
public: 
  tdfc_ddot(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef tdfc_ddot* OPERATOR_tdfc_ddot;
#define NEW_tdfc_ddot new tdfc_ddot
#else
typedef void* OPERATOR_tdfc_ddot;
void *NEW_tdfc_ddot(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
#endif
