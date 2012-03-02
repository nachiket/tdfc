#include "ScoreStream.h"
#include <cstdlib>
#include <cstdio>

#include "add.h"

int main(int argc, char* argv[]) {

	score_init();

	UNSIGNED_SCORE_STREAM a = NEW_UNSIGNED_SCORE_STREAM(8);
	UNSIGNED_SCORE_STREAM b = NEW_UNSIGNED_SCORE_STREAM(8);
	UNSIGNED_SCORE_STREAM c = NEW_UNSIGNED_SCORE_STREAM(8);
	cout << "Args=" << atoi(argv[1]) << endl;
	
	NEW_add(a,b,c);

	for(int sample=0; sample<atoi(argv[1]);sample++) {
		if(!STREAM_FULL(a)) {
			STREAM_WRITE_NOACC(a,(long long int)sample);
		}
		if(!STREAM_FULL(b)) {
			STREAM_WRITE_NOACC(b,(long long int)sample);
		}
	}

	while(!STREAM_EOS(c)) {
		if(!STREAM_EMPTY(c)) {
			int c_val=(long long int)STREAM_READ_NOACC(c);
			cout << "c_val=" << c_val << endl;
		}
	}
	
	STREAM_CLOSE(a);
	STREAM_CLOSE(b);

	score_exit();
}
