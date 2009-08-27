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
// SCORE TDF compiler:  verilog back end
// $Revision: 1.5 $
//
//////////////////////////////////////////////////////////////////////////////


#ifndef _TDF_EVERILOG_
#define _TDF_EVERILOG_

// - Verilog names for clock and reset signals
#define CLOCK_NAME "clock"
#define RESET_NAME "reset"


#include <LEDA/core/string.h>
#include <LEDA/core/list.h>
class Operator;
class OperatorBehavioral;
class OperatorCompose;
class EVerilogInfo;


// - Command line options to set stream implementation
//   for Verilog page emission (-everilog)
//     * "-qim" "-qid" "-qili" "-qilo" "-qiw" for page  input queues
//     * "-qom" "-qod" "-qoli" "-qolo" "-qow" for page output queues
//     * "-qlm" "-qld" "-qlli" "-qllo" "-qlw" for page  local streams
// - Depths to be replaced by stream depth bounds analysis
extern string gPageInputQueueModule;        // "-qim"  module         , in  q: Q_srl
extern int    gPageInputQueueDepth;         // "-qid"  depth          , in  q:  2
extern int    gPageInputQueueLogicInDepth;  // "-qili" logic in  depth, in  q: -1
extern int    gPageInputQueueLogicOutDepth; // "-qilo" logic out depth, in  q: -1
extern int    gPageInputQueueWireDepth;     // "-qiw"  wire      depth, in  q: -1
extern string gPageOutputQueueModule;       // "-qom"  module         , out q: Q_srl
extern int    gPageOutputQueueDepth;        // "-qod"  depth          , out q:  2
extern int    gPageOutputQueueLogicInDepth; // "-qoli" logic in  depth, out q: -1
extern int    gPageOutputQueueLogicOutDepth;// "-qolo" logic out depth, out q: -1
extern int    gPageOutputQueueWireDepth;    // "-qow"  wire      depth, out q: -1
extern string gPageLocalQueueModule;        // "-qlm"  module         , local: Q_srl
extern int    gPageLocalQueueDepth;         // "-qld"  depth          , local:  2
extern int    gPageLocalQueueLogicInDepth;  // "-qlli" logic in  depth, local: -1
extern int    gPageLocalQueueLogicOutDepth; // "-qllo" logic out depth, local: -1
extern int    gPageLocalQueueWireDepth;     // "-qlw"  wire      depth, local: -1


// - Used by "everilog.cc" and "everilog_compose.cc"
bool          isConstWidth                   (Type *t, int *width,
					      int private_recurse_top=true);
list<Symbol*> args_with_retsym_first         (Operator *op);
EVerilogInfo* tdfToVerilog_scanTdf_alloc     (OperatorBehavioral *op);
void          tdfToVerilog_scanTdf_dealloc   (OperatorBehavioral *op,
					      EVerilogInfo *info);
string        tdfToVerilog_fsmbb_toString    (OperatorBehavioral *op,
					      EVerilogInfo *info);
string        tdfToVerilog_fsm_dp_toString   (OperatorBehavioral *op,
					      EVerilogInfo *info);
void          tdfToVerilog_blackbox_toFile   (Operator *op);
void          tdfToVerilog_toFile            (OperatorBehavioral *op);
void          addEmptyLine                   (string *s);

// - Exported
void instanceSegmentOps    (Operator *op);	      
void tdfToVerilog          (OperatorBehavioral *op);  // in everilog.cc
void tdfToVerilog_compose  (OperatorCompose *op);     // in everilog_compose.cc
void tdfToVerilog_instance (Operator *iop,
			    list<OperatorBehavioral*> *instances);


#endif // _TDF_EVERILOG_
