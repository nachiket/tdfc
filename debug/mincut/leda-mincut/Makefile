LEDA_INCLUDE_DIR = /project/cs/brass/a/tools/free/LEDA-4.0/LEDA-4.0-i386-linux-eg++-1.1/incl
LEDA_LIB_DIR = /project/cs/brass/a/tools/free/LEDA-4.0/LEDA-4.0-i386-linux-eg++-1.1

CFLAGS = -I${LEDA_INCLUDE_DIR} -L${LEDA_LIB_DIR}
LIBS = -lG -lL

mincut:	mincut.cc
	g++ ${CFLAGS} -o $@ $< ${LIBS}
