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
   int i3;
   int i4;
   int i5;
   int i6;
   int i7;
   int i8;
} tdfc_dgemv_arg;

class tdfc_dgemv: public ScoreOperator {
public: 
  tdfc_dgemv(unsigned long i0,unsigned long i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4,DOUBLE_SCORE_STREAM i5,DOUBLE_SCORE_STREAM i6,DOUBLE_SCORE_STREAM i7,DOUBLE_SCORE_STREAM i8,DOUBLE_SCORE_STREAM i9,DOUBLE_SCORE_STREAM i10);
  void *proc_run();
  void get_graphviz_strings();
private: 
  unsigned long cc_M;
  unsigned long cc_N;
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef tdfc_dgemv* OPERATOR_tdfc_dgemv;
#define NEW_tdfc_dgemv new tdfc_dgemv
#else
typedef void* OPERATOR_tdfc_dgemv;
void *NEW_tdfc_dgemv(unsigned long i0,unsigned long i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4,DOUBLE_SCORE_STREAM i5,DOUBLE_SCORE_STREAM i6,DOUBLE_SCORE_STREAM i7,DOUBLE_SCORE_STREAM i8,DOUBLE_SCORE_STREAM i9,DOUBLE_SCORE_STREAM i10);
#endif
