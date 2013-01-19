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
// SCORE TDF compiler:  Verilog pipelined stream w/queue minus queue "Q_lwpipe"
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_Q_LWPIPE_
#define _TDF_Q_LWPIPE_


const char* Q_lwpipe = "\n\
// Q_lwpipe.v\n\
//\n\
//  - Stream with parameterizable queue depth, bit width,\n\
//      logic pipelining depth, and interconnect (wire) pipelining depth,\n\
//      but WITHOUT THE QUEUE\n\
//      (this is the first half of ``Q_srl_reserve_lwpipe.v'')\n\
//       this is: [ Q_fwd_pipe --> Q_pipe_noretime ]\n\
//       out  of: [ Q_fwd_pipe --> Q_pipe_noretime --> Q_srl_reserve ]\n\
//  - Stream I/O is triple (data, valid, back-pressure),\n\
//      with EOS concatenated into the data\n\
//  - 2 <= depth <= 256\n\
//  - 1 <= width\n\
//  - 0 <= lpipe    (logic             pipelining depth)\n\
//  - 0 <= wpipe    (interconnect/wire pipelining depth)\n\
//\n\
//  - Synplify 7.1\n\
//  - Eylon Caspi,  8/2/04\n\
\n\
\n\
`ifdef  Q_lwpipe\n\
`else\n\
`define Q_lwpipe\n\
\n\
\n\
`include \"Q_fwd_pipe.v\"\n\
`include \"Q_pipe_noretime.v\"\n\
// `include \"Q_srl_reserve.v\"\n\
\n\
\n\
module Q_lwpipe (clock, reset, i_d, i_v, i_b, o_d, o_v, o_b);\n\
\n\
   parameter depth   = 16;   // - greatest #items in queue (IGNORED)\n\
   parameter width   = 16;   // - width of data (i_d, o_d)\n\
   parameter lpipe   =  0;   // - logic pipelining depth (retimable fwd regs)\n\
   parameter wpipe   =  0;   // - wire  pipelining depth (nonretim bidir regs)\n\
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
   parameter 	      l0     = (lpipe==0);\n\
   parameter 	        w0   = (wpipe==0);\n\
   parameter 	      l0w0   = (l0 && w0);\n\
   \n\
   parameter 	      lpipe_ = (l0 ? 1 : lpipe); // - safe lpipe for Q_fwd_pipe\n\
   parameter 	      wpipe_ = (w0 ? 1 : wpipe); // - safe wpipe for Q_pipe_noretime\n\
\n\
   // - the following ``wire'' and ``assign'' stmts are used\n\
   //     to bypass the logic pipelining module if lpipe==0 and\n\
   //     to bypass the wire  pipelining module if wpipe==0\n\
   \n\
   wire [width-1:0]   lid,lod,wid,wod,qid,qod;	// - q_lpipe connections\n\
   wire 	      liv,lov,wiv,wov,qiv,qov;	// - q_wpipe connections\n\
   wire 	      lib,lob,wib,wob,qib,qob;	// - q       connections\n\
\n\
						// - input stream connections\n\
   assign 	      i_b = l0w0 ? qib : w0 ? lib : l0 ? wib : lib ;\n\
   \n\
   assign 	      lid = i_d;		// - q_lpipe connections\n\
   assign 	      liv = i_v;\n\
   assign 	      lob = w0 ? qib : wib ;\n\
\n\
   assign 	      wid = l0 ? i_d : lod ;	// - q_wpipe connections\n\
   assign 	      wiv = l0 ? i_v : lov ;\n\
   assign 	      wob = qib ;\n\
\n\
						// - q connections\n\
   assign 	      qid = l0w0 ? i_d : w0 ? lod : l0 ? wod : wod ;\n\
   assign 	      qiv = l0w0 ? i_v : w0 ? lov : l0 ? wov : wov ;\n\
   assign 	      qob = o_b ;\n\
\n\
   assign 	      o_d = qod ;		// - output stream connections\n\
   assign 	      o_v = qov ;\n\
\n\
   assign 	      qod = qid ;		// - bypass queue\n\
   assign 	      qov = qiv ;\n\
   assign 	      qib = qob ; 	      \n\
   \n\
   // - logic pipelining\n\
   Q_fwd_pipe      #(lpipe_,width) q_lpipe (clock,reset,lid,liv,lib,lod,lov,lob);\n\
\n\
   // - interconnect pipelining\n\
   Q_pipe_noretime #(wpipe_,width) q_wpipe (clock,reset,wid,wiv,wib,wod,wov,wob);\n\
\n\
   // - NO queue with reserve capacity\n\
   // Q_srl_reserve #(depth,width,lpipe+wpipe*2) q (clock,reset,qid,qiv,qib,qod,qov,qob);\n\
   \n\
endmodule // Q_lwpipe\n\
\n\
\n\
`endif  // `ifdef  Q_lwpipe\n\
";


#endif	// #ifndef _TDF_Q_LWPIPE_
