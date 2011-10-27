INCLUDE_PATHS = -I/opt/leda/incl \
		-I/opt/ScoreRuntime 

LIBRARY_PATHS = -L/opt/ScoreRuntime \
		-L/opt/leda

LIBRARIES= -lleda -lX11 -lScoreRuntime -lpthread

ALL_TDF=*.tdf

ALL: 	$(ALL_TDF:.tdf=.cu) add.o 

$(ALL_TDF:.tdf=.cu):
	tdfc -ecuda *.tdf 

add.o:	add.cpp *.tdf $(ALL_TDF:.tdf=.cu)
	nvcc add.cu add.cuh
	g++ add.cpp \
		$(ALL_TDF:.tdf=.cu) \
		-o add.o

clean:	
	rm -f $(ALL_TDF:.tdf=.cu) $(ALL_TDF:.tdf=.cuh)
