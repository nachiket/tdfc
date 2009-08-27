###########################################################################
# directory definitions:

# customizations go here:

DEBUGFLAG = 
PROFLAG =

include $(SCORE_PATH)/app/common/Makefile.common

## the name of the user executable
TARGET = score_jpeg

## this is typically the driver
SCORE_C_SRCS = makejpeg.C

########################################

## list simple tdf sources here:
## they will be converted straight to .cc

SCORE_TDF_SIMPLE_SRCS = \
	CheckZero.tdf  \
	HuffMachine.tdf  \
	ISub.tdf  \
	JPEG_mod.tdf  \
	JZLE.tdf  \
	MixToHuff.tdf  \
	tpose.tdf  \
	zigzag_.tdf  \
	adjust.tdf  \
	fllm.tdf  \
	repeater_a.tdf  \
	repeater_x.tdf

#SCORE_TDF_SIMPLE_SRCS = \
#	CheckZero.tdf  \
#	HuffMachine.tdf  \
#	ISub.tdf  \
#	JPEG_mod.tdf  \
#	JZLE.tdf  \
#	MixToHuff.tdf  \
#	muxer.tdf  \
#	tpose.tdf  \
#	write_seg.tdf  \
#	zigzag_.tdf  \
#	adjust.tdf  \
#	fllm.tdf  \
#	repeater_a.tdf  \
#	repeater_x.tdf

## list complex tdf sources here:
## they will be converted to .xc

SCORE_TDF_COMPLEX_SRCS = \
	jquant.tdf 

SCORE_TDF_PROCESS_COMPLEX_SRCS = $(SCORE_TDF_COMPLEX_SRCS:.tdf=.tdf.xc)

## this shows conversion to .cc for both
## simple and complex tdf sources

SCORE_CC_SIMPLE_SRCS = $(SCORE_TDF_SIMPLE_SRCS:.tdf=.cc)

SCORE_CC_COMPLEX_SRCS = \
	jquant.cc \
	jquant_noinline.cc 

## pull together all tdf sources to be used as an argument for 
## tdfc
SCORE_TDF_SRCS = $(SCORE_TDF_SIMPLE_SRCS) $(SCORE_TDF_PROCESS_COMPLEX_SRCS)

## all cc sources
SCORE_CC_SRCS = $(SCORE_CC_SIMPLE_SRCS) $(SCORE_CC_COMPLEX_SRCS)


## all object files generated from .cc tdfc emitions
SCORE_CC_OBJS = $(SCORE_CC_SRCS:.cc=.o)

## the objects generated from driver, etc, code
SCORE_C_OBJS = $(SCORE_C_SRCS:.C=.o)

## all objects are here
OBJS = $(SCORE_CC_OBJS) $(SCORE_C_OBJS) $(MAIN_OBJECTS)

###########################################################################
# targets:

all: $(TARGET) force_instances

force_instances:
	$(MAKE) -f $(INSTANCE_MAKEFILE)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(MAIN_LIBRARIES)

%.tdf.xc: %.tdf
	rm -f $@
	$(PARSER) $(XCFLAGS) $< > $@

$(SCORE_CC_SRCS): $(SCORE_TDF_SRCS)
	$(PARSER) $(PARSERFLAGS) $^ 

df:     $(SCORE_TDF_SRCS) 
	$(PARSER) -eIR $^
	rm -f *.fsm
	cat *.df

clean:
	rm $(OBJS) $(TARGET)

reallyclean:
	rm -f *.o *.so *.cc *.h $(TARGET) *.fauto *~ *.instances *.xc

depend:
	$(CXX) $(INCLUDES) -MM -MG $(SCORE_C_SRCS) > Makefile.dep


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







