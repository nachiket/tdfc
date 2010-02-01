#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <ScoreStream.h>

#include "add.h"

using namespace std;

int main() {
	score_init();
	
	DOUBLE_SCORE_STREAM a=NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM b=NEW_DOUBLE_SCORE_STREAM();
	BOOLEAN_SCORE_STREAM c=NEW_BOOLEAN_SCORE_STREAM();
	DOUBLE_SCORE_STREAM d=NEW_DOUBLE_SCORE_STREAM();

	NEW_add(a,b,c,d);

	STREAM_WRITE_NOACC(a, 1.0);
	STREAM_WRITE_NOACC(b, 1.0);
	STREAM_WRITE(c, 1);
	STREAM_WRITE_NOACC(a, 1.0);
	STREAM_WRITE_NOACC(b, 1.0);
	STREAM_WRITE(c, 0);

	bool done=false;
	while(!done) {
//		done=STREAM_EOS(c);
		cout << "d=" << STREAM_READ_NOACC(d) << endl;
	}

	
	score_exit();
}
