##############################################################################
##
## Copyright (c) 1999 The Regents of the University of California 
## Permission to use, copy, modify, and distribute this software and
## its documentation for any purpose, without fee, and without a
## written agreement is hereby granted, provided that the above copyright 
## notice and this paragraph and the following two paragraphs appear in
## all copies. 
##
## IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
## DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
## LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
## EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE. 
##
## THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
## INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
## AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON
## AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
## PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
##
##############################################################################
##
## BRASS source file
##
## SCORE TDF compiler:  Makefile, for gmake
## $Revision: 1.154 $
##
##############################################################################

#
# To build tdfc:
#   - check out latest files in top dir:	co -r RCS/*
#   - check out latest source files:		cd src;  co -r RCS/*;  cd ..
#   - remove old binaries:			cd i386-any-linux;
#						make reallyclean
#   - make new binaries:			make depend && make tdfc
#

################################################################
#
#  User settings:
#

SRC_DIR	= src
VPATH	= .:${SRC_DIR}


#  - select tool versions:

#PURIFY	= purify -cache-dir=/tmp/purify
PURIFY	= 


#  - select compiler version:

TOOLS_PATH	= /usr/bin
COMPILER	= $(TOOLS_PATH)/g++-4.4
PERL		= $(TOOLS_PATH)/perl
LEX		= $(TOOLS_PATH)/flex
YACC		= $(TOOLS_PATH)/bison
FLEXLIB	= -lfl
PARSELIB	= 
LEDA_LIBS	= -lleda -lX11
# LEDA_LIBS	= -L/usr/local/SCORE/lib -lL
EXTRA_LIBS	= -lstdc++

#  - select extra tool options:

#EXTRA_CFLAGS	= -O -g -Wall
EXTRA_CFLAGS	=    -g -Wall -DDEBUG
EXTRA_BFLAGS	=
#                 ^^^^^^^^
#                 ...shouldn't have to do this!                

#
#  End user settings
#
################################################################


LEDAHOME=/opt/leda
CC=$(COMPILER)

TARGET		= tdfc
HSRCS		= assert_tdf.h parse.h  version.h \
		  tree.h       suite.h  operator.h  state_split.h  state.h  stmt.h  expr.h \
		  symbol.h     type.h   misc.h      file.h   ops.h   gc.h \
		  instance.h   feedback.h  bindvalues.h \
		  ccannote.h   cctypes.h \
		  ccbody.h     ccheader.h  ccinstance.h \
		  cccase.h     cccopy.h    cceval.h      ccmem.h \
		  ccprep.h     ccrename.h  ccstmt.h     cctype.h ccGappaStmt.h \
		  cctestif.h \
		  rateTable.h  rateInstance.h  ribody.h  linux_meminfo.h \
		  canonical.h  mincut.h    clusterstates.h  feedback_fsm.h \
		  ir_graph.h   ir_analyze_registers.h    ir_misc.h \
		  dismantle.h  pagedfg.h   pagenetlist.h  pagenetlistdot.h \
		  blockdfg.h   streamdepth.h  \
		  synplify.h   dummypages.h  pipeext.h \
		  everilog_symtab.h  everilog.h  everilog_compose.h \
		  Q_blackbox.h     Q_wire.h Q_srl.h       Q_srl_prenone.h \
		  Q_fifo_bubl.h    Q_fifo_bubl_noen.h        \
		  Q_pipe.h         Q_pipe_noretime.h         \
		  Q_fwd_pipe.h     Q_fwd_pipe_noretime.h     \
		  Q_srl_reserve.h  Q_srl_reserve_lwpipe.h    \
		  Q_lwpipe.h       Q_srl_reserve_nolwpipe.h
CSRCS		= tdfc.cc \
		  tree.cc      suite.cc operator.cc state.cc stmt.cc expr.cc \
		  symbol.cc    type.cc  misc.cc     file.cc  ops.cc  gc.cc \
		  instance.cc  feedback.cc bindvalues.cc \
		  ccbody.cc    cccudabody.cc cccudawrapper.cc ccmicroblazebody.cc ccdfgbody.cc ccheader.cc cccudaheader.cc ccmicroblazeheader.cc ccinstance.cc \
		  ccgappabody.cc ccGappaStmt.cc\
		  cccase.cc    cccopy.cc   cceval.cc     ccmem.cc \
		  ccprep.cc    ccrename.cc ccstmt.cc     cctype.cc \
		  cctestif.cc \
		  rateTable.cc rateInstance.cc ribody.cc linux_meminfo.cc \
		  canonical.cc mincut.cc   clusterstates.cc feedback_fsm.cc \
		  ir_graph.cc  ir_analyze_registers.cc   ir_arch_dep.cc \
		  ir_bindtimearea.cc ir_create_dfg.cc ir_create_newrep.cc \
		  ir_create_ofg.cc ir_create_sfg.cc \
		  dismantle.cc pagedfg.cc  pagenetlist.cc pagenetlistdot.cc \
		  blockdfg.cc  streamdepth.cc \
		  synplify.cc  dummypages.cc pipeext.cc \
		  everilog_symtab.cc everilog.cc everilog_compose.cc
AUTO_CSRCS	= parse_tdf.tab.cc tokenize_tdf.cc
OTHER_SRCS	= parse_tdf.y      tokenize_tdf.l
REAL_SRCS	= $(HSRCS) $(CSRCS) $(OTHER_SRCS)
ALL_CSRCS	= $(CSRCS) $(AUTO_CSRCS)
OBJS		= $(ALL_CSRCS:.cc=.o)


# - max RCS revision number "X.Y" from all '.h', '.cc' files:
TDFC_VERSION	= $(shell $(PERL) -e				\
			 'while (<>) {				\
			   if (/Revision: (\d+)\.(\d+) \$$/	\
			       && ($$1>$$r1 || $$2>$$r2)   )	\
			     { $$r1=$$1; $$r2=$$2 }		\
			 }					\
			 if (($$r1 eq "") || ($$r2 eq ""))	\
			   { print "unknown" }			\
			 else					\
			   { print "$$r1.$$r2"; }'		\
		    $(SRC_DIR)/../CHANGES			\
		   )

CFLAGS	= -I. -I$(SRC_DIR) -I$(LEDAHOME)/incl -L$(LEDAHOME) -DCXX=\"$(CC)\" -DTDFC_VERSION=\"$(TDFC_VERSION)\" $(PLATFORM_CFLAGS) $(EXTRA_CFLAGS)
BFLAGS	= -v -d -t $(EXTRA_BFLAGS)
#LIBS	= -lfl -ly -lL $(EXTRA_LIBS)
LIBS	= $(PARSELIB) $(FLEXLIB) $(LEDA_LIBS) $(EXTRA_LIBS)


all:	$(SPECIFY_CC) depend $(TARGET)


$(TARGET):	${OBJS}
	rm -f $@
	${PURIFY} ${CC} -o $@ ${CFLAGS} $^ ${LIBS}


### HACK: clusterstates.cc needs newer g++ to avoid linker bug (eylon 2/4/01)
#clusterstates.o:	clusterstates.cc
#	/usr/sww/bin/g++ -c ${CFLAGS} $<

%.o:	%.cc
	${CC} -c ${CFLAGS} $<

parse_tdf.tab.cc:	parse_tdf.y 
	bison ${BFLAGS} $^ -o $@
	test -f parse_tdf.tab.cc.h && mv parse_tdf.tab.cc.h parse_tdf.tab.h || \
	test -f parse_tdf.tab.hh   && mv parse_tdf.tab.hh   parse_tdf.tab.h

tokenize_tdf.cc:	tokenize_tdf.l parse_tdf.tab.h
	${LEX} $<
	mv lex.yy.c $@

depend:	$(ALL_CSRCS) 
	rm -f Makefile.dep
	$(CC) -M ${CFLAGS} $^ >| Makefile.dep

#version.h:	$(REAL_SRCS:version.h=)
#	$(PERL) -e \
#	  'while (<>) { \
#	     if (/Revision: (\d+)\.(\d+) \$$/ && ($$1>$$r1 || $$2>$$r2)) \
#		{ $$r1=$$1; $$r2=$$2 } \
#	   } \
#	   print "static const char version[]=\"$$r1.$$r2\";\n";' \
#	$^ > $@
#	@# - extract max RCS revision number "X.Y" from all '.h', '.cc' files

clean:
	rm -f *.o $(TARGET)

reallyclean:	clean
	rm -f *.tab.h *.tab.cc *.output tokenize_tdf.cc
	rm -f Makefile.dep
	touch Makefile.dep

sinclude Makefile.dep		# - automatically-generated dependencies
sinclude Makefile.local		# - user stuff

