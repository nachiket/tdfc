#include "ScoreStream.h"
#include "topAll.h"
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <pthread.h>

#include <unistd.h>

// 4/8 splits
// {3/4, 2/0, 2/1, 3/5, 3/6, 4/14, 5/30, 6/62, 7/126}
//int hTabDCDiff_Y[] = {772,512,513,773,774,1038,1310,1598,1918};

// {2/0, 2/1, 2/2, 3/6, 4/14, 5/30, 6/62, 7/126, 8/254}
//int hTabDCDiff_C[] = {512,513,514,774,1038,1310,1598,1918,2302};

// but I need some manageable number for debugging.  Also, just one
// 2D-DCT by itself is already very expensive when it is completely
// spatial- an LLM might cost 6 512 lut pages in each dimension....
// I know I'm blowing out any conceivable physical array.

UNSIGNED_SCORE_STREAM pA,pB,pC,pD,uA,uC;
UNSIGNED_SCORE_STREAM q;

unsigned char ibuffer[512*16];

ifstream inmFile("model.mpg",ios::in);
ifstream *inFile=0;
ofstream outFile("mscore.mpg",ios::out);

int main(int argc, char* argv[])
{
  int noPics = 5;
  ifstream *inFile = 
    new ifstream("nobile_00_04.yuv",ios::in );

#if 0
  // ifstream inFile("/project/cs/brass/a/jyehtmp/cifseq/mobile/nobile_00_04.yuv",ios::in );
  int noPics;

  if (argc<2) {
    printf("Usage: score_mpeg <no. pictures> (images file)\n");
    exit(1);
  } else {
    noPics = atoi(argv[1]);
  }

  if (argc<3) {
    inFile = new ifstream("/project/cs/brass/a/jyehtmp/cifseq/mobile/nrobile_00_04.yuv",ios::in );
  } else {
    inFile = new ifstream(argv[2],ios::in);
  }
#endif

  unsigned char copyToken;
  unsigned long long wToken,zToken;

  score_init();

  pA = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);
  pB = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);
  pC = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);
  pD = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);
  uA = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);
  uC = NEW_WRITE_UNSIGNED_SCORE_STREAM(64);

  q = NEW_READ_UNSIGNED_SCORE_STREAM(8);


  printf("Got here alpha\n");

  new topAll(noPics,352*288,352,pA,pB,pC,pD,uA,uC,q);
	     //#include "segInst.txt"
  //segDCTtab[0],segDCTtab[1],acLVtab,mvTab,pattTab,q);

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
	if (bigIndex%2) {
	  STREAM_WRITE(pC,wToken); STREAM_WRITE(pD,zToken); 
	} else {
	  STREAM_WRITE(pA,wToken); STREAM_WRITE(pB,zToken); 
	}
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
	  if (bigIndex%2) {
	    STREAM_WRITE(uC,wToken);
	  } else {
	    STREAM_WRITE(uA,wToken);
	  }
	}
	for (int fivIndex = 0; fivIndex < 8; fivIndex++) {
	  wToken = 0;
	  for (int fyrIndex = 0; fyrIndex < 8; fyrIndex++) {
	    wToken = (wToken<<8)+ibuffer[(thrIndex*16+fivIndex+8+(fyrIndex*352))];
	  } // fyrIndex
	  if (bigIndex%2) {
	    STREAM_WRITE(uC,wToken);
	  } else {
	    STREAM_WRITE(uA,wToken);
	  }
	}
      } // thrIndex
      printf("Done with loop %d\n",index);
    }
  }

  printf("Done with stream writes\n");

  STREAM_CLOSE(uA); STREAM_CLOSE(uC);
  STREAM_CLOSE(pA); STREAM_CLOSE(pB); STREAM_CLOSE(pC); STREAM_CLOSE(pD);

  int counter = 0;

  while (1) {
    //    int index = 0;

    int res;
    if (STREAM_EOS(q)) {break;}
    STREAM_READ(q,res);
    outFile.put((unsigned char) res);
    //    cout << "output " << res << endl;
    //    index++;

    if (( (counter ++) % 1000) == 0) {
      cout << counter << " ";
    }
  }

  // sequence ending code
  outFile.put(0); outFile.put(0);
  outFile.put(1); outFile.put(0xb7);



  STREAM_FREE(q); outFile << flush;

  score_exit();

  return(0);

}
