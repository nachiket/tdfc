------------------------------------------------------------------------------

ABOUT

    This document describes how to find, build, use, and maintain

    Eylon Caspi, eylon@cs.berkeley.edu
    tdfc, the SCORE TDF compiler, version 1.160, 9/1/05.


    Nachiket Kapre, nachiket@cs.caltech.edu
    tdfc, the SCORE TDF compiler with extensions for GraphMachine compilation, version 1.17, 8/27/2009.

------------------------------------------------------------------------------

FINDING TDFC

    (1) Distribution

        The latest version of tdfc, compiled for x86 Linux,
        is available via NFS on the U.C.Berkeley, Computer Science,
        BRASS group's file space:

        /project/cs/brass/a/tools/brass/gold/SCORE/tdfc/

    (2) CVS

        tdfc source code lives in the BRASS CVS repository.

        At Berkeley, use:
            setenv CVSROOT /project/cs/brass/a/tools/brass/develop/CVSROOT

        Remotely, use:
            setenv CVSROOT :ext:user@dynne.cs.berkeley.edu:/project/cs/brass/a/tools/brass/develop/CVSROOT
            setenv CVS_RSH ssh        

------------------------------------------------------------------------------

DIRECTORY HIERARCHY

    + tdfc
        + CHANGES                       - History of changes
        + Makefile                      - Makefile to build tdfc
        + README.txt                    - This document
        + TODO                          - Future work
        + TODO-cc                       - Future work for ``tdfc -ecc'' (old?)
        + apps/                         - Application build tree
            + everilog-158-BASE/        - Application build tree for tdfc 1.158
            + everilog-160-BASE/        - Application build tree for tdfc 1.160
        + cctest/                       - Test cases to debug ``tdfc -ecc''
        + debug/                        - Test cases to debug   tdfc
        + examples/                     - TDF examples
        + i386-any-cygwin/              - Compile tdfc in here (Cygwin)
        + i386-any-linux/               - Compile tdfc in here (Linux)
        + info/                         - Documentation + dev notes (some old)
        + powerpc-apple-darwin/         - Compile tdfc in here (Mac OS X)
        + src/                          - tdfc Source code
        + tdfc -> i386-any-linux/tdfc   - Symbolic link to compiled tdfc
        + test/                         - Test cases to regress tdfc (old)
        + tools/                        - Scripts to assist compiling TDF
        + verilog/                      - Components for ``tdfc -everilog''
            + misc/                     - Hand coded Verilog examples, tests
            + queues/                   - Hand coded Verilog queues
            + segmentops/               - Hand coded Verilog segment operators
            + zle/                      - Hand coded zero-length encoder app

------------------------------------------------------------------------------

BUILDING TDFC

    tdfc is used primarily on x86 Linux,
    but it has been successfully compiled for:

      - x86 Linux  (default Makefile),
      - x86 Windows with Cygwin,
      - Mac OS X
      - Solaris

    Required tools / libraries:

      - g++ 2.96   (or another version compatible with LEDA)
      - GNU flex   (tested version 2.5.4)
      - GNU bison  (tested version 1.5.4)
      - GNU make   (tested version 3.80)
      - LEDA       (tested version 3.7.1 - 4.2)

    About LEDA:

       - LEDA = Library of Efficient Data structures and Algorithms
       - Originally free from Max Plank Institut Informatik
       - Now commercial from Algorithmic Solutions
           <http://www.algorithmic-solutions.com/enleda.htm>
       - LEDA 3.7.1 source code:
           /project/cs/brass/a/tools/free/LEDA-3.7.1/
       - LEDA 3.7.1 binaries for x86 Linux, g++ 2.96:  (recommended)
           /project/cs/brass/a/tools/free/LEDA-3.7.1/LEDA-3.7.1-i386-linux-g++-2.96/
       - LEDA binaries for other compilers / architectures:
           /project/cs/brass/a/tools/free/LEDA-*/
       - Note, our versions of LEDA are old and incompatible with g++ 3

    Building tdfc:

      - Edit ``Makefile'' for your architecture and LEDA version

      - cd i386-any-linux  (or another architectural build directory)
      - ln -s ../Makefile
      - make depend
      - make tdfc
      - cd ..
      - ln -s i386-any-linux/tdfc

------------------------------------------------------------------------------

USING TDFC

    tdfc manual (man page):   tdfc/info/tdfc.man

    Compiling to FPGA:        tdfc/info/compiling-to-fpga-howto.txt
                              tdfc/info/compiling-to-fpga-limitations.txt

    Compiling to C:           tdfc/info/howto/tdfc.html

    TDF language reference:   ``Programming SCORE''
                              <http://brass.cs.berkeley.edu/documents/programming-score.html>

------------------------------------------------------------------------------

COMPILING JOE'S SEVEN MULTIMEDIA APPS TO FPGA

    - Joe Yeh's seven multimedia applications, tested on Xilinx Virtex-II Pro:

        IIR                -  IIR filter
        jpeg_decode_nopar  -  JPEG    image decoder
        jpeg_encode_nopar  -  JPEG    image encoder
        mpeg_encode_nopar  -  MPEG 1  video encoder, {IPP}    frame pattern
        mpeg_encode_PB     -  MPEG 1  video encoder, {IBBPBB} frame pattern
        wavelet            -  Wavelet image encoder
        wavelet_decode     -  Wavelet image decoder

    - cd apps
    - cp -rp everilog-160-BASE everilog-160     - new copy of appl. build tree
    - cd everilog-160
    - cvs co `cat APPS`                         - get 7 applications
    - foreach app (`cat APPS`)
        pushd $app
        ./make-synplify                         - run tdfc on each application
        popd

    - To compile the resulting Verilog to FPGA, see:

        Compiling to FPGA:      tdfc/info/compiling-to-fpga-howto.txt

    - Results:

        Stream graphs:          tdfc/info/netlists-tdfc-1.160/
        Structural statistics:  tdfc/info/STATS-160-apps.txt
        Clock rates (no PAR):   tdfc/info/STATS-160-L3W0-syn180-ise61i-platforms.txt
        Complete results:       ``Design Automation for Streaming Systems''
                                <http://www.cs.berkeley.edu/~eylon/phd/>

------------------------------------------------------------------------------

MAINTAINING TDFC

    - Important source files:

        * tdfc/src/tdfc.cc          - Main entry, cmd line, main compile flow
        * tdfc/src/tree.h           - Base class for TDF abstract syntax tree
        * tdfc/src/tokenize_tdf.l   - Flex tokenizer         front-end
        * tdfc/src/parse_tdf.y      - Bison lexical analyzer front-end
        * tdfc/src/cc*.cc           - C       back-end  (``tdfc -ecc'')
        * tdfc/src/everilog*.cc     - Verilog back-end  (``tdfc -everilog'')
        * tdfc/src/Q_*.h            - Verilog queues from tdfc/verilog/queues

    - Compiler flow:

        * tdfc/info/tdfc-passes-new.ppt

    - CVS discipline:

        * Check out from CVS:   cvs co tdfc
                                cd     tdfc
        * Edit source  files
        * Edit CHANGES file to reflect history, assign new version number
        * Check in to CVS:      cvs commit
        * Tag version 1.XXX:    cvs tag v1_XXX

        * Additional info:      tdfc/info/howto/modify-tdfc.html

------------------------------------------------------------------------------
