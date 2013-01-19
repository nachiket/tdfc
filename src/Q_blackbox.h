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
// SCORE TDF compiler:  Verilog black-box stream queue "Q_blackbox"
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_Q_BLACKBOX_
#define _TDF_Q_BLACKBOX_


static char* Q_blackbox = "\n\
// Q_blackbox.v\n\
//\n\
//  - Black box stream queue\n\
//  - depth parameter is ignored\n\
//  - 1 <= width\n\
//\n\
//  - Synplify 7.1\n\
//  - Eylon Caspi,  11/18/03\n\
\n\
\n\
`ifdef  Q_blackbox\n\
`else\n\
`define Q_blackbox\n\
\n\
\n\
module Q_blackbox (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b)  /* synthesis syn_black_box */ ;\n\
\n\
   parameter depth =  0;    // - greatest #items in queue  (unused)\n\
   parameter width = 16;    // - width of data (i_d, o_d)\n\
\n\
   input     clock;\n\
   input     reset;\n\
   \n\
   input  [width-1:0] i_d;	// - input  stream data (concat data + eos)\n\
   input              i_v;	// - input  stream valid\n\
   output             i_b;	// - input  stream back-pressure\n\
\n\
   output [width-1:0] o_d;	// - output stream data (concat data + eos)\n\
   output             o_v;	// - output stream valid\n\
   input              o_b;	// - output stream back-pressure\n\
\n\
endmodule // Q_blackbox\n\
\n\
\n\
`endif	// `ifdef  Q_blackbox\n\
";


#endif	// #ifndef _TDF_Q_BLACKBOX_
