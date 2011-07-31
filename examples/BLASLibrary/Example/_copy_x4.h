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
} _copy_x4_arg;

class _copy_x4: public ScoreOperator {
public: 
  _copy_x4(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef _copy_x4* OPERATOR__copy_x4;
#define NEW__copy_x4 new _copy_x4
#else
typedef void* OPERATOR__copy_x4;
void *NEW__copy_x4(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2,DOUBLE_SCORE_STREAM i3,DOUBLE_SCORE_STREAM i4);
#endif
