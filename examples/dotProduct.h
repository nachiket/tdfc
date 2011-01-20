// cctdfc autocompiled header file
// tdfc version 1.160
// Tue Jan 18 16:43:50 2011

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
} dotProduct_arg;

class dotProduct: public ScoreOperator {
public: 
  dotProduct(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
  void *proc_run();
  void get_graphviz_strings();
private: 
  pthread_t rpt;
  static ScoreOperatorElement *instances;
};
typedef dotProduct* OPERATOR_dotProduct;
#define NEW_dotProduct new dotProduct
#else
typedef void* OPERATOR_dotProduct;
void *NEW_dotProduct(DOUBLE_SCORE_STREAM i0,DOUBLE_SCORE_STREAM i1,DOUBLE_SCORE_STREAM i2);
#endif
