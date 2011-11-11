INCLUDE_PATHS = -I/opt/leda/incl \
		-I/opt/include/cuda \
		-I/opt/ScoreRuntime 

LIBRARY_PATHS = -L/opt/ScoreRuntime \
		-L/opt/cuda \
		-L/opt/leda

LIBRARIES= -lleda -lX11 -lScoreRuntime -lpthread

ALL_TDF=*.tdf

ALL: 	$(ALL_TDF:.tdf=.cu) add.o 

$(ALL_TDF:.tdf=.cu):
	tdfc -ecuda *.tdf 
	mv add.cpp addtest.cu

add.o:	addtest.cu *.tdf $(ALL_TDF:.tdf=.cu)
	nvcc -m64 -o add.o -c add.cu
	nvcc -m64 -o add.cu.o -c addtest.cu
	g++ -fPIC -m64 -I. -I/opt/cuda/include -L/usr/lib/nvidia-current -L/opt/cuda/lib64 add.cu.o add.o -lcuda -lcudart

clean:	
	rm -f $(ALL_TDF:.tdf=.cu) $(ALL_TDF:.tdf=.cuh)
