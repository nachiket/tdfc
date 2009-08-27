// ADPCM encoder (16-->4 bits)
// Adapted from UCLA MediaBench Intel/DVI ADPCM encoder
// Based on CS262 project code (Eylon Caspi, 11/23/98)
//
// Eylon Caspi, 4/5/00
//
// 8 pages, ~35-slow
//
// "#define NIBBLE" to output NIBBLE mode, else outputs byte mode


#include "Score.h"
#include "AdpcmEnc.h"


main ()
{
  unsigned short srcArray[]			// Source array, 16-bit ints
				= {0x0010,
				   0x0001,
				   0x0012,
				   0x0003,	// #4
				   0x0014,
				   0x0005,
				   0x0016,
				   0x0007,	// #8
				   0x0018,
				   0x0009,
				   0x001a,
				   0x000b,	// #12
				   0x001c,
				   0x000d,
				   0x001e,
				   0x000f,	// #16
				   0x0000,
				   0x1010,
				   0x2345,
				   0x4321	// #20
				};
  int srcSize = sizeof(srcArray)/sizeof(*srcArray);
  #ifdef NIBBLE
  int dstSize = srcSize;
  #else
  int dstSize = (srcSize+1)/2;
  #endif
  unsigned char dstArray[dstSize];

  cerr << "SCORE ADPCM Encoder ";
  #ifdef NIBBLE
  cerr << "(nibble mode)\n";
  #else
  cerr << "(byte mode)\n";
  #endif

  cerr << "Initializing SCORE\n";
  score_init();

  cerr << "Initializing streams\n";
  SIGNED_SCORE_STREAM   adpcm_in  = NEW_WRITE_SIGNED_SCORE_STREAM(16);
  UNSIGNED_SCORE_STREAM adpcm_out = NEW_READ_UNSIGNED_SCORE_STREAM(4);
  // SIGNED_SCORE_STREAM   adpcm_in  = NEW_SIGNED_SCORE_STREAM(16);
  // UNSIGNED_SCORE_STREAM adpcm_out = NEW_UNSIGNED_SCORE_STREAM(4);

  cerr << "Initializing operator\n";
  NEW_AdpcmEnc(adpcm_in,adpcm_out);

  cerr << "Writing to operator\n";
  int i;
  for (i=0; i<srcSize; i++)
    STREAM_WRITE(adpcm_in,srcArray[i]);
  STREAM_CLOSE(adpcm_in);

  cerr << "Reading from operator\n";
  for (i=0; i<dstSize && !STREAM_EOS(adpcm_out); i++)
  {
    STREAM_READ(adpcm_out,dstArray[i]);
    #ifdef NIBBLE
    cout << (int)dstArray[i] << '\n';
    #else
    cout << hex << setw(2) << (int)dstArray[i] << '\n';
    #endif
  }
  if (i!=dstSize || !STREAM_EOS(adpcm_out))
    cerr << "Output not finished  (got " << i << " outputs)\n";
  STREAM_FREE(adpcm_out);

  cerr << "Exiting SCORE\n";
  score_exit();

  cerr << "Done\n";
}
