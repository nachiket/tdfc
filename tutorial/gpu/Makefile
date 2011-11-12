INCLUDE_PATHS = -I/opt/leda/incl \
		-I/opt/ScoreRuntime \
		-I/opt/cuda/include 

LIBRARY_PATHS = -L/opt/leda \
		-L/opt/ScoreRuntime \
		-L/opt/cuda/lib64 \
		-L/usr/lib/nvidia-current 

LIBRARIES= -lleda -lX11 -lScoreRuntime -lpthread

ALL_TDF=*.tdf

ALL: 	$(ALL_TDF:.tdf=.cu) add.o 

NVCC_FLAGS=-O3 -arch sm_20 -Xptxas "-v"

$(ALL_TDF:.tdf=.cu):
	tdfc -ecuda *.tdf 
	mv add.cpp addtest.cu

add.o:	addtest.cu *.tdf $(ALL_TDF:.tdf=.cu)
	nvcc $(NVCC_FLAGS) -o add.cu.o addtest.cu add.cu 
#	g++ -fPIC -I. -I/opt/cuda/include -L/usr/lib/nvidia-current -L/opt/cuda/lib64 add.cu.o add.o -lcuda -lcudart

clean:	
	rm -f $(ALL_TDF:.tdf=.cu) $(ALL_TDF:.tdf=.cuh)
