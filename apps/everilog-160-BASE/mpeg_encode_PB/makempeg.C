#include "ScoreStream.h"
#include "topAll.h"
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <pthread.h>

#include <unistd.h>

UNSIGNED_SCORE_STREAM pA,pB,uA;
UNSIGNED_SCORE_STREAM q;

//SIGNED_SCORE_SEGMENT segDCTtab[2];

//UNSIGNED_SCORE_SEGMENT acLVtab,mvTab,pattTab;

unsigned char ibuffer[512*16];

ifstream inmFile("model.mpg",ios::in);
ifstream *inFile=0;
ofstream outFile("mscore.mpg",ios::out);

int main(int argc, char* argv[])
{

  int noPics;

  if (argc<2) {
#if 0
    printf("Usage: score_mpeg <no. pictures> (images file)\n");
    exit(1);
#endif
    noPics = 7;
  } else {
    noPics = atoi(argv[1]);
  }

  if (argc<3) {
   // inFile = new ifstream("/project/cs/brass/a/jyehtmp/cifseq/mobile/mobileR0039.yuv",ios::in );
    inFile = new ifstream("mobileR0039.yuv",ios::in );
  } else {
    inFile = new ifstream(argv[2],ios::in);
  }

  unsigned char copyToken;
  unsigned long long wToken,zToken;

  score_init();

  pA = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);
  pB = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);

  uA = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);
  //  uC = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);

  q = NEW_READ_UNSIGNED_SCORE_STREAM(8);

//  acLVtab = NEW_UNSIGNED_SCORE_SEGMENT(112,12);
//    mvTab = NEW_UNSIGNED_SCORE_SEGMENT(17,9);
//    pattTab = NEW_UNSIGNED_SCORE_SEGMENT(64,9);
//    unsigned long long *copyAC = (unsigned long long *) acLVtab->data();
//    unsigned long long *copyMV = (unsigned long long *) mvTab->data();
//    unsigned long long *copyP = (unsigned long long *) pattTab->data();

//    for (int index = 0; index < 112; index++) {
//      copyAC[index] = (unsigned long long) hTabACRL_LV[index];
//    }
//    for (int index = 0; index < 17; index++) {
//      copyMV[index] = (unsigned long long) hTabMVvals[index];
//    }
//    for (int index = 0; index < 64; index++) {
//      copyP[index] = (unsigned long long) hTabPatt[index];
//    }

  printf("Got here alpha\n");

  new topAll(noPics,352*288,352,pA,pB,uA,q);
	     //#include "segInst.txt"
  //acLVtab,mvTab,pattTab,q);

  printf("Got here beta\n");
  
  // write seq, gop headers out to file 
  for (int index = 0; index < 108; index++) {
    inmFile.get(copyToken);
    outFile.put((unsigned char) copyToken);
  }
  
  for (int bigIndex = 0; bigIndex < noPics; bigIndex++) {
    for (int index = 0; index < 18; index++) {
      for (int thrIndex = 0; thrIndex < (352*16); thrIndex++) {
	inFile->get(ibuffer[thrIndex]);
      } // thrIndex
      for (int thrIndex = 0; thrIndex < 352; thrIndex++) {
	wToken = 0; zToken  = 0;
	for (int fyrIndex = 0; fyrIndex < 8; fyrIndex++) {
	  wToken = (wToken<<8)+ibuffer[(thrIndex+(fyrIndex*352))];
	  zToken = (zToken<<8)+ibuffer[(thrIndex+(fyrIndex*352))+352*8];
	} //fyrIndex
	STREAM_WRITE(pA,wToken); STREAM_WRITE(pB,zToken);
#if 0
	if (bigIndex%2) {
	  STREAM_WRITE(pC,wToken); STREAM_WRITE(pD,zToken); 
	} else {
	  STREAM_WRITE(pA,wToken); STREAM_WRITE(pB,zToken); 
	}
#endif

      } // thrIndex
      printf("Done with loop %d\n",index);
    }
    for (int index = 0; index < 18; index++) {
      for (int thrIndex = 0; thrIndex < (176*16); thrIndex++) {
	inFile->get(ibuffer[thrIndex]);
      } // thrIndex
      for (int thrIndex = 0; thrIndex < 22; thrIndex++) {
	for (int fivIndex = 0; fivIndex < 8; fivIndex++) {
	  wToken = 0;
	  for (int fyrIndex = 0; fyrIndex < 8; fyrIndex++) {
	    wToken = (wToken<<8)+ibuffer[(thrIndex*16+fivIndex+(fyrIndex*352))];
	  } // fyrIndex
	  STREAM_WRITE(uA,wToken);
#if 0
	  if (bigIndex%2) {
	    STREAM_WRITE(uC,wToken);
	  } else {
	    STREAM_WRITE(uA,wToken);
	  }
#endif
	}
	for (int fivIndex = 0; fivIndex < 8; fivIndex++) {
	  wToken = 0;
	  for (int fyrIndex = 0; fyrIndex < 8; fyrIndex++) {
	    wToken = (wToken<<8)+ibuffer[(thrIndex*16+fivIndex+8+(fyrIndex*352))];
	  } // fyrIndex
	  STREAM_WRITE(uA,wToken);
#if 0
	  if (bigIndex%2) {
	    STREAM_WRITE(uC,wToken);
	  } else {
	    STREAM_WRITE(uA,wToken);
	  }
#endif
	}
      } // thrIndex
      printf("Done with loop %d\n",index);
    }
  }

  printf("Done with stream writes\n");

  STREAM_CLOSE(uA); //STREAM_CLOSE(uC);
  STREAM_CLOSE(pA); STREAM_CLOSE(pB); //STREAM_CLOSE(pC); STREAM_CLOSE(pD);

  printf("Closed all input streams\n");

  int index = 0;

  while (1) {
    int res;
    if (STREAM_EOS(q)) {break;}
    STREAM_READ(q,res);
    outFile.put((unsigned char) res);

    if ((index % 100) == 0) {
      cout << index << " -- output " << res << endl;
    }
    index++;
  }

  // sequence ending code
  outFile.put(0); outFile.put(0);
  outFile.put(1); outFile.put(0xb7);



  STREAM_FREE(q); outFile << flush;

  score_exit();

  return(0);

}
