#include "ScoreStream.h"
#include <cstdlib>
#include <cstdio>

#include "add8.h"

int main() {

	score_init();

	DOUBLE_SCORE_STREAM a = NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM b = NEW_DOUBLE_SCORE_STREAM();
	DOUBLE_SCORE_STREAM c = NEW_DOUBLE_SCORE_STREAM();
	cout << "What?" << endl;
	
	NEW_add8(a,b,c);

	STREAM_WRITE_DOUBLE(a,1.0);
	STREAM_WRITE_DOUBLE(a,1.0);
	STREAM_WRITE_DOUBLE(b,2.0);
	STREAM_WRITE_DOUBLE(b,3.0);
	STREAM_CLOSE(a);
	STREAM_CLOSE(b);

	while(!STREAM_EOS(c)) {
		if(!STREAM_EMPTY(c)) {
			double c_val=STREAM_READ_DOUBLE(c);
			cout << "c_val=" << c_val << endl;
		}
	}
	
	STREAM_CLOSE(a);
	STREAM_CLOSE(b);

	score_exit();
}
