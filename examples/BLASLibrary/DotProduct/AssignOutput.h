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
} AssignOutput_arg;

class AssignOutput: public ScoreOperator {
public: 
  AssignOutput(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef AssignOutput* OPERATOR_AssignOutput;
#define NEW_AssignOutput new AssignOutput
#else
typedef void* OPERATOR_AssignOutput;
void *NEW_AssignOutput(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1);
#endif
