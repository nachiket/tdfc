Hand coded Verilog for Zero run-length encoder with no EOS cases
----------------------------------------------------------------

TDF files:

    zle.tdf:     Original TDF

    zle_xc.tdf:  TDF expanded by "tdfc -xc"


Verilog files, styled for Synplify 7.1:

    zle_xc*.v:   Hand coded verilog, separate FSM and datapath modules;
		 FSM: implicit fire signal.
		      [ case (state) ... if (i_v) "fire" else "no fire" ]
		 DP:  flags in assign, outputs + regs in always

    zle_xc2*.v:  Hand coded verilog, separate FSM and datapath modules;
		 FSM: explicit fire signal,
		      duplicate "no fire" code in each state case.
                      [ case (state) ... if (fire) "fire" else "no fire" ]
		 DP:  identical to zle_xc*.v
		 --> Compile for speed:  same speed. 9% smaller
		 --> Compile for area:   19% faster, same area

    zle_xc3*.v:  Hand coded verilog, separate FSM and datapath modules;
		 FSM: explicit fire signal,
		      single "no fire" code outside case
                      [ if (!fire) "no fire" else case (state) ... "fire" ]
		 DP:  identical to zle_xc*.v
		 --> same results as zle_xc*.v

    zle_xc4*.v:  Same as zle_xc*.v, using `define instead of parameter.
		 --> syntax errors

    zle_xc5*.v:  Same as zle_xc*.v,
		 with FSM split to have separate always/case blocks
		 for FSM next-state and for FSM output functions.
		 --> same result as zle_xc*.v

    zle_xc6*.v:  Hand coded verilog, single module FSM + datapath;
		 FSM:  implicit fire signal,
		       single behavioral FSM block for FSM + all datapath.
		 DP:   in FSM block
		 --> Compile for speed:  12% slower, 7% smaller than zle_xc*.v
		 --> Compile for area:    6% slower, 9% smaller than zle_xc*.v

/*
    zle_xc7*.v:  Hand coded verilog, single module FSM + datapath;
		 FSM: implicit fire signal,
		      single behavioral FSM block for FSM + partial datapath.
		 DP:  flags in assign, outside FSM block,
		      outputs + regs inside    FSM block.

    zle_xc8*.v:  Hand coded verilog, single module FSM + datapath;
		 FSM:  has implicit fire signal,
		 DP:   entirely in "assign", outside FSM block
*/

    zle_xc9*.v:  Same as zle_xc*.v, with partial datapath resource sharing;
		 Hand coded verilog, separate FSM and datapath modules;
		 Unlike zle_xc*.v where FSM sends state to DP for mux selects,
		 zle_xc9*.v sends separate mux selects for DP output,
		 register, and history (consumption) to get smaller muxes.
		 FSM: implicit fire signal.
		      [ case (state) ... if (i_v) "fire" else "no fire" ]
		 DP:  flags, outputs, regs choices in assign, choose in always,
		      datapath resource sharing, but no flag wire sharing.
		 --> Compile for speed:  24% slower,  4% smaller than zle_xc*.v
		 --> Compile for area:    3% slower, 24% smaller than zle_xc*.v

    zle_xcA*.v:  Same as zle_xc*.v, with FULL datapath resource sharing;
		 Hand coded verilog, separate FSM and datapath modules;
		 Extends zle_xc9*.v by having FSM send flag select to DP,
		 and DP returns only one instead of all flags to FSM.
		 FSM: implicit fire signal.
		      [ case (state) ... if (i_v) "fire" else "no fire" ]
		 DP:  flags, outputs, regs choices in assign, choose in always,
		      datapath resource sharing and flag wire sharing.
		 --> Compile for speed:   5% slower,  4% smaller than zle_xc*.v
		 --> Compile for area:    3% slower, 25% smaller than zle_xc*.v
		 --> Almost same results as zle_xc9*.v

    zle_xcB*.v:  Same as zle_xc*.v with 7->8 bit datapath instead of 3->4 bit.
		 --> 237MHz max  (datapath speed)

    zle_xcC*.v:  Same as zle_xc*.v, but FSM uses sequenced,
		 blocking assignments  (e.g.:  x=y; if (c) x=z;)
		 instead of mutually-exclusive non-blocking assignments
		 (e.g.:  if (c) x<=z; else x<=y;)
		 --> Same result as zle_xc*.v

    zle_xcQ2*.v:   Same as zle_xc*.v, surrounded by Q2 queues
		   --> 286MHz max

    zle_xcBQ2*.v:  Same as zle_xcB*.v (7->8 bit), surrounded by Q2 queues
		   --> 237MHz max  (datapath speed)

    zle*.v:      TDF Without "-xc", 3 states with nested ifs.
		 Hand coded verilog, separate FSM and datapath modules;
		 FSM: implicit fire signal.
		      [ case (state) ... if (i_v) "fire" else "no fire" ]
		 DP:  flags in assign, outputs + regs in always
		 --> 237MHz max

    zle_2*.v:    Same as zle*.v,
		 except DP has duplicate "if (fire)" check in each state case
		 (zle*.v has "if (fire)" once, outside state case stmt)
		 --> almost same as zle*.v

    zleB*.v:     Same as zle*.v with 7->8 bit datapath instead of 3->4 bit.
		 --> 237MHz max  (datapath speed)
    

    zleQ2*.v:    Same as zle*.v, surrounded by Q2 queues
		 --> 236MHz max  (just under datapath speed)

    zle_BQ2*.v:  Same as zle_B*.v (7->8 bit), surrounded by Q2 queues
		 --> 182MHz max  (well under datapath speed... new crit path)


Synthesis with Synplify 7.1 -- observations:

  (1)  Separate FSM and datapath are roughly additive in # of LUTs
         (within 0%-10%)

  (2)  Synplify FSM compiler infers fsm from "always @(inputs)";
         it is able to swallow input logic (e.g. explicit fire signal)
         into next-state function, but not into output function...
         (did I miss some directive?)

  (3)  Explicit fire FSM (zle_xc2) is not faster than implicit fire,
         unless compiling for area, in which case it is bigger!
         - fire signal does _not_ become FSM input,
             but it is computed and used to produce FSM outputs
         - Eylon's wish list:  write Verilog code with a named fire signal
                               that Synplify optimizes away;  no go, see (2)

  (4)  Explicit fire FSM with single "no fire" code (zle_xc3)
         is identical to implicit fire FSM (zle_xc) under optimization,
	 and yet the unoptimized mapping gets better area
	 than zle_xc/zle_xc3 optimized!
         - fire signal is _not_ FSM input,
             but it is computed and used to produce FSM outputs

  (5)  Synplify can create an FSM from multiple "always @(...)"
         combinational blocks, e.g. FSM next-state + FSM output blocks.
         Same results as single, unified block.  (see zle_xc5)

  (6)  Datapath sharing with densely encoded output mux selects (zle_xc9*.v),
         mapped for area, is roughly same speed but 25% smaller
         than no datapath sharing (zle_xc*.v).
         Interestingly, mapped for speed is 24% slower at same area.

  (7)  Flag wire sharing (zle_xcA*.v) on top of datapath sharing
         yields roughly same results as plain datapath sharing (zle_xc9*.v)

  (8)  Specifying aggressive target clock:  (for zle_xcA*.v, with pads)
         - request nothing:                                     --> 124MHz
         - request 130MHz:  "Changing pad type from OBUF to OBUF_F_24",
		            and no logic changes	        --> 188MHz
	 - request 200MHz:  "Changing pad type from OBUF to OBUF_F_24",
			    and Added 5 registers + 1 LUT via
			    "Timing Driven Replication"         --> 222MHz
         - request 230MHz with retiming and pipelining:
			    "Changing pad type from OBUF to OBUF_F_24",
			    and Added 4 registers + 4 LUT via
			    "Timing Driven Replication",
			    and Added 2 register via Retiming   --> 232MHz
       (is there a way to prototype without pads?)

  (9)  To work without pads, check implementation option
         Device Implementation Option : Disable I/O Insertion
	 --> clock rate increases to 200-350MHz

 (10)  Q2 queue I/O limits clock rate to ~230-280MHz  (Q2 itself is 380MHz)

 (11)  Synplify recovers dataflow from sequenced blocking assignments.
         Programmer can assign to a reg (whether register or temporary)
	 multiple times and infer entire computation correctly.

 (12)  If a non-register reg is live only within a begin-end block,
         then Synplify is smart enough to treat it as a wire / temporary
	 and not instantiate latches for other paths or for next cycle.


Questions:

  (1)  Synplify does not seem to flatten module hierarchy before synthesis.
       Is there a directive to flatten, so as to expose more optimizations?
       [ read synthesis directives ]

  (2)  Try without -xc.  How bad is clock cycle?

  (3)  Try with end conditions.


[ Original location:  /home/cs/eylon/brass/tdfc/verilog/ ]
