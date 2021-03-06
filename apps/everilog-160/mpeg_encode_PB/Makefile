###########################################################################
# directory definitions:

# customizations go here:

DEBUGFLAG = 
PROFLAG =

include $(SCORE_PATH)/app/common/Makefile.common
# include /project/cs/brass/a/tools/brass/develop/SCORE/apps/common/Makefile.common
# include Makefile.common

## the name of the user executable
TARGET = score_mpeg

## this is typically the driver
SCORE_C_SRCS = makempeg.C


########################################

## list simple tdf sources here:
## they will be converted straight to .cc

SCORE_TDF_SIMPLE_SRCS = \
	topAll.tdf \
	muxToSegY.tdf \
	muxToSegC.tdf \
	modeModule.tdf \
	pattDetect.tdf \
	DCTab.tdf \
	HuffMachine.tdf \
	IModule.tdf \
	ISub.tdf \
	MZLE.tdf \
	MixToHuff.tdf \
	PModule.tdf \
	Passthru.tdf \
	adjust.tdf \
	breakup.tdf \
	choose8.tdf \
	chromAdd.tdf \
	error_cmp.tdf \
	fllm.tdf \
	motTrunc.tdf \
	motion.tdf \
	muxCodeRef.tdf \
	muxReforEst.tdf \
	muxtoDCT.tdf \
	muxToP.tdf \
	muxToI.tdf \
	pmode.tdf \
	prepeat2.tdf \
	proc_end.tdf \
	repeat_motion.tdf \
	segRefManage.tdf \
	seg_manage.tdf \
	segcBank.tdf \
	segc_manage.tdf \
	segrepC.tdf \
	segrepY.tdf\
	segyBank.tdf \
	segyMref.tdf \
	splitter.tdf \
	sumi.tdf \
	switchoff8.tdf \
	switchoff8s.tdf \
	tpose.tdf \
	unify.tdf \
	unifyc.tdf \
	zigzag_.tdf  \
	distribute.tdf

#SCORE_TDF_SIMPLE_SRCS = \
#	muxToSegY.tdf \
#	muxToSegC.tdf \
#	modeModule.tdf \
#	pattDetect.tdf \
#	debugSegment_rw.tdf \
#	segWrap.tdf \
#	DCTab.tdf \
#	HuffMachine.tdf \
#	IModule.tdf \
#	ISub.tdf \
#	MZLE.tdf \
#	MixToHuff.tdf \
#	PModule.tdf \
#	Passthru.tdf \
#	adjust.tdf \
#	breakup.tdf \
#	breakupY.tdf \
#	choose8.tdf \
#	chromAdd.tdf \
#	error_cmp.tdf \
#	fllm.tdf \
#	motTrunc.tdf \
#	motion.tdf \
#	muxCodeRef.tdf \
#	muxReforEst.tdf \
#	muxer.tdf \
#	muxtoDCT.tdf \
#	muxToP.tdf \
#	muxToI.tdf \
#	pmode.tdf \
#	prepeat2.tdf \
#	proc_end.tdf \
#	repeat_motion.tdf \
#	segRefManage.tdf \
#	seg_manage.tdf \
#	segcBank.tdf \
#	segc_manage.tdf \
#	segrepC.tdf \
#	segrepY.tdf\
#	segyBank.tdf \
#	segyMref.tdf \
#	splitter.tdf \
#	sumi.tdf \
#	switchoff8.tdf \
#	switchoff8s.tdf \
#	topAll.tdf \
#	tpose.tdf \
#	unify.tdf \
#	unifyc.tdf \
#	zigzag_.tdf  \
#	distribute.tdf

## list complex tdf sources here:
## they will be converted to .xc

SCORE_TDF_COMPLEX_SRCS = \
	mquant.tdf.xc \
	repeatTar.tdf.xc

#SCORE_TDF_PROCESS_COMPLEX_SRCS = $(SCORE_TDF_COMPLEX_SRCS:.tdf=.tdf.xc)

## this shows conversion to .cc for both
## simple and complex tdf sources

SCORE_CC_SIMPLE_SRCS = $(SCORE_TDF_SIMPLE_SRCS:.tdf=.cc)

SCORE_CC_COMPLEX_SRCS = \
	mquant.cc \
	mquant_noinline.cc \
	repeatTar.cc \
	repeatTar_noinline.cc 

## pull together all tdf sources to be used as an argument for 
## tdfc
SCORE_TDF_SRCS = $(SCORE_TDF_SIMPLE_SRCS) $(SCORE_TDF_COMPLEX_SRCS)

## all cc sources

#EXTRA_CC_SRC =  __copy_u2.cc __copy_s2.cc \
#                __copy_u4.cc __copy_u16.cc \
#                __copy_b9.cc __copy_u3.cc __copy_s3.cc

EXTRA_CC_SRC =  _copy_s3.cc   _copy_u2.cc  _copy_u4.cc \
                _copy_u16.cc  _copy_u3.cc \
		in_copy.cc  dctstream_copy.cc


SCORE_CC_SRCS = $(SCORE_CC_SIMPLE_SRCS) \
                $(SCORE_CC_COMPLEX_SRCS) \
		$(EXTRA_CC_SRC)

## all object files generated from .cc tdfc emitions
SCORE_CC_OBJS = $(SCORE_CC_SRCS:.cc=.o)

## the objects generated from driver, etc, code
SCORE_C_OBJS = $(SCORE_C_SRCS:.C=.o)

## all objects are here
OBJS = $(SCORE_CC_OBJS) $(SCORE_C_OBJS) $(MAIN_OBJECTS)

BUILD_OBJS = $(SCORE_C_OBJS) topAll.o

###########################################################################
# targets:

all: $(TARGET) force_instances

force_instances:
	$(MAKE) -f $(INSTANCE_MAKEFILE)

$(TARGET): makempeg.o topAll_new.o $(MAIN_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(MAIN_LIBRARIES)


makempeg.C : topAll.cc


topAll_new.cc : topAll.cc
	./comment_out.pl < topAll.cc > topAll_new.cc

#$(TARGET): makempeg.o topAll.o $(MAIN_OBJECTS)
#	$(CXX) $(CXXFLAGS) -o $@ $^ $(MAIN_LIBRARIES)

%.tdf.xc: %.tdf
	rm -f $@
	$(PARSER) $(XCFLAGS) $< > $@

$(SCORE_CC_SRCS): $(SCORE_TDF_SRCS)
	$(PARSER) $(PARSERFLAGS) $^ 2>&1 | grep -v "lose information" |grep -v READ |grep -v WRITTEN

clean:
	rm $(SCORE_CC_OBJS) $(SCORE_C_OBJS) $(TARGET)

reallyclean:
	rm -f *.o *.so *.cc *.h $(TARGET) *.fauto *~ *.instances *.xc program.feed*

depend:
	$(CXX) $(INCLUDES) -MM -MG $(SCORE_C_SRCS) > Makefile.dep


echo:
	@echo $(OBJS)


-include Makefile.dep


# cluster_clean:
# 	rm -f *.part *.graph *.flat

# cluster: JPEG_mod.part
# 	$(CLUSTER_PATH)/applypartition $<

# JPEG_mod.part: JPEG_mod.graph
# 	$(CLUSTER_PATH)/cluster $(PPC) $< $@

# JPEG_mod.graph: JPEG_mod.flat
# 	rm -f $@
# 	$(CLUSTER_PATH)/flat2graph $< > $@

# JPEG_mod.flat: $(SCORE_TDF_SRCS)
# 	rm -f $@
# 	$(PARSER) $(PARSER_INCLUDES) JPEG_mod.tdf > $@

# IModule.cc:	$(SCORE_TDF_SRCS)
# 	rm -f mquant.tdf.xc
# 	$(PARSER) $(XCFLAGS) mquant.tdf > mquant.tdf.xc
# 	$(PARSER) $(PARSERFLAGS) $(SCORE_TDF_SRCS) mquant.tdf.xc







