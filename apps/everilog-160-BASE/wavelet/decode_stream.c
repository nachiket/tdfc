static const char SccsId[] = "@(#)unsquish.c	1.3 08/06/98";

/* Modifications made by Joseph Yeh, U.C. Berkeley */

/***************************************************************************/
/*                                                                         */
/*  Copyright (September 1997) Honeywell Inc. Unpublished - All rights     */
/*  reserved. This material may be reproduced by, or for, the U.S.         */
/*  Government pursuant to the copyright license under the clause at       */
/*  DFARS 252.227-7013 (Oct. 1988).                                        */
/*                                                                         */
/***************************************************************************/

#include<stdio.h>
#include<strings.h>

#define NUMROWS  512
#define NUMCOLS  512

#define BLOCKS_SAVED  7
#define GL_INC  4.0 

#define IMG_SIZE 262146 

#define BLOCK_SIZE  128
#define ZERO_MARK  30

int min(a,b) {return ((a<b)?(a):(b));}
int max(a,b) {return ((a>b)?(a):(b));}

/* I/O Variables */
int img_size,num_cols,num_rows,nbytes;

/* Wavelet variables */
int int_data[IMG_SIZE];

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
int quant_buf[10][IMG_SIZE];
unsigned char dc_vals[2560];
int block_maxval[10] = {256, 512, 512, 2048, 512, 512, 1024, 0, 0, 0};
int block_thresh[10] = {0, 39, 27, 104, 79, 50,191, 99999, 99999, 99999};
int q_size[10] = {4096,4096,4096,4096,16384,16384,16384,0,0,0};
/* RLE variables */
unsigned char rle_buf[20][IMG_SIZE];
unsigned int rle_size[20];
unsigned int code_size[20];

/* Huffman variables */
#define HUFTABSIZE 1000000
#define HUFTABSIZE2 1000000
int htable[HUFTABSIZE];
int h2table[HUFTABSIZE];
unsigned char codep[10][IMG_SIZE]; 

int Huff[255][2] =

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
{   975,    14},{  2351,    15},{    89,     7}};


void reverse_wavelet();
void dequantization(); 
void RLE_decode();
void hufinit();
void entropy_decode();
unsigned char hufdec(unsigned long *nb,unsigned char* codeb);
int read_format_file(void);
int write_image();

/********************************************************************
 * Routine: main(int argc, char **argv)
 * The expected input is the name of the file to be decompressed. The name
 * of the output ".pgm" file is an optional input. It will calls all
 * routines required for decompression. 
 *********************************************************************/
main()
{

   FILE *fp_junk;
  int index;
   fp_junk = fopen("c_int","w");

  read_compressed_file();
  entropy_decode(); 
  RLE_decode(); 
  dequantization(); 
  fwrite(int_data, sizeof(int), 512*512, fp_junk);
  fclose(fp_junk);
  reverse_wavelet();
  write_image();
}


/********************************************************************
 * Routine: write_image(int argc, char **argv)
 * write_image first writes the PGM format file header to the user
 * specified file and then appends the decompressed image from array 
 * data to output file.
 *********************************************************************/
int write_image()
{
  FILE *fp_out_image;
  unsigned char data[IMG_SIZE];
  int i;
  char p5[10];

 /* Open user specified file for writing */
  fp_out_image = fopen ("imageout.pgm", "wb");

 /* Write PGM format header in output file */
  fprintf(fp_out_image,"P5\n");
  fprintf(fp_out_image,"%d %d\n", num_cols, num_rows);
  fprintf(fp_out_image,"%d\n", 255);

 /* Convert image from integer bytes to char bytes */
  for (i = 0; i < img_size; i++) 
  {
     if(int_data[i]<0.0) data[i]=0;
     else if(int_data[i]> 255.0) data[i]=255;
     else data[i] = int_data[i];
  }

  /* Write image in output file */
  fwrite(data, sizeof(unsigned char), img_size, fp_out_image);

  fclose(fp_out_image);
}

/********************************************************************
 * Routine: block_dequantize(int x, int y, int size, int num)
 *********************************************************************/
block_dequantize_DC(int x, int y, int size, int num)
{
  int i,j,ind;
  int minval,maxval,qsize;
  unsigned char a,b,c,d,e;

  /*  for (i=0; i<512; i++) {

    a = dc_vals[5*i]; b = dc_vals[5*i+1]; c = dc_vals[5*i+2];
    d = dc_vals[5*i+3]; e = dc_vals[5*i+4];

    quant_buf[0][8*i] = (int) (a&31);
    quant_buf[0][8*i+1] = (int) ((a>>5)+(b&3)*8);
    quant_buf[0][8*i+2] = (int) ((b>>2)&31);
    quant_buf[0][8*i+3] = (int) ((b>>7)+(c&15)*2);
    quant_buf[0][8*i+4] = (int) ((c>>4)+(d%2)*16);
    quant_buf[0][8*i+5] = (int) ((d>>1)&31);
    quant_buf[0][8*i+6] = (int) ((d>>6)+(e&7)*4);
    quant_buf[0][8*i+7] = (int) (e>>3);

  }*/

  qsize = 0;

  for(j=y; j<y+size; j++)
    for(i=x; i<x+size; i++)
    /*    for(j=y; j<y+size; j++) */
      {
	ind=i+512*j;
	int_data[ind] = (quant_buf[0][qsize])*10 + 5 - 32;
	/*printf("%d, %d\n",dc_vals[qsize],int_data[ind]);*/
	qsize++;
      }
   
}


/********************************************************************
 * Routine: block_dequantize(int x, int y, int size, int num)
 *********************************************************************/
block_dequantize(int x, int y, int size, int num)
{
  int i,j,ind;
  int qsize;
  int diff,findex,quanta;
  int thresh[30];
  

  diff= (block_maxval[num]-block_thresh[num]);
  quanta = diff/16;

  findex=0.5;

  for(i=15; i<29; i++) {
    thresh[i]= block_thresh[num] + (i-15)*quanta+quanta/2;
    thresh[i-15]= -thresh[i];
  }
  
  thresh[29] = block_thresh[num] + 15*quanta; thresh[14] = -thresh[29];    

  if(num>=BLOCKS_SAVED)
  {

    for(j=y; j<y+size; j++)
      for(i=x; i<x+size; i++)
/*     for(i=x; i<x+size; i++) */
/*       for(j=y; j<y+size; j++) */
      {
        ind=i+512*j;
        int_data[ind]=0;
      }
  }
  else
    {
      qsize=0;

      for(j=y; j<y+size; j++)
	for(i=x; i<x+size; i++)
	/*	for(j=y; j<y+size; j++) */
	  {
	    ind=i+512*j;
	    if(quant_buf[num][qsize]==ZERO_MARK) 
	      int_data[ind]=0;
	    else 
	      int_data[ind]=thresh[quant_buf[num][qsize]];
	      
	    qsize++;
	  }
    }
}       
      


/********************************************************************
 * Routine: void dequantization()
 *********************************************************************/
void dequantization()
{
  int bl,num,i;

  num=0;
  for(i=0; i<img_size; i++) int_data[i]=0;
  block_dequantize_DC(0,0,BLOCK_SIZE/2,num++);
  for(bl=BLOCK_SIZE/2; bl<512; bl<<=1)
  {
    block_dequantize(0,bl,bl,num++);
    block_dequantize(bl,0,bl,num++);
    block_dequantize(bl,bl,bl,num++);
  }
}

/********************************************************************
 * Routine: void entropy_decode()
 *********************************************************************/
void entropy_decode()
{
  unsigned long nb;
  int i,j;

  hufinit();

  for (i = 1; i < BLOCKS_SAVED; i++) {
    nb=0; j=0;
    while (nb < 8*(code_size[i]))
    {
      rle_buf[i][j++]=hufdec(&nb,codep[i]);
    }
    rle_size[i] = j;
  }
 
}

/********************************************************************
 * Routine: void  RLE_decode()
 *********************************************************************/
void  RLE_decode()
{
  unsigned int bl;

  for (bl = 1; bl < 7; bl++)
    block_RLE_decode(bl);
}


/********************************************************************
 * Routine: block_RLE_decode(int num)
 *********************************************************************/
block_RLE_decode(int num)
{
  int i,j,value,count;

  j=0; i=0;
  /*for(i=0; i<rle_size[num]; i++)*/
  while(i<rle_size[num])
  {
    value=rle_buf[num][i++];
    if(value<ZERO_MARK){ quant_buf[num][j++]=value;}

    else for(count=0; count<=(value-ZERO_MARK); count++){
                                       quant_buf[num][j++]=ZERO_MARK;
    }
  }
}

/********************************************************************
 * Routine: unsigned char hufdec(unsigned long *nb)
 *********************************************************************/
unsigned char hufdec(unsigned long *nb,unsigned char *codeb)
{
  int val,bytn,bitn,bitv;

  val=1;
  while(htable[val]<0)
  {
    bytn=(*nb)>>3;
    bitn=(*nb)&7;

    bitv=((codeb[bytn]&(1<<bitn))>0);
    val=(val<<1)+bitv;

    (*nb)++;
  }

  return(htable[val]);
}


/********************************************************************
 * Routine: void hufinit()
 *********************************************************************/
void hufinit()
{
  int i,j,val,huf;

  for(i=0; i<HUFTABSIZE; i++) htable[i]=-1;

  for(i=0; i<255; i++)
  {
    val=1;
    huf=Huff[i][0];
    for(j=0; j<Huff[i][1]; j++)
    {
      val=(val<<1)+(huf&1);
      huf>>=1;
    }
    htable[val]=i;
  }
}
 
/********************************************************************
 *  Routine: int read_compressed_file(void)
 *********************************************************************/
int read_compressed_file(void)
{

  FILE *dc_file,*ac_file[6];
  int i,j,index,a,rlind;
 
  num_rows=NUMROWS;
  num_cols=NUMCOLS;
  img_size=num_rows*num_cols;

  dc_file = fopen("dcout.raw","rb");
  ac_file[1] = fopen("ac1out.raw","rb");
  ac_file[0] = fopen("ac2out.raw","rb");
  ac_file[2] = fopen("ac3out.raw","rb");
  ac_file[4] = fopen("ac4out.raw","rb");
  ac_file[3] = fopen("ac5out.raw","rb");
  ac_file[5] = fopen("ac6out.raw","rb");
  
  for (index = 0; index < 6; index++) {
    rlind = 0;
    while ((a = getc(ac_file[index])) != EOF){
      codep[index+1][rlind++] = (unsigned char) a;
    } 
    code_size[index+1] = rlind;
    fclose(ac_file[index]);
  }

  for (index = 0; index<4096; index++) {
    a = fgetc(dc_file);
    quant_buf[0][index] = (int) a;
  }

  /*     for (j=0; j<4096;  printf("%d\n", dc_vals[j]); */
  
  fclose(dc_file); 

}
 

/********************************************************************
 *  Routine: void bcdf22(int x[], int n, int st)
 *********************************************************************/
void bcdf22(int x[], int n, int st)
{
  int s[256],d[256], mid, i;
 
  mid=(n/2)-1;
 
  for (i=0;i<=mid;i++)
  {
    s[i]=x[i*st];
    d[i]=x[(i+mid+1)*st];
  }
 
  s[mid]=s[mid]-((d[mid-1]+d[mid])>>3);
  d[mid]=d[mid]+s[mid]+s[mid];
  for (i=mid-1; i>0; i--)
  {
    s[i]=s[i]-((d[i-1]+d[i])>>3);
    d[i]=d[i]+s[i]+s[i+1];
  }
  s[0]=s[0]-((d[0]+d[0])>>3);
  d[0]=d[0]+s[0]+s[1];
 
  for(i=0; i<=mid; i++)
  {
    x[2*st*i]=s[i];
    x[2*st*i+st]=d[i]/2;
  }
}
 
/********************************************************************
 *  Routine: void reverse_wavelet()
 *********************************************************************/
#define ROW  1
#define COL  512
void reverse_wavelet()
{
  int i,nt;
 
  for (nt=BLOCK_SIZE;nt<=512;nt<<=1)
  {
    for (i=0;i<nt;i++) bcdf22(&int_data[i],nt,COL);
    for (i=0;i<nt*512;i+=512) bcdf22(&int_data[i],nt,ROW);
  }
}



