#include "ScoreStream.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "segment_seqrw_compose.h"
using namespace std;

ofstream* ScoreOperator::fout=NULL;

int main(int argc, char *argv[]) {

    score_init();

    UNSIGNED_SCORE_SEGMENT contents = NEW_UNSIGNED_SCORE_SEGMENT(16, 16);
    for(int i=0;i<16;i++) {
	SEGMENT_WRITE(contents, 100+i,i);
    }
    UNSIGNED_SCORE_STREAM rddata = NEW_UNSIGNED_SCORE_STREAM(16);
    UNSIGNED_SCORE_STREAM wrdata = NEW_UNSIGNED_SCORE_STREAM(16);

    NEW_segment_seqrw_compose(contents, rddata, wrdata);
    
    int index=0;
    while(1) {
	    // wait for something to be inserted into rddata
	    while(STREAM_EMPTY(rddata)) {
	    }

	    if(STREAM_EOFR(rddata)) {
		    STREAM_READ_NOACC(rddata);
	    } else {

		    long long int data=STREAM_READ_NOACC(rddata);
		    cout << "Read data item=" << data << endl;

		    // wait for non-full condition
		    while(STREAM_FULL(wrdata)) {
		    }

		    data=200+index;
		    STREAM_WRITE_NOACC(wrdata, data);	  
		    cout << "Wrote data item=" << data << endl;
		    index++;	
	    }
    }

    score_exit();
}

