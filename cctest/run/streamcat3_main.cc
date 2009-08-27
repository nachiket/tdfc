// streamcat3_main.cc
//  - test driver for streamcat3 (concatenate 3 streams in sequence)
//  - Eylon Caspi 8/27/99

#include "ScoreStream.h"
#include "streamcat3.h"

int main ()
{
  const int numStreams	=3;		// - number of streams to concat
  const int streamLength=4;		// - #elements in each stream
  const int streamWidth	=8;		// - bit width of each stream

  int errors=0;

  // - initialize stream contents as (10 20 30 ...)
  int streamValues[numStreams][streamLength],		// - values to emit
      expect[numStreams*streamLength],			// - values to expect
      val=0, pos=0;
  for (int s=0; s<numStreams; s++)
    for (int i=0; i<streamLength; i++)
      streamValues[s][i] = expect[pos++] = (val+=10);

  // - create streams
  TypedScoreStream<SCORE_STREAM_UNSIGNED_TYPE>* streams[numStreams];
  for (int s=0; s<numStreams; s++)
    streams[s] = NEW_UNSIGNED_SCORE_STREAM(streamWidth);

  // SCORE_STREAM so=streamcat (streamWidth,
  //				streams[0],streams[1]);
     SCORE_STREAM so=streamcat3(streamWidth,
				streams[0],streams[1],streams[2]);
  // SCORE_STREAM so=streamcat4(streamWidth,
  //				streams[0],streams[1],streams[2],streams[3]);

  // - write to input-streams out-of-order
  for (int i=0; i<streamLength; i++)
    for (int s=0; s<numStreams; s++)
      {
	int val=streamValues[s][i];
	cerr << "streamcat3_main: writing " << val
	     << " to stream " << s << '\n';
	STREAM_WRITE(streams[s],val);
      }

  // - close input-streams
  for (int s=0; s<numStreams; s++)
    STREAM_CLOSE(streams[s]);

  // - read return stream
  for (int j=0; j<numStreams*streamLength; j++)
  {
    int ret=STREAM_READ(so);
    cerr << "streamcat3_main:  at position " << j << " got " << ret;
    if (ret!=expect[j])
    {
      cerr << " expected " << expect[j] << '\n';
      errors++;
    }
    else
      cerr << " OK\n";
  }
  
  // - expect EOS on return stream
  while (!STREAM_EOS(so))
  {
    int ret=STREAM_READ(so);
    cerr << "streamcat_main:  expect eos got " << ret << '\n';
    errors++;
  }

  cout << "Done, " << errors << " errors\n";

  // for (int s=0; s<numStreams; s++)
  //   STREAM_FREE(streams[s]);
  STREAM_FREE(so);

  return 0;
}
