INCLUDE_PATHS = -I/opt/leda/incl \
		-I/opt/ScoreRuntime 

LIBRARY_PATHS = -L/opt/leda \
		-L/opt/ScoreRuntime

LIBRARIES= -lleda -lX11 -lScoreRuntime -lpthread

ALL_TDF=*.tdf

ALL: 	$(ALL_TDF:.tdf=.cc) add.o twoadd.o

$(ALL_TDF:.tdf=.cc):
	tdfc -ecc *.tdf 

add.o:	add_test.cpp *.tdf $(ALL_TDF:.tdf=.cc)
	g++-4.4 -g -Wno-write-strings \
		$(INCLUDE_PATHS) \
		$(LIBRARY_PATHS) \
		add_test.cpp \
		$(ALL_TDF:.tdf=.cc) \
		-o add_test.o 
		$(LIBRARIES) 

clean:	
	rm -f $(ALL_TDF:.tdf=.cc) $(ALL_TDF:.tdf=.h)
