#include "ScoreStream.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "segment_seqr_compose.h"
using namespace std;

ofstream* ScoreOperator::fout=NULL;

int main(int argc, char *argv[]) {

    score_init();

    UNSIGNED_SCORE_SEGMENT contents = NEW_UNSIGNED_SCORE_SEGMENT(16, 16);
//    contents->returnAccess();
//    for(int i=0;i<16;i++) {
//	contents->setData(i,i);
//    }
    UNSIGNED_SCORE_STREAM rddata = NEW_UNSIGNED_SCORE_STREAM(16);

    NEW_segment_seqr_compose(contents, rddata);
    
    int index=0;
    while(1) {
    	if(!STREAM_EMPTY(rddata)) {
//	  cout << "Index= " << index++ << " Data= " << STREAM_READ_NOACC(rddata) << endl;
	  STREAM_READ_NOACC(rddata);
	}
    }

    score_exit();
}

