#include "Score.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "segment_leaf.h"
#include "segment_compose.h"
using namespace std;

ofstream* ScoreOperator::fout=NULL;

int main(int argc, char *argv[]) {

    score_init();

    UNSIGNED_SCORE_SEGMENT contents = NEW_UNSIGNED_SCORE_SEGMENT(256, 16);
//    unsigned long long *data = (unsigned long long *)GET_SEGMENT_DATA(contents);
//    cout << "Data pointer=" << data << endl;
//    for(int i=0;i<256;i++) {
//	   data[i]=256;
//    }
    for(int i=0;i<256;i++) {
      SEGMENT_WRITE(contents, 30001, i);
    }

    NEW_segment_compose(contents);

    while(1) {
    }

    score_exit();
}

