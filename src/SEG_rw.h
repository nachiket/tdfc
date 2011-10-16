//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 1999 The Regents of the University of California 
// Permission to use, copy, modify, and distribute this software and
// its documentation for any purpose, without fee, and without a
// written agreement is hereby granted, provided that the above copyright 
// notice and this paragraph and the following two paragraphs appear in
// all copies. 
//
// IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
// DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
// LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION,
// EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE. 
//
// THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON
// AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO
// PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
//
//////////////////////////////////////////////////////////////////////////////
//
// BRASS source file
//
// SCORE TDF compiler:  Verilog wrapper for Read-Write segment
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_SEG_RW_
#define _TDF_SEG_RW_


const char* SEG_rw = "\n\
// SEG_rw.v\n\
//\n\
//  - A read-write segment mapped to a BlockRAM\n\
//\n\
//  - Eylon Caspi,  11/18/03\n\
//  - Nachiket Kapre,  16/Oct/11\n\
\n\
\n\
`ifdef  SEG_rw\n\
`else\n\
`define SEG_rw\n\
\n\
\n\
module SEG_rw (clock, reset, addr_d, addr_e, addr_v, addr_b, dataR_d, dataR_e, dataR_v, dataR_b, dataW_d, dataW_e, dataW_v, dataW_b, write_d, write_e, write_v, write_b);\n\
\n\
   parameter nelems =  0;    // - greatest #items in memory (maybe unused) \n\
   parameter awidth = 16;    // - width of arrr (addr_d)\n\
   parameter dwidth = 16;    // - width of data (dataR_d, dataW_d)\n\
\n\
   input     clock;\n\
   input     reset;\n\
   \n\
   input  [awidth-1:0] addr_d;\n\
   input  addr_e; \n\
   input  addr_v;\n\
   output addr_b;\n\
   output [dwidth-1:0] dataR_d;\n\
   output dataR_e;\n\
   output dataR_v;\n\
   input  dataR_b;\n\
   input  [dwidth-1:0] dataW_d;\n\
   input  dataW_e;\n\
   input  dataW_v;\n\
   output dataW_b;\n\
   input  write_d;\n\
   input  write_e;\n\
   input  write_v;\n\
   output write_b;\n\
\n\
   wire state;\n\
   wire [1:0] statecase;\n\
   wire flag_steady_0;\n\
   wire flag_steady_1;\n\
\n\
   SEG_rw_fsm fsm (clock, reset, addr_e, addr_v, addr_b, dataR_e, dataR_v, dataR_b, dataW_e, dataW_v, dataW_b, write_e, write_v, write_b, state, statecase, flag_steady_0, flag_steady_1);\n\
   SEG_rw_dp #(.nelems(nelems),.awidth(awidth),.dwidth(dwidth)) dp (clock, reset, addr_d, dataR_d, dataW_d, write_d, state, statecase, flag_steady_0, flag_steady_1);\n\
\n\
\n\
endmodule // SEG_rw\n\
\n\
\n\
`endif	// `ifdef  SEG_rw\n\
";


#endif	// #ifndef _TDF_SEG_RW_
