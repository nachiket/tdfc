#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <ScoreStream.h>

#include "add.h"

using namespace std;

int main() {
	score_init();
	cout << "I am here"<<endl;
	DOUBLE_SCORE_STREAM a=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM b=NEW_DOUBLE_SCORE_STREAM();
	BOOLEAN_SCORE_STREAM c=NEW_BOOLEAN_SCORE_STREAM();
	//DOUBLE_SCORE_STREAM d=NEW_BOOLEAN_SCORE_STREAM();
	DOUBLE_SCORE_STREAM d=NEW_DOUBLE_SCORE_STREAM();

	NEW_add(a,b,c,d);

	STREAM_WRITE_NOACC(a, 1.0);
	STREAM_WRITE_NOACC(b, 1.0);
	STREAM_WRITE(c, 1);
	STREAM_WRITE_NOACC(a, 1.0);
	STREAM_WRITE_NOACC(b, 1.0);
	STREAM_WRITE(c, 1);
	STREAM_WRITE_NOACC(a, 1.0);
	STREAM_WRITE_NOACC(b, 1.0);
	STREAM_WRITE(c, 0);
	STREAM_WRITE_NOACC(a, 1.0);
	STREAM_WRITE_NOACC(b, 1.0);
	STREAM_WRITE(c, 0);

//	STREAM_CLOSE(a);
//	STREAM_CLOSE(b);
	sleep(1);
	
	cout << "I am here"<<endl;
	bool done=false;
	while(!done) {
    	        cout << "I am here"<<endl;
		done=STREAM_EOS(d);
		cout<<done<<endl;		
		cout << "d=" << STREAM_READ_NOACC(d) << endl;
	}

	
	score_exit();
}
