# LEDA_DIR	 = /project/cs/brass/a/tools/free/LEDA-4.2/LEDA-4.2-i386-linux-g++-2.95/
# LEDA_DIR	 = /project/cs/brass/a/tools/free/LEDA-4.0/LEDA-4.0-i386-linux-eg++-1.1
LEDA_DIR	 = /project/cs/brass/a/tools/free/LEDA-3.8/LEDA-3.8a-linux-egcs-1.1
LEDA_INCLUDE_DIR = ${LEDA_DIR}/incl
LEDA_LIB_DIR	 = ${LEDA_DIR}

CFLAGS = -g -I${LEDA_INCLUDE_DIR} -L${LEDA_LIB_DIR}
LIBS = -lG -lL

mincut:	mincut.cc ../../../src/mincut.cc ../../../src/clusterstates.cc
	g++ ${CFLAGS} -o $@ $< ${LIBS}
