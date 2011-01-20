#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <ScoreStream.h>

#include "dotProduct.h"

using namespace std;

int main() {
	score_init();
	
        DOUBLE_SCORE_STREAM a=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM b=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM d=NEW_DOUBLE_SCORE_STREAM();

	NEW_dotProduct(a,b,d);

	STREAM_WRITE_DOUBLE(a, 1.0);
	STREAM_WRITE_DOUBLE(a, 2.0);
	STREAM_WRITE_DOUBLE(a, 3.0);
        FRAME_CLOSE(a);	
	
//	STREAM_WRITE_DOUBLE(a, 4.0);
//	STREAM_WRITE_DOUBLE(a, 5.0);
        
        STREAM_WRITE_DOUBLE(b, 1.0);
	STREAM_WRITE_DOUBLE(b, 2.0);
	STREAM_WRITE_DOUBLE(b, 3.0);
	FRAME_CLOSE(b);	

//	STREAM_WRITE_DOUBLE(b, 4.0);
//	STREAM_WRITE_DOUBLE(b, 5.0);

       cout<<"About to get Result"<<endl;
	bool done=false;
	while(!done) {
		done=STREAM_EOS(d);
		cout << "d=" << STREAM_READ_DOUBLE(d) << endl;
	}

	
	score_exit();
}
