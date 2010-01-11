all:	add8.cc wrapper.cc
#dd8_test.o:	add8_test.cc ~/workspace/ScoreRuntime/libScoreRuntime.a add8.cc add8.h add8.tdf ~/workspace/tdfc/tdfc
#	g++ -Wno-write-strings -I/opt/leda/incl -I/home/nachiket/workspace/ScoreRuntime -L/home/nachiket/workspace/ScoreRuntime -L/opt/leda add8_test.cc add8.cc -o add8_test.o -lleda -lX11 -lScoreRuntime -lpthread
add8.cc add8.h:	add8.tdf ~/workspace/tdfc/tdfc
	tdfc -embz add8.tdf
wrapper.cc wrapper.h:	wrapper.tdf ~/workspace/tdfc/tdfc
	tdfc -embz wrapper.tdf add8.tdf
clean:
	rm *.o *.h
