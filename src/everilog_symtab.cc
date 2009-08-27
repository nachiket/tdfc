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
// SCORE TDF compiler:  Symbol table for Verilog emission
// $Revision: 1.1 $
//
//////////////////////////////////////////////////////////////////////////////

// - used internally by "everilog.cc" and "everilog_compose.cc"


#include "everilog_symtab.h"
#include <LEDA/core/map.h>

using leda::map;

using std::ofstream;
using std::cerr;
using std::cout;
using std::endl;

void verilogKeywords (set<string> *keywords)
{
  // - initialize a set of Verilog keywords
  // - http://toolbox.xilinx.com/docsan/xilinx4/data/docs/xst/verilog10.html

  char* keywords_array[] = {
	"always",	"and",		"assign",	"begin",
	"buf",		"bufif0",	"bufif1",	"case",
	"casex",	"casez",	"cmos",		"deassign",
	"default",	"defparam",	"disable",	"edge",
	"else",		"end",		"endcase",	"endfunction",
	"endmodule",	"endprimitive", "endspecify",	"endtable",
	"endtask",	"event",	"for",		"force",
	"forever",	"function",	"highz0",	"highz1",
	"if",		"ifnone",	"initial",	"inout",
	"input",	"integer",	"join",		"large",
	"macromodule",	"medium",	"module",	"nand",
	"negedge",	"nmos",		"nor",		"not",
	"notif0",	"notif1",	"or",		"output",
	"parameter",	"pmos",		"posedge",	"primitive",
	"pull0",	"pull1",	"pulldown",	"pullup",
	"rcmos",	"real",		"realtime",	"reg",
	"release",	"repeat",	"rnmos",	"rpmos",
	"rtran",	"rtranif0",	"rtranif1",	"scalared",
	"small",	"specify",	"specparam",	"strong0",
	"strong1",	"supply0",	"supply1",	"table",
	"task",		"time",		"tran",		"tranif0",
	"tranif1",	"tri",		"tri0",		"tri1",
	"triand",	"trior",	"trireg",	"vectored",
	"wait",		"wand",		"weak0",	"weak1",
	"while",	"wire",		"wor",		"xnor",
	"xor"
  };
  int numKeywords = sizeof(keywords_array) / sizeof(char*);
  keywords->empty();
  for (int i=0; i<numKeywords; i++)
    keywords->insert(keywords_array[i]);
}
