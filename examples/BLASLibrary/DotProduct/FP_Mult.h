// cctdfc autocompiled header file
// tdfc version 1.160
// Tue Mar  8 12:54:38 2011

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
} FP_Mult_arg;

class FP_Mult: public ScoreOperator {
public: 
  FP_Mult(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef FP_Mult* OPERATOR_FP_Mult;
#define NEW_FP_Mult new FP_Mult
#else
typedef void* OPERATOR_FP_Mult;
void *NEW_FP_Mult(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
#endif
