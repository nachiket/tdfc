/* Original copyright information: */

/***************************************************************************/
/*                                                                         */
/*  Copyright (September 1997) Honeywell Inc. Unpublished - All rights     */
/*  reserved. This material may be reproduced by, or for, the U.S.         */
/*  Government pursuant to the copyright license under the clause at       */
/*  DFARS 252.227-7013 (Oct. 1988).                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************
 *
 * compressimage
 * Version 1.0
 *
 * File         : compress.c
 * Company      : Honeywell Technology Center, Minneapolis, MN 55412
 * Date         : November 10 1997
 * Authors      : Luiz Pires and Deepa Pandalai
 *
/***************************************************************************/

/* Modifications made by Joseph Yeh, U.C. Berkeley */

/***************************************************************************

// Copyright (c) 1999 The Regents of the University of California 
// Permission to use, copy, modify, and distribute this software and
// its documentation for any purpose, without fee, and without a
// written agreement is hereby granted, provided that the above copyright 
// notice and this paragraph and the following two paragraphs appear in
// all copies. 
//
// IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
// DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
// LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE. 
//
// THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON
// AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
// PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 

/***************************************************************************/

#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<time.h>
#include "Score.h"

#define NBLKS 10
#define BLKS_SAVED 7

#define IMG_SIZE 262146 

#define BLOCK_SIZE 128
#define ZERO_MARK 30

int min(a,b) {return ((a<b)?(a):(b));}
int max(a,b) {return ((a>b)?(a):(b));}

/* I/O Variables */
#define DEFAULT_CRATIO  128         /* Default Compression Level */
char in_name[80],out_name[80];
int img_size, nbytes, cratio;

/* Wavelet variables */
#define ROW 1
#define COL 512
int int_data[IMG_SIZE];

void reverse(char s[])
{

  int c,i,j;

  for (i=0, j = strlen(s)-1; i < j; i++, j--) {
      c = s[i];
      s[i] = s[j];
      s[j] = c;
  }
}


void itoa(int n, char s[])
{
  int i, sign;

  if ((sign = n) < 0)
    n = -n;

  i = 0;
  do { /* generate digits in reverse order */
       s[i++] = n % 10 + '0';
     } while ((n /= 10) > 0);

  if (sign < 0)
    s[i++] = '-';
  s[i] = '\0';

  reverse(s);
}


/* Quantization variables */
/* 
   The image will be divided into 10 blocks, the first four will be of
   size 64x64 pixels (4096 pixels), then three of size 128x128 (16384 pixels)
   and the remaining three of size 256x256 pixels (65536 pixels).
   These blocks will be arranged as represented below:

   +---+---+-------+---------------+
   | 4K| 4K|       |               |
   +---+---|  16K  |               |
   | 4K| 4K|       |               |
   +---+---+-------+      65K      |
   |       |       |               |
   |  16K  |  16K  |               |
   |       |       |               |
   +-------+-------+---------------+
   |               |               |
   |               |               |
   |               |               |
   |      65K      |      65K      |
   |               |               |
   |               |               |
   |               |               |
   +---------------+---------------+
*/
int blocksize[NBLKS]={4096,4096,4096,4096,16384,16384,16384,65536,65536,65536};
int blockthresh[NBLKS]= {0, 39, 27, 104, 79, 50,191, 99999, 99999, 99999};
int blockminthresh[NBLKS]= {0, 10, 10, 30, 20, 15, 60, 10000,10000,10000};
/* from barbara */
int blockmaxval[NBLKS] = {256, 512, 512, 2048, 512,  512, 1024, 0, 0, 0};
int blockrange[NBLKS];
unsigned char dc_vals[2560];
/* RLE variables */
unsigned char rle_buf[NBLKS][IMG_SIZE];
unsigned int rle_size[NBLKS];


/* Huffman Encoding variables */
unsigned char codep[IMG_SIZE];  

/* Parameters for a fixed huffman encoding tree. These parameters are  */
/* expected to work reasonably well with most images.                  */

int Huff[255][2] =

/* N.B: 255 is not used because in the original CS262 based 
   implementation, it was reserved as a symbol for the run-length
   page to tell the Huffman coder that it was done sending symbols. */

{{     0,     3},{     6,     4},{     9,     5},{    14,     6},
{    26,     7},{    23,     7},{    46,     8},{   111,     8},
{   290,     9},{   495,     9},{   490,    10},{   665,    10},
{   527,    10},{  2041,    11},{   505,    10},{     4,     3},
{     1,     4},{    30,     5},{    10,     6},{     2,     7},
{    57,     7},{    66,     8},{     7,     8},{   354,     9},
{   430,     9},{   135,     9},{   450,    10},{    87,     9},
{  1218,    11},{    25,     9},{     3,     3},{     5,     4},
{    18,     5},{    13,     5},{    31,     5},{    58,     6},
{    61,     6},{    55,     6},{    42,     7},{   110,     7},
{    29,     7},{   103,     7},{    39,     7},{   162,     8},
{   106,     8},{    90,     8},{   218,     8},{   121,     8},
{    93,     8},{   215,     8},{    79,     8},{    34,     9},
{   143,     8},{   482,     9},{   221,     9},{   327,     9},
{   239,     9},{   175,     9},{   207,     9},{   738,    10},
{   793,    10},{   409,    10},{   455,    10},{    98,    10},
{   153,    10},{   761,    10},{   706,    11},{  1002,    10},
{   234,    10},{   391,    10},{    71,    10},{   943,    10},
{   962,    11},{   967,    10},{   903,    10},{    15,    10},
{   583,    10},{  1634,    11},{  1250,    11},{  1730,    11},
{   249,    11},{   226,    11},{  1295,    11},{   174,    11},
{   194,    12},{  2242,    12},{  1017,    11},{  1223,    11},
{  1583,    11},{  1273,    11},{   199,    11},{    47,    11},
{  1770,    12},{  2013,    12},{  1986,    12},{   855,    11},
{   746,    12},{   711,    11},{  1367,    12},{  1071,    12},
{   271,    11},{  8157,    13},{  2658,    13},{   783,    12},
{   431,    11},{  3353,    13},{  3375,    13},{  2329,    12},
{  8130,    13},{  3818,    12},{  1501,    12},{  3758,    13},
{  4061,    13},{  2969,    12},{  3783,    12},{   989,    13},
{  2511,    12},{  6754,    13},{  1305,    13},{  5423,    13},
{  1198,    13},{  5085,    13},{   921,    12},{  3993,    12},
{  6621,    13},{  4911,    13},{   343,    11},{  1807,    13},
{   686,    13},{  2525,    13},{  1735,    12},{  3246,    14},
{   463,    12},{  1710,    14},{   610,    13},{   815,    13},
{   281,    13},{ 13486,    14},{  1999,    13},{   559,    12},
{  6095,    13},{  4706,    13},{ 11695,    14},{  4377,    13},
{  7133,    13},{   303,    12},{   477,    14},{  4573,    13},
{  3037,    13},{  1945,    12},{ 10926,    14},{  6830,    13},
{  5903,    13},{  4034,    13},{  2794,    12},{  3549,    12},
{  1879,    11},{  3415,    13},{  3023,    13},{  7342,    13},
{ 16143,    14},{  6703,    15},{ 11727,    14},{ 12239,    14},
{ 15119,    14},{  7119,    13},{ 13263,    14},{  7854,    14},
{ 23343,    15},{  5806,    14},{ 24527,    15},{  5583,    15},
{ 13593,    14},{ 14895,    14},{  7511,    13},{ 15151,    14},
{ 15407,    14},{ 30159,    15},{ 11311,    14},{  9647,    14},
{ 15823,    14},{  4782,    14},{  7449,    14},{ 20431,    15},
{ 14639,    14},{  7471,    13},{  5935,    13},{ 17839,    15},
{ 10031,    14},{  9902,    14},{ 12079,    14},{ 18735,    15},
{  8669,    14},{ 19887,    15},{ 10543,    14},{ 16335,    14},
{  1455,    15},{ 11438,    14},{  6927,    15},{  5071,    15},
{  3535,    15},{ 19919,    15},{ 12047,    14},{ 22831,    15},
{ 24335,    15},{ 24367,    15},{ 21935,    15},{  2863,    14},
{ 23311,    15},{  5401,    14},{ 15791,    14},{ 20271,    15},
{ 19503,    15},{ 18223,    15},{ 23599,    15},{ 24015,    15},
{ 21455,    15},{ 23983,    15},{ 15641,    14},{ 11023,    14},
{  7631,    15},{ 16046,    14},{  4047,    15},{  5551,    15},
{  1327,    13},{  3119,    15},{ 16175,    14},{ 13775,    15},
{  2734,    14},{ 12974,    14},{ 21967,    15},{  2607,    14},
{  3855,    14},{ 13998,    14},{ 23087,    15},{  6959,    15},
{  5294,    14},{  7599,    15},{ 11055,    14},{  7951,    15},
{ 13743,    14},{  8143,    15},{  7215,    15},{  7983,    15},
{  1487,    14},{  6447,    15},{  9167,    14},{ 10799,    14},
{  9679,    14},{  1839,    15},{  2831,    14},{  3887,    15},
/*{   975,    14},{  2351,    15},{  3503,    15},{    89,     7}};*/
{   975,    14},{  2351,    15},{    89,     7}};


/* Function Declarations */
void forward_wavelet();
void quantization();
void entropy_encode();
void hufenc(unsigned char, int *);
int write_compressed_file(char *);
int read_image(char *);

/********************************************************************
 * Routine: main(int argc, char **argv)
 * Expects image file name and the scale factor for compression as
 * input parameters. Calls all the required routines for compression.
 * The compressed file is the output of write_compressed_file().
 * This file format is specific to this program and includes all image
 * information required for decompression. 
 *********************************************************************/
main(int argc, char **argv)
{
  float a,b,c;
  float diff1,diff2;
  score_init();
  initialize(argc, argv);   
  read_image(in_name);
  a = clock();
  forward_wavelet();
  b = clock();
  quantization(); 
  entropy_encode();
  c = clock();
  diff1 = (b-a)/CLOCKS_PER_SEC;
  diff2 = (c-b)/CLOCKS_PER_SEC;
  printf("Transform is %2.2f seconds\n",diff1);
  printf("Quant/entropy coding is %2.2f seconds\n",diff2);
  write_compressed_file(in_name);
  score_exit();
}


initialize(argc, argv)
int argc;
char **argv;
{
  int i;

  cratio = DEFAULT_CRATIO;
  if(argc==3) cratio = atoi(argv[2]);
  if(argc> 1) strcpy(in_name,argv[1]);

  if (argc < 2 || argc > 3 || cratio < 0 || cratio > 255 ){
    printf("Syntax: %s image.pgm [compression rate]\n",argv[0]);
    printf("        Where image.pgm is the name of the image to be\n");
    printf("        compressed and the optional compression ratio parameter\n");
    printf("        specifies how aggressively to compress the image.\n");
    printf("        (O = least compression and  255 = highest compression)\n");
    printf("        Examples: %s lena.pgm OR %s sample.pgm 140\n"
                                                             ,argv[0],argv[0]);
    exit(-1);
  }

  for(i=0; i<9; i++) { 
    blockthresh[i]=(blockthresh[i]*cratio)>>7;
    if (blockthresh[i] < blockminthresh[i])
      blockthresh[i] = blockminthresh[i];
  }
 
}


/********************************************************************
 * Routine: read_image(in_name)
 * read_image opens the specified image file, calculates the size
 * of image the imagE to be compressed (excluding the .pgm header), and
 * makes a copy of the image as integers in array int_data.
 *********************************************************************/
int read_image(in_name)
char *in_name;
{
  FILE *fp_in_image;
  unsigned char char_in[IMG_SIZE];
  int num_cols,num_rows,num_bits;
  int i;
  char p5[10];

  /* Open input image file */
  if ((fp_in_image = fopen (in_name, "rb")) == NULL) {
      fprintf (stderr, "\n Could not open file \"%s\"\n\n", in_name);
      return(1);
  }

  /* Skip .pgm image file header */
  fscanf(fp_in_image,"%s\n %d %d\n %d\n",p5,&num_rows,&num_cols,&num_bits);

  /* Copy image bytes following the header into char array char_in */
  img_size = num_cols * num_rows;
  fread(char_in, img_size, 1, fp_in_image);
  fclose(fp_in_image);

  /* Copy into integer arrays int_data */
  for (i = 0; i < img_size; i++) int_data[i]  = char_in[i];
}



/********************************************************************
 * Routine: void quantization()
 * 
 *********************************************************************/
void quantization()
{
  int bl,num;

  num=0;
  block_quantize_DC(0,0,BLOCK_SIZE/2,num++);
  for(bl=BLOCK_SIZE/2; (bl<512 && num<BLKS_SAVED); bl<<=1)
  {
    block_quantize_RLE(0,bl,bl,num++);
    block_quantize_RLE(bl,0,bl,num++);
    block_quantize_RLE(bl,bl,bl,num++);
  }
}

/********************************************************************
 * Routine: block_quantize_DC(int x, int y, int size, int num)
 *********************************************************************/

block_quantize_DC(int x, int y, int size, int num)
{
  int i,j,val;
  int qsize=0;
  int minval,maxval,allmax;
  int a,b,c,d,e,f,g,h;

  /* size is always a multiple of 8 */

  for(j=y; j<y+size; j++)
    for(i=x; i<x+size; i+=8)
  /*  for(i=x; i<x+size; i++)
    for(j=y; j<y+size; j+=8) */
    {
      a=int_data[i+512*j+0]+32;
      b=int_data[i+512*j+1]+32;
      c=int_data[i+512*j+2]+32;
      d=int_data[i+512*j+3]+32;
      e=int_data[i+512*j+4]+32;
      f=int_data[i+512*j+5]+32;
      g=int_data[i+512*j+6]+32;
      h=int_data[i+512*j+7]+32;

      if (a < 0) a = 0; if (e < 0) e = 0;
      if (b < 0) b = 0; if (f < 0) f = 0;
      if (c < 0) c = 0; if (g < 0) g = 0;
      if (d < 0) d = 0; if (h < 0) h = 0;

      if (a > 319) a = 319;      if (e > 319) e = 319;
      if (b > 319) b = 319;      if (f > 319) f = 319;
      if (c > 319) c = 319;      if (g > 319) g = 319;
      if (d > 319) d = 319;      if (h > 319) h = 319;

      a /= 10; b /= 10; c /= 10; d /= 10;
      e /= 10; f /= 10; g /= 10; h /= 10; 

      dc_vals[5*(qsize/8)] = (unsigned char) (a + (b&7)*32);
      dc_vals[5*(qsize/8)+1]=(unsigned char) ((b>>3) + c*4 + (d%2)*128);
      dc_vals[5*(qsize/8)+2]= (unsigned char) ((d>>1) + (e&15)*16);
      dc_vals[5*(qsize/8)+3]= (unsigned char) ((e>>4) + f*2 + (g&3)*64);
      dc_vals[5*(qsize/8)+4]= (unsigned char) ((g>>2) + h*8);

      qsize+=8;
    }

}


/********************************************************************
 * Routine: block_quantize(int x, int y, int size, int num)
 *********************************************************************/
block_quantize_RLE(int x, int y, int size, int num)
{
  int i,j,val;
  int minval,maxval,allmax;
  int q,jr,count;
  int zerostate = 0;

  jr=0; count=0;
  blockrange[num] = blockmaxval[num]-blockthresh[num];

  for(j=y; j<y+size; j++)
    for(i=x; i<x+size; i++)
/*   for(i=x; i<x+size; i++) */
/*     for(j=y; j<y+size; j++) */
    {
      val=int_data[i+512*j];
      if(abs(val)<blockthresh[num]) q = ZERO_MARK;
      else if(val < 0){
	q = classify(-val,num);
      }
      else {
        q = 15+classify(val,num);
      }
      /* zerostate = "In the middle of a zerorun?" */
      if((q!=ZERO_MARK) && (zerostate != 1)) {
	rle_buf[num][jr]=q;
	jr++;
      }
      else if((q!=ZERO_MARK) && (zerostate == 1)) {
	rle_buf[num][jr]=count+ZERO_MARK-1;
	count = 0;
	jr++; zerostate=0;
	rle_buf[num][jr]=q;
	jr++;
      }
      else
	{
	  zerostate=1;
	  count++;
	  if (count==255-ZERO_MARK) {
	    rle_buf[num][jr] = 254;
	    jr++; zerostate=0; count=0;
	  }
	}
    }

  if (zerostate) {
    	rle_buf[num][jr]=count+ZERO_MARK-1;
	count = 0;
	jr++; zerostate=0;
  }
  
  rle_size[num]=jr;

}
     

/********************************************************************
 * Routine: int classify(int val)
 *********************************************************************/
int classify(int val,int num)
{

  int newval;

  newval = val;
  if (newval > blockmaxval[num]) newval = blockmaxval[num];
  newval = newval-blockthresh[num];

  newval *= 16;
  newval /= blockrange[num];
  
  if (newval > 14) newval = 14; 

  return newval;

}

/********************************************************************
 * Routine: void entropy_encode()
 *********************************************************************/
void entropy_encode()
{
  int i,j,nb;
  int rlecount;

  for(i=0; i<IMG_SIZE; i++) codep[i]=0;

  rlecount=0;
  nb=0;
  for (i = 1; i < BLKS_SAVED; i++)
  {
    for(j = 0; j < rle_size[i]; j++)
      hufenc(rle_buf[i][j],&nb);
    rlecount+=rle_size[i];
  }

  nbytes = ((nb+7)/8);
  printf("Compressed size is %d bytes\n",nbytes+60+2560);
}

/********************************************************************
 * Routine: void hufenc(unsigned char ich, int *nb)
 *********************************************************************/
void hufenc(unsigned char ich, int *nb)
{
  int i,nbits,val;
  int bytn,bitn;

  nbits=Huff[ich][1];
  val=Huff[ich][0];
  for(i=0; i<nbits; i++)
  {
    bytn=(*nb)>>3;
    bitn=(*nb)&7;
    if((val&(1<<i))>0) codep[bytn]|=(1<<bitn);
    (*nb)++;
  }
}

/********************************************************************
 * Routine: int write_compressed_file()
 *********************************************************************/
int write_compressed_file(in_name)
char *in_name;
{
  FILE *fp_out_file;
  int index;

  if(strlen(in_name)>4) {
    strcat(out_name,"x_");
    strncat(out_name,in_name,strlen(in_name)-4); //cheesy
  }
  else {
    strcat(out_name,in_name);
  }
  strcat(out_name,".cmp");
  
 
  if ((fp_out_file= fopen (out_name, "wb")) == NULL) {
      fprintf (stderr, "\n Could not open file %s \n\n",out_name);
      return(1);
  }

  fwrite(blockthresh, sizeof(int) ,BLKS_SAVED, fp_out_file);
  fwrite(rle_size    , sizeof(int) ,BLKS_SAVED, fp_out_file);
  fwrite(dc_vals, sizeof(unsigned char), 2560, fp_out_file);
  fwrite(&nbytes     , sizeof(int) , 1, fp_out_file);
  fwrite(codep       , sizeof(char),nbytes, fp_out_file); 

  /*  for (index=1; index<BLKS_SAVED; index++) 
    fwrite(rle_buf[index],sizeof(unsigned char),rle_size[index],fp_out_file);*/

  fclose(fp_out_file);
}

/********************************************************************
 * Routine: void fcdf22(int x[], int n, int st)
 *********************************************************************/
void fcdf22(int x[], int n, int st)
{
  int s[256],d[256];
  int mid, i;

  mid=(n/2)-1;
 
  for (i=0;i<=mid;i++)
  {
    s[i]=x[2*i*st];
    d[i]=x[2*i*st+st];
  }
 
  d[0]=d[0]+d[0]-s[0]-s[1];
  s[0]=s[0]+((d[0]+d[0])>>3);
  for (i=1;i<mid;i++)
  { 
    d[i]=d[i]+d[i]-s[i]-s[i+1];
    s[i]=s[i]+((d[i-1]+d[i])>>3);
  }
  d[mid]=d[mid]+d[mid]-s[mid]-s[mid];
  s[mid]=s[mid]+((d[mid-1]+d[mid])>>3);
 
  for(i=0; i<=mid; i++)
  {
    x[i*st]=s[i];
    x[(i+mid+1)*st]=d[i];
  }
}

/********************************************************************
 * Routine: void forward_wavelet()
 *********************************************************************/
void forward_wavelet()
{
  int i,j,nt,ind;
  int maxval,minval,val;
  FILE *helpme;

  helpme = fopen("helpme","wb");

  for (nt=512,ind=0;nt>=BLOCK_SIZE;nt>>=1,ind++)
  {
    char num[2];
    itoa(ind,num);
    for (i=0;i<nt*512;i+=512)
      fcdf22(&int_data[i],nt,ROW);
 
    for (i=0;i<nt;i++) 
      fcdf22(&int_data[i],nt,COL);

    if (ind==2) { 
      fwrite(int_data, sizeof(int), 512*512, helpme);
    }     
    
  }

  fclose(helpme);


}

