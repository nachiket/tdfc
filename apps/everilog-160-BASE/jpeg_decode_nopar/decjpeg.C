#include "ScoreStream.h"
#include "JPEG_dec.h"
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <pthread.h>

#include <unistd.h>


SIGNED_SCORE_STREAM p[8];
UNSIGNED_SCORE_STREAM q;
//SIGNED_SCORE_SEGMENT segwrtab[2];
UNSIGNED_SCORE_SEGMENT dcLtab,dcVtab,acLtab,acVtab;

int ibuffer[512*8];

//ifstream inFile("/project/cs/brass/a/jyehtmp/database/jpg/koala~37.jpg",ios::in );
//ifstream inFile("/project/cs/brass/a/jyehtmp/database/jpg/goldhill.jpg",ios::in );
ifstream *inFile = 0;
ofstream outFile("bscore.pgm",ios::out);


int main(int argc, char* argv[])
{

  if (argc==1) {
    inFile = new ifstream("koala~37.jpg",ios::in);
  } else {
    inFile = new ifstream(argv[1],ios::in);
  }

  unsigned char ch;

  score_init();

  for (int index = 0; index < 16; index++) {
    p[index] = NEW_READ_SIGNED_SCORE_STREAM(9);
  }
  q = NEW_WRITE_UNSIGNED_SCORE_STREAM(8);

  //segwrtab[0] = NEW_SIGNED_SCORE_SEGMENT(64,16);
  //segwrtab[1] = NEW_SIGNED_SCORE_SEGMENT(64,16);
  
  new JPEG_dec(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],q);
  //	       segwrtab[0],segwrtab[1]);

  // scan through input header
  for (int index = 0; index < 328; index++) {
    inFile->get(ch);
    // outFile.put((unsigned char) copyToken);
  }
  
  outFile << "P5" << endl;
  outFile << "512 512" << endl;
  outFile << "255" << endl;

//    for (int index = 0; index < 44529; index++) {
//      inFile.get(ch);
//      STREAM_WRITE(q,(int) ch);
//    }

  while (!inFile->eof()) {
    inFile->get(ch);
    STREAM_WRITE(q,(int) ch);
  }

  STREAM_CLOSE(q);

  for (int index = 0; index < 64; index++) {

    for (int thrIndex = 0; thrIndex < 512; thrIndex++) {
      for (int fyrIndex = 0; fyrIndex < 8; fyrIndex++) {
	STREAM_READ(p[fyrIndex],ibuffer[(thrIndex+(fyrIndex*512))]);
	// reading columns in
      } // fyrIndex
    } // thrIndex

    for (int thrIndex = 0; thrIndex < (512*8); thrIndex++) {
      outFile.put((unsigned char) ibuffer[thrIndex]);
    } // thrIndex

  }

  STREAM_FREE(p[0]); STREAM_FREE(p[1]); STREAM_FREE(p[2]); 
  STREAM_FREE(p[3]); STREAM_FREE(p[4]); STREAM_FREE(p[5]);
  STREAM_FREE(p[6]); STREAM_FREE(p[7]);

  score_exit();
  return(0);

}




