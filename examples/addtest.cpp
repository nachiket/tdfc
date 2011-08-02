#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <ScoreStream.h>

#include "add.h"

using namespace std;

int main() {
	score_init();
	
	// stream definitions
	DOUBLE_SCORE_STREAM a=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM b=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM c=NEW_DOUBLE_SCORE_STREAM();

	// instantiating the ADDER..
	NEW_add(a,b,c);

	// test inputs
	STREAM_WRITE_NOACC(a, 1.0);
	STREAM_WRITE_NOACC(b, 1.0);
	
	STREAM_WRITE_NOACC(a, 2.0);
	STREAM_WRITE_NOACC(b, 3.0);

	sleep(1);
	
	bool done=false;
	while(!done) {
		done=STREAM_EOS(c);
		cout << "c=" << STREAM_READ_NOACC(c) << endl;
	}

	score_exit();
}
