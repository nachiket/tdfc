###########################################################################
# directory definitions:

# customizable parameters:

DEBUGFLAG =
PROFLAG =

include $(SCORE_PATH)/app/common/Makefile.common

TARGET = encode

########################################
# sources:

.PRECIOUS:      %.cc %.h

# list tdf operator descriptions here:
SCORE_TDF_SRCS = $(shell ls *.tdf)

# list C driver sources here:
SCORE_C_SRCS = encode.C

##########################################################################

SCORE_TDF_OBJS = $(SCORE_TDF_SRCS:.tdf=.o)
SCORE_C_OBJS = $(SCORE_C_SRCS:.C=.o)
OBJS = $(SCORE_TDF_OBJS) $(SCORE_C_OBJS) $(MAIN_OBJECTS)

###########################################################################
# targets:

all: $(EXPORT_FILES) $(TARGET) force_instances

force_instances:
	$(MAKE) -f $(INSTANCE_MAKEFILE)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(MAIN_LIBRARIES) 

%.h %.cc: %.tdf
	$(PARSER) $(PARSERFLAGS) $(PARSER_INCLUDES) $(SCORE_TDF_SRCS)

clean:
	rm -f *.o $(TARGET) *~
	$(MAKE) -f $(INSTANCE_MAKEFILE) clean

reallyclean:
	rm -f *.o *.so *.cc *.h $(TARGET) *.fauto *~ *.instances *.xc

# unused?

depend: 
	$(CXX) $(INCLUDES) -MM -MG $(SCORE_C_SRCS) > Makefile.dep


-include Makefile.dep
