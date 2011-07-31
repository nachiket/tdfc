// cctdfc autocompiled header file
// tdfc version 1.160
// Fri May 27 18:23:10 2011

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
} tdfc_vadd_arg;

class tdfc_vadd: public ScoreOperator {
public: 
  tdfc_vadd(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1,UNSIGNED_SCORE_STREAM i2);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef tdfc_vadd* OPERATOR_tdfc_vadd;
#define NEW_tdfc_vadd new tdfc_vadd
#else
typedef void* OPERATOR_tdfc_vadd;
void *NEW_tdfc_vadd(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1,UNSIGNED_SCORE_STREAM i2);
#endif
