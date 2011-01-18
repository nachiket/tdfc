#
# Makefile for set of TDF applications each with single source file
# 'xxx.tdf' --(tdf)--> 'xxx.out'
# 'make -i'  makes all apps
#

################################################################
# user-modifiable:

PARSER		= /home/abid/PhD/HighLevelSynthesis/Compiler/tdfc
TDFCFLAGS	= 

################################################################
# do not modify:

TDFSRCS		= $(shell ls *.tdf)
OUTFILES	= $(TDFSRCS:.tdf=.out)

target: $(OUTFILES)

%.out:	%.tdf $(PARSER)
	$(PARSER) $(TDFCFLAGS) $< > $@ 2>&1

clean:
	rm *.out
