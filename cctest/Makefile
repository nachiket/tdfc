#SCORE_RUNTIME=/scratch/SCORE/runtime
#SCORE_RUNTIME=/project/cs/brass/a/tools/brass/develop/SCORE/runtime
SCORE_RUNTIME=/project/cs/brass/a/tools/brass/gold/SCORE/runtime
#SCORE_RUNTIME=/home/cs/rhuang/brass/sim/try
#LEDA_LIB_DIR = /project/cs/brass/a/tools/free/LEDA-3.8/LEDA-3.8-linux-egcs-1.0
#LEDA_LIB_DIR = /scratch/LEDA-3.8
LEDA_LIB_DIR =/project/cs/brass/a/tools/free/LEDA-3.8/LEDA-3.8a-linux-egcs-1.1
#LEDA_INCLUDE_DIR = /project/cs/brass/a/tools/free/LEDA-3.8/LEDA-3.8-linux-egcs-1.0/incl
#LEDA_INCLUDE_DIR = /scratch/LEDA-3.8/incl
LEDA_INCLUDE_DIR = /project/cs/brass/a/tools/free/LEDA-3.8/LEDA-3.8a-linux-egcs-1.1/incl
PERFCTR_LIB_DIR = /project/cs/brass/a/tools/free/perfctr-0.6

CFLAGS =  -g -Wall -I$(SCORE_RUNTIME) -I$(LEDA_INCLUDE_DIR) -I$(PERFCTR_LIB_DIR)
CXXFLAGS = -g -Wall -I$(SCORE_RUNTIME) -I$(LEDA_INCLUDE_DIR) -I$(PERFCTR_LIB_DIR)
MASTER_HEADERS=$(SCORE_RUNTIME)/ScoreOperator.h \
	$(SCORE_RUNTIME)/Score.h \
	$(SCORE_RUNTIME)/ScoreSegment.h

################################################################
# user-modifiable:

ifeq ($(OSTYPE),solaris)
CPP		= /usr/sww/lib/gcc-lib/sparc-sun-solaris2.5.1/2.7.2.2/cpp
TDF		= ../sparc-sun-solaris/tdfc
else
ifeq ($(OSTYPE),cygwin32) 
CPP=cpp
TDF		= ../i386-any-cygwin/tdfc
else
ifeq ($(OSTYPE),linux) 
CPP=cpp
TDF		= ../i386-any-linux/tdfc
#TDF		= /scratch/SCORE/tdfc/i386-any-linux/tdfc
endif
endif
endif

ifeq ($(USE_SSH), true)
TDF_COMMAND             = ssh ribbit cd $(PWD) ";" $(PWD)/../sparc-sun-solaris/tdfc
TDF		        = ../sparc-sun-solaris/tdfc
else
TDF_COMMAND             = $(TDF)
endif

TFLAGS		= -ecc

.PRECIOUS:	%.cc %.h


################################################################

DSRCS		= $(shell ls *.tdf)
ISRCS		= $(shell ls *.fuser)
ALLSRCS		= $(DSRCS)
MASTEROBJS	= $(DSRCS:.tdf=.o) 
CCFILES	= $(DSRCS:.tdf=.cc)

target: $(MASTEROBJS) force_instances
# want to force instance build in subsidiary makefile so 
#  will read the instance files created along with master objects
force_instances:
	$(MAKE) -f Makefile.instance
targetcc: $(CCFILES)

%.cc:	%.tdf ${TDF} 
	${TDF_COMMAND} ${TFLAGS} $< 

xor3.cc: xor.tdf xor3.tdf ${TDF} 
	${TDF_COMMAND} ${TFLAGS} xor.tdf xor3.tdf

streamcat3.cc:	streamcat.tdf streamcat3.tdf ${TDF}
	${TDF_COMMAND} ${TFLAGS} streamcat.tdf streamcat3.tdf

streamcat4.cc:	streamcat.tdf streamcat4.tdf ${TDF}
	${TDF_COMMAND} ${TFLAGS} streamcat.tdf streamcat4.tdf

Decode_compWavelet.cc:	Decode_compWavelet.tdf LiftWaveH.tdf LiftWaveV.tdf Quant_DC.tdf Quant.tdf ZLE.tdf
	${TDF_COMMAND} ${TFLAGS} Decode_compWavelet.tdf LiftWaveH.tdf LiftWaveV.tdf Quant_DC.tdf Quant.tdf ZLE.tdf
#	touch Decode_compWavelet.cc
#	touch Decode_compWavelet.o

compWavelet.cc:	compWavelet.tdf LiftWaveH.tdf LiftWaveV.tdf Quant_DC.tdf Quant.tdf ZLE.tdf
	${TDF_COMMAND} ${TFLAGS} compWavelet.tdf LiftWaveH.tdf LiftWaveV.tdf Quant_DC.tdf Quant.tdf ZLE.tdf
#	touch compWavelet.cc
#	touch compWavelet.o

%.o:	%.cc %.h $(MASTER_HEADERS)

clean:
	rm -f *.cc *.h *.o *.so *.instances core

reallyclean:	clean
	rm -f *.fauto
