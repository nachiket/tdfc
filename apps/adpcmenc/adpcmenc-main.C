// ADPCM encoder (16-->4 bits)
// Adapted from UCLA MediaBench Intel/DVI ADPCM encoder
// Based on CS262 project code (Eylon Caspi, 11/23/98)
//
// Eylon Caspi, 4/5/00
//
// 8 pages, ~35-slow
//
// Reads 16-bit ints from stdin, writes byte-packed nibbles to stdout
// (NIBBLE mode not supported).


#include "Score.h"
#include "AdpcmEnc.h"


#define	NSAMPLES 1000			// #samples in input buffer (even)

#define	DSAMPLES ((NSAMPLES+1)/2)	// #samples in output buffer


main ()
{
  short         srcbuf[NSAMPLES];
  unsigned char dstbuf[DSAMPLES];

  cerr << "-- SCORE ADPCM Encoder\n";

  cerr << "-- Initializing SCORE\n";
  score_init();

  cerr << "-- Initializing streams\n";
  SIGNED_SCORE_STREAM   adpcm_in  = NEW_WRITE_SIGNED_SCORE_STREAM(16);
  UNSIGNED_SCORE_STREAM adpcm_out = NEW_READ_UNSIGNED_SCORE_STREAM(8);

  cerr << "-- Initializing operator\n";
  NEW_AdpcmEnc(adpcm_in,adpcm_out);

  int done=0, nsamples_total=0, dsamples_total=0;
  while (!done)
  {
    int nsamples = (read(0,srcbuf,NSAMPLES*2)+1)/2;	// - shorts from input
    int dsamples = (nsamples+1)/2;			// - chars to output
    if (nsamples<0)
    {
      // - error
      cerr << "-- error reading input file\n";
      done=1;
    }
    else if (nsamples==0)
    {
      // - no more file input
      cerr << "-- Closing stream to operator\n";
      STREAM_CLOSE(adpcm_in);
      done=1;
    }
    else
    {
      // - got file input
      cerr << "-- Writing samples " << nsamples_total << "-"
	   << nsamples_total+nsamples-1 << " to operator\n";
      int i;
      for (i=0; i<nsamples; i++, nsamples_total++)
	STREAM_WRITE(adpcm_in,srcbuf[i]);
      if (nsamples<NSAMPLES)
      {
	// - no more file input
	cerr << "-- Closing stream to operator\n";
	STREAM_CLOSE(adpcm_in);
	done=1;
      }

      cerr << "-- Reading outputs " << dsamples_total << "-"
	   << dsamples_total+dsamples-1 << " from operator\n";
      for (i=0; i<dsamples && !STREAM_EOS(adpcm_out); i++, dsamples_total++)
      {
	STREAM_READ(adpcm_out,dstbuf[i]);
	// cout << hex << setw(2) << (int)dstbuf[i] << '\n';	// - ascii
	cout << dstbuf[i];					// - binary
      }
      if (i<dsamples)
	cerr << "-- Too little output (got " << i << " outputs)\n";
    }
  }

  if (!STREAM_EOS(adpcm_out))
    cerr << "-- Superfluous output at end\n";

  cerr << "-- Freeing stream from operator\n";
  STREAM_FREE(adpcm_out);

  cerr << "-- Exiting SCORE\n";
  score_exit();

  cerr << "-- Done\n";
}
