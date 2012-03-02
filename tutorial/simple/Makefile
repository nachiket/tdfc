INCLUDE_PATHS = -I/opt/leda/incl \
		-I/opt/ScoreRuntime 

LIBRARY_PATHS = -L/opt/leda \
		-L/opt/ScoreRuntime

LIBRARIES= -lleda -lX11 -lScoreRuntime -lpthread

#ALL_TDF=$(wildcard *.tdf)
ALL_TDF := add.tdf
ALL_TDF_CC := $(ALL_TDF:.tdf=.cc)

ALL: 	add.o $(ALL_TDF:.tdf=.cc)

$(ALL_TDF:.tdf=.cc): $(ALL_TDF)
	tdfc -ecc $(ALL_TDF) 

add.o:	add_test.cc $(ALL_TDF) $(ALL_TDF_CC)
	g++-4.4 -g -Wno-write-strings \
		$(INCLUDE_PATHS) \
		$(LIBRARY_PATHS) \
		add_test.cc \
		$(ALL_TDF:.tdf=.cc) \
		-o add_test.o \
		$(LIBRARIES) 

clean:	
	rm -f $(ALL_TDF:.tdf=.cc) $(ALL_TDF:.tdf=.h)
