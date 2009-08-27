###########################################################################
# directory definitions:


#SCORE_RUNTIME	= /project/cs/brass/a/tools/brass/gold/SCORE/runtime
SCORE_RUNTIME	= /home/eecs/yurym/ScoreRuntime
LEDA_INCLUDE_DIR= /project/cs/brass/a/tools/free/LEDA-4.0/LEDA-4.0-i386-linux-eg++-1.1/incl
LEDA_LIB_DIR	= /project/cs/brass/a/tools/free/LEDA-4.0/LEDA-4.0-i386-linux-eg++-1.1
PERFCTR_LIB_DIR = /project/cs/brass/a/tools/free/perfctr-0.6

CFLAGS	 = -Wall -I$(SCORE_RUNTIME) -I$(LEDA_INCLUDE_DIR) -I$(PERFCTR_LIB_DIR)\
	   $(EXTRA_CFLAGS)
CXXFLAGS = -Wall -I$(SCORE_RUNTIME) -I$(LEDA_INCLUDE_DIR) -I$(PERFCTR_LIB_DIR)\
	   $(EXTRA_CFLAGS)

CC	= /usr/bin/gcc
CXX	= /usr/bin/g++

###########################################################################
# libraries:

MAIN_LIBRARIES=-L$(LEDA_LIB_DIR) -L$(PERFCTR_LIB_DIR) -lL -lG -ldl -lpthread -lperfctr -lsysmp
MAIN_OBJECTS=$(SCORE_RUNTIME)/ScoreRuntime.a
#SCHEDULER_LIBRARIES=-L$(LEDA_LIB_DIR) -lL -lG -ldl -lpthread

###########################################################################
# Task Description Format:

TDFC = ../../tdfc
#TDFC = /project/cs/brass/a/tools/brass/gold/SCORE/tdfc/i386-any-linux/tdfc
TDFCFLAGS = -ecc $(EXTRA_TDFCFLAGS)
#XCFLAGS = -xc
XCFLAGS = 

########################################

.PRECIOUS:      %.cc %.h

#SCORE_TDF_SRCS	= $(shell ls *.tdf)
#SCORE_TDF_OBJS	= $(SCORE_TDF_SRCS:.tdf=.o)

SCORE_TDF_SRCS	= AdpcmEnc.tdf
SCORE_TDF_CC	= DiffSign.cc DivMul2.cc DivMul1.cc DivMul0.cc \
		  ValPredUpdate.cc IndexOffsetTable.cc MuxNibbles.cc \
		  copy_unsigned_3.cc AdpcmEnc.cc
SCORE_TDF_OBJS	= $(SCORE_TDF_CC:.cc=.o)

DEBUGFLAG = -g 
PROFLAG =

CFLAGS        =  $(DEBUGFLAG) -Wall $(PROFLAG) -I$(SCORE_RUNTIME) -I$(LEDA_INCLUDE_DIR) -I$(PERFCTR_LIB_DIR) $(EXTRA_CFLAGS)
CXXFLAGS      =  $(DEBUGFLAG) -Wall $(PROFLAG) -I$(SCORE_RUNTIME) -I$(LEDA_INCLUDE_DIR) -I$(PERFCTR_LIB_DIR) $(EXTRA_CFLAGS)


###########################################################################
# targets:

all: adpcmenc-main force_instances

force_instances:
	$(MAKE) -f Makefile.instance

adpcmenc-main:	adpcmenc-main.o $(SCORE_TDF_OBJS) $(MAIN_OBJECTS)
	g++ -o $@ $(CXXFLAGS) $^ $(MAIN_LIBRARIES)

adpcmenc-main.o:	adpcmenc-main.C $(SCORE_TDF_CC)

$(SCORE_TDF_CC):	AdpcmEnc.cc

AdpcmEnc.cc:	$(SCORE_TDF_SRCS)
	$(TDFC) $(XCFLAGS) $(TDFCFLAGS) $^

clean:
	rm *.o score_mpeg

reallyclean:

	rm -f *.o *.so *.cc *.h adpcmenc-main *.fauto *~ *.instances

# unused?
