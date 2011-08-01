#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ScoreStream.h>
#include "tdfc_vadd.h"

using namespace std;

int main() {
	score_init();
	
        DOUBLE_SCORE_STREAM x=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM y=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM z=NEW_DOUBLE_SCORE_STREAM();
	
	NEW_tdfc_vadd(x,y,z);

	STREAM_WRITE_DOUBLE(x, 1.0);
	STREAM_WRITE_DOUBLE(x, 2.0);
	STREAM_WRITE_DOUBLE(x, 3.0);
	STREAM_WRITE_DOUBLE(x, 3.0);
        FRAME_CLOSE(x);	
	
	STREAM_WRITE_DOUBLE(y, 2.0);
	STREAM_WRITE_DOUBLE(y, 3.0);
	STREAM_WRITE_DOUBLE(y, 4.0);
	STREAM_WRITE_DOUBLE(y, 5.0);
        FRAME_CLOSE(y);	

       cout<<"About to get Result"<<endl;
	bool done=false;
	while(!done) {
		done=STREAM_EOS(z);
		cout << "z=" << STREAM_READ_DOUBLE(z) << endl;
	}

	cout<<"Exiting"<<endl;
	score_exit();
}
