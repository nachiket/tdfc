#include "ScoreStream.h"
#include <cstdlib>
#include <cstdio>

#include "add.h"

int main() {

	score_init();

	UNSIGNED_SCORE_STREAM a = NEW_UNSIGNED_SCORE_STREAM(8);
	UNSIGNED_SCORE_STREAM b = NEW_UNSIGNED_SCORE_STREAM(8);
	UNSIGNED_SCORE_STREAM c = NEW_UNSIGNED_SCORE_STREAM(8);
	cout << "What?" << endl;
	
	NEW_add(a,b,c);

	STREAM_WRITE_NOACC(a,1.0);
	STREAM_WRITE_NOACC(a,1.0);
	STREAM_WRITE_NOACC(b,2.0);
	STREAM_WRITE_NOACC(b,3.0);

	while(!STREAM_EOS(c)) {
		if(!STREAM_EMPTY(c)) {
			double c_val=STREAM_READ_NOACC(c);
			cout << "c_val=" << c_val << endl;
		}
	}
	
	STREAM_CLOSE(a);
	STREAM_CLOSE(b);

	score_exit();
}
