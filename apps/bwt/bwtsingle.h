// cctdfc autocompiled header file
// tdfc version 1.160
// Fri Apr 16 09:58:58 2010

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
} bwtsingle_arg;

class bwtsingle: public ScoreOperator {
public: 
  bwtsingle(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_SEGMENT i1,SIGNED_SCORE_SEGMENT i2,SIGNED_SCORE_SEGMENT i3,SIGNED_SCORE_SEGMENT i4,BOOLEAN_SCORE_SEGMENT i5,BOOLEAN_SCORE_SEGMENT i6,UNSIGNED_SCORE_STREAM i7);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef bwtsingle* OPERATOR_bwtsingle;
#define NEW_bwtsingle new bwtsingle
#else
typedef void* OPERATOR_bwtsingle;
void *NEW_bwtsingle(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_SEGMENT i1,SIGNED_SCORE_SEGMENT i2,SIGNED_SCORE_SEGMENT i3,SIGNED_SCORE_SEGMENT i4,BOOLEAN_SCORE_SEGMENT i5,BOOLEAN_SCORE_SEGMENT i6,UNSIGNED_SCORE_STREAM i7);
#endif
