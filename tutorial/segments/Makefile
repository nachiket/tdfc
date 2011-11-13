INCLUDE_PATHS = -I/opt/leda/incl \
		-I/opt/ScoreRuntime 

LIBRARY_PATHS = -L/opt/leda \
		-L/opt/ScoreRuntime

LIBRARIES= -lleda -lX11 -lScoreRuntime -lpthread

ALL_TDF=*.tdf

ALL: 	$(ALL_TDF:.tdf=.cc) segment_r_test.o segment_seq_r_test.o segment_seq_cyclic_r_test.o

$(ALL_TDF:.tdf=.cc):
	tdfc -ecc *.tdf 

clean:	
	rm -f $(ALL_TDF:.tdf=.cc) $(ALL_TDF:.tdf=.h)

segment_r_test.o:	segment_r_test.cpp *.tdf $(ALL_TDF:.tdf=.cc)
	g++-4.4 -g -Wno-write-strings \
		$(INCLUDE_PATHS) \
		$(LIBRARY_PATHS) \
		segment_r_test.cpp \
		$(ALL_TDF:.tdf=.cc) \
		-o segment_r_test.o \
		$(LIBRARIES) 

segment_seq_r_test.o:	segment_seq_r_test.cpp *.tdf $(ALL_TDF:.tdf=.cc)
	g++-4.4 -g -Wno-write-strings \
		$(INCLUDE_PATHS) \
		$(LIBRARY_PATHS) \
		segment_seq_r_test.cpp \
		$(ALL_TDF:.tdf=.cc) \
		-o segment_seq_r_test.o \
		$(LIBRARIES) 

segment_seq_cyclic_r_test.o:	segment_seq_cyclic_r_test.cpp *.tdf $(ALL_TDF:.tdf=.cc)
	g++-4.4 -g -Wno-write-strings \
		$(INCLUDE_PATHS) \
		$(LIBRARY_PATHS) \
		segment_seq_cyclic_r_test.cpp \
		$(ALL_TDF:.tdf=.cc) \
		-o segment_seq_cyclic_r_test.o \
		$(LIBRARIES) 

