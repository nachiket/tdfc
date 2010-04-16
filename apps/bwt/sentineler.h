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
} sentineler_arg;

class sentineler: public ScoreOperator {
public: 
  sentineler(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef sentineler* OPERATOR_sentineler;
#define NEW_sentineler new sentineler
#else
typedef void* OPERATOR_sentineler;
void *NEW_sentineler(UNSIGNED_SCORE_STREAM i0,UNSIGNED_SCORE_STREAM i1);
#endif
