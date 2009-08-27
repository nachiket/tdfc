// ADPCM encoder (16-->4)
// Adapted from UCLA MediaBench Intel/DVI ADPCM encoder
//
// Eylon Caspi, 11/23/98
//
// 8 pages, ~35-slow

import score.*;

////////

class DiffSign extends ComputePage
{
  public PageInput  inVal		= new PageInput (this,16,1),
		    inValPred		= new PageInput (this,16,1);
  public PageOutput outDiff		= new PageOutput(this,16),
		    outSign_ValPred	= new PageOutput(this,1),
		    outSign_MuxNibbles	= new PageOutput(this,1),
		    outDone		= new PageOutput(this,1);
  
  public DiffSign()
  {
    setName("DiffSign");
    betweenFirings(2);
//  betweenOutputs(6);
  }
  
  public int step()
  {
    int  val  = (int)(inVal.dequeue());
    long done = (val==0x7fff ? 1 : 0);
    int  diff = val - (int)(inValPred.dequeue());
    if (diff>=0) {
      outDiff.queue((long)diff, 6);
      outSign_ValPred.queue   (0, 2);
      outSign_MuxNibbles.queue(0, 2);
    }
    else {
      outDiff.queue((long)-diff, 6);
      outSign_ValPred.queue   (1, 2);
      outSign_MuxNibbles.queue(1, 2);
    }
    outDone.queue(done,1);
    if (done==1)
      signalDone(6);
    return 6;
  }
}

////////

class DivMul2 extends ComputePage
{
  public PageInput  inDiff		= new PageInput (this,16,1),
    //		    inStep		= new PageInput (this,16,1);
		    inStepHigh		= new PageInput (this,8,1),
		    inStepLow		= new PageInput (this,8,1),
		    inDone		= new PageInput (this,1,1);
  public PageOutput outDelta2_MuxNibbles= new PageOutput(this,1),
		    outDelta2_Index	= new PageOutput(this,1),
  		    outDiffReduced2	= new PageOutput(this,16),//diff(+step)
  		    outStepZeroed2	= new PageOutput(this,16),//step (or 0)
		    outDone		= new PageOutput(this,1);

  public DivMul2 ()
  {
    setName("DivMul2");
    betweenFirings(2);
//  betweenOutputs(6);
  }
  
  public int step()
  {
    long done = inDone.dequeue();
    int  diff = (int)(inDiff.dequeue());
//  int  step = (int)(inStep.dequeue());
    int  step = (int)((inStepHigh.dequeue()<<8) | inStepLow.dequeue());
    int  diffReduced2 = diff-step;
    if (diffReduced2>=0) {
      outDelta2_MuxNibbles.queue(1, 2);
      outDelta2_Index.queue     (1, 2);
      outDiffReduced2.queue((long)diffReduced2, 6);
      outStepZeroed2.queue((long)step, 4);
    }
    else {
      outDelta2_MuxNibbles.queue(0, 2);
      outDelta2_Index.queue     (0, 2);
      outDiffReduced2.queue((long)diff, 6);
      outStepZeroed2.queue(0, 4);
    }
    outDone.queue(done,1);
    if (done==1)
      signalDone(6);
    return 6;
  }
}

class DivMul1 extends ComputePage
{
  public PageInput  inDiffReduced2	= new PageInput (this,16,1),
		    inStepZeroed2	= new PageInput (this,16,1),
    //		    inStep		= new PageInput (this,16,1);
		    inStepHigh		= new PageInput (this,8,1),
		    inStepLow		= new PageInput (this,8,1),
		    inDone		= new PageInput (this,1,1);
  public PageOutput outDelta1_MuxNibbles= new PageOutput(this,1),
		    outDelta1_Index	= new PageOutput(this,1),

  		    outDiffReduced1	= new PageOutput(this,16),//diff(+step)
								  //  (+step/2)
  		    outStepZeroed1	= new PageOutput(this,16),//step (or 0)
  		    outVPDiff1		= new PageOutput(this,16),
		    outDone		= new PageOutput(this,1);

  public DivMul1 ()
  {
    setName("DivMul1");
    betweenFirings(2);
//  betweenOutputs(6);
  }
  
  public int step()
  {
    long done         = inDone.dequeue();
//  int  step         = (int)(inStep.dequeue());
    int  step         = (int)((inStepHigh.dequeue()<<8) | inStepLow.dequeue());
    int  diffReduced2 = (int)(inDiffReduced2.dequeue());
    int  stepZeroed2  = (int)(inStepZeroed2.dequeue());
    int  diffReduced1 = diffReduced2 - (step>>1);
    if (diffReduced1>=0) {
      outDelta1_MuxNibbles.queue(1, 2);
      outDelta1_Index.queue     (1, 2);
      outDiffReduced1.queue((long)diffReduced1, 6);
      outStepZeroed1.queue((long)step>>1, 4);
    }
    else {
      outDelta1_MuxNibbles.queue(0, 2);
      outDelta1_Index.queue     (0, 2);
      outDiffReduced1.queue((long)diffReduced1, 6);
      outStepZeroed1.queue(0, 4);
    }
    outVPDiff1.queue( (long)((step>>3) + stepZeroed2), 4 );
    outDone.queue(done,1);
    if (done==1)
      signalDone(6);
    return 6;
  }
}

class DivMul0 extends ComputePage
{
  public PageInput  inDiffReduced1	= new PageInput (this,16,1),
		    inStepZeroed1	= new PageInput (this,16,1),
    //		    inStep		= new PageInput (this,16,1),
		    inStepHigh		= new PageInput (this,8,1),
		    inStepLow		= new PageInput (this,8,1),
		    inVPDiff1		= new PageInput (this,16,1),
    //		    inVPDiff0		= new PageInput (this,16,1),
		    inDone		= new PageInput (this,1,1);
  public PageOutput outDelta0_MuxNibbles= new PageOutput(this,1),
		    outDelta0_Index	= new PageOutput(this,1),
  		    outVPDiffAbs	= new PageOutput(this,16),
		    outDone_ValPred	= new PageOutput(this,1),
		    outDone_MuxNibbles	= new PageOutput(this,1),
		    outDone_Index	= new PageOutput(this,1);

  public DivMul0 ()
  {
    setName("DivMul0");
    betweenFirings(2);
//  betweenOutputs(9);
  }
  
  public int step()
  {
    long done         = inDone.dequeue();
//  int  step         = (int)(inStep.dequeue());
    int  step         = (int)((inStepHigh.dequeue()<<8) | inStepLow.dequeue());
    int  diffReduced1 = (int)(inDiffReduced1.dequeue());
    int  stepZeroed1  = (int)(inStepZeroed1.dequeue());
    int  VPDiff1      = (int)(inVPDiff1.dequeue());
    int  VPDiff0      = VPDiff1 + stepZeroed1;
    int  diffReduced0 = diffReduced1 - (step>>2);
    int  stepZeroed0;
    if (diffReduced0>=0) {
      outDelta0_MuxNibbles.queue(1, 2);
      outDelta0_Index.queue     (1, 2);
      stepZeroed0 = step>>2;
    }
    else {
      outDelta0_MuxNibbles.queue(0, 2);
      outDelta0_Index.queue     (0, 2);
      stepZeroed0 = 0;
    }
    outVPDiffAbs.queue( (long)(VPDiff0 + stepZeroed0), 9 );
    outDone_ValPred.queue   (done,1);
    outDone_MuxNibbles.queue(done,1);
    outDone_Index.queue     (done,1);
    if (done==1)
      signalDone(9);
    return 9;
  }
}

////////

class ValPredUpdate extends ComputePage
{
  public PageInput  inVPDiffAbs	= new PageInput (this,16,1),
		    inSign	= new PageInput (this,1,1),
		    inDone	= new PageInput (this,1,1);
  public PageOutput outValPred	= new PageOutput(this,16);

  public int valPred=0;

  public ValPredUpdate()
  {
    setName("ValPredUpdate");
    betweenFirings(8);
//  betweenFirings(8);
  }

  public int step()
  {
    long done   = inDone.dequeue();
    int  sign   = (int)(inSign.dequeue());
    int  VPDiff = (int)(inVPDiffAbs.dequeue());
    int  valPredTmp;
    if (sign==1)
      VPDiff = -VPDiff;
    valPredTmp = valPred + VPDiff + (sign==1?1:0);
    if (valPredTmp<-32768)
      valPred = -32768;
    else if (valPredTmp>32767)
      valPred = 32767;
    else
      valPred = valPredTmp;
    outValPred.queue((long)valPred, 8);
    if (done==1)
      signalDone(8);
    return 8;
  }
}

////////

class IndexOffsetTable extends ComputePage
{
  public PageInput  inIndex		= new PageInput (this,7,1),
                    inDelta2		= new PageInput (this,1,1),
                    inDelta1		= new PageInput (this,1,1),
                    inDelta0		= new PageInput (this,1,1),
		    inDone		= new PageInput (this,1,1);
  public PageOutput outIndex_Index	= new PageOutput(this,7),
		    outIndex_StepHi	= new PageOutput(this,7),
		    outIndex_StepLo	= new PageOutput(this,7),
		    outDone_StepHi	= new PageOutput(this,1),
		    outDone_StepLo	= new PageOutput(this,1);

  private static int indexOffset[] = {-1, -1, -1, -1, 2, 4, 6, 8};

  public IndexOffsetTable()
  {
    setName("IndexOffsetTable");
    betweenFirings(1);
//  betweenOutputs(7);
  }

  public int step()
  {
    long done  = inDone.dequeue();
    int  index =  (int)(inIndex.dequeue());
    int  delta = ((int)(inDelta2.dequeue())<<2) |
                 ((int)(inDelta1.dequeue())<<1) |
                 ((int)(inDelta0.dequeue())   ) ;
    int  newIndex          = index + indexOffset[delta];
    int  newIndex_minus_ee = index + indexOffset[delta] - 88;
    long outIndexVal	   = (newIndex<0)          ? 0  :
			     (newIndex_minus_ee>0) ? 88 :
 			     ((long)newIndex);
    outIndex_Index.queue (outIndexVal, 7);
    outIndex_StepHi.queue(outIndexVal, 7);
    outIndex_StepLo.queue(outIndexVal, 7);
    outDone_StepHi.queue(done,1);
    outDone_StepLo.queue(done,1);
    if (done==1)
      signalDone(7);
    return 7;
  }
}

////////

abstract
class StepTable extends ComputePage
{
  public PageInput  inIndex	    = new PageInput (this,7,1),
		    inDone	    = new PageInput (this,1,1);
  public PageOutput outStep_DivMul2 = new PageOutput(this,8),	// hi/lo bytes
		    outStep_DivMul1 = new PageOutput(this,8), 
		    outStep_DivMul0 = new PageOutput(this,8);
  
  protected static int steps[] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
  };
}

class StepTableLow extends StepTable
{
  public StepTableLow()
  {
    setName("StepTableLow");
    betweenFirings(1);
//  betweenOutputs(5);
  }

  public int step()
  {
    long done       = inDone.dequeue();
    int  index      = (int)(inIndex.dequeue());
    long outStepVal = (long)(steps[index]&0xf);
    outStep_DivMul2.queue(outStepVal,5);
    outStep_DivMul1.queue(outStepVal,5);
    outStep_DivMul0.queue(outStepVal,5);
    if (done==1)
      signalDone(5);
    return 5;
  }
}

class StepTableHigh extends StepTable
{
  public StepTableHigh()
  {
    setName("StepTableHigh");
    betweenFirings(1);
//  betweenOutputs(5);
  }

  public int step()
  {
    long done       = inDone.dequeue();
    int  index      = (int)(inIndex.dequeue());
    long outStepVal = (long)(steps[index]>>4);
    outStep_DivMul2.queue(outStepVal,5);
    outStep_DivMul1.queue(outStepVal,5);
    outStep_DivMul0.queue(outStepVal,5);
    if (done==1)
      signalDone(5);
    return 5;
  }
}

////////

class MuxNibbles extends ComputePage
{
  public PageInput  inSign		= new PageInput (this,1,2),
                    inDelta2		= new PageInput (this,1,2),
                    inDelta1		= new PageInput (this,1,2),
                    inDelta0		= new PageInput (this,1,2),
		    inDone		= new PageInput (this,1,2);
  public PageOutput outPackedADPCM	= new PageOutput(this,8);

  public MuxNibbles()
  {
    setName("MuxNibbles");
    betweenFirings(1);
//  betweenOutputs(1);
  }

  public int step()
  {
    long done      = inDone.dequeue() | inDone.dequeue();
    int  hiNibble  = ((int)(inSign.dequeue())  <<3) |
                     ((int)(inDelta2.dequeue())<<2) |
                     ((int)(inDelta1.dequeue())<<1) |
                     ((int)(inDelta0.dequeue())   ) ;
    int  loNibble  = ((int)(inSign.dequeue())  <<3) |
                     ((int)(inDelta2.dequeue())<<2) |
                     ((int)(inDelta1.dequeue())<<1) |
                     ((int)(inDelta0.dequeue())   ) ;
    int packedByte = (hiNibble<<4) | loNibble;		// little-endian
							// (1st nibble high)
    outPackedADPCM.queue( (long)packedByte, 1 );
    if (done==1)
      signalDone(1);
    return 1;
  }
}

////////

class AdpcmDesign extends Design
{
  private DiffSign		cpDiffSign		= new DiffSign();
  private DivMul2		cpDivMul2		= new DivMul2();
  private DivMul1		cpDivMul1		= new DivMul1();
  private DivMul0		cpDivMul0		= new DivMul0();
  private ValPredUpdate		cpValPredUpdate		= new ValPredUpdate();
  private IndexOffsetTable	cpIndexOffsetTable	= new IndexOffsetTable();
  private StepTableLow		cpStepTableLow		= new StepTableLow();
  private StepTableHigh		cpStepTableHigh		= new StepTableHigh();
  private MuxNibbles		cpMuxNibbles		= new MuxNibbles();

  private long  initZero[]	     = {0};
  private long  initIndex[]	     = {0};		// 0-88
  private char	sinkArray[]	     = new char[16];	// need char[4]
  private char	srcArray[]	     = {0x10,0x00,	// little-endian
					0x01,0x00,
					0x12,0x00,
					0x03,0x00,
					0x14,0x00,
					0x05,0x00,
					0x16,0x00,
					0x07,0x00,
					0x18,0x00,
					0x09,0x00,
					0x1a,0x00,
					0x0b,0x00,
					0x1c,0x00,
					0x0d,0x00,
					0x1e,0x00,
					0x0f,0x00,
					0x00,0x00,	// EOS padding
					0xff,0x7f};	// EOS

  private CMBSeqSrc  cmbSrc  = new CMBSeqSrc ( srcArray.length, srcArray,16,2);
  private CMBSeqSink cmbDest = new CMBSeqSink(sinkArray.length,sinkArray, 8,1);

  public AdpcmDesign() {
    cmbSrc.setName("cmbSrc");
    cmbDest.setName("cmbDest");

    connect( cmbSrc.dataOut,			cpDiffSign.inVal );
    connect( cpDiffSign.outDiff,		cpDivMul2.inDiff );
    connect( cpDiffSign.outSign_ValPred,	cpValPredUpdate.inSign );
    connect( cpDiffSign.outSign_MuxNibbles,	cpMuxNibbles.inSign );
    connect( cpDiffSign.outDone,		cpDivMul2.inDone );
    connect( cpDivMul2.outDiffReduced2,		cpDivMul1.inDiffReduced2 );
    connect( cpDivMul2.outStepZeroed2,		cpDivMul1.inStepZeroed2 );
    connect( cpDivMul2.outDelta2_MuxNibbles,	cpMuxNibbles.inDelta2 );
    connect( cpDivMul2.outDelta2_Index,		cpIndexOffsetTable.inDelta2 );
    connect( cpDivMul2.outDone,			cpDivMul1.inDone );
    connect( cpDivMul1.outDiffReduced1,		cpDivMul0.inDiffReduced1 );
    connect( cpDivMul1.outStepZeroed1,		cpDivMul0.inStepZeroed1 );
    connect( cpDivMul1.outDelta1_MuxNibbles,	cpMuxNibbles.inDelta1 );
    connect( cpDivMul1.outDelta1_Index,		cpIndexOffsetTable.inDelta1 );
    connect( cpDivMul1.outVPDiff1,		cpDivMul0.inVPDiff1 );
    connect( cpDivMul1.outDone,			cpDivMul0.inDone );
    connect( cpDivMul0.outVPDiffAbs,		cpValPredUpdate.inVPDiffAbs );
    connect( cpDivMul0.outDelta0_MuxNibbles,	cpMuxNibbles.inDelta0 );
    connect( cpDivMul0.outDelta0_Index,		cpIndexOffsetTable.inDelta0 );
    connect( cpDivMul0.outDone_ValPred,		cpValPredUpdate.inDone );
    connect( cpDivMul0.outDone_MuxNibbles,	cpMuxNibbles.inDone );
    connect( cpDivMul0.outDone_Index,		cpIndexOffsetTable.inDone );
    connect( cpValPredUpdate.outValPred,	cpDiffSign.inValPred,		initZero );
    connect( cpIndexOffsetTable.outIndex_Index,	cpIndexOffsetTable.inIndex,	initIndex );
    connect( cpIndexOffsetTable.outIndex_StepHi,cpStepTableHigh.inIndex,	initIndex );
    connect( cpIndexOffsetTable.outIndex_StepLo,cpStepTableLow.inIndex,		initIndex );
    connect( cpIndexOffsetTable.outDone_StepHi,	cpStepTableHigh.inDone,		initZero );
    connect( cpIndexOffsetTable.outDone_StepLo,	cpStepTableLow.inDone,		initZero );
    connect( cpStepTableHigh.outStep_DivMul2,	cpDivMul2.inStepHigh );
    connect( cpStepTableHigh.outStep_DivMul1,	cpDivMul1.inStepHigh );
    connect( cpStepTableHigh.outStep_DivMul0,	cpDivMul0.inStepHigh );
    connect( cpStepTableLow.outStep_DivMul2,	cpDivMul2.inStepLow );
    connect( cpStepTableLow.outStep_DivMul1,	cpDivMul1.inStepLow );
    connect( cpStepTableLow.outStep_DivMul0,	cpDivMul0.inStepLow );
    connect( cpMuxNibbles.outPackedADPCM,	cmbDest.dataIn );
  }

  private String packedByteToString (char c)
  {
    return "(" + (int)(c>>4) + "," + (int)(c&0xf) + ")";
  }

  public String sinkArrayToString ()
  {
    String str = "{" + packedByteToString(sinkArray[0]);
    int i;
    for (i=1; i<sinkArray.length; i++)
      str += ", " + packedByteToString(sinkArray[i]);
    str += "}";
    return str;
  }
}

////////

class adpcm_app_9page
{
  private final static int	 numOfFrames	= 20;		// need 8
  private final static long	 timeSlice	= 10000;
  private final static int	 CMBtoCPratio	= 1;

  public static void debug ()
  {
    // Scheduler.DEBUG	= true;
    Simulator.insanity	= true;
    Page.insanity	= true;
    Event.insanity	= true;
    EventQueue.insanity	= true;
    // CMBSeqSink.DEBUG	= true;
  }

  public static void main(String args[])
  {
    // debug();
    Simulator	mySim		= new Simulator(numOfFrames,timeSlice);
    Scheduler	mySched		= new Scheduler(CMBtoCPratio);
    AdpcmDesign	myAdpcmDesign	= new AdpcmDesign();
    mySched.addDesign(myAdpcmDesign);
    System.err.println("Simulator exit code:  " + mySim.run());
    System.err.println("Simulator time:       " + mySim.getTime());
    System.err.println("result nibbles:       " + myAdpcmDesign.sinkArrayToString());
  }
}
