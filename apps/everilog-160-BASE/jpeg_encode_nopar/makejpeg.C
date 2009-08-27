#include "ScoreStream.h"
#include "JPEG_mod.h"
#include <iostream.h>
#include <fstream.h>
#include <strings.h>
#include <pthread.h>

#include <unistd.h>


int hTabDCDiff_L[] = {2,3,3,3,3,3,4,5,6,7,8,9};

int hTabDCDiff_V[] = {0,2,3,4,5,6,14,30,62,126,254,510};

int hTabACRL_L[] = 
    { 4,  2,  2,  3,  4,  5,  7,  8, 10, 16, 16,  0,  0,  0,  0,  0,
     0,  4,  5,  7,  9, 11, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  5,  8, 10, 12, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  6,  9, 12, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  6, 10, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  7, 11, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  7, 12, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  8, 12, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  9, 15, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  9, 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0,  9, 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0, 10, 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0, 11, 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
     0, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0,
    11, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  0,  0,  0,  0,  0};

int hTabACRL_V[] =
    {10,    0,     1,     4,    11,    26,   120,  248,
     1014, 65410, 65411,0,0,0,0,0,
     0,    12,    27,   121,   502,  2038, 65412, 65413,
     65414, 65415, 65416,0,0,0,0,0,
     0,    28,   249,  1015,  4084, 65417, 65418, 65419,
     65420, 65421, 65422,0,0,0,0,0,
     0,    58,   503,  4085, 65423, 65424, 65425, 65426,
     65427, 65428, 65429,0,0,0,0,0,
     0,    59,  1016, 65430, 65431, 65432, 65433, 65434,
     65435, 65436, 65437,0,0,0,0,0,
     0,   122,  2039, 65438, 65439, 65440, 65441, 65442,
     65443, 65444, 65445,0,0,0,0,0,
     0,   123,  4086, 65446, 65447, 65448, 65449, 65450,
     65451, 65452, 65453,0,0,0,0,0,
     0,   250,  4087, 65454, 65455, 65456, 65457, 65458,
     65459, 65460, 65461,0,0,0,0,0,
     0,   504, 32704, 65462, 65463, 65464, 65465, 65466,
     65467, 65468, 65469,0,0,0,0,0,
     0,   505, 65470, 65471, 65472, 65473, 65474, 65475,
     65476, 65477, 65478,0,0,0,0,0,
     0,   506, 65479, 65480, 65481, 65482, 65483, 65484,
     65485, 65486, 65487,0,0,0,0,0,
     0,  1017, 65488, 65489, 65490, 65491, 65492, 65493,
     65494, 65495, 65496,0,0,0,0,0,
     0,  1018, 65497, 65498, 65499, 65500, 65501, 65502,
     65503, 65504, 65505,0,0,0,0,0,
     0,  2040, 65506, 65507, 65508, 65509, 65510, 65511,
     65512, 65513, 65514,0,0,0,0,0,
     0, 65515, 65516, 65517, 65518, 65519, 65520, 65521,
     65522, 65523, 65524,0,0,0,0,0,
     2041, 65525, 65526, 65527, 65528, 65529, 65530, 65531,
     65532, 65533, 65534,0,0,0,0,0} ;

// but I need some manageable number for debugging.  Also, just one
// 2D-DCT by itself is already very expensive when it is completely
// spatial- an LLM might cost 6 512 lut pages in each dimension....
// I know I'm blowing out any conceivable physical array.

SIGNED_SCORE_STREAM p[8];
UNSIGNED_SCORE_STREAM q;
//SIGNED_SCORE_SEGMENT segwrtab[2];
UNSIGNED_SCORE_SEGMENT dcLtab,dcVtab,acLtab,acVtab;

unsigned char ibuffer[512*8];

//ifstream inFile("/project/cs/brass/a/jyehtmp/database/raw/barbara.raw",ios::in );
ifstream *inFile = 0;
ifstream inmFile("model.jpg",ios::in);
ofstream outFile("bscore.jpg",ios::out);


int main(int argc, char* argv[])
{

  unsigned long long token, caster;
  unsigned char copyToken;

  if (argc==1) {
    inFile = new ifstream("barbara.raw",ios::in );
  } else {
    inFile = new ifstream(argv[1],ios::in);
  }

  score_init();

  for (int index = 0; index < 8; index++) {
    p[index] = NEW_WRITE_SIGNED_SCORE_STREAM(9);
  }
  q = NEW_READ_UNSIGNED_SCORE_STREAM(8);

  //segwrtab[0] = NEW_SIGNED_SCORE_SEGMENT(64,16);
  //segwrtab[1] = NEW_SIGNED_SCORE_SEGMENT(64,16);
  dcLtab = NEW_UNSIGNED_SCORE_SEGMENT(12,8);
  dcVtab = NEW_UNSIGNED_SCORE_SEGMENT(12,16);
  acLtab = NEW_UNSIGNED_SCORE_SEGMENT(256,8);
  acVtab = NEW_UNSIGNED_SCORE_SEGMENT(256,16);

  unsigned long long *copyLen = (unsigned long long *) dcLtab->data();
  unsigned long long *copyVal = (unsigned long long *) dcVtab->data();

  for (int index = 0; index < 12; index++) {
    copyLen[index] = (unsigned long long) hTabDCDiff_L[index];
    copyVal[index] = (unsigned long long) hTabDCDiff_V[index]; 
  }

  copyLen = (unsigned long long *) acLtab->data();
  copyVal = (unsigned long long *) acVtab->data();

  for (int index = 0; index < 256; index++) {
    copyLen[index] = (unsigned long long) hTabACRL_L[index];
    copyVal[index] = (unsigned long long) hTabACRL_V[index]; 
  }
  
  new JPEG_mod(p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],q,
	       dcLtab,dcVtab,acLtab,acVtab);

  // write header out to file 
  for (int index = 0; index < 328; index++) {
    inmFile.get(copyToken);
    outFile.put((unsigned char) copyToken);
  }
  

  for (int index = 0; index < 64; index++) {
    for (int thrIndex = 0; thrIndex < (512*8); thrIndex++) {
      inFile->get(ibuffer[thrIndex]);
    } // thrIndex
    for (int thrIndex = 0; thrIndex < 512; thrIndex++) {
      // cout << "hi!" << endl;
      for (int fyrIndex = 0; fyrIndex < 8; fyrIndex++) {
	STREAM_WRITE(p[fyrIndex],ibuffer[(thrIndex+(fyrIndex*512))]);
	// cout << (ibuffer[(thrIndex+(fyrIndex*512))]) << endl;
	// writing columns in
      } // fyrIndex
    } // thrIndex
    //printf("Done with loop %d\n",index);
  }

  printf("Done with stream writes\n");

  for (int index = 0; index < 8; index++) {
    STREAM_CLOSE(p[index]);
  }

  while (1) {
    int index = 0;
    int hello = 0;
    int res;
    if (STREAM_EOS(q)) {break;}
    //    cout << "consumer: before read " << index << endl;
    STREAM_READ(q,res);
    //    cout << "Res is " << res << endl;
    if ((hello == 1) && (res != 0)) { cout << "Seq error " << res << endl;}
    if (res == 255) {hello = 1;} else {hello = 0;}
    // putInt(&outac1File, res);
    outFile.put((unsigned char) res);
    //    cout << "consumer: after  read " << index << endl;
    index++;
  }


  outFile.put(255); outFile.put(0xd9);

  STREAM_FREE(q); outFile << flush;

  score_exit();

  return(0);

}
