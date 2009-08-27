# stl's SCORE Makefile

# customizable parameters:

DEBUGFLAG = -O3 -DNDEBUG
PROFLAG =

# include /cs/research/ic/software/SCORE.aug02/app/common/Makefile.common
include /usr/local/SCORE/app/common/Makefile.common

# name of the executable user program
TARGET = myapp

# sources:
.PRECIOUS:      %.cc %.h

# list tdf operator descriptions here:
SCORE_TDF_SRCS = $(shell ls *.tdf)

# list C driver sources here:
SCORE_C_SRCS = myapp.C

# all dependencies below will force correct building process for typical
# fileflow (.tdf --> { master C++, instance C++} 
# if you require more complexity, such as -xc transformations, separate
# the "special" files and deal with them separately.

# unless there are special cases to handle, code below does not need
# to be modified

##########################################################################

SCORE_TDF_OBJS = $(SCORE_TDF_SRCS:.tdf=.o)
SCORE_C_OBJS = $(SCORE_C_SRCS:.C=.o)
OBJS = $(SCORE_TDF_OBJS) $(SCORE_C_OBJS) $(MAIN_OBJECTS)

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
	$(MAKE) -f Makefile.instance clean

reallyclean:
	rm -f *.o *.so *.cc *.h $(TARGET) *.fauto *~ *.instances

depend: 
	$(CXX) $(INCLUDES) -MM -MG $(SCORE_C_SRCS) > Makefile.dep

-include Makefile.dep
